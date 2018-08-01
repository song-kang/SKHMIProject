/**
 *
 * �� �� �� : Agent.cpp
 * �������� : 2014-4-26 21:39
 * ��    �� : �ۿ���(skt001@163.com)
 * �޸����� : $Date: $
 * ��ǰ�汾 : $Revision: $
 * �������� : ��Ԫ��������
 * �޸ļ�¼ : 
 *            $Log: $
 *
 * Ver  Date        Author  Comments
 * ---  ----------  ------  -------------------------------------------
 * 001	2014-4-26	�ۿ�������ļ�
 *
 **/

#include "Agent.h"
#if 0
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

#include "UnitMgrApplication.h"
extern CUnitMgrApplication g_app;

CAgent::CAgent()
{
	m_bQuit = false;
	m_SessionList.setShared(true);
	m_SessionList.setAutoDelete(true);
	m_iSelectPid = 0;
}

CAgent::~CAgent()
{

}

bool CAgent::Start(int iAgentPort)
{
	m_iAgentPort = iAgentPort;
	m_pUnitChkSelfSvr->SetChkSeflCallbackFun(ChkSelfAnalogChg,ChkSelfStateChg,this);
	S_CREATE_THREAD(ThreadListen,this);
	return true;
}

bool CAgent::Stop()
{
	m_bQuit = true;
	time_t last = ::time(NULL);
	while(m_ListenSocket.IsConnected() == false || m_SessionList.count() > 0)
	{
		if(::time(NULL) - last > 10)
		{
			//10�볬ʱǿ���˳�
			return false;
		}
		SApi::UsSleep(100000);
	}
	return true;
}

void* CAgent::ThreadListen(void *lp)
{
	#ifndef WIN32
		pthread_detach(pthread_self());
	#endif
	CAgent *pThis = (CAgent*)lp;
	int ret;
	SString sMsg,sRet;
	BYTE *pBuffer;
	int iLength;

	//Bind
	while(!pThis->m_bQuit)
	{
		SApi::UsSleep(10000);
		if(!pThis->m_ListenSocket.Create())
		{
			LOGFAULT("��������˿�ʱ����SOCKETʧ��!�����߳�����ʧ��!!");
			break;
		}
		if(!pThis->m_ListenSocket.Bind(pThis->m_iAgentPort))
		{
			LOGWARN("����������˿�(%d)ʱʧ��!1���Ӻ�����!",pThis->m_iAgentPort);
			SApi::UsSleep(1000*1000);
			continue;
		}
		if(!pThis->m_ListenSocket.Listen())
		{
			LOGWARN("��������˿�(%d)ʱʧ��!1���Ӻ�����!",pThis->m_iAgentPort);
			SApi::UsSleep(1000*1000);
			continue;
		}
		LOGDEBUG("��ʼ����TCP����˿�(%d)!",pThis->m_iAgentPort);
		while(!pThis->m_bQuit)
		{
			SApi::UsSleep(10000);
			SSocket *pSocket = new SSocket();
			if(!pThis->m_ListenSocket.Accept(*pSocket))
			{
				delete pSocket;
				if(pThis->m_bQuit)
					break;
				LOGWARN("�ȴ���������˿�(%d)����ʱʧ��!1���Ӻ�����!",pThis->m_iAgentPort);
				SApi::UsSleep(1000*1000);
				break;
			}
			//�õ�һ���µ�����,�Ƚ������¼��Ϣ��������ȷ�Ž����Ự
			pSocket->SetTimeout(1000,1000);//���ý��ճ�ʱ��100���룬��ֹ�Է����������ݶ�����
			pBuffer = NULL;
			ret = SApplication::ReceiveMsg(pSocket,sMsg,pBuffer,iLength);
			if(ret == 0 || ret == -1)
			{
				//�ж��Ƿ�Ϊtelnet����
				char ch[8];
				pSocket->SetTimeout(2000,1000);
				int times=0;
				while(1)
				{
					SApi::UsSleep(1000);
					ret = pSocket->Receive(ch,1);
					if(ret == 1)
					{
						if(ch[0] == '\r' || ch[1] == '\n')
						{
							times++;
						}
					}
					else
						break;
					if(times>=2)
					{
						LOGDEBUG("�õ��µ�TELNET����(%s:%d)!",pSocket->GetPeerIp().data(),pSocket->GetPeerPort());
						//Start telnet service
						stuTelnetParam *p = new stuTelnetParam();
						p->pAgent = pThis;
						p->pSocket = pSocket;
						S_CREATE_THREAD(ThreadTelnet,p);
						pSocket = NULL;
						break;
					}
				}
				if(pSocket != NULL)
				{
					pSocket->Close();
					delete pSocket;
					continue;
				}
				else
				{
					continue;
				}
			}
			if(ret <= 0)
			{
				if(pBuffer != NULL)
				{
					delete[] pBuffer;
				}
				pSocket->Close();
				delete pSocket;
				continue;
			}
			if(pBuffer != NULL)
			{
				delete[] pBuffer;
			}
			if(SString::GetAttributeValue(sMsg,"act") != "login")
			{
				pSocket->Close();
				delete pSocket;
				continue;
			}
			sRet = sMsg + ";result=1;";
			ret = SApplication::SendMsg(pSocket,sRet);
			if(ret > 0)
				pThis->NewClientLogin(pSocket,sMsg);
			else
			{
				pSocket->Close();
				delete pSocket;
			}
			SApi::UsSleep(10000);
		}
	}
	pThis->m_ListenSocket.Close();
	return 0;
}

