/**
 *
 * �� �� �� : Watch.cpp
 * �������� : 2015-8-1 13:36
 * �޸����� : $Date: $
 * ��ǰ�汾 : $Revision: $
 * �������� : ���Ź�������
 * �޸ļ�¼ : 
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
// ��    ��:  ���������ļ�
// ��    ��:  SK
// ����ʱ��:  2015-8-1 13:52
// ����˵��:  void
// �� �� ֵ:  true/false
//////////////////////////////////////////////////////////////////////////
bool CWatch::Load(SString sPathFile)
{
	//�ػ������б�
	CConfigBase::Load(sPathFile);
	SXmlConfig xml;
	if(!xml.ReadConfig(sPathFile))
	{
		LOGFAULT("�������ļ�[%s]ʱʧ��!�����ļ��Ƿ�����Ҹ�ʽ�Ƿ���ȷ!\n",sPathFile.data());
		exit(0);
		return false;
	}
	LOGDEBUG("�����ػ���������!");
	m_Lock.lock();
	SBaseConfig *pHd = xml.SearchChild("hard_dog");
	bool bHardDog = false;
	if(pHd != NULL && pHd->GetAttribute("open").toLower() == "true")
	{
		bHardDog = true;
		if(bHardDog && m_bCmdNoDog)
		{
			LOGWARN("���Ź���������ʱָ����nodog��������������ļ��п���Ӳ�����Ź������ý�������Ч��������Ч��ȥ��nodog�������б�����!");
			bHardDog = false;
		}
	}
	if(bHardDog == false && m_bHardWatch == true)
	{
		LOGWARN("�ر�Ӳ�����Ź�����!");
		m_WatchDog.Close();//��;�ر�
	}

	m_bHardWatch = bHardDog;
	LOGWARN("Ӳ�����Ź�����Ϊ%s!",m_bHardWatch?"����":"�ر�");
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
		LOGDEBUG("�ػ�����%d:mod=%s;cmd=%s;arg=%s;path=%s;first_delay_sec=%d;",
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
// ��    ��:  ���в��ػ����н���
// ��    ��:  SK
// ����ʱ��:  2015-8-1 14:03
// ����˵��:  
// �� �� ֵ:  
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
			printf("��ӭʹ�ð���!\n");
			printf("%s nodog   \t�Թر�Ӳ�����Ź���ʽ����\n",argv[0]);
			printf("%s noreload\t�Թر������ļ�������ӹ��ܵķ�ʽ����\n",argv[0]);
			printf("%s version \t��ʾ��ģ��İ汾��Ϣ\n",argv[0]);
			printf("%s help    \t��ʾ��������Ϣ\n",argv[0]);
			printf("%s stopall \tֹͣ���б��ػ��Ľ���,����ȫ�ر�Ӳ�����Ź�\n",argv[0]);
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
				LOGFAULT("�򿪿��Ź�ʧ��!");
			}
			else
			{
				LOGDEBUG("�򿪿��Ź��ɹ�!");
			}
			if(m_WatchDog.FeedDog())
			{
				LOGDEBUG("ι���ɹ�!");
			}
			else
			{
				LOGWARN("ι��ʧ��!");
			}
			if(m_WatchDog.Close())
			{
				LOGDEBUG("�رտ��Ź��ɹ�!");
			}
			else
			{
				LOGDEBUG("�رտ��Ź�ʧ��!");
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
			LOGFAULT("�򿪿��Ź�ʧ��!");
		}
		else
		{
			LOGDEBUG("Ӳ�����Ź��Ѵ�!");
		}
	}

	S_CREATE_THREAD(ThreadWatch,this);
	return true;
}

//////////////////////////////////////////////////////////////////////////
// ��    ��:  ��ֹ���б��ػ��Ľ���
// ��    ��:  SK
// ����ʱ��:  2015-8-1 13:40
// ����˵��:  void
// �� �� ֵ:  void
//////////////////////////////////////////////////////////////////////////
void CWatch::HaltAll()
{
	LOGDEBUG("�յ��ر�����ģ���ָ��!");
	//ȡ��ǰ������Ϣ
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
			//ģ���Ƿ�����?
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
				LOGERROR("��ֹģ��(%s)!",sModule.data());
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
			//ģ���Ƿ�����?
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
					LOGERROR("ǿ����ֹģ��(%s,pid=%d)!",sModule.data(),pid);
					SApi::SystemKillProcess(pid);
				}
			}
		}
		m_Lock.unlock();
		if(iRuns == 0)
			break;
	}


	LOGWARN("��ֹ�ػ���������!");
	//ParseCommandLine("quit");
	exit(1);
}

//////////////////////////////////////////////////////////////////////////
// ��    ��:  �Ȳ����������ػ����򣬲���ֹ
// ��    ��:  SK
// ����ʱ��:  2015-8-1 14:04
// ����˵��:  void
// �� �� ֵ:  void
//////////////////////////////////////////////////////////////////////////
void CWatch::HaltOtherSelfModule()
{
	//ȡ��ǰ������Ϣ
	SStringList list;
	int i,ps=SApi::GetSystemProcess(list);
	SString sModule,sModuleInfo,sTemp;
	int pid=0;
	SString sMyMod = SLog::getModuleName();

	LOGDEBUG("�Ȳ����������ػ����򣬲���ֹ");
	//�Ȳ����������ػ����򣬲���ֹ
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
			LOGDEBUG("��������pid=%d",pid);
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
	//��Ӳ�����Ź�
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
			//SApi::UsSleep(5000000);//5��һ��ι��
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
// ��    ��:  ִ��һ���ػ�����
// ��    ��:  SK
// ����ʱ��:  2015-8-1 15:16
// ����˵��:  void
// �� �� ֵ:  void
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
		LOGWARN("�����ػ�����!���ػ�������:%d��",cnt);
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
			continue;//����
		if(pExtModule->iStartErrTimes > 1000 && abs(::time(NULL)-pExtModule->tLastStartErr) < 300)
			continue;//����
		sModule = pExtModule->sModule;
		//LOGDEBUG("%s",sModule.data());
		//ģ���Ƿ�����?
		bRun = false;
		for(i=0;i<ps;i++)
		{
			sModuleInfo = *list[i];
			sTemp = SString::GetIdAttribute(3,sModuleInfo,",");
#ifdef WIN32
			if(sTemp.find("(32") >0)
				sTemp = sTemp.Left(sTemp.find("(32")).stripWhiteSpace();
			//ȥ��.exe
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
			LOGWARN("����ģ��(%s, %s)δ����!׼������!",sModule.data(),pExtModule->sCmdLine.data());
#ifndef WIN32
			if(pExtModule->sPath != "")
			{
				LOGWARN("�л�·����:[%s]!",pExtModule->sPath.data());
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
				LOGERROR("����ģ��(%s, %s)ʧ��!",sModule.data(),pExtModule->sCmdLine.data());
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
