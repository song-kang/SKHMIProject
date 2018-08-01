/**
 *
 * �� �� �� : AgentEx.cpp
 * �������� : 2016-2-2 11:30
 * ��    �� : �ۿ���(skt001@163.com)
 * �޸����� : $Date: $
 * ��ǰ�汾 : $Revision: $
 * �������� : ��Ԫ��������ࣨ�°汾��
 * �޸ļ�¼ : 
 *            $Log: $
 *
 * Ver  Date        Author  Comments
 * ---  ----------  ------  -------------------------------------------
 * 001	2016-2-2	�ۿ�������ļ�
 *
 **/
#include "AgentEx.h"

CSpAgentClientProx::CSpAgentClientProx()
{
	m_bAgentUplink = true;
	m_pUnitAgent = NULL;
}

CSpAgentClientProx::~CSpAgentClientProx()
{

}

//////////////////////////////////////////////////////////////////////////
// ��    ��:  �Ͽ�����˺�Ļص�
// ��    ��:  �ۿ���
// ����ʱ��:  2016-2-16 16:18
// ����˵��:  void
// �� �� ֵ:  void
//////////////////////////////////////////////////////////////////////////
void CSpAgentClientProx::OnClose()
{
#if 0
	register unsigned long pos = 0;
	register CSpUnitAgent::stuAgentSessionParam *p = m_pUnitAgent->m_AgentSessionParams.FetchFirst(pos);
	while(p)
	{
		if(p->pClient == this)
			break;
		p = m_pUnitAgent->m_AgentSessionParams.FetchNext(pos);
	}
	if(p == NULL)
	{
		Stop();
		LOGERROR("��Ч�ļ����Ự!");
		return ;
	}
	m_pUnitAgent->m_AgentSessionParams.remove(p);
	m_pUnitAgent->m_RemoveAgentSessionParams.append(p);
	if(m_pUnitAgent->m_RemoveAgentSessionParams.count() > 3)
		m_pUnitAgent->m_RemoveAgentSessionParams.remove(0);
#endif
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
bool CSpAgentClientProx::ProcessAgentMsg(WORD wMsgType,stuSpUnitAgentMsgHead *pMsgHead,SString &sHeadStr,BYTE* pBuffer/*=NULL*/,int iLength/*=0*/)
{
	//�����������Ϣ��ֻ��Ҫ������Ҫƥ��Ľ��̣�����Ҫת������������
	stuSTcpPackage *pPackage = PackageBuild(wMsgType,pMsgHead,pBuffer,iLength);
	pPackage->m_sHead = sHeadStr;
	m_pUnitAgent->ProcessAgentMsg(true,pPackage);
	delete pPackage;
	return true;
}

CSpUnitAgent::CSpUnitAgent()
{
	//�����ַ�
	SetStartWord(0xeb57);
	m_SessionParams.setAutoDelete(false);
	m_SessionParams.setShared(true);
	m_RemoveSessionParams.setAutoDelete(true);
	m_RemoveSessionParams.setShared(true);
	m_AgentSessionParams.setShared(true);
	m_AgentSessionParams.setAutoDelete(false);
	m_RemoveAgentSessionParams.setAutoDelete(true);
	m_RemoveAgentSessionParams.setShared(true);
	m_AgentClients.setAutoDelete(false);
	m_AgentClients.setShared(true);
	m_pMCastFounder = NULL;
	m_pApp = NULL;
	memset(&m_SelfUnitInfo,0,sizeof(m_SelfUnitInfo));
}

CSpUnitAgent::~CSpUnitAgent()
{
	m_AgentClients.setAutoDelete(true);
	m_SessionParams.setAutoDelete(true);
	m_AgentSessionParams.setAutoDelete(true);
}

////////////////////////////////////////////////////////////////////////
// ��    ��:  ��������
// ��    ��:  �ۿ���
// ����ʱ��:  2016-2-3 11:13
// ����˵��:  @port��ʾ�˿�,@ip��ʾ�����IP��ַ
// �� �� ֵ:  true��ʾ���������ɹ�,false��ʾ��������ʧ��
//////////////////////////////////////////////////////////////////////////
bool CSpUnitAgent::Start(int port,SString ip/*=""*/)
{
	m_pApp = SApplication::GetPtr();

// 	if(m_pApp->m_iUnitId == 0)
// 	{
// 		SString ips = SSocket::GetLocalIp().c_str();
// 		int i,cnt = SString::GetAttributeCount(ips,",");
// 		for(i=1;i<=cnt;i++)
// 		{
// 			m_pApp->m_iUnitId ^= SSocket::IpStrToIpv4(SString::GetIdAttribute(i,ips,",").data());
// 		}
// 	}
	

	m_SelfUnitInfo.m_iUnitId = m_pApp->m_iUnitId;
	strcpy(m_SelfUnitInfo.m_sSoftType,m_pApp->m_sSoftwareType.data());
	strcpy(m_SelfUnitInfo.m_sUnitName,m_pApp->m_sUnitName.data());
	strcpy(m_SelfUnitInfo.m_sUnitDesc,m_pApp->m_sUnitDesc.data());
	if(!STcpServerBase::Start(port,ip))
	{
		LOGERROR("�����������ʱʧ�ܣ�");
		return false;
	}


// 	if(m_pMCastFounder != NULL)
// 	{
// 		memcpy(m_SelfUnitInfo.m_iUnitIp,m_pMCastFounder->GetSelfIps(),sizeof(m_SelfUnitInfo.m_iUnitIp));
// 		//SKT_CREATE_THREAD(ThreadAgentComm,this);
// 	}
	return true;
}

//////////////////////////////////////////////////////////////////////////
// ��    ��:  ȡ���еĴ���Ԫ�б�
// ��    ��:  �ۿ���
// ����ʱ��:  2016-2-12 9:10
// ����˵��:  @UnitInfos��ʾ���÷��صĵ�Ԫ��Ϣ�б�
// �� �� ֵ:  ����Ԫ����
//////////////////////////////////////////////////////////////////////////
int CSpUnitAgent::GetUnitList(SPtrList<stuSpUnitAgentUnitInfo> &UnitInfos)
{
	unsigned long pos;
	UnitInfos.clear();
	UnitInfos.setAutoDelete(false);
	UnitInfos.append(&m_SelfUnitInfo);
	stuAgentSessionParam *pAgent = m_AgentSessionParams.FetchFirst(pos);
	while(pAgent)
	{
		UnitInfos.append((stuSpUnitAgentUnitInfo*)&pAgent->m_UnitInfo);
		pAgent = m_AgentSessionParams.FetchNext(pos);
	}
	return UnitInfos.count();
}


//////////////////////////////////////////////////////////////////////////
// ��    ��:  ȡ��ǰ��Ԫ������Ӧ���б�
// ��    ��:  �ۿ���
// ����ʱ��:  2016-2-12 9:54
// ����˵��:  @ProcInfos��ʾӦ�ý��̵��б����÷��أ�
// �� �� ֵ:  Ӧ������
//////////////////////////////////////////////////////////////////////////
int CSpUnitAgent::GetProcList(SPtrList<stuSpUnitAgentProcInfo> &ProcInfos)
{
	ProcInfos.clear();
	ProcInfos.setAutoDelete(false);
	//ProcInfos.append(&m_pApp->m_SelfProcId);
	unsigned long pos=0;
	stuSessionParam *pSession = m_SessionParams.FetchFirst(pos);
	while(pSession)
	{
		ProcInfos.append(&pSession->ProcInfo);
		pSession = m_SessionParams.FetchNext(pos);
	}
	return ProcInfos.count();
}

//////////////////////////////////////////////////////////////////////////
// ��    ��:  �µ����ӳ�ʱ���󣬿ɽ�SOCKET����������;
// ��    ��:  �ۿ���
// ����ʱ��:  2016-2-5 17:17
// ����˵��:  @pSocketΪSOCKETָ��
// �� �� ֵ:  true��ʾSOCKET���ɹ��йܣ�Ӧ�ӷ����з��룬������Ҫ������ά����SOCKET����false��ʾ���йܣ�Ӧ�����ͷŸ�����
//////////////////////////////////////////////////////////////////////////
bool CSpUnitAgent::OnNewTimeoutSocket(SSocket* &pSocket)
{
	char ch[8];
	pSocket->SetTimeout(2000,1000);
	char *pBuf = ch;
	int ret;
	while(1)
	{
		SApi::UsSleep(1000);
		ret = pSocket->Receive(pBuf,4);
		if(ret >= 1)
		{
			pBuf += ret;
			if(ch[0] == '\r' && ch[1] == '\n' && ch[2] == '\r' && ch[3] == '\n')
			{
				LOGDEBUG("�õ��µ�TELNET����(%s:%d)!",pSocket->GetPeerIp().data(),pSocket->GetPeerPort());
				//Start telnet service
				stuTelnetParam *p = new stuTelnetParam();
				p->pAgent = this;
				p->pSocket = pSocket;
				S_CREATE_THREAD(ThreadTelnet,p);
				pSocket = NULL;
				return true;
			}
		}
		else
			break;
	}
	return false;
}

//////////////////////////////////////////////////////////////////////////
// ��    ��:  TELNET�����߳�
// ��    ��:  �ۿ���
// ����ʱ��:  2016-2-5 17:26
// ����˵��:  @lpΪthis
// �� �� ֵ:  NULL
//////////////////////////////////////////////////////////////////////////
void* CSpUnitAgent::ThreadTelnet(void *lp)
{
#ifndef WIN32
	pthread_detach(pthread_self());
#endif
	stuTelnetParam *p = (stuTelnetParam*)lp;
	CSpUnitAgent *pThis = p->pAgent;
	SSocket *pSocket = p->pSocket;
	int ret;
	SString cmd,sRet,last_cmd;
	char ch;
	int select_pid=0;//��ǰѡ�еĽ��̣�0��ʾ��ѡ��

	cmd = "welcome";
	sRet = pThis->ProcessTelnetCmd(select_pid,cmd);
	cmd = "";
	pSocket->Send(sRet.data(),sRet.length());
	pSocket->SetTimeout(100,2000);
	while(!pThis->IsQuit())
	{
		ret = pSocket->Receive(&ch,1);
		if(ret < 0)
			break;
		if(ret == 0)
		{
			SApi::UsSleep(100000);
			continue;
		}
		if(ch == 8)
		{
			//�˸��
			if(cmd.length()>0)
			{
				pSocket->Send((void*)" ",1);
				pSocket->Send((void*)&ch,1);

				cmd = cmd.left(cmd.length()-1);
			}
			else
			{
				pSocket->Send((void*)" ",1);
			}
			continue;
		}
		if(ch == '\n')
			continue;
		if(ch == '\r')
		{
			if(cmd == "quit")
			{
				sRet = "�ټ�!��ӭ�´�����!\r\n";
				ret = pSocket->Send(sRet.data(),sRet.length());
				SApi::UsSleep(100000);
				break;
			}
			if(cmd.length() == 3 && cmd.at(0) == 0x1b && cmd.at(1) == 0x5b && cmd.at(2) == 0x41)
			{
				//���ϼ�
				cmd = last_cmd;
				sRet = cmd + "\r\n";
				pSocket->Send(sRet.data(),sRet.length());
			}
			if(cmd.length() > 0)
				last_cmd = cmd;
			sRet = pThis->ProcessTelnetCmd(select_pid,cmd);
			cmd = "";
			ret = pSocket->Send(sRet.data(),sRet.length());
			if(ret != sRet.length())
				break;
		}
		else
		{
			cmd += ch;
		}

	}
	LOGDEBUG("�˳�TELNET����!");
	pSocket->Close();
	delete pSocket;
	delete p;
	return NULL;
}


////////////////////////////////////////////////////////////////////////
// ��    ��:  ����¼��ת��Ϊ�����ı�
// ��    ��:  �ۿ���
// ����ʱ��:  2015-3-31 14:24
// ����˵��:  @rsΪ��¼��
//         :  @sTextΪ����ı�
// �� �� ֵ:  void
//////////////////////////////////////////////////////////////////////////
void CSpUnitAgent::RecordsetToText(SRecordset &rs,SString &sText)
{
	int row,rows = rs.GetRows();
	int col,cols = rs.GetColumns();
	if(cols <= 0)
		return;
	SString text;
	int len;
	int *col_len = new int[cols];
	for(col=0;col<cols;col++)
	{
		col_len[col] = rs.GetColumnName(col).length();
	}
	//��ɨ��ȫ�����ݣ�ȷ���������ݳ���
	for(row=0;row<rows;row++)
	{
		for(col=0;col<cols;col++)
		{
			len = rs.GetValue(row,col).length();
			if(len > col_len[col])
				col_len[col] = len;
		}
	}

	//���ɱ�ͷ
	for(col=0;col<cols;col++)
	{
		sText += SString::toFillString(rs.GetColumnName(col),col_len[col],' ',false);
		sText += " ";
	}
	sText += "\r\n";
	for(col=0;col<cols;col++)
	{
		sText += SString::toFillString("-",col_len[col],'-',false);
		sText += " ";
	}
	sText += "\r\n";

	for(row=0;row<rows;row++)
	{
		for(col=0;col<cols;col++)
		{
			sText += SString::toFillString(rs.GetValue(row,col),col_len[col],' ',false);
			sText += " ";
		}
		sText += "\r\n";
	}
	for(col=0;col<cols;col++)
	{
		sText += SString::toFillString("-",col_len[col],'-',false);
		sText += " ";
	}
	sText += "\r\n";
	delete[] col_len;
}

SString CSpUnitAgent::ProcessTelnetCmd(int &select_pid,SString &cmd)
{
	SString sRet = "";
	SString sCmdAll,sTemp;
	sCmdAll = cmd;
	cmd = SString::GetIdAttribute(1,cmd," ");

	stuSessionParam *pSessionParam = NULL;
	if(select_pid != 0)
	{
		pSessionParam = SearchParamByPid(select_pid);
		if(pSessionParam == NULL)
		{
			sTemp.sprintf("����(%d)�Ѿ��˳�, �Զ��˳���ǰѡ�����!\r\n",select_pid);
			sRet = sTemp;
			select_pid  = 0;
		}
	}
	if(cmd == "welcome")
	{
		sRet += "��ӭʹ��SDP/SSP����TELNET�ͻ��˳���\r\n�Ͼ����������Ƽ����޹�˾\r\n�汾��:V"+m_pApp->GetModuleVer()+"\r\n��¼ʱ��:"+SDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss")+"\r\n";
		sRet += "������'help'�鿴������������!\r\n";
	}
	else if(cmd == "help")
	{
		if(pSessionParam == NULL)
		{
			sRet += "--------------------------------------------------------------\r\n";
			sRet += "SDP/SSP����TELNET�ͻ��˳���(V"+m_pApp->GetModuleVer()+")������Ϣ\r\n";
			sRet += "--------------------------------------------------------------\r\n";
			sRet += "help                 ��ʾ������Ϣ\r\n";
			sRet += "ps                   ��ʾ���е�¼�Ľ����б�\r\n";
			sRet += "reboot               ����������ǰ��Ԫ����\r\n";
			sRet += "halt                 �رյ�ǰ��Ԫ����\r\n";
			//sRet += "telnet_reset         �������telnetd����\r\n";
			sRet += "select <PID>         ѡ��ָ���Ľ���\r\n";
			sRet += "quit                 �˳�\r\n";
		}
		else
		{
			sRet += "--------------------------------------------------------------\r\n";
			sRet += "SDP/SSP����TELNET�ͻ��˳���(V"+m_pApp->GetModuleVer()+")������Ϣ\r\n";
			sRet += "--------------------------------------------------------------\r\n";
			sRet += "help                 ��ʾ������Ϣ\r\n";
			sRet += "ps                   ��ʾ���е�¼�Ľ����б�\r\n";
			sRet += "reboot               ����������ǰ��Ԫ����\r\n";
			sRet += "halt                 �رյ�ǰ��Ԫ����\r\n";
			//sRet += "telnet_reset         �������telnetd����\r\n";
			sRet += "select <PID>         ѡ��ָ���Ľ���\r\n";
			sRet += "exit                 �˳�ѡ��Ľ���\r\n";
			sRet += "--------------------------------------------------------------\r\n";
			sRet += "cmd [parameter]      ִ��ָ���������ڲ�����(����Ϊ��ʱ��ʾ�ڲ������б�)\r\n";
			sRet += "restart              ����������ǰ���̵ķ���\r\n";
			sRet += "stop                 �˳���ǰ���̵ķ���\r\n";
			sRet += "--------------------------------------------------------------\r\n";
			sRet += "log level none       �ر���־����                             \r\n";
			sRet += "log level fault      ������־����Ϊ���ع��ϼ�                 \r\n";
			sRet += "log level error      ������־����Ϊһ�����                 \r\n";
			sRet += "log level warn       ������־����Ϊ���漶                     \r\n";
			sRet += "log level debug      ������־����Ϊ���Լ�                     \r\n";
			sRet += "log level basedebug  ������־����Ϊ������Լ�                 \r\n";
			sRet += "--------------------------------------------------------------\r\n";
			sRet += "log to console on    �����־������̨                         \r\n";
			sRet += "log to console off   ֹͣ�����־������̨                     \r\n";
			sRet += "log to file <path>   �����־��ָ���ļ�                       \r\n";
			sRet += "log to file off      ֹͣ��־���ļ�                           \r\n";
			sRet += "log file size <n>    ָ����־�ļ���󳤶�                     \r\n";
			sRet += "log to TCP <ip:n>    �����־��ָ��IP�Ͷ˿ڵ�TCP������        \r\n";
			sRet += "log to TCP off       ֹͣ��־��TCP������                      \r\n";
			sRet += "log to UDP <ip:n>    �����־��ָ��IP�Ͷ˿ڵ�UDP������        \r\n";
			sRet += "log to UDP off       ֹͣ��־��UDP������                      \r\n";
			sRet += "--------------------------------------------------------------\r\n";
			sRet += "quit                 �˳�\r\n";
		}
	}
	else if(cmd == "ps")
	{
		SRecordset rs;
		rs.SetSize(m_SessionParams.count(),11);
		rs.SetColumnName(0,"PID");
		rs.SetColumnName(1,"CPU%");
		rs.SetColumnName(2,"ģ����");
		rs.SetColumnName(3,"�汾��");
		rs.SetColumnName(4,"CRC");
		rs.SetColumnName(5,"����ʱ��");
		rs.SetColumnName(6,"����");
		rs.SetColumnName(7,"��¼IP");
		rs.SetColumnName(8,"��¼�˿�");
		rs.SetColumnName(9,"����ʱ��");
		rs.SetColumnName(10,"��¼ʱ��");
		unsigned long pos = 0;
		int iRow = 0;
		int cpu=0;
		stuSessionParam *pSession = m_SessionParams.FetchFirst(pos);
		while(pSession)
		{
			rs.SetValue(iRow,0, SString::toString((int)pSession->ProcInfo.pid));
			rs.SetValue(iRow,1, SString::toString(cpu));
			rs.SetValue(iRow,2, pSession->ProcInfo.name);
			rs.SetValue(iRow,3, pSession->ProcInfo.version);
			rs.SetValue(iRow,4, pSession->ProcInfo.crc);
			rs.SetValue(iRow,5, pSession->ProcInfo.build_time);
			rs.SetValue(iRow,6, pSession->ProcInfo.desc);
			rs.SetValue(iRow,7, pSession->ProcInfo.login_ip);
			rs.SetValue(iRow,8, SString::toString((int)pSession->ProcInfo.login_port));
			rs.SetValue(iRow,9, SDateTime::makeDateTime((time_t)pSession->ProcInfo.begin_time).toString("yyyy-MM-dd hh:mm:ss"));
			rs.SetValue(iRow,10,SDateTime::makeDateTime((time_t)pSession->ProcInfo.login_time).toString("yyyy-MM-dd hh:mm:ss"));

			iRow++;
			pSession = m_SessionParams.FetchNext(pos);
		}
		SString sTemp;
		RecordsetToText(rs,sTemp);
		sRet += sTemp;
		sRet += SString::toFormat("��%d������.\r\n",m_SessionParams.count());
	}
	else if(cmd == "reboot")
	{
		SApi::SystemReboot();
	}
	else if(cmd == "halt")
	{
		SApi::SystemHalt();
	}
	// 	else if(cmd == "telnet_reset")
	// 	{
	// 		::system("killall telnetd");
	// 	}
	else if(cmd == "select")
	{
		int pid = SString::GetIdAttributeI(2,sCmdAll," ");
		stuSessionParam *pSessionParam2 = SearchParamByPid(pid);
		if(pSessionParam2 == NULL)
		{
			sTemp.sprintf("��Ч��PID=%d, ѡ��ʧ��!\r\n",pid);
			sRet += sTemp;
		}
		else
		{
			select_pid = pid;
			pSessionParam = pSessionParam2;
			sTemp.sprintf("ѡ�����(%d:%s,V%s,%s)�ɹ�!\r\n",pSessionParam->ProcInfo.pid,pSessionParam->ProcInfo.name,pSessionParam->ProcInfo.version,pSessionParam->ProcInfo.desc);
			sRet += sTemp;
		}
	}
	else if(cmd == "exit")
	{
		if(select_pid == 0)
		{
			sTemp.sprintf("δѡ�����, ��ǰ�������!\r\n");
			sRet += sTemp;
		}
		else
		{
			select_pid = 0;
			pSessionParam = NULL;
			sRet += "";
		}
	}
	else if(cmd == "info")
	{
		pSessionParam = SearchParamByPid(select_pid);
		if(pSessionParam == NULL)
		{
			select_pid = 0;
		}
// 		if(select_pid == 0)
// 		{
// 			//���Լ����ʹ�����
// 			sTemp = "act=info;";
// 			g_app.ProcessAgentMsg(0,sTemp);
// 			sRet += g_app.GetInfo()+"\r\n";
// 		}
// 		else
// 		{
// 			sRet += pThisSession->GetInfo()+"\r\n";
// 		}
	}
	else if(cmd == "restart")
	{
		pSessionParam = SearchParamByPid(select_pid);
		if(pSessionParam == NULL)
		{
			select_pid = 0;
		}
		if(select_pid == 0)
		{
			sTemp.sprintf("δѡ�����, ��ǰ�������!\r\n");
			sRet += sTemp;
		}
		else
		{		
// 			SString sMsg="act=restart;";
// 			pThisSession->AddSendPackage(sMsg);
		}
	}
	else if(cmd == "stop")
	{
		pSessionParam = SearchParamByPid(select_pid);
		if(pSessionParam == NULL)
		{
			select_pid = 0;
		}
		if(select_pid == 0)
		{
			sTemp.sprintf("δѡ�����, ��ǰ�������!\r\n");
			sRet += sTemp;
		}
		else
		{
			SApplication::GetPtr()->SendAgentMsg(&pSessionParam->ProcInfo.process_id,SP_UA_MSG_APP_STOP,SApplication::NewMsgSn());
// 			SString sMsg="act=stop;";
// 			pThisSession->AddSendPackage(sMsg);
		}
	}
	else if(cmd == "log")
	{
		pSessionParam = SearchParamByPid(select_pid);
		if(pSessionParam == NULL)
		{
			select_pid = 0;
		}
		if(select_pid == 0)
		{
			//���Լ����ʹ�����
			SApplication::ParseCommandLine(sCmdAll);
		}
		else
		{
// 			SString sMsg="act=log;cmd="+sCmdAll+";";
// 			pThisSession->AddSendPackage(sMsg);
		}
	}
	else if(cmd == "")
	{
		sRet += "";
	}
	else if(cmd == "cmd")
	{
		//��ʾ�����б���������
		//ִ���ڲ����������
		if(select_pid == 0)
		{
			sRet += "����ѡ��������Ľ���!\r\n";
		}
		else
		{
			stuSTcpPackage *pRecvPackage=NULL;
			SString sHead;
			stuSpUnitAgentMsgHead msghead;
			memcpy(&msghead.m_DstId, &pSessionParam->ProcInfo.process_id,sizeof(msghead.m_DstId));
			memcpy(&msghead.m_SrcId, &m_pApp->m_SelfProcId.process_id,sizeof(msghead.m_SrcId));
			msghead.m_dwMsgSn = m_pApp->NewMsgSn();
			msghead.m_dwParamReault = 0;
			m_pApp->SendAgentMsgAndRecv(&pSessionParam->ProcInfo.process_id,SP_UA_MSG_PROC_TXT_CMD_REQ,m_pApp->NewMsgSn(),0,(BYTE*)sCmdAll.data(),sCmdAll.length()+1,NULL,SP_UA_MSG_PROC_TXT_CMD_ACK,pRecvPackage,10);			
			if(pRecvPackage != NULL && pRecvPackage->m_wFrameType == SP_UA_MSG_PROC_TXT_CMD_ACK && pRecvPackage->m_pAsduBuffer != NULL && pRecvPackage->m_iAsduLen>0)
			{
				pRecvPackage->m_pAsduBuffer[pRecvPackage->m_iAsduLen-1]='\0';
				sRet += (char*)(pRecvPackage->m_pAsduBuffer+sizeof(stuSpUnitAgentMsgHead));
			}
			if(pRecvPackage != NULL)
				delete pRecvPackage;
		}
	}
	else  if(cmd == "system")
	{
		cmd = sCmdAll.mid(7);
		SString line = cmd + " > .line.txt";
		::system(line.data());
		SFile f(".line.txt");
		if(!f.open(IO_ReadOnly))
		{
			sRet += "[" + cmd+"] command not found!\r\n";
		}
		else
		{
			sRet += "";
			SString sLine;
			f.seekBegin();
			while(!f.atEnd())
			{
				sLine = f.readLine();
				if(sLine.length() == 0)
					break;
				sRet += sLine;
			}
			f.close();

			if(sRet.length() == 0)
			{
				sRet += "[" + cmd+"] command not found! Input 'help' for more infomation!\r\n";
			}
		}
		f.remove();
	}
	pSessionParam = SearchParamByPid(select_pid);
	if(pSessionParam == NULL)
	{
		select_pid = 0;
		sRet += "[uk-agent]# ";
	}
	else
	{
		sTemp.sprintf("[%d:%s@uk-agent]# ",pSessionParam->ProcInfo.pid,pSessionParam->ProcInfo.name);
		sRet += sTemp;
	}
	return sRet;
}

//////////////////////////////////////////////////////////////////////////
// ��    ��:  ���ӵ�ָ����������Ԫ
// ��    ��:  �ۿ���
// ����ʱ��:  2016-2-23 17:21
// ����˵��:  @pUnit
// �� �� ֵ:  void
//////////////////////////////////////////////////////////////////////////
void CSpUnitAgent::ConnectAgent(CSpUnit *pUnit)
{
	stuAgentSessionParam *pAgentParam = new stuAgentSessionParam();
	pAgentParam->pClient = new CSpAgentClientProx();
	pAgentParam->pClient->m_pUnitAgent = this;
	memcpy(&pAgentParam->pClient->m_SelfProcId,&m_pApp->m_SelfProcId,sizeof(m_pApp->m_SelfProcId));
	pAgentParam->pSession = NULL;
	memset(&pAgentParam->m_UnitInfo,0,sizeof(pAgentParam->m_UnitInfo));
	pAgentParam->m_UnitInfo.unit_id = pUnit->m_iUnitId;
	SString::strncpy(pAgentParam->m_UnitInfo.soft_type,sizeof(pAgentParam->m_UnitInfo.soft_type), m_pApp->m_sSoftwareType.data());
	SString::strncpy(pAgentParam->m_UnitInfo.unit_name,sizeof(pAgentParam->m_UnitInfo.unit_name), pUnit->m_sUnitName.data());
	SString::strncpy(pAgentParam->m_UnitInfo.unit_desc,sizeof(pAgentParam->m_UnitInfo.unit_desc), pUnit->m_sUnitDesc.data());
	pAgentParam->m_UnitInfo.unit_ip[0] = SSocket::IpStrToIpv4(pUnit->m_sUnitCommIp.data());
	m_AgentSessionParams.append(pAgentParam);
	pAgentParam->pClient->SetAppPtr(SApplication::GetPtr());
	pAgentParam->pClient->Start(pUnit->m_sUnitCommIp,C_SP_UNITAGENT_PORT);
}

//////////////////////////////////////////////////////////////////////////
// ��    ��:  �����ͨѶ�ػ����̣���ʱ����δ���ӵĴ�������������
// ��    ��:  �ۿ���
// ����ʱ��:  2016-2-3 11:18
// ����˵��:  @lpΪthis
// �� �� ֵ:  NULL
//////////////////////////////////////////////////////////////////////////
void* CSpUnitAgent::ThreadAgentComm(void* lp)
{
	CSpUnitAgent *pThis = (CSpUnitAgent*)lp;
	pThis->BeginThread();
	SPtrList<stuMCastUnitInfo> units;
	stuMCastUnitInfo *pUnitInfo;
	int i,cnt;
	unsigned long pos=0;
	stuAgentSessionParam *pAgentParam;
	SString sIp;
	int ms,ttl;
	units.setAutoDelete(true);
	while(!pThis->IsQuit())
	{
		cnt = pThis->m_pMCastFounder->FindUnit(units);
		if(cnt <= 0)
		{
			for(i=0;!pThis->IsQuit()&&i<5;i++)
				SApi::UsSleep(1000000);
			continue;
		}
		pUnitInfo = units.FetchFirst(pos);		
		while(pUnitInfo)
		{
			//������Ǳ�ϵͳ��Ӧ������ͺ������
			if(SString::equals(pThis->m_pApp->m_sSoftwareType.data(),pUnitInfo->soft_type) == false)
			{
				pUnitInfo = units.FetchNext(pos);
				continue;
			}
			pAgentParam = NULL;
			for(i=0;i<8;i++)
			{
				if(pUnitInfo->unit_ip[i] == 0)
					break;
				pAgentParam = pThis->SearchAgentParamByIp(pUnitInfo->unit_ip[i]);
				if(pAgentParam != NULL)
					break;
			}
			if(pAgentParam != NULL)
			{
				//�Ѿ������ڵ�ǰ���������У�����
				pUnitInfo = units.FetchNext(pos);
				continue;
			}
			//ͨ��CLIENT���Ӵ���
			for(i=0;i<8;i++)
			{
				if(pUnitInfo->unit_ip[i] == 0)
					break;
				sIp = SSocket::Ipv4ToIpStr(pUnitInfo->unit_ip[i]);
				if(SSocket::Ping(sIp.data(),ms,ttl) == 0)
				{
					//PING��ͨ�����øõ�ַ����
					pThis->m_AgentSessionParams.lock();
					pAgentParam = pThis->SearchAgentParamByIp(pUnitInfo->unit_ip[i]);
					if(pAgentParam != NULL)
					{
						//��Ȼ���ҵ��ˣ������ǶԷ���������������������ǰ����
						pThis->m_AgentSessionParams.unlock();
						break;
					}
					pAgentParam = new stuAgentSessionParam();
					pAgentParam->pClient = new CSpAgentClientProx();
					pAgentParam->pClient->m_pUnitAgent = pThis;
					memcpy(&pAgentParam->pClient->m_SelfProcId,&pThis->m_pApp->m_SelfProcId,sizeof(pThis->m_pApp->m_SelfProcId));
					pAgentParam->pSession = NULL;
					memcpy(&pAgentParam->m_UnitInfo,pUnitInfo,sizeof(pAgentParam->m_UnitInfo));
					pThis->m_AgentSessionParams.append(pAgentParam,false);
					pThis->m_AgentSessionParams.unlock();
					pAgentParam->pClient->SetAppPtr(SApplication::GetPtr());
					pAgentParam->pClient->Start(sIp,C_SP_UNITAGENT_PORT);
					break;
				}
			}
			pUnitInfo = units.FetchNext(pos);
		}		

		for(i=0;!pThis->IsQuit()&&i<10;i++)
			SApi::UsSleep(500000);
	}
	pThis->EndThread();
	return NULL;
}

//////////////////////////////////////////////////////////////////////////
// ��    ��:  ����Ӧ�úż�ʵ���ż�����Ӧ�Ĳ���ָ��
// ��    ��:  �ۿ���
// ����ʱ��:  2016-2-3 19:11
// ����˵��:  @appid,instid
// �� �� ֵ:  stuSessionParam*
//////////////////////////////////////////////////////////////////////////
CSpUnitAgent::stuSessionParam* CSpUnitAgent::SearchParamByAppId(WORD appid,WORD instid/*=0*/)
{
	register unsigned long pos = 0;
	register stuSessionParam *p = m_SessionParams.FetchFirst(pos);
	while(p)
	{
		if(p->ProcInfo.process_id.GetApplicationId() == appid && p->ProcInfo.process_id.GetAppInstId() == instid)
			return p;
		p = m_SessionParams.FetchNext(pos);
	}
	return NULL;
}

//////////////////////////////////////////////////////////////////////////
// ��    ��:  ���ݽ��̺ż�����Ӧ�Ĳ���ָ��
// ��    ��:  �ۿ���
// ����ʱ��:  2016-2-3 19:11
// ����˵��:  @appid,instid
// �� �� ֵ:  stuSessionParam*
//////////////////////////////////////////////////////////////////////////
CSpUnitAgent::stuSessionParam* CSpUnitAgent::SearchParamByPid(int pid)
{
	register unsigned long pos = 0;
	register stuSessionParam *p = m_SessionParams.FetchFirst(pos);
	while(p)
	{
		if(p->ProcInfo.pid == pid)
			return p;
		p = m_SessionParams.FetchNext(pos);
	}
	return NULL;
}

//////////////////////////////////////////////////////////////////////////
// ��    ��:  ���ݵ�Ԫ��ż�����Ӧ�Ĵ������ָ��
// ��    ��:  �ۿ���
// ����ʱ��:  2016-2-3 18:45
// ����˵��:  @unitid
// �� �� ֵ:  stuAgentSessionParam*
//////////////////////////////////////////////////////////////////////////
CSpUnitAgent::stuAgentSessionParam* CSpUnitAgent::SearchAgentParamByUnitId(DWORD unitid)
{
	register unsigned long pos = 0;
	register stuAgentSessionParam *p = m_AgentSessionParams.FetchFirst(pos);
	while(p)
	{
		if(p->m_UnitInfo.unit_id == unitid)
			return p;
		p = m_AgentSessionParams.FetchNext(pos);
	}
	return NULL;
}

//////////////////////////////////////////////////////////////////////////
// ��    ��:  ����IP��ַ������Ӧ�Ĵ������ָ��
// ��    ��:  �ۿ���
// ����ʱ��:  2016-2-3 13:16
// ����˵��:  @ip
// �� �� ֵ:  stuAgentSessionParam*
//////////////////////////////////////////////////////////////////////////
CSpUnitAgent::stuAgentSessionParam* CSpUnitAgent::SearchAgentParamByIp(DWORD ip)
{
	register unsigned long pos = 0;
	register stuAgentSessionParam *p = m_AgentSessionParams.FetchFirst(pos);
	while(p)
	{
		for(int i=0;i<8;i++)
		{
			if(p->m_UnitInfo.unit_ip[i] == 0)
				break;
			if(p->m_UnitInfo.unit_ip[i] == ip)
				return p;
		}
		p = m_AgentSessionParams.FetchNext(pos);
	}
	return NULL;
}


//////////////////////////////////////////////////////////////////////////
// ��    ��:  ���ݻỰ������Ӧ�Ĵ������ָ�� 
// ��    ��:  �ۿ���
// ����ʱ��:  2016-2-3 13:19
// ����˵��:  @pSession
// �� �� ֵ:  stuAgentSessionParam*
//////////////////////////////////////////////////////////////////////////
CSpUnitAgent::stuAgentSessionParam* CSpUnitAgent::SearchAgentParamBySession(STcpServerSession *pSession)
{
	register unsigned long pos = 0;
	register stuAgentSessionParam *p = m_AgentSessionParams.FetchFirst(pos);
	while(p)
	{
		if(p->pSession == pSession)
			return p;
		p = m_AgentSessionParams.FetchNext(pos);
	}
	return NULL;
}

////////////////////////////////////////////////////////////////////////
// ��    ��:  ��¼�ص��麯��
// ��    ��:  �ۿ���
// ����ʱ��:  2016-2-2 16:44
// ����˵��:  @ip��¼�ͻ��˵�IP
//            @port��¼�ͻ��˵Ķ˿�
//            @sLoginHead��¼�ַ���
// �� �� ֵ:  true��ʾ�����¼,false��ʾ�ܾ���¼
//////////////////////////////////////////////////////////////////////////
bool CSpUnitAgent::OnLogin(SString ip,int port,SString &sLoginHead)
{
	//�ͻ����Ƿ��ǺϷ������
	SString sSoftType = SString::GetAttributeValue(sLoginHead,"soft_type");
	if(sSoftType != m_pApp->m_sSoftwareType)
		return false;
	return true;
}


//////////////////////////////////////////////////////////////////////////
// ��    ��:  �»Ự�ص��麯��
// ��    ��:  �ۿ���
// ����ʱ��:  2016-2-2 16:48
// ����˵��:  @pSessionΪ�´����ĻỰʵ��
// �� �� ֵ:  void
//////////////////////////////////////////////////////////////////////////
void CSpUnitAgent::OnNewSession(STcpServerSession *pSession)
{
	pSession->GetSessionSock()->SetTimeout(1000,100);
	stuSTcpPackage *pPackage=NULL;
	int ret = pSession->RecvFrame(pSession->GetSessionSock(),pPackage);
	if(ret > 0 && pPackage != NULL)
	{
		if(pPackage->m_wFrameType == SP_UA_MSG_REGISTER && pPackage->m_iAsduLen == sizeof(stuSpUnitAgentProcInfo))
		{
			//��ͨ���̵���
			stuSessionParam *pParam = new stuSessionParam();
			memcpy(&pParam->ProcInfo,pPackage->m_pAsduBuffer,sizeof(stuSpUnitAgentProcInfo)-32);//�����Ƶ�¼��Ϣ
			strcpy(pParam->ProcInfo.login_ip, pSession->GetPeerIp().data());
			pParam->ProcInfo.login_port = (DWORD)pSession->GetPeerPort();
			pParam->ProcInfo.login_time = (DWORD)SDateTime::getNowSoc();
			pParam->pSession = pSession;
			pSession->SetUserPtr(pParam);//�������ӷ���ָ��1
			m_SessionParams.append(pParam);

		}
		else if(pPackage->m_wFrameType == SP_UA_MSG_REG_AGENT && pPackage->m_iAsduLen == sizeof(stuMCastUnitInfo))
		{
			//�ֵܴ�����̵���
			m_AgentSessionParams.lock();
			stuAgentSessionParam *pParam = SearchAgentParamByIp(SSocket::IpStrToIpv4(pSession->GetPeerIp().data()));
			if(pParam != NULL)
			{
				delete pPackage;
				LOGWARN("�ظ��Ĵ������(from: %s:%d)!",pSession->GetPeerIp().data(),pSession->GetPeerPort());
				pSession->GetSessionSock()->Close();
				m_AgentSessionParams.unlock();
				return;
			}
			pParam = new stuAgentSessionParam();
			memcpy(&pParam->m_UnitInfo,pPackage->m_pAsduBuffer,sizeof(pParam->m_UnitInfo));
			pParam->pClient = NULL;
			pParam->pSession = pSession;
			pSession->SetUserPtr2(pParam);//�������ӷ���ָ��2
			m_AgentSessionParams.append(pParam,false);
			m_AgentSessionParams.unlock();
		}
		else
		{
			LOGWARN("��Ч�Ĵ������(from: %s:%d)!",pSession->GetPeerIp().data(),pSession->GetPeerPort());
			pSession->GetSessionSock()->Close();
		}
		OnRecvFrame(pSession,pPackage);
	}
	if(pPackage != NULL)
		delete pPackage;
}

//////////////////////////////////////////////////////////////////////////
// ��    ��:  ɾ���Ự�Ļص��麯��
// ��    ��:  �ۿ���
// ����ʱ��:  2016-2-2 16:48
// ����˵��:  @pSessionΪ������ɾ���ĻỰʵ��
// �� �� ֵ:  void
//////////////////////////////////////////////////////////////////////////
void CSpUnitAgent::OnDeleteSession(STcpServerSession *pSession)
{
	stuSessionParam *pParam = SearchParamBySession(pSession);
	if(pParam != NULL)
	{
		m_SessionParams.remove(pParam);
		m_RemoveSessionParams.append(pParam);
		if(m_RemoveSessionParams.count() > 3)
			m_RemoveSessionParams.remove(0);
	}
	else
	{
		stuAgentSessionParam *pAgentParam = SearchAgentParamBySession(pSession);
		if(pAgentParam != NULL)
		{
			m_AgentSessionParams.remove(pAgentParam);
			m_RemoveAgentSessionParams.append(pAgentParam);
			if(m_RemoveAgentSessionParams.count() > 3)
				m_RemoveAgentSessionParams.remove(0);
		}
	}
}

//////////////////////////////////////////////////////////////////////////
// ��    ��:  ������̻��������������Ĵ�����Ϣ
// ��    ��:  �ۿ���
// ����ʱ��:  2016-2-23 22:27
// ����˵��:  
// �� �� ֵ:  
//////////////////////////////////////////////////////////////////////////
void CSpUnitAgent::ProcessAgentMsg(/*stuSessionParam *pSessionParam,stuAgentSessionParam *pAgentSessionParam,*/bool bFromAgent,stuSTcpPackage *pPackage)
{
	if(pPackage->m_iAsduLen < sizeof(stuSpUnitAgentMsgHead))
		return;
	stuSpUnitAgentMsgHead *pMsgHead = (stuSpUnitAgentMsgHead*)pPackage->m_pAsduBuffer;
	pMsgHead->Swap();
	unsigned long pos = 0;
	if(pMsgHead->m_DstId.m_iUnitId == m_pApp->m_iUnitId || pMsgHead->m_DstId.m_iUnitId == 0)
	{
		//Ŀ��Ϊ��ǰ��Ԫ
		register stuSessionParam *pProc = m_SessionParams.FetchFirst(pos);
		bool bSended = false;
		while(pProc)
		{
			if(pProc->ProcInfo.process_id.m_iApplicationId == pMsgHead->m_DstId.m_iApplicationId)
			{
				bSended = true;				
				if(pMsgHead->m_DstId.GetApplicationId() == SP_UA_APPNO_AGENT)
					m_pApp->AddToRecv(NewPackage(pPackage));
				else
					pProc->pSession->AddToSend(NewPackage(pPackage));
			}
			pProc = m_SessionParams.FetchNext(pos);
		}
// 		//������ȷ�ʹ���̵��������ĲŸ��ʹ��ź�
// 		if(pPackage->m_wFrameType > SP_UA_MSG_SEND_ERR && pMsgHead->m_DstId.m_iUnitId != 0)
// 		{
// 			stuSpUnitAgentMsgHead ResMsgHead;
// 			ResMsgHead.m_DstId = pMsgHead->m_SrcId;
// 			ResMsgHead.m_SrcId = pMsgHead->m_DstId;
// 			ResMsgHead.m_dwMsgSn = pMsgHead->m_dwMsgSn;
// 			ResMsgHead.m_dwParamReault = pPackage->m_wFrameType;
// 			AgentMsgAddToSend(pSessionParam,pAgentSessionParam,m_pApp->PackageBuild((WORD)(bSended?SP_UA_MSG_SEND_OK:SP_UA_MSG_SEND_ERR),&ResMsgHead));
// 		}
	}
	if(!bFromAgent && pMsgHead->m_DstId.m_iUnitId != m_pApp->m_iUnitId)
	{
		//���Ǵ���ת����������Ϣ������Ҫͨ������ת������ת��
		register stuAgentSessionParam *pAgent = m_AgentSessionParams.FetchFirst(pos);
		while(pAgent)
		{
			if(pMsgHead->m_DstId.m_iUnitId == 0 || pMsgHead->m_DstId.m_iUnitId == pAgent->m_UnitInfo.unit_id)
			{
				if(pAgent->pClient != NULL)
					pAgent->pClient->AddToSend(NewPackage(pPackage));
				else if(pAgent->pSession != NULL)
					pAgent->pSession->AddToSend(NewPackage(pPackage));
			}
			pAgent = m_AgentSessionParams.FetchNext(pos);
		}
	}

	
}

////////////////////////////////////////////////////////////////////////
// ��    ��:  �Ự�߳̽��յ����ĺ�Ļص��麯����������ͨ���˺�������ʱ��Ϣ
// ��    ��:  �ۿ���
// ����ʱ��:  2016-2-2 16:48
// ����˵��:  @pSessionΪ�Ựʵ��ָ�룬����ͨ��ָ����Զ˷�������
//            @pPackageΪ�ս��յ������ݰ�
// �� �� ֵ:  true��ʾ�Ѿ�������ϣ��˱��Ŀ����ͷ��ˣ�false��ʾδ�����˱���Ӧ������ն���
//////////////////////////////////////////////////////////////////////////
bool CSpUnitAgent::OnRecvFrame(STcpServerSession *pSession,stuSTcpPackage *pPackage)
{
	{
		ProcessAgentMsg(SearchAgentParamBySession(pSession) != NULL,pPackage);
	}
	
	return true;
	if(pPackage->m_iAsduLen < sizeof(stuSpUnitAgentMsgHead))
		return true;
	stuSpUnitAgentMsgHead *pMsgHead = (stuSpUnitAgentMsgHead*)pPackage->m_pAsduBuffer;
	pMsgHead->Swap();
	int iAsduLen = pPackage->m_iAsduLen-sizeof(stuSpUnitAgentMsgHead);	
	stuSessionParam *pSessionParam = pSession==NULL?NULL:SearchParamBySession(pSession);
	bool bSended = false;
	unsigned long pos = 0;
	if(pSessionParam == NULL)
	{
		//�����������Ϣ��ֻ��Ҫ������Ҫƥ��Ľ��̣�����Ҫת������������
		register stuSessionParam *pProc = m_SessionParams.FetchFirst(pos);
		while(pProc)
		{
			if(pProc->ProcInfo.process_id.m_iApplicationId == pMsgHead->m_DstId.m_iApplicationId)
			{
				bSended = true;
				if(pMsgHead->m_DstId.GetApplicationId() == SP_UA_APPNO_AGENT /*&& pPackage->m_wFrameType != SP_UA_MSG_PROC_TXT_CMD_ACK && pPackage->m_wFrameType != SP_UA_MSG_PROC_TXT_CMD_REQ*/)
				{
					m_pApp->AddToRecv(NewPackage(pPackage));
					//m_pApp->ProcessAgentMsg(pPackage->m_wFrameType,pMsgHead,iAsduLen==0?NULL:(pPackage->m_pAsduBuffer+sizeof(stuSpUnitAgentMsgHead)),iAsduLen);//����������̵���Ϣ������Ҫ������ת����ֱ�ӵ���
				}
				else
					pProc->pSession->AddToSend(NewPackage(pPackage));
			}
			pProc = m_SessionParams.FetchNext(pos);
		}
		if(pPackage->m_wFrameType > SP_UA_MSG_SEND_ERR && (bSended || pMsgHead->m_DstId.m_iUnitId != 0))
		{
			//���ͳɹ����߲��ǹ㲥��Ϣ��Ӧ�ø�����ϢͶ�ͽ��
			pMsgHead->m_dwParamReault = (DWORD)pPackage->m_wFrameType;
			stuSpUnitAgentProcessId id;
			memcpy(&id,&pMsgHead->m_SrcId,sizeof(id));
			memcpy(&pMsgHead->m_SrcId,&pMsgHead->m_DstId,sizeof(id));
			memcpy(&pMsgHead->m_DstId,&id,sizeof(id));
			pSession->AddToSend(m_pApp->PackageBuild((WORD)(bSended?SP_UA_MSG_SEND_OK:SP_UA_MSG_SEND_ERR),pMsgHead));
			//((STcpCommBase*)pSession)->SendFrame(pSession->GetSessionSock(),(WORD)(bSended?SP_UA_MSG_SEND_OK:SP_UA_MSG_SEND_ERR),(BYTE*)pMsgHead,sizeof(stuSpUnitAgentMsgHead));
		}
		return true;
	}
	else
	{
		//��ͨ���̵���Ϣ�������ش����⣬��������Ҫת������������		
		if(pMsgHead->m_DstId.m_iUnitId == 0 || pMsgHead->m_DstId.m_iUnitId == m_pApp->m_iUnitId)
		{
			register stuSessionParam *pProc = m_SessionParams.FetchFirst(pos);
			while(pProc)
			{
				if(pProc->ProcInfo.process_id.m_iApplicationId == pMsgHead->m_DstId.m_iApplicationId)
				{
					bSended = true;
					if(pMsgHead->m_DstId.GetApplicationId() == SP_UA_APPNO_AGENT /*&& pPackage->m_wFrameType != SP_UA_MSG_PROC_TXT_CMD_ACK && pPackage->m_wFrameType != SP_UA_MSG_PROC_TXT_CMD_REQ*/)
					{
						m_pApp->AddToRecv(NewPackage(pPackage));
// 						pPackage = NULL;
// 						m_pApp->ProcessAgentMsg(pPackage->m_wFrameType, pMsgHead,iAsduLen==0?NULL:(pPackage->m_pAsduBuffer+sizeof(stuSpUnitAgentMsgHead)),iAsduLen);//����������̵���Ϣ������Ҫ������ת����ֱ�ӵ���
					}
					else
						pProc->pSession->AddToSend(NewPackage(pPackage));
				}
				pProc = m_SessionParams.FetchNext(pos);
			}
		}
		bool bSendedAgent = false;
		register stuAgentSessionParam *pAgent = m_AgentSessionParams.FetchFirst(pos);
		while(pAgent)
		{
			if(pMsgHead->m_DstId.m_iUnitId == 0 || pMsgHead->m_DstId.m_iUnitId == pAgent->m_UnitInfo.unit_id)
			{
				bSendedAgent = true;
				if(pAgent->pClient != NULL)
					pAgent->pClient->AddToSend(NewPackage(pPackage));
				else if(pAgent->pSession != NULL)
					pAgent->pSession->AddToSend(NewPackage(pPackage));
			}
			pAgent = m_AgentSessionParams.FetchNext(pos);
		}
		//�ظ��ʹ���Ϣ
		if(pPackage->m_wFrameType > SP_UA_MSG_SEND_ERR)
		{
			pMsgHead->m_dwParamReault = (DWORD)pPackage->m_wFrameType;
			stuSpUnitAgentProcessId id;
			memcpy(&id,&pMsgHead->m_SrcId,sizeof(id));
			memcpy(&pMsgHead->m_SrcId,&pMsgHead->m_DstId,sizeof(id));
			memcpy(&pMsgHead->m_DstId,&id,sizeof(id));
			if(!bSendedAgent)
			{
				//û�д�ͨ������ת��				
				((STcpCommBase*)pSession)->SendFrame(pSession->GetSessionSock(),(WORD)(bSended?SP_UA_MSG_SEND_OK:SP_UA_MSG_SEND_ERR),(BYTE*)pMsgHead,sizeof(stuSpUnitAgentMsgHead));
			}
			else if(bSended)
			{
				//��ͨ������ת��ʱ���籾���ѳɹ�Ͷ��
				((STcpCommBase*)pSession)->SendFrame(pSession->GetSessionSock(),(WORD)(bSended?SP_UA_MSG_SEND_OK:SP_UA_MSG_SEND_ERR),(BYTE*)pMsgHead,sizeof(stuSpUnitAgentMsgHead));
			}
		}
// 		if(pPackage->m_wFrameType == SP_UA_MSG_PROC_TXT_CMD)
// 			return false;//�ض���Ϣ��Ҫ�ŵ����ն��н����쳣����
	}

#if 0
	switch(pPackage->m_wFrameType)
	{
	case SP_UA_CMD_REGISTER://ע��
		if(pPackage->m_iAsduLen == sizeof(pSessionParam->ProcInfo))
			memcpy(&pSessionParam->ProcInfo,pPackage->m_pAsduBuffer,sizeof(pSessionParam->ProcInfo)-32);//�����Ƶ�¼��Ϣ
		break;
	case SP_UA_CMD_PS://�н����б�
		{
			m_SessionParams.lock();
			int i,cnt = m_SessionParams.count();
			stuSpUnitAgentProcInfo *pInfos = (cnt==0?NULL:(new stuSpUnitAgentProcInfo[cnt]));
			unsigned long pos=0;
			stuSpUnitAgentProcInfo *pInfos0 = pInfos;
			stuSessionParam *p = m_SessionParams.FetchFirst(pos);
			while(p)
			{
				memcpy(pInfos0++,&p->ProcInfo,sizeof(stuSessionParam));
				p = m_SessionParams.FetchNext(pos);
			}
			m_SessionParams.unlock();
			pSession->SendFrame(SP_UA_CMD_PS,(BYTE*)pInfos,sizeof(stuSpUnitAgentProcInfo)*cnt);
		}
		break;
	case SP_UA_CMD_REBOOT://����
		LOGWARN("�յ��ͻ���[%s->%s:%s]��reboot����,����������ϵͳ!!!",pSessionParam->ProcInfo.login_ip,pSessionParam->ProcInfo.name,pSessionParam->ProcInfo.desc);
		SApi::SystemReboot();
		break;
	case SP_UA_CMD_HALT://�ػ�
		LOGWARN("�յ��ͻ���[%s->%s:%s]��halt����,���رղ���ϵͳ!!!",pSessionParam->ProcInfo.login_ip,pSessionParam->ProcInfo.name,pSessionParam->ProcInfo.desc);
		SApi::SystemReboot();
		break;
	default:
		return false;
	}
#endif
	return true;
}