void* CAgent::ThreadTelnet(void *lp)
{
	#ifndef WIN32
		pthread_detach(pthread_self());
	#endif
	stuTelnetParam *p = (stuTelnetParam*)lp;
	CAgent *pThis = p->pAgent;
	SSocket *pSocket = p->pSocket;
	int ret;
	SString cmd,sRet;
	char ch;

	cmd = "welcome";
	sRet = pThis->ProcessTelnetCmd(cmd);
	cmd = "";
	pSocket->Send(sRet.data(),sRet.length());
	pSocket->SetTimeout(100,2000);
	while(!pThis->m_bQuit)
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
			sRet = pThis->ProcessTelnetCmd(cmd);
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

void CAgent::ChkSelfAnalogChg(void *cbParam,stuUnitCheckSelfAnalog *pAnalog)
{
	CAgent *pThis = (CAgent*)cbParam;
	SString sMsg = "act=checkself_analog_chg;";
	unsigned long pos;
	pThis->m_SessionLock.lock();
	CAgentSession *pToSession = pThis->m_SessionList.FetchFirst(pos);
	while(pToSession)
	{
		pToSession->AddSendPackage(sMsg,(BYTE*)pAnalog,sizeof(stuUnitCheckSelfAnalog));
		pToSession = pThis->m_SessionList.FetchNext(pos);
	}
	pThis->m_SessionLock.unlock();
}

void CAgent::ChkSelfStateChg(void *cbParam,stuUnitCheckSelfState *pState)
{
	CAgent *pThis = (CAgent*)cbParam;
	SString sMsg = "act=checkself_state_chg;";
	unsigned long pos;
	pThis->m_SessionLock.lock();
	CAgentSession *pToSession = pThis->m_SessionList.FetchFirst(pos);
	while(pToSession)
	{
		pToSession->AddSendPackage(sMsg,(BYTE*)pState,sizeof(stuUnitCheckSelfState));
		pToSession = pThis->m_SessionList.FetchNext(pos);
	}
	pThis->m_SessionLock.unlock();
}

CAgentSession* CAgent::SearchSession(int pid)
{
	unsigned long pos=0;
	CAgentSession *pThisSession = m_SessionList.FetchFirst(pos);
	while(pThisSession)
	{
		if(pThisSession->m_iPid == pid)
			break;
		pThisSession = m_SessionList.FetchNext(pos);
	}
	return pThisSession;
}
SString CAgent::ProcessTelnetCmd(SString &cmd)
{
	SString sRet = "";
	SString sCmdAll,sTemp;
	sCmdAll = cmd;
	cmd = SString::GetIdAttribute(1,cmd," ");
	CAgentSession *pThisSession = NULL;
	if(m_iSelectPid != 0)
	{
		pThisSession = SearchSession(m_iSelectPid);
		if(pThisSession == NULL)
		{
			sTemp.sprintf("����(%d)�Ѿ��˳�, �Զ��˳���ǰѡ�����!\r\n",m_iSelectPid);
			sRet = sTemp;
			m_iSelectPid  = 0;
		}
	}
	if(cmd == "welcome")
	{
		sRet += "��ӭʹ��SDP/SSP����TELNET�ͻ��˳���\r\n�Ͼ����������Ƽ����޹�˾\r\n�汾��:V"+g_app.GetModuleVer()+"\r\n��¼ʱ��:"+SDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss")+"\r\n";
		sRet += "������'help'�鿴������������!\r\n";
	}
	else if(cmd == "help")
	{
		if(pThisSession == NULL)
		{
			sRet += "--------------------------------------------------------------\r\n";
			sRet += "SDP/SSP����TELNET�ͻ��˳���(V"+g_app.GetModuleVer()+")������Ϣ\r\n";
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
			sRet += "SDP/SSP����TELNET�ͻ��˳���(V"+g_app.GetModuleVer()+")������Ϣ\r\n";
			sRet += "--------------------------------------------------------------\r\n";
			sRet += "help                 ��ʾ������Ϣ\r\n";
			sRet += "ps                   ��ʾ���е�¼�Ľ����б�\r\n";
			sRet += "reboot               ����������ǰ��Ԫ����\r\n";
			sRet += "halt                 �رյ�ǰ��Ԫ����\r\n";
			//sRet += "telnet_reset         �������telnetd����\r\n";
			sRet += "select <PID>         ѡ��ָ���Ľ���\r\n";
			sRet += "exit                 �˳�ѡ��Ľ���\r\n";
			sRet += "--------------------------------------------------------------\r\n";
			sRet += "info                 ��ʾ��ǰ���̵�״̬��Ϣ����\r\n";
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
		SString sTemp;
		m_SessionLock.lock();
		int cpu,cnt = m_SessionList.count();
		sTemp.sprintf("��%d������:\r\n",cnt);
		sRet += sTemp;
		int i=0,len[10];
		memset(len,0,sizeof(len));
		unsigned long pos=0;
		sTemp = "PID,CPU%,ģ����,�汾��,CRC,����ʱ��,����,��¼IP,��¼�˿�,��¼ʱ��";
		for(i=0;i<10;i++)
		{
			len[i] = SString::GetIdAttribute(i+1,sTemp,",").length();
		}
		CAgentSession *pSession = m_SessionList.FetchFirst(pos);
		while(pSession)
		{
			cpu = pSession->GetCpuPercent();
			sTemp.sprintf("%d,%d%%,%s,V%s,%s,%s,%s,%s,%d,%s",pSession->m_iPid,cpu,pSession->m_sModName.data(),
				pSession->m_sModVer.data(),pSession->m_sModCrc.data(),pSession->m_sModCompileTime.data(),
				pSession->m_sModDesc.data(),pSession->m_sLoginIp.data(),
				pSession->m_iLoginPort,pSession->m_dtLoginTime.toString("yyyy-MM-dd hh:mm:ss.zzz").data());
			for(i=0;i<10;i++)
			{
				if(SString::GetIdAttribute(i+1,sTemp,",").length() > len[i])
					len[i] = SString::GetIdAttribute(i+1,sTemp,",").length();
			}
			pSession = m_SessionList.FetchNext(pos);
		}
		sTemp = "PID,CPU%,ģ����,�汾��,CRC,����ʱ��,����,��¼IP,��¼�˿�,��¼ʱ��";
		for(i=0;i<10;i++)
		{
			sRet += SString::toFillString(SString::GetIdAttribute(i+1,sTemp,","),len[i],' ',false)+"  ";
		}
		sRet += "\r\n";
		for(i=0;i<10;i++)
		{
			sRet += SString::toFillString("",len[i],'-',false)+"  ";
		}
		sRet += "\r\n";
		pSession = m_SessionList.FetchFirst(pos);
		while(pSession)
		{
			cpu = pSession->GetCpuPercent();
			sTemp.sprintf("%d,%d%%,%s,V%s,%s,%s,%s,%s,%d,%s",pSession->m_iPid,cpu,pSession->m_sModName.data(),
				pSession->m_sModVer.data(),pSession->m_sModCrc.data(),pSession->m_sModCompileTime.data(),
				pSession->m_sModDesc.data(),pSession->m_sLoginIp.data(),
				pSession->m_iLoginPort,pSession->m_dtLoginTime.toString("yyyy-MM-dd hh:mm:ss.zzz").data());
			for(i=0;i<10;i++)
			{
				sRet += SString::toFillString(SString::GetIdAttribute(i+1,sTemp,","),len[i],' ',false)+"  ";
			}
			sRet += "\r\n";
			pSession = m_SessionList.FetchNext(pos);
		}
		m_SessionLock.unlock();
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
		CAgentSession *pSession = SearchSession(pid);
		if(pSession == NULL)
		{
			sTemp.sprintf("��Ч��PID=%d, ѡ��ʧ��!\r\n",pid);
			sRet += sTemp;
		}
		else
		{
			m_iSelectPid = pid;
			pThisSession = pSession;
			sTemp.sprintf("ѡ�����(%d:%s,V%s,%s)�ɹ�!\r\n",pSession->m_iPid,pSession->m_sModName.data(),pSession->m_sModVer.data(),pSession->m_sModDesc.data());
			sRet += sTemp;
		}
	}
	else if(cmd == "exit")
	{
		if(m_iSelectPid == 0)
		{
			sTemp.sprintf("δѡ�����, ��ǰ�������!\r\n");
			sRet += sTemp;
		}
		else
		{
			m_iSelectPid = 0;
			pThisSession = NULL;
			sRet += "";
		}
	}
	else if(cmd == "info")
	{
		m_SessionLock.lock();
		pThisSession = SearchSession(m_iSelectPid);
		if(pThisSession == NULL)
		{
			m_iSelectPid = 0;
		}
		if(m_iSelectPid == 0)
		{
			//���Լ����ʹ�����
			sTemp = "act=info;";
			g_app.ProcessAgentMsg(0,sTemp);
			sRet += g_app.GetInfo()+"\r\n";
		}
		else
		{
			sRet += pThisSession->GetInfo()+"\r\n";
		}
		m_SessionLock.unlock();
	}
	else if(cmd == "restart")
	{
		m_SessionLock.lock();
		pThisSession = SearchSession(m_iSelectPid);
		if(pThisSession == NULL)
		{
			m_iSelectPid = 0;
		}
		if(m_iSelectPid == 0)
		{
			sTemp.sprintf("δѡ�����, ��ǰ�������!\r\n");
			sRet += sTemp;
		}
		else
		{		
			SString sMsg="act=restart;";
			pThisSession->AddSendPackage(sMsg);
		}
		m_SessionLock.unlock();
	}
	else if(cmd == "stop")
	{
		m_SessionLock.lock();
		pThisSession = SearchSession(m_iSelectPid);
		if(pThisSession == NULL)
		{
			m_iSelectPid = 0;
		}
		if(m_iSelectPid == 0)
		{
			sTemp.sprintf("δѡ�����, ��ǰ�������!\r\n");
			sRet += sTemp;
		}
		else
		{
			SString sMsg="act=stop;";
			pThisSession->AddSendPackage(sMsg);
		}
		m_SessionLock.unlock();
	}
	else if(cmd == "log")
	{
		m_SessionLock.lock();
		pThisSession = SearchSession(m_iSelectPid);
		if(pThisSession == NULL)
		{
			m_iSelectPid = 0;
		}
		if(m_iSelectPid == 0)
		{
			//���Լ����ʹ�����
			SApplication::ParseCommandLine(sCmdAll);
		}
		else
		{
			SString sMsg="act=log;cmd="+sCmdAll+";";
			pThisSession->AddSendPackage(sMsg);
		}
		m_SessionLock.unlock();
	}
	else if(cmd == "")
	{
		sRet += "";
	}
	else
	{
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
	m_SessionLock.lock();
	pThisSession = SearchSession(m_iSelectPid);
	if(pThisSession == NULL)
	{
		m_iSelectPid = 0;
	}
	if(pThisSession == NULL)
	{
		sRet += "[uk-agent]# ";
	}
	else
	{
		sTemp.sprintf("[%d:%s@uk-agent]# ",pThisSession->m_iPid,pThisSession->m_sModName.data());
		sRet += sTemp;
	}
	m_SessionLock.unlock();
	return sRet;
}

bool CAgent::NewClientLogin(SSocket *pSocket,SString &sMsg)
{
	CAgentSession *pNew = new CAgentSession();
	pNew->m_pSocket = pSocket;
	pNew->m_bGuest = SString::GetAttributeValue(sMsg,"guest") == "true";
	pNew->m_iPid = SString::GetAttributeValueI(sMsg,"pid");
	pNew->m_sModName = SString::GetAttributeValue(sMsg,"modname");
	pNew->m_sModVer = SString::GetAttributeValue(sMsg,"modver");
	pNew->m_sModDesc = SString::GetAttributeValue(sMsg,"moddesc");
	pNew->m_sModCrc = SString::GetAttributeValue(sMsg,"modcrc");
	pNew->m_sModCompileTime = SString::GetAttributeValue(sMsg,"modtime");
	pNew->m_dtLoginTime = SDateTime::currentDateTime();
	pNew->m_sLoginIp = pSocket->GetPeerIp().c_str();
	pNew->m_iLoginPort = pSocket->GetPeerPort();
	pNew->m_pAgent = this;
	m_SessionList.append(pNew);
	pNew->Begin();

	return true;
}

bool CAgentSession::Begin()
{
	S_CREATE_THREAD(ThreadSession,this);
	return true;
}

void* CAgentSession::ThreadSession(void *lp)
{
	#ifndef WIN32
		pthread_detach(pthread_self());
	#endif
	CAgentSession *pThis = (CAgentSession*)lp;
	CAgent *pAgent = pThis->m_pAgent;
	SSocket *pSocket = pThis->m_pSocket;
	int ret;
	SString act,sMsg;
	BYTE* pBuffer=NULL;
	int iLength;
	pSocket->SetTimeout(10,2000);
	while(!pAgent->m_bQuit)
	{
		//���ղ��������б���
		while(!pAgent->m_bQuit)
		{
			ret = pSocket->CheckForRecv();
			if(ret < 0)
			{
				goto error;
			}
			if(ret == 0)
			{
				break;
			}
			ret = SApplication::ReceiveMsg(pSocket,sMsg,pBuffer,iLength);
			if(ret < 0)
			{
				goto error;
			}
			if(ret == 0)
				break;
			act = SString::GetAttributeValue(sMsg,"act");
			if(act == "putfile")
			{
				//REQ: act=putfile;path_file=XXX;size=all_size_byte;cont=0/1;
				//RES: act=putfile;result=1/0;
				SString pathfile = SString::GetAttributeValue(sMsg,"path_file");
				SString tempfile = pAgent->GetExePath() + ".temp.swap";
				int size = SString::GetAttributeValueI(sMsg,"size");
				//�Զ��ϴ��ļ�����Ҫ����������д��־
				LOGWARN("�յ��ͻ���(%s:%d)�ϴ��ļ�(%s)����,�ļ���СΪ:%d!",
					pSocket->GetPeerIp().c_str(),pSocket->GetPeerPort(),pathfile.data(),size);
				if(pathfile.left(1) != "/" && pathfile.left(2).right(1) != ":")
				{
					//���Ǿ���·��������·��
					pathfile = pAgent->GetExePath()+pathfile;
				}
				SFile file(tempfile);
				if(!file.open(IO_Truncate))
				{
					LOGERROR("�ϴ���ʱ�ļ�(%s)��ʧ��!",tempfile.data());
				}

				time_t t1 = ::time(NULL);
				int cnt = 0;
				bool err = false;
				while(1)
				{
					if(iLength > 0)
						file.writeBlock(pBuffer,iLength);
					cnt += iLength;
					if(SString::GetAttributeValueI(sMsg,"cont") == 0)
					{
						//û�к���֡��
						break;
					}
					
					//����֡
					ret = SApplication::ReceiveMsg(pSocket,sMsg,pBuffer,iLength);
					if(ret < 0)
					{
						err = true;
						break;
					}
					if(ret == 0)
					{
						if(::time(NULL)-t1 > 5)
						{
							err = true;
							break;//timeout
						}
						SApi::UsSleep(1000);
						continue;
					}
					if(SString::GetAttributeValue(sMsg,"act") != "putfile")
						continue;
				}
				file.close();
				if(err || cnt != size)
				{
					err = false;
					//���ݲ�����
					LOGERROR("�ϴ�Ŀ���ļ�(%s)�����쳣(�ļ���С:%d,ʵ�ʻ��:%d)!�ļ�������!",pathfile.data(),size,cnt);
					SFile::remove(tempfile);
				}
				else
				{
					if(!SFile::copy(tempfile,pathfile))
					{
						err = false;
						LOGERROR("�ϴ���ʱ�ļ����Ƶ��ļ�(%s)ʱʧ��!",pathfile.data());
					}
					SFile::remove(tempfile);
				}
				sMsg.sprintf("act=putfile;result=%d;",err?0:1);
				pThis->AddSendPackage(sMsg);
			}
			else
			{
				LOGDEBUG("�����յ���Ϣ:%s",sMsg.data());
				pThis->ProcessMsg(pSocket,sMsg,pBuffer,iLength);
			}
			if(pBuffer != NULL)
			{
				delete[] pBuffer;
				pBuffer = NULL;
			}
			//SApi::UsSleep(1000);
		}

		//���ʹ�������
		int iSend = 0;
		while(!pAgent->m_bQuit)
		{
			CAgentPackage *pPack = pThis->m_SendPacket[0];
			if(pPack == NULL)
				break;
			LOGDEBUG("��������Ϣ:%s",pPack->sMsg.data());
			ret = SApplication::SendMsg(pSocket,pPack->sMsg,pPack->pBuffer,pPack->iLength);
			if(ret <= 0)
				goto error;
			iSend++;
			pThis->m_SendPacket.remove(0);
			//SApi::UsSleep(1000);
		}
		//if(iSend == 0)
		for(iSend=0;iSend<100;iSend++)
		{
			if(pSocket->CheckForRecv() > 0 || pThis->m_SendPacket.count()>0)
				break;
			SApi::UsSleep(2000);
		}
	}

error:

	pSocket->Close();
	pAgent->m_SessionLock.lock();
	pAgent->m_SessionLock.unlock();
	pAgent->m_SessionList.remove(pThis);
	return NULL;
}

int CAgentSession::ProcessMsg(SSocket *pSocket,SString &sMsg,BYTE* pBuffer/*=NULL*/,int iLength/*=0*/)
{
	SString sRet,sTemp;
	if(SString::IsAttribute(sMsg,"id"))
	{
		CAgentPackage *pPackage = new CAgentPackage();
		pPackage->sMsg = sMsg;
		pPackage->pBuffer = pBuffer;
		pPackage->iLength = iLength;
		m_RecvPacket.append(pPackage);
		return 0;//��id���ԣ������ǻظ������������ŵ����ն�����
	}

	SString act = SString::GetAttributeValue(sMsg,"act");
	if(act == "echo")
	{
		AddSendPackage(sMsg);
	}
	else if(act == "ps")
	{
		m_pAgent->m_SessionLock.lock();
		int i,cnt = m_pAgent->m_SessionList.count();
		sRet.sprintf("act=ps;size=%d;",cnt);
		unsigned long pos;
		CAgentSession *pSession = m_pAgent->m_SessionList.FetchFirst(pos);
		i=0;
		while(pSession)
		{
			if(!pSession->m_bGuest)
			{
				sTemp.sprintf("item%d=%d,%d,%s,V%s,%s,%s,%s,%s,%d,%s;",i+1,(int)pSession,pSession->m_iPid,pSession->m_sModName.data(),
					pSession->m_sModVer.data(),pSession->m_sModCrc.data(),pSession->m_sModCompileTime.data(),
					pSession->m_sModDesc.data(),pSession->m_sLoginIp.data(),
					pSession->m_iLoginPort,pSession->m_dtLoginTime.toString("yyyy-MM-dd hh:mm:ss.zzz").data());
				sRet += sTemp;
			}
			i++;
			pSession = m_pAgent->m_SessionList.FetchNext(pos);
		}
		AddSendPackage(sRet);
		m_pAgent->m_SessionLock.unlock();
	}
	else if(act == "reboot")
	{
		LOGWARN("�յ��ͻ���[%s->%s:%s]��reboot����,����������ϵͳ!!!",this->m_sLoginIp.data(),this->m_sModName.data(),this->m_sModDesc.data());
		SApi::SystemReboot();
	}
	else if(act == "halt")
	{
		LOGWARN("�յ��ͻ���[%s->%s:%s]��halt����,���رղ���ϵͳ!!!",this->m_sLoginIp.data(),this->m_sModName.data(),this->m_sModDesc.data());
		SApi::SystemHalt();
	}
	else if(act == "cfg_reset")
	{
		LOGWARN("�յ��ͻ���[%s->%s:%s]��cfg_reset����,��ת�����������ߵĴ���ͻ���!",this->m_sLoginIp.data(),this->m_sModName.data(),this->m_sModDesc.data());
		//�ؼ������ļ������˸ı䣬��Ҫ������Ϣ�ַ����������ߵĿͻ���
		unsigned long pos;
		m_pAgent->m_SessionLock.lock();
		CAgentSession *pToSession = m_pAgent->m_SessionList.FetchFirst(pos);
		while(pToSession)
		{
			pToSession->AddSendPackage(sMsg,pBuffer,iLength);
			pToSession = m_pAgent->m_SessionList.FetchNext(pos);
		}
		m_pAgent->m_SessionLock.unlock();
	}
	else if(act == "prox")
	{
		m_pAgent->m_SessionLock.lock();
		CAgentSession *pToSession = (CAgentSession*)SString::GetAttributeValueI(sMsg,"to_handle");
		if(pToSession == NULL || !m_pAgent->m_SessionList.exist(pToSession))
		{
			sRet.sprintf("act=err_handle;handle=%ld;",(long)pToSession);
			AddSendPackage(sRet,pBuffer,iLength);
			m_pAgent->m_SessionLock.unlock();
			return 0;
		}
		sMsg.replace("act=","act0=");
		sMsg.replace("act2=","act=");
		sRet.sprintf("from_handle=%ld;",(long)this);
		sRet += sMsg;
		pToSession->AddSendPackage(sRet,pBuffer,iLength);
		m_pAgent->m_SessionLock.unlock();
	}
	else if(act == "getfile")
	{
		//�Զ�ȡ�ļ�
		//REQ: act=getfile;path_file=XXX;
		//RES: act=getfile;result=1/0;size=all_size_byte;cont=0/1;
		SString pathfile = SString::GetAttributeValue(sMsg,"path_file");
		if(pathfile.left(1) != "/" && pathfile.left(2).right(1) != ":")
		{
			//���Ǿ���·��������·��
			pathfile = m_pAgent->GetExePath()+pathfile;
		}
		SFile file(pathfile);
		int size = file.size();
		if(!file.open(IO_ReadOnly))
		{
			//���ļ�ʧ�ܣ�����ʧ�ܸ��Զ�
			sRet = "act=getfile;result=0;size=0;cont=0;";
			AddSendPackage(sRet);
		}
		else if(size == 0)
		{
			//0�ֽ��ļ�
			sRet = "act=getfile;result=1;size=0;cont=0;";
			AddSendPackage(sRet);
			file.close();
		}
		else
		{
			int len,readlen;
			len = size;
			if(len > 1400)
				len = 1400;
			while(1)
			{
				BYTE* pBuffer2 = new BYTE[len];
				readlen = file.readBlock(pBuffer2,len);
				sRet.sprintf("act=getfile;result=1;size=%d;cont=%d;",size,file.atEnd()?0:1);
				AddSendPackage(sRet,pBuffer2,readlen);
				if(file.atEnd())
					break;
			}
			file.close();
		}
	}
	else if(act == "get_chk_analog")
	{
		//ȡģ�����Լ���Ϣ��
		int from = -1;
		int to = -1;
		if(SString::IsAttribute(sMsg,"from"))
			from = SString::GetAttributeValueI(sMsg,"from");
		if(SString::IsAttribute(sMsg,"to"))
			to = SString::GetAttributeValueI(sMsg,"to");
		SPtrList<stuUnitCheckSelfAnalog> list;
		m_pAgent->m_pUnitChkSelfSvr->SearchAnalog(list,from,to);
		sMsg += SString::toFormat("cnt=%d;",list.count());
		if(list.count() == 0)
		{
			AddSendPackage(sMsg);
		}
		else
		{
			stuUnitCheckSelfAnalog *pBuf = (stuUnitCheckSelfAnalog*)(new BYTE[sizeof(stuUnitCheckSelfAnalog)*list.count()]);
			int iLen;
			unsigned long pos=0;
			list.toNextArray(pBuf,iLen,list.count(),pos);
			AddSendPackage(sMsg,(BYTE*)pBuf,list.count()*sizeof(*pBuf));
		}
	}
	else if(act == "get_chk_state")
	{
		//ȡ״̬���Լ���Ϣ��
		int from = -1;
		int to = -1;
		if(SString::IsAttribute(sMsg,"from"))
			from = SString::GetAttributeValueI(sMsg,"from");
		if(SString::IsAttribute(sMsg,"to"))
			to = SString::GetAttributeValueI(sMsg,"to");
		SPtrList<stuUnitCheckSelfState> list;
		m_pAgent->m_pUnitChkSelfSvr->SearchState(list,from,to);
		sMsg += SString::toFormat("cnt=%d;",list.count());
		if(list.count() == 0)
		{
			AddSendPackage(sMsg);
		}
		else
		{
			stuUnitCheckSelfState *pBuf = (stuUnitCheckSelfState*)(new BYTE[sizeof(stuUnitCheckSelfState)*list.count()]);
			int iLen;
			unsigned long pos=0;
			list.toNextArray(pBuf,iLen,list.count(),pos);
			AddSendPackage(sMsg,(BYTE*)pBuf,list.count()*sizeof(*pBuf));
		}
	}

	return 0;
}

CAgentPackage* CAgentSession::SearchResult(SString &id,int timeout_s/*=2*/)
{
	int i,cnt;
	CAgentPackage *p;
	time_t soc = ::time(NULL);
	while(1)
	{
		cnt = m_RecvPacket.count();
		for(i=0;i<cnt;i++)
		{
			p = m_RecvPacket[i];
			if(SString::GetAttributeValue(p->sMsg,"id") == id)
			{
				m_RecvPacket.remove(p);
				return p;
			}
		}
		if(::time(NULL)-soc > timeout_s)
			break;
		SApi::UsSleep(1000);
	}
	return NULL;
}

void CAgentSession::AddSendPackage(SString &sMsg,BYTE* pBuffer/*=NULL*/,int iLength/*=0*/)
{
	CAgentPackage *pPackage = new CAgentPackage();
	pPackage->sMsg = sMsg;
	pPackage->pBuffer = pBuffer;
	pPackage->iLength = iLength;
	m_SendPacket.append(pPackage);
}

int CAgentSession::GetCpuPercent()
{
//	LOGDEBUG("into GetCpuPercent");
	SString cmd,id;
	id = SApi::GetRecordIndex();
	cmd.sprintf("id=%s;act=cpupercent;",id.data());
	AddSendPackage(cmd);
	CAgentPackage *p = SearchResult(id);
	if(p == NULL)
		return 0;
	int cpu = SString::GetAttributeValueI(p->sMsg,"cpu");
//	LOGDEBUG("GET cpu=%d",cpu);
	delete p;
	return cpu;
}

SString CAgentSession::GetInfo()
{
//	LOGDEBUG("into GetCpuPercent");
	SString cmd,id;
	id = SApi::GetRecordIndex();
	cmd.sprintf("id=%s;act=info;",id.data());
	AddSendPackage(cmd);
	CAgentPackage *p = SearchResult(id);
	if(p == NULL)
		return "";
	id = SString::GetAttributeValue(p->sMsg,"info");
//	LOGDEBUG("GET cpu=%d",cpu);
	delete p;
	return id;
}
#endif