/**
 *
 * 文 件 名 : UnitMgrApplication.cpp
 * 创建日期 : 2014-4-26 21:45
 * 作    者 : 邵凯田(skt001@163.com)
 * 修改日期 : $Date: $
 * 当前版本 : $Revision: $
 * 功能描述 : 单元管理应用程序类
 * 修改记录 : 
 *            $Log: $
 *
 * Ver  Date        Author  Comments
 * ---  ----------  ------  -------------------------------------------
 * 001	2014-4-26	邵凯田　创建文件
 *
 **/

#include "UnitMgrApplication.h"
#include "..\SKLic\slicense.h"

void* CUnitMgrApplication::ThreadLic(void* lp)
{
	CUnitMgrApplication *pThis = (CUnitMgrApplication*)lp;
	pThis->BeginThread();
	while(!pThis->IsQuit())
	{
		SApi::UsSleep(3600000000);
		SLicense lic;
		if(!lic.CheckLicense()) {
			printf("Licsence error\n");
			break;
		}
	}
	pThis->EndThread();
	pThis->ExitByLic();
	return NULL;
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CUnitMgrApplication::CUnitMgrApplication()
{
	m_PuttingParam.setAutoDelete(true);
}

CUnitMgrApplication::~CUnitMgrApplication()
{
	//CConfigMgr::Quit();
}

////////////////////////////////////////////////////////////////////////
// 描    述:  启动服务，开启所有应用的服务，该函数必须由派生类实现，且启动服务完成后必须返回
// 作    者:  邵凯田
// 创建时间:  2014:4:26 21:46
// 参数说明:  void
// 返 回 值:  true/false
//////////////////////////////////////////////////////////////////////////
bool CUnitMgrApplication::Start()
{
	SString sFile = GetConfPath()+"sys_unitconfig.xml";
	if(!m_UnitConfig.Load(sFile))
	{
		LOGFAULT("加载单元配置文件(%s)时失败!",sFile.data());
		return false;
	}
	//检查单元IP地址配置是否正确
	SString local_ips = SSocket::GetLocalIp().c_str();
	if(local_ips.length() == 0)
	{
		LOGERROR("网络配置异常!无法获取本机IP地址!");
		return false;
	}
	int i,ips = SString::GetAttributeCount(local_ips,",");
	SString ip;
	bool bOk = false;
	for(i=1;i<=ips;i++)
	{
		ip = SString::GetIdAttribute(i,local_ips,",");
		if(ip == m_UnitConfig.GetUnitIp())
		{
			bOk = true;
			break;
		}
	}
	if(!bOk)
	{
		LOGERROR("单元配置异常，单元IP(%s)不存在于本机的IP地址列表(%s)!",m_UnitConfig.GetUnitIp().data(),local_ips.data());
		return false;
	}

	m_iUnitId = m_UnitConfig.GetUnitId();
	m_sUnitName = m_UnitConfig.GetUnitName();
	m_sUnitDesc = m_UnitConfig.GetUnitDesc();
	m_sSoftwareType = m_UnitConfig.GetSystemType();
	if(IsParam("soft_type"))
	{
		m_sSoftwareType = GetParam("soft_type");
		m_UnitConfig.m_sSystemType = m_sSoftwareType;
		LOGDEBUG("软件型号被运行参数配置为:%s",m_sSoftwareType.data());
	}
	else if(m_sSoftwareType.length() == 0)
	{
		LOGWARN("未指定软件型号!您可以通过参数-soft_type=XXX来指定! ");
	}

	m_MCastFounder.SetUnitConfig(&m_UnitConfig);
	if(!m_MCastFounder.Start())
	{
		LOGWARN("启动组播自发现服务失败!");
	}

	m_Agent.SetMCastFounderPtr(&m_MCastFounder);
	m_Agent.m_SelfUnitInfo.m_iUnitIp[0] = SSocket::IpStrToIpv4(m_UnitConfig.m_sThisIp.data());
	if(!m_Agent.Start(m_iAgentPort))
	{
		LOGFAULT("单元代理服务加载失败!");
		return false;
	}
	//根据单元号确定连接方式，总是用小的单元号连接大的单元号
	unsigned long pos = 0;
	CSpUnit *pUnit = m_UnitConfig.m_Units.FetchFirst(pos);
	while(pUnit)
	{
		if(pUnit->m_iUnitId > m_UnitConfig.m_iUnitId)
		{
			m_Agent.ConnectAgent(pUnit);
		}
		pUnit = m_UnitConfig.m_Units.FetchNext(pos);
	}

	SKT_CREATE_THREAD(ThreadLic,this);
	return true;
}

////////////////////////////////////////////////////////////////////////
// 描    述:  停止服务，关闭所有应用的服务
// 作    者:  邵凯田
// 创建时间:  2014:4:26 21:46
// 参数说明:  void
// 返 回 值:  true/false
//////////////////////////////////////////////////////////////////////////
bool CUnitMgrApplication::Stop()
{
	m_MCastFounder.Stop();
	return m_Agent.Stop();
}


//////////////////////////////////////////////////////////////////////////
// 描    述:  处理文本命令
// 作    者:  邵凯田
// 创建时间:  2016-2-13 15:12
// 参数说明:  @sCmd为命令内容，空字符串表示取表命令列表
//         :  @sResult为返回结果，纯文本
// 返 回 值:  true表示成功，false表示失败
//////////////////////////////////////////////////////////////////////////
bool CUnitMgrApplication::ProcessTxtCmd(SString &sCmd,SString &sResult)
{
	if(sCmd.length() == 0)
	{
		sResult =	
			"cmd get_units                      取所有的代理单元\r\n"
			"cmd get_procs                      取本单元所有的进程列表\r\n"
			"cmd get_pid <appid> [inst_id=0]    取指定类型的进程标识信息\r\n"
			"";
		return true;
	}
	SString act = SString::GetIdAttribute(1,sCmd," ");
	if(act == "get_units")
	{
		SPtrList<stuSpUnitAgentUnitInfo> units;
		m_Agent.GetUnitList(units);
		sResult += SString::toFormat("共%d个代理单元\r\n",units.count());
		unsigned long pos = 0;
		stuSpUnitAgentUnitInfo *p = units.FetchFirst(pos);
		while(p)
		{
			sResult += SString::toFormat("%s  单元号:%u  IP:%s  %s  %s\r\n",
				p->m_sSoftType,p->m_iUnitId,SSocket::Ipv4ToIpStr(p->m_iUnitIp[0]).c_str(),p->m_sUnitName,p->m_sUnitDesc);
			p = units.FetchNext(pos);
		}
	}
	else if(act == "get_procs")
	{
		SPtrList<stuSpUnitAgentProcInfo> ProcInfos;
		m_Agent.GetProcList(ProcInfos);
		sResult += SString::toFormat("共%d个活动进程\r\n",ProcInfos.count());
		unsigned long pos = 0;
		stuSpUnitAgentProcInfo *p = ProcInfos.FetchFirst(pos);
		while(p)
		{
			sResult += SString::toFormat("%d:%s %s  V%s  CRC:%s\r\n",
				p->pid,p->name,p->desc,p->version,p->crc);
			p = ProcInfos.FetchNext(pos);
		}
	}
	else if(act == "get_pid")
	{
		sResult += "尚未实现\r\n";
	}
	else
		sResult += "无效的命令\r\n";
	return true;
}

////////////////////////////////////////////////////////////////////////
// 描    述:  代理消息处理接口，由派生类实现，处理函数必须尽量短小，快速返回
// 作    者:  邵凯田
// 创建时间:  2016-2-3 15:20
// 参数说明:  @wMsgType表示消息类型
//         :  @pMsgHead为消息头
//         :  @sHeadStr消息头字符串
//         :  @pBuffer为消息携带的二进制数据内容，NULL表示无二进制数据
//         :  @iLength为二进制数据长度
// 返 回 值:  true表示处理成功，false表示处理失败
//////////////////////////////////////////////////////////////////////////
#ifndef _WITHOUT_AGENT_CHANNEL_
bool CUnitMgrApplication::ProcessAgentMsg(WORD wMsgType,stuSpUnitAgentMsgHead *pMsgHead,SString &sHeadStr,BYTE* pBuffer/*=NULL*/,int iLength/*=0*/)
{
	//响应报文头
	stuSTcpPackage *pPackage = NULL;
	switch(wMsgType)
	{
	case SP_UA_MSG_REQ_GET_UNIT://取单元信息
		{
			SPtrList<stuSpUnitAgentUnitInfo> UnitInfos;
			m_Agent.GetUnitList(UnitInfos);
			if(UnitInfos.count() == 0)
			{
				SendAgentMsg(&pMsgHead->m_SrcId,SP_UA_MSG_RES_GET_UNIT,pMsgHead->m_dwMsgSn,0);
				goto end;
			}
			stuSpUnitAgentUnitInfo *p = new stuSpUnitAgentUnitInfo[UnitInfos.count()];
			unsigned long pos=0;
			int iSize = 0;
			UnitInfos.toNextArray(p,iSize,UnitInfos.count(),pos);
			LOGDEBUG("SP_UA_MSG_REQ_GET_UNIT=%d",UnitInfos.count());
			SendAgentMsg(&pMsgHead->m_SrcId,SP_UA_MSG_RES_GET_UNIT,pMsgHead->m_dwMsgSn,UnitInfos.count(),(BYTE*)p,sizeof(stuSpUnitAgentUnitInfo)*UnitInfos.count());
			delete[] p;
			goto end;
		}
		break;
	case SP_UA_MSG_REQ_GET_PROCINFO://取进程信息
		{
			SPtrList<stuSpUnitAgentProcInfo> ProcInfos;
			m_Agent.GetProcList(ProcInfos);
			if(ProcInfos.count() == 0)
			{
				SendAgentMsg(&pMsgHead->m_SrcId,SP_UA_MSG_RES_GET_PROCINFO,pMsgHead->m_dwMsgSn,0);
				goto end;
			}
			stuSpUnitAgentProcInfo *p = new stuSpUnitAgentProcInfo[ProcInfos.count()];
			unsigned long pos=0;
			int iSize = 0;
			ProcInfos.toNextArray(p,iSize,ProcInfos.count(),pos);
			LOGDEBUG("SP_UA_MSG_REQ_GET_PROCINFO=%d",ProcInfos.count());
			SendAgentMsg(&pMsgHead->m_SrcId,SP_UA_MSG_RES_GET_PROCINFO,pMsgHead->m_dwMsgSn,ProcInfos.count(),(BYTE*)p,sizeof(stuSpUnitAgentProcInfo)*ProcInfos.count());
			delete[] p;
			goto end;
		}
		break;
	case SP_UA_MSG_REQ_GET_PROCID://取进程标识
		{
			SPtrList<stuSpUnitAgentProcInfo> ProcInfos;
			m_Agent.GetProcList(ProcInfos);
			if(ProcInfos.count() == 0 || iLength != sizeof(stuSpUnitAgentProcessId))
			{
				SendAgentMsg(&pMsgHead->m_SrcId,SP_UA_MSG_RES_GET_PROCID,pMsgHead->m_dwMsgSn,0);
				goto end;
			}
			stuSpUnitAgentProcessId *pReq = (stuSpUnitAgentProcessId*)pBuffer;
			stuSpUnitAgentProcessId *p = new stuSpUnitAgentProcessId[ProcInfos.count()];
			unsigned long pos=0;
			int idx = 0;
			stuSpUnitAgentProcInfo *p0 = ProcInfos.FetchFirst(pos);
			while(p0)
			{
				if(pReq->GetApplicationId() == p0->process_id.GetApplicationId() && (pReq->GetAppInstId() == 0 || pReq->GetAppInstId() == p0->process_id.GetAppInstId()))
				{
					memcpy(p+idx,&p0->process_id,sizeof(stuSpUnitAgentProcessId));
					idx++;
				}
				p0 = ProcInfos.FetchNext(pos);
			}
			LOGDEBUG("SP_UA_MSG_REQ_GET_PROCID=%d",idx);
			if(idx == 0)
			{
				if(pMsgHead->m_DstId.m_iUnitId == m_iUnitId)//明确给自己的才返回空
					SendAgentMsg(&pMsgHead->m_SrcId,SP_UA_MSG_RES_GET_PROCID,pMsgHead->m_dwMsgSn,0);
			}
			else
				SendAgentMsg(&pMsgHead->m_SrcId,SP_UA_MSG_RES_GET_PROCID,pMsgHead->m_dwMsgSn,idx,(BYTE*)p,sizeof(stuSpUnitAgentProcessId)*idx);
			delete[] p;
			goto end;
		}
		break;
	case SP_UA_MSG_REQ_LS: //列目录
		{
			if(pBuffer == NULL || iLength != sizeof(stuSpUnitAgentLsReq))
				break;
			stuSpUnitAgentLsReq *pReq = (stuSpUnitAgentLsReq*)pBuffer;
			SString sPath = pReq->path;
			if(sPath.right(1) != "\\" && sPath.right(1) != "/")
			{
#ifdef WIN32
				sPath += "\\";
#else
				sPath += "/";
#endif
			}
			SString sFile = m_pApp->GetHomePath();
			if(sFile.length() > 0)
				sFile = sFile.left(sFile.length()-1);
			sPath.replace("{@APP_PATH@}",sFile);
			SDir dir(sPath,pReq->filter);
			int i,cnt = dir.count();
			stuSpUnitAgentLsRes *pRes = NULL;
			for(i=0;i<cnt;i++)
			{
				sFile = dir[i];
				if(sFile == "." || sFile == "..")
					cnt--;
			}
			if(cnt > 0)
				pRes = new stuSpUnitAgentLsRes[cnt];
			stuSpUnitAgentLsRes *pRes2 = pRes;
			SString attr;
			for(i=0;i<cnt;i++)
			{
				sFile = dir[i];
				if(sFile == "." || sFile == "..")
					continue;				
				SString::strncpy(pRes2->name,sizeof(pRes2->name),sFile.data());
				attr = dir.attribute(i);
				if(SString::GetAttributeValueI(attr,"isdir") == 1)
				{
					pRes2->is_dir = true;
					pRes2->file_size = 0;
				}
				else
				{
					pRes2->is_dir = false;
					pRes2->file_size = SFile::fileSize(sPath+sFile);
				}
				pRes2++;
			}
			LOGDEBUG("SP_UA_MSG_REQ_LS=%d",cnt);
			SendAgentMsg(&pMsgHead->m_SrcId,SP_UA_MSG_RES_LS,pMsgHead->m_dwMsgSn,cnt,(BYTE*)pRes,sizeof(*pRes)*cnt);
			if(pRes != NULL)
				delete[] pRes;
			break;
		}
	case SP_UA_MSG_REQ_GET_FILE: //取文件
		{
			if(pBuffer == NULL || iLength != sizeof(stuSpUnitAgentGetFileReq))
				break;
			stuSpUnitAgentGetFileReq *pReq = (stuSpUnitAgentGetFileReq*)pBuffer;
			SString sPath = pReq->path_file;			
			SString sFile = m_pApp->GetHomePath();
			if(sFile.length() > 0)
				sFile = sFile.left(sFile.length()-1);
			sPath.replace("{@APP_PATH@}",sFile);
			SFile file(sPath);
			if(!file.open(IO_ReadOnly))
			{
				LOGWARN("读取文件失败!file=%s",sPath.data());
				SendAgentMsg(&pMsgHead->m_SrcId,SP_UA_MSG_RES_GET_FILE,pMsgHead->m_dwMsgSn,0);
				break;
			}
			SString sResHeadStr;
			int file_size = file.size();
			if(file_size <= 0)
			{
				//指定文件大小为0
				SendAgentMsg(&pMsgHead->m_SrcId,SP_UA_MSG_RES_GET_FILE,pMsgHead->m_dwMsgSn,0);
				break;
			}
			sResHeadStr.sprintf("filesize=%d;",file_size);
			file.seekBegin();
			BYTE *pBuf = NULL;
			int ret,size;
			int sn=1;
			while(file_size > 0)
			{
				size = file_size;
				if(size > 102400)
					size = 102400;
				if(pBuf == NULL)
					pBuf = new BYTE[size];
				ret = file.readBlock(pBuf,size);
				if(ret != size)
				{
					LOGWARN("读取文件内容时失败!");
					break;
				}
				file_size -= size;
				if(file_size<=0)
					sn = 0;//最后一帧给0
				if(!SendAgentMsg(&pMsgHead->m_SrcId,SP_UA_MSG_RES_GET_FILE,pMsgHead->m_dwMsgSn,sn++,pBuf,size,&sResHeadStr))
					break;
				if(GetSendPackagePtr()->count() > 1000)
					SApi::UsSleep(10000);
			}
			break;
		}
	case SP_UA_MSG_REQ_PUT_FILE: //存文件
		{
			SString sFile = SString::GetAttributeValue(sHeadStr,"filename");
			int filesize = SString::GetAttributeValueI(sHeadStr,"filesize");//文件大小
			int pice = SString::GetAttributeValueI(sHeadStr,"pice");//从1开始
			int cont = SString::GetAttributeValueI(sHeadStr,"cont");//1表示有后续帧，0表示没有后续帧
			int ret;
			SString sHome = m_pApp->GetHomePath();
			if(sHome.length() > 0)
				sHome = sHome.left(sHome.length()-1);
			sFile.replace("{@APP_PATH@}",sHome);
			stuPuttingFileParam *pParam = SearchPuttingParam(sFile);
			if(pice == 1)
			{
				if(pParam != NULL)
				{
					LOGWARN("存文件时重复，将删除原文件");
					delete pParam;
					//SendAgentMsg(&pMsgHead->m_SrcId,SP_UA_MSG_RES_PUT_FILE,pMsgHead->m_dwMsgSn,101);
					//break;
				}
				pParam = new stuPuttingFileParam();
				pParam->sFileName = sFile;
				pParam->begin_soc = (int)SDateTime::getNowSoc();
				pParam->file.setFileName(sFile+".tmp");
				if(!pParam->file.open(IO_Truncate))
				{
					delete pParam;
					LOGWARN("存文件时写临时文件失败!file=%s.tmp",sFile.data());
					SendAgentMsg(&pMsgHead->m_SrcId,SP_UA_MSG_RES_PUT_FILE,pMsgHead->m_dwMsgSn,102);
					break;
				}
				m_PuttingParam.append(pParam);
				pParam->file.seekBegin();
			}
			else
			{
				if(pParam == NULL)
				{
					LOGWARN("存文件时错误，未收到有效的首帧报文!");
					break;
				}
			}
			if(iLength > 0)
			{
				ret = pParam->file.writeBlock((BYTE*)pBuffer,iLength);
				if(ret != iLength)
				{
					pParam->file.close();
					SFile::remove(pParam->file.filename());
					LOGWARN("存文件时写文件内容失败!file=%s.tmp",sFile.data());
					SendAgentMsg(&pMsgHead->m_SrcId,SP_UA_MSG_RES_PUT_FILE,pMsgHead->m_dwMsgSn,103);
					m_PuttingParam.remove(pParam);
					break;
				}
			}
			if(cont == 0)
			{
				//没有后续帧了，验证文件大小
				pParam->file.close();
				if(pParam->file.size() != filesize)
				{
					SFile::remove(pParam->file.filename());
					LOGWARN("存文件时，文件大小验证失败!file=%s.tmp",sFile.data());
					SendAgentMsg(&pMsgHead->m_SrcId,SP_UA_MSG_RES_PUT_FILE,pMsgHead->m_dwMsgSn,104);
					m_PuttingParam.remove(pParam);
					break;
				}
				//文件验证成功,改名称
				SFile::remove(pParam->sFileName);
				SFile::rename(pParam->sFileName+".tmp",pParam->sFileName);
				SendAgentMsg(&pMsgHead->m_SrcId,SP_UA_MSG_RES_PUT_FILE,pMsgHead->m_dwMsgSn,0);
				m_PuttingParam.remove(pParam);
			}
			break;
		}
	case SP_UA_MSG_REQ_DEL_FILE: //删除文件
		{
			SString sFile = SString::GetAttributeValue(sHeadStr,"filename");
			SString sHome = m_pApp->GetHomePath();
			if(sHome.length() > 0)
				sHome = sHome.left(sHome.length()-1);
			sFile.replace("{@APP_PATH@}",sHome);

			if(!SFile::exists(sFile))
			{
				LOGWARN("待删除文件不存在!file=%s",sFile.data());
				SendAgentMsg(&pMsgHead->m_SrcId,SP_UA_MSG_RES_DEL_FILE,pMsgHead->m_dwMsgSn,101);
				break;
			}
			if(!SFile::remove(sFile))
			{
				LOGWARN("删除文件失败!file=%s",sFile.data());
				SendAgentMsg(&pMsgHead->m_SrcId,SP_UA_MSG_RES_DEL_FILE,pMsgHead->m_dwMsgSn,102);
				break;
			}
			SendAgentMsg(&pMsgHead->m_SrcId,SP_UA_MSG_RES_DEL_FILE,pMsgHead->m_dwMsgSn,0);
			break;
		}
	default:
		return false;
	}

end:
	return true;
};
#endif
