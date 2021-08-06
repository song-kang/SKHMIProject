/**
 *
 * �� �� �� : UnitMgrApplication.cpp
 * �������� : 2014-4-26 21:45
 * ��    �� : �ۿ���(skt001@163.com)
 * �޸����� : $Date: $
 * ��ǰ�汾 : $Revision: $
 * �������� : ��Ԫ����Ӧ�ó�����
 * �޸ļ�¼ : 
 *            $Log: $
 *
 * Ver  Date        Author  Comments
 * ---  ----------  ------  -------------------------------------------
 * 001	2014-4-26	�ۿ�������ļ�
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
// ��    ��:  �������񣬿�������Ӧ�õķ��񣬸ú���������������ʵ�֣�������������ɺ���뷵��
// ��    ��:  �ۿ���
// ����ʱ��:  2014:4:26 21:46
// ����˵��:  void
// �� �� ֵ:  true/false
//////////////////////////////////////////////////////////////////////////
bool CUnitMgrApplication::Start()
{
	SString sFile = GetConfPath()+"sys_unitconfig.xml";
	if(!m_UnitConfig.Load(sFile))
	{
		LOGFAULT("���ص�Ԫ�����ļ�(%s)ʱʧ��!",sFile.data());
		return false;
	}
	//��鵥ԪIP��ַ�����Ƿ���ȷ
	SString local_ips = SSocket::GetLocalIp().c_str();
	if(local_ips.length() == 0)
	{
		LOGERROR("���������쳣!�޷���ȡ����IP��ַ!");
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
		LOGERROR("��Ԫ�����쳣����ԪIP(%s)�������ڱ�����IP��ַ�б�(%s)!",m_UnitConfig.GetUnitIp().data(),local_ips.data());
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
		LOGDEBUG("����ͺű����в�������Ϊ:%s",m_sSoftwareType.data());
	}
	else if(m_sSoftwareType.length() == 0)
	{
		LOGWARN("δָ������ͺ�!������ͨ������-soft_type=XXX��ָ��! ");
	}

	m_MCastFounder.SetUnitConfig(&m_UnitConfig);
	if(!m_MCastFounder.Start())
	{
		LOGWARN("�����鲥�Է��ַ���ʧ��!");
	}

	m_Agent.SetMCastFounderPtr(&m_MCastFounder);
	m_Agent.m_SelfUnitInfo.m_iUnitIp[0] = SSocket::IpStrToIpv4(m_UnitConfig.m_sThisIp.data());
	if(!m_Agent.Start(m_iAgentPort))
	{
		LOGFAULT("��Ԫ����������ʧ��!");
		return false;
	}
	//���ݵ�Ԫ��ȷ�����ӷ�ʽ��������С�ĵ�Ԫ�����Ӵ�ĵ�Ԫ��
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
// ��    ��:  ֹͣ���񣬹ر�����Ӧ�õķ���
// ��    ��:  �ۿ���
// ����ʱ��:  2014:4:26 21:46
// ����˵��:  void
// �� �� ֵ:  true/false
//////////////////////////////////////////////////////////////////////////
bool CUnitMgrApplication::Stop()
{
	m_MCastFounder.Stop();
	return m_Agent.Stop();
}


//////////////////////////////////////////////////////////////////////////
// ��    ��:  �����ı�����
// ��    ��:  �ۿ���
// ����ʱ��:  2016-2-13 15:12
// ����˵��:  @sCmdΪ�������ݣ����ַ�����ʾȡ�������б�
//         :  @sResultΪ���ؽ�������ı�
// �� �� ֵ:  true��ʾ�ɹ���false��ʾʧ��
//////////////////////////////////////////////////////////////////////////
bool CUnitMgrApplication::ProcessTxtCmd(SString &sCmd,SString &sResult)
{
	if(sCmd.length() == 0)
	{
		sResult =	
			"cmd get_units                      ȡ���еĴ���Ԫ\r\n"
			"cmd get_procs                      ȡ����Ԫ���еĽ����б�\r\n"
			"cmd get_pid <appid> [inst_id=0]    ȡָ�����͵Ľ��̱�ʶ��Ϣ\r\n"
			"";
		return true;
	}
	SString act = SString::GetIdAttribute(1,sCmd," ");
	if(act == "get_units")
	{
		SPtrList<stuSpUnitAgentUnitInfo> units;
		m_Agent.GetUnitList(units);
		sResult += SString::toFormat("��%d������Ԫ\r\n",units.count());
		unsigned long pos = 0;
		stuSpUnitAgentUnitInfo *p = units.FetchFirst(pos);
		while(p)
		{
			sResult += SString::toFormat("%s  ��Ԫ��:%u  IP:%s  %s  %s\r\n",
				p->m_sSoftType,p->m_iUnitId,SSocket::Ipv4ToIpStr(p->m_iUnitIp[0]).c_str(),p->m_sUnitName,p->m_sUnitDesc);
			p = units.FetchNext(pos);
		}
	}
	else if(act == "get_procs")
	{
		SPtrList<stuSpUnitAgentProcInfo> ProcInfos;
		m_Agent.GetProcList(ProcInfos);
		sResult += SString::toFormat("��%d�������\r\n",ProcInfos.count());
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
		sResult += "��δʵ��\r\n";
	}
	else
		sResult += "��Ч������\r\n";
	return true;
}

////////////////////////////////////////////////////////////////////////
// ��    ��:  ������Ϣ����ӿڣ���������ʵ�֣����������뾡����С�����ٷ���
// ��    ��:  �ۿ���
// ����ʱ��:  2016-2-3 15:20
// ����˵��:  @wMsgType��ʾ��Ϣ����
//         :  @pMsgHeadΪ��Ϣͷ
//         :  @sHeadStr��Ϣͷ�ַ���
//         :  @pBufferΪ��ϢЯ���Ķ������������ݣ�NULL��ʾ�޶���������
//         :  @iLengthΪ���������ݳ���
// �� �� ֵ:  true��ʾ����ɹ���false��ʾ����ʧ��
//////////////////////////////////////////////////////////////////////////
#ifndef _WITHOUT_AGENT_CHANNEL_
bool CUnitMgrApplication::ProcessAgentMsg(WORD wMsgType,stuSpUnitAgentMsgHead *pMsgHead,SString &sHeadStr,BYTE* pBuffer/*=NULL*/,int iLength/*=0*/)
{
	//��Ӧ����ͷ
	stuSTcpPackage *pPackage = NULL;
	switch(wMsgType)
	{
	case SP_UA_MSG_REQ_GET_UNIT://ȡ��Ԫ��Ϣ
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
	case SP_UA_MSG_REQ_GET_PROCINFO://ȡ������Ϣ
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
	case SP_UA_MSG_REQ_GET_PROCID://ȡ���̱�ʶ
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
				if(pMsgHead->m_DstId.m_iUnitId == m_iUnitId)//��ȷ���Լ��Ĳŷ��ؿ�
					SendAgentMsg(&pMsgHead->m_SrcId,SP_UA_MSG_RES_GET_PROCID,pMsgHead->m_dwMsgSn,0);
			}
			else
				SendAgentMsg(&pMsgHead->m_SrcId,SP_UA_MSG_RES_GET_PROCID,pMsgHead->m_dwMsgSn,idx,(BYTE*)p,sizeof(stuSpUnitAgentProcessId)*idx);
			delete[] p;
			goto end;
		}
		break;
	case SP_UA_MSG_REQ_LS: //��Ŀ¼
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
	case SP_UA_MSG_REQ_GET_FILE: //ȡ�ļ�
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
				LOGWARN("��ȡ�ļ�ʧ��!file=%s",sPath.data());
				SendAgentMsg(&pMsgHead->m_SrcId,SP_UA_MSG_RES_GET_FILE,pMsgHead->m_dwMsgSn,0);
				break;
			}
			SString sResHeadStr;
			int file_size = file.size();
			if(file_size <= 0)
			{
				//ָ���ļ���СΪ0
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
					LOGWARN("��ȡ�ļ�����ʱʧ��!");
					break;
				}
				file_size -= size;
				if(file_size<=0)
					sn = 0;//���һ֡��0
				if(!SendAgentMsg(&pMsgHead->m_SrcId,SP_UA_MSG_RES_GET_FILE,pMsgHead->m_dwMsgSn,sn++,pBuf,size,&sResHeadStr))
					break;
				if(GetSendPackagePtr()->count() > 1000)
					SApi::UsSleep(10000);
			}
			break;
		}
	case SP_UA_MSG_REQ_PUT_FILE: //���ļ�
		{
			SString sFile = SString::GetAttributeValue(sHeadStr,"filename");
			int filesize = SString::GetAttributeValueI(sHeadStr,"filesize");//�ļ���С
			int pice = SString::GetAttributeValueI(sHeadStr,"pice");//��1��ʼ
			int cont = SString::GetAttributeValueI(sHeadStr,"cont");//1��ʾ�к���֡��0��ʾû�к���֡
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
					LOGWARN("���ļ�ʱ�ظ�����ɾ��ԭ�ļ�");
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
					LOGWARN("���ļ�ʱд��ʱ�ļ�ʧ��!file=%s.tmp",sFile.data());
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
					LOGWARN("���ļ�ʱ����δ�յ���Ч����֡����!");
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
					LOGWARN("���ļ�ʱд�ļ�����ʧ��!file=%s.tmp",sFile.data());
					SendAgentMsg(&pMsgHead->m_SrcId,SP_UA_MSG_RES_PUT_FILE,pMsgHead->m_dwMsgSn,103);
					m_PuttingParam.remove(pParam);
					break;
				}
			}
			if(cont == 0)
			{
				//û�к���֡�ˣ���֤�ļ���С
				pParam->file.close();
				if(pParam->file.size() != filesize)
				{
					SFile::remove(pParam->file.filename());
					LOGWARN("���ļ�ʱ���ļ���С��֤ʧ��!file=%s.tmp",sFile.data());
					SendAgentMsg(&pMsgHead->m_SrcId,SP_UA_MSG_RES_PUT_FILE,pMsgHead->m_dwMsgSn,104);
					m_PuttingParam.remove(pParam);
					break;
				}
				//�ļ���֤�ɹ�,������
				SFile::remove(pParam->sFileName);
				SFile::rename(pParam->sFileName+".tmp",pParam->sFileName);
				SendAgentMsg(&pMsgHead->m_SrcId,SP_UA_MSG_RES_PUT_FILE,pMsgHead->m_dwMsgSn,0);
				m_PuttingParam.remove(pParam);
			}
			break;
		}
	case SP_UA_MSG_REQ_DEL_FILE: //ɾ���ļ�
		{
			SString sFile = SString::GetAttributeValue(sHeadStr,"filename");
			SString sHome = m_pApp->GetHomePath();
			if(sHome.length() > 0)
				sHome = sHome.left(sHome.length()-1);
			sFile.replace("{@APP_PATH@}",sHome);

			if(!SFile::exists(sFile))
			{
				LOGWARN("��ɾ���ļ�������!file=%s",sFile.data());
				SendAgentMsg(&pMsgHead->m_SrcId,SP_UA_MSG_RES_DEL_FILE,pMsgHead->m_dwMsgSn,101);
				break;
			}
			if(!SFile::remove(sFile))
			{
				LOGWARN("ɾ���ļ�ʧ��!file=%s",sFile.data());
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
