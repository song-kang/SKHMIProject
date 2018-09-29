/**
 *
 * 文 件 名 : Watch.cpp
 * 创建日期 : 2015-8-1 13:36
 * 修改日期 : $Date: $
 * 当前版本 : $Revision: $
 * 功能描述 : 看门狗服务类
 * 修改记录 : 
 *
 **/

#include "Watch.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CWatch::CWatch()
{
	m_bQuit = m_bQuited = false;
	m_bHardWatch = false;
	m_ExtModuleList.setAutoDelete(true);
	SetAutoReload(true);
	SetConfigType(SPCFG_USER);
}

CWatch::~CWatch()
{

}


//////////////////////////////////////////////////////////////////////////
// 描    述:  加载配置文件
// 作    者:  SK
// 创建时间:  2015-8-1 13:52
// 参数说明:  void
// 返 回 值:  true/false
//////////////////////////////////////////////////////////////////////////
bool CWatch::Load(SString sPathFile)
{
	//守护进程列表
	CConfigBase::Load(sPathFile);
	SXmlConfig xml;
	if(!xml.ReadConfig(sPathFile))
	{
		LOGFAULT("打开配置文件[%s]时失败!请检查文件是否存在且格式是否正确!\n",sPathFile.data());
		exit(0);
		return false;
	}
	LOGDEBUG("加载守护进程配置!");
	m_Lock.lock();
	SBaseConfig *pHd = xml.SearchChild("hard_dog");
	bool bHardDog = false;
	if(pHd != NULL && pHd->GetAttribute("open").toLower() == "true")
	{
		bHardDog = true;
		if(bHardDog && m_bCmdNoDog)
		{
			LOGWARN("看门狗程序运行时指定了nodog参数，因此配置文件中开启硬件看门狗的配置将不会生效，如须生效请去除nodog参数运行本程序!");
			bHardDog = false;
		}
	}
	if(bHardDog == false && m_bHardWatch == true)
	{
		LOGWARN("关闭硬件看门狗功能!");
		m_WatchDog.Close();//中途关闭
	}

	m_bHardWatch = bHardDog;
	LOGWARN("硬件看门狗设置为%s!",m_bHardWatch?"开启":"关闭");
	m_ExtModuleList.clear();
	SBaseConfig *pList = xml.GetChildNode("watch-list");
	if(pList == NULL)
		pList = xml.GetChildNode("watch");
	int iModule,iModules = pList==NULL?0:pList->GetChildCount("process");
	for(iModule=0;iModule<iModules;iModule++)
	{
		stuExtModule *pNew = new stuExtModule;
		pNew->iStartErrTimes = 0;
		SBaseConfig *pMod = pList->GetChildNode(iModule,"process");
		pNew->sModule = pMod->GetAttribute("module");
		pNew->sCmdLine= pMod->GetAttribute("cmdline");
		pNew->sArg	  = pMod->GetAttribute("arg");
		pNew->sPath   = pMod->GetAttribute("path");
		pNew->first_delay_sec = pMod->GetAttributeI("first_delay_sec");
		pNew->is_first = true;
		if(pNew->sPath.length() == 0)
			pNew->sPath = m_sBinPath;
		m_ExtModuleList.append(pNew);
		LOGDEBUG("守护进程%d:mod=%s;cmd=%s;arg=%s;path=%s;first_delay_sec=%d;",
			iModule+1,
			pNew->sModule.data(),
			pNew->sCmdLine.data(),
			pNew->sArg.data(),
			pNew->sPath.data(),
			pNew->first_delay_sec);
	}
	m_Lock.unlock();
	return true;
}

//////////////////////////////////////////////////////////////////////////
// 描    述:  运行并守护所有进程
// 作    者:  SK
// 创建时间:  2015-8-1 14:03
// 参数说明:  
// 返 回 值:  
//////////////////////////////////////////////////////////////////////////
bool CWatch::Run(int argc, char* argv[])
{
	if(argc > 1)
	{
		SString sParam = argv[1];
		if(sParam == "?" || sParam == "help" || sParam == "-help")
		{
			printf("\n");
			printf("===================================================================================\n");
			printf("欢迎使用帮助!\n");
			printf("%s nodog   \t以关闭硬件看门狗方式启动\n",argv[0]);
			printf("%s noreload\t以关闭配置文件变更监视功能的方式启动\n",argv[0]);
			printf("%s version \t显示本模块的版本信息\n",argv[0]);
			printf("%s help    \t显示本帮助信息\n",argv[0]);
			printf("%s stopall \t停止所有被守护的进程,并安全关闭硬件看门狗\n",argv[0]);
			printf("===================================================================================\n");
			printf("\n");
			return false;
		}
	}
	HaltOtherSelfModule();
	
	bool bReload = true;
	if(argc > 1)
	{
		SString sParam = argv[1];
		if(sParam == "stopall")
		{
			LOGDEBUG("Begin to HaltAll...");

			if(!m_WatchDog.Open())
			{
				LOGFAULT("打开看门狗失败!");
			}
			else
			{
				LOGDEBUG("打开看门狗成功!");
			}
			if(m_WatchDog.FeedDog())
			{
				LOGDEBUG("喂狗成功!");
			}
			else
			{
				LOGWARN("喂狗失败!");
			}
			if(m_WatchDog.Close())
			{
				LOGDEBUG("关闭看门狗成功!");
			}
			else
			{
				LOGDEBUG("关闭看门狗失败!");
			}
			HaltAll();
			return false;
		}
	}
	Watch();

	if(m_bHardWatch)
	{
		if(!m_WatchDog.Open())
		{
			LOGFAULT("打开看门狗失败!");
		}
		else
		{
			LOGDEBUG("硬件看门狗已打开!");
		}
	}

	S_CREATE_THREAD(ThreadWatch,this);
	return true;
}

//////////////////////////////////////////////////////////////////////////
// 描    述:  中止所有被守护的进程
// 作    者:  SK
// 创建时间:  2015-8-1 13:40
// 参数说明:  void
// 返 回 值:  void
//////////////////////////////////////////////////////////////////////////
void CWatch::HaltAll()
{
	LOGDEBUG("收到关闭所有模块的指令!");
	//取当前进程信息
	SStringList list;
	int i,ps;
	SString sModule,sModuleInfo,sTemp;
	bool bRun;
	int iModule,iModules;
	int pid=0,p;

	time_t last=::time(NULL);
	bool iRuns = 0;
	while(::time(NULL) - last < 10)
	{
		list.clear();
		ps=SApi::GetSystemProcess(list);
		iRuns = 0;
		m_Lock.lock();
		iModules = m_ExtModuleList.count();
		for(iModule=0;iModule<iModules;iModule++)
		{
			stuExtModule *pExtModule = m_ExtModuleList[iModule];
			sModule = pExtModule->sModule;
			//模块是否运行?
			bRun = false;
			for(i=0;i<ps;i++)
			{
				sModuleInfo = *list[i];
				sTemp = SString::GetIdAttribute(3,sModuleInfo,",");
#ifdef WIN32
				if(sTemp.find("(32") >0)
					sTemp = sTemp.Left(sTemp.find("(32")).stripWhiteSpace();
				sTemp += ".exe";
#endif
				p = sTemp.find(sModule);
				if(p < 0)
					continue;
				if(sTemp.length() == p+sModule.length() ||
					 sTemp.at(p+sModule.length()) == ' ')
				{
					pid  = SString::GetIdAttributeI(1,sModuleInfo,",");
					bRun = true;
					iRuns ++;
					break;
				}
			}
			if(bRun)
			{
				LOGERROR("中止模块(%s)!",sModule.data());
#ifdef WIN32
				SApi::SystemKillProcess(pid);
#else
				//int spid = 
				SApi::SystemExec(pExtModule->sCmdLine,sModule,"stop");
				//SApi::SystemKillProcess(pid);
#endif
			}
		}
		m_Lock.unlock();

		if(iRuns == 0)
			break;

		SApi::UsSleep(1000*1000);
	}

	SApi::UsSleep(1000*1000);
	while(1)
	{
		iRuns = 0;
		list.clear();
		ps=SApi::GetSystemProcess(list);
		m_Lock.lock();
		iModules = m_ExtModuleList.count();
		for(iModule=0;iModule<iModules;iModule++)
		{
			stuExtModule *pExtModule = m_ExtModuleList[iModule];
			sModule = pExtModule->sModule;
			//模块是否运行?
			bRun = false;
			for(i=0;i<ps;i++)
			{
				sModuleInfo = *list[i];
				sTemp = SString::GetIdAttribute(3,sModuleInfo,",");
#ifdef WIN32
				if(sTemp.find("(32") >0)
					sTemp = sTemp.Left(sTemp.find("(32")).stripWhiteSpace();
				sTemp += ".exe";
#endif
				p = sTemp.find(sModule);
				if(p < 0)
					continue;
				if(sTemp.length() == p+sModule.length() ||
					 sTemp.at(p+sModule.length()) == ' ')
				{
					iRuns ++;
					pid  = SString::GetIdAttributeI(1,sModuleInfo,",");
					LOGERROR("强行中止模块(%s,pid=%d)!",sModule.data(),pid);
					SApi::SystemKillProcess(pid);
				}
			}
		}
		m_Lock.unlock();
		if(iRuns == 0)
			break;
	}


	LOGWARN("中止守护程序运行!");
	//ParseCommandLine("quit");
	exit(1);
}

//////////////////////////////////////////////////////////////////////////
// 描    述:  先查找其它的守护程序，并终止
// 作    者:  SK
// 创建时间:  2015-8-1 14:04
// 参数说明:  void
// 返 回 值:  void
//////////////////////////////////////////////////////////////////////////
void CWatch::HaltOtherSelfModule()
{
	//取当前进程信息
	SStringList list;
	int i,ps=SApi::GetSystemProcess(list);
	SString sModule,sModuleInfo,sTemp;
	int pid=0;
	SString sMyMod = SLog::getModuleName();

	LOGDEBUG("先查找其它的守护程序，并终止");
	//先查找其它的守护程序，并终止
	int mypid = SApi::GetPid();
	for(i=0;i<ps;i++)
	{
		sModuleInfo = *list[i];
		sTemp = SString::GetIdAttribute(3,sModuleInfo,",");
#ifdef WIN32
		if(sTemp.find("(32") >0)
			sTemp = sTemp.Left(sTemp.find("(32")).stripWhiteSpace();
		sTemp += ".exe";
#endif
		if(sTemp.find(sMyMod) < 0)
			continue;
		pid  = SString::GetIdAttributeI(1,sModuleInfo,",");

		if(pid != mypid)
		{
			LOGDEBUG("结束进程pid=%d",pid);
			SApi::SystemKillProcess(pid);
		}
	}
}

void* CWatch::ThreadWatch(void *lp)
{
	#ifndef WIN32
		pthread_detach(pthread_self());
	#endif
	CWatch *pThis = (CWatch*)lp;
	bool bFirst = true;
	//打开硬件看门狗
	while(1)
	{
		if (bFirst)
		{
			bFirst= false;
			SApi::UsSleep(5000000);
		}
		if(pThis->m_bHardWatch)
		{
			pThis->m_WatchDog.FeedDog();
			//SApi::UsSleep(5000000);//5秒一次喂狗
			//pThis->m_WatchDog.Close();
			//break;
		}
		pThis->Watch();
		SApi::UsSleep(2000000);
	}

	pThis->m_bQuited = true;
	return NULL;
}

//////////////////////////////////////////////////////////////////////////
// 描    述:  执行一次守护动作
// 作    者:  SK
// 创建时间:  2015-8-1 15:16
// 参数说明:  void
// 返 回 值:  void
//////////////////////////////////////////////////////////////////////////
void CWatch::Watch()
{
	SStringList list;
	SString sModule,sModuleInfo,sTemp;
	int p,pid,i,ps=SApi::GetSystemProcess(list);
	m_Lock.lock();
	int m,cnt = m_ExtModuleList.count();
	bool bRun;
	static bool bFirst = true;
	static int first_soc = (int)SDateTime::getNowSoc();
	if(bFirst)
	{
		LOGWARN("开启守护进程!被守护进程数:%d个",cnt);
		bFirst = false;
	}
	for(m=0;m<cnt;m++)
	{
		stuExtModule *pExtModule = m_ExtModuleList[m];
		if(pExtModule->is_first)
		{
			int now_soc = (int)SDateTime::getNowSoc();
			if(now_soc - first_soc >= pExtModule->first_delay_sec)
			{
				pExtModule->is_first = false;
			}
			else
			{
				continue;
			}
		}
		if(pExtModule->iStartErrTimes > 100 && abs(::time(NULL)-pExtModule->tLastStartErr) < 60)
			continue;//忽略
		if(pExtModule->iStartErrTimes > 1000 && abs(::time(NULL)-pExtModule->tLastStartErr) < 300)
			continue;//忽略
		sModule = pExtModule->sModule;
		//LOGDEBUG("%s",sModule.data());
		//模块是否运行?
		bRun = false;
		for(i=0;i<ps;i++)
		{
			sModuleInfo = *list[i];
			sTemp = SString::GetIdAttribute(3,sModuleInfo,",");
#ifdef WIN32
			if(sTemp.find("(32") >0)
				sTemp = sTemp.Left(sTemp.find("(32")).stripWhiteSpace();
			//去掉.exe
			//sTemp += ".exe";
			if(sModule.right(4) != ".exe") 
				sModule += ".exe";
#endif
			p = sTemp.find(sModule);
			if(p < 0)
				continue;
			
			if(sTemp.length() == p+sModule.length() || sTemp.at(p+sModule.length()) == ' ')
			{
				pid  = SString::GetIdAttributeI(1,sModuleInfo,",");
				bRun = true;
				break;
			}
		}
		if(!bRun)
		{
			LOGWARN("独立模块(%s, %s)未运行!准备启动!",sModule.data(),pExtModule->sCmdLine.data());
#ifndef WIN32
			if(pExtModule->sPath != "")
			{
				LOGWARN("切换路径到:[%s]!",pExtModule->sPath.data());
				chdir(pExtModule->sPath.data());
				//SString sCmd = "cd " + pExtModule->sPath + "\r\n";
				//system(sCmd.data());
			}
#endif
			int spid = SApi::SystemExec(pExtModule->sCmdLine,sModule,pExtModule->sArg,pExtModule->sPath);
			if(spid <= 0)
			{
				pExtModule->iStartErrTimes ++;
				pExtModule->tLastStartErr = ::time(NULL);
				LOGERROR("启动模块(%s, %s)失败!",sModule.data(),pExtModule->sCmdLine.data());
			}
			else
			{
				pExtModule->iStartErrTimes = 0;
				SApi::UsSleep(500*1000);
			}
		}		
	}
	m_Lock.unlock();
}
