/**
 *
 * 文 件 名 : AgentEx.cpp
 * 创建日期 : 2016-2-2 11:30
 * 作    者 : 邵凯田(skt001@163.com)
 * 修改日期 : $Date: $
 * 当前版本 : $Revision: $
 * 功能描述 : 单元代理服务类（新版本）
 * 修改记录 : 
 *            $Log: $
 *
 * Ver  Date        Author  Comments
 * ---  ----------  ------  -------------------------------------------
 * 001	2016-2-2	邵凯田　创建文件
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
// 描    述:  断开服务端后的回调
// 作    者:  邵凯田
// 创建时间:  2016-2-16 16:18
// 参数说明:  void
// 返 回 值:  void
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
		LOGERROR("无效的级联会话!");
		return ;
	}
	m_pUnitAgent->m_AgentSessionParams.remove(p);
	m_pUnitAgent->m_RemoveAgentSessionParams.append(p);
	if(m_pUnitAgent->m_RemoveAgentSessionParams.count() > 3)
		m_pUnitAgent->m_RemoveAgentSessionParams.remove(0);
#endif
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
bool CSpAgentClientProx::ProcessAgentMsg(WORD wMsgType,stuSpUnitAgentMsgHead *pMsgHead,SString &sHeadStr,BYTE* pBuffer/*=NULL*/,int iLength/*=0*/)
{
	//代理过来的消息，只需要发给需要匹配的进程，不需要转发给其它代理
	stuSTcpPackage *pPackage = PackageBuild(wMsgType,pMsgHead,pBuffer,iLength);
	pPackage->m_sHead = sHeadStr;
	m_pUnitAgent->ProcessAgentMsg(true,pPackage);
	delete pPackage;
	return true;
}

CSpUnitAgent::CSpUnitAgent()
{
	//启动字符
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
// 描    述:  启动服务
// 作    者:  邵凯田
// 创建时间:  2016-2-3 11:13
// 参数说明:  @port表示端口,@ip表示服务端IP地址
// 返 回 值:  true表示服务启动成功,false表示服务启动失败
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
		LOGERROR("启动代理服务时失败！");
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
// 描    述:  取所有的代理单元列表
// 作    者:  邵凯田
// 创建时间:  2016-2-12 9:10
// 参数说明:  @UnitInfos表示引用返回的单元信息列表
// 返 回 值:  代理单元数量
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
// 描    述:  取当前单元的所有应用列表
// 作    者:  邵凯田
// 创建时间:  2016-2-12 9:54
// 参数说明:  @ProcInfos表示应用进程的列表（引用返回）
// 返 回 值:  应用数量
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
// 描    述:  新的连接超时请求，可将SOCKET用于其它用途
// 作    者:  邵凯田
// 创建时间:  2016-2-5 17:17
// 参数说明:  @pSocket为SOCKET指针
// 返 回 值:  true表示SOCKET被成功托管（应从服务中分离，不再需要服务类维护该SOCKET），false表示不托管，应立即释放该连接
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
				LOGDEBUG("得到新的TELNET连接(%s:%d)!",pSocket->GetPeerIp().data(),pSocket->GetPeerPort());
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
// 描    述:  TELNET服务线程
// 作    者:  邵凯田
// 创建时间:  2016-2-5 17:26
// 参数说明:  @lp为this
// 返 回 值:  NULL
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
	int select_pid=0;//当前选中的进程，0表示无选中

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
			//退格键
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
				sRet = "再见!欢迎下次再来!\r\n";
				ret = pSocket->Send(sRet.data(),sRet.length());
				SApi::UsSleep(100000);
				break;
			}
			if(cmd.length() == 3 && cmd.at(0) == 0x1b && cmd.at(1) == 0x5b && cmd.at(2) == 0x41)
			{
				//向上键
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
	LOGDEBUG("退出TELNET服务!");
	pSocket->Close();
	delete pSocket;
	delete p;
	return NULL;
}


////////////////////////////////////////////////////////////////////////
// 描    述:  将记录集转换为对齐文本
// 作    者:  邵凯田
// 创建时间:  2015-3-31 14:24
// 参数说明:  @rs为记录集
//         :  @sText为输出文本
// 返 回 值:  void
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
	//先扫描全部数据，确定各列数据长度
	for(row=0;row<rows;row++)
	{
		for(col=0;col<cols;col++)
		{
			len = rs.GetValue(row,col).length();
			if(len > col_len[col])
				col_len[col] = len;
		}
	}

	//生成表头
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
			sTemp.sprintf("进程(%d)已经退出, 自动退出当前选择进程!\r\n",select_pid);
			sRet = sTemp;
			select_pid  = 0;
		}
	}
	if(cmd == "welcome")
	{
		sRet += "欢迎使用SDP/SSP代理TELNET客户端程序\r\n南京悠阔电气科技有限公司\r\n版本号:V"+m_pApp->GetModuleVer()+"\r\n登录时间:"+SDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss")+"\r\n";
		sRet += "请输入'help'查看操作命令详情!\r\n";
	}
	else if(cmd == "help")
	{
		if(pSessionParam == NULL)
		{
			sRet += "--------------------------------------------------------------\r\n";
			sRet += "SDP/SSP代理TELNET客户端程序(V"+m_pApp->GetModuleVer()+")帮助信息\r\n";
			sRet += "--------------------------------------------------------------\r\n";
			sRet += "help                 显示帮助信息\r\n";
			sRet += "ps                   显示所有登录的进程列表\r\n";
			sRet += "reboot               重新启动当前单元主机\r\n";
			sRet += "halt                 关闭当前单元主机\r\n";
			//sRet += "telnet_reset         清除所有telnetd进程\r\n";
			sRet += "select <PID>         选择指定的进程\r\n";
			sRet += "quit                 退出\r\n";
		}
		else
		{
			sRet += "--------------------------------------------------------------\r\n";
			sRet += "SDP/SSP代理TELNET客户端程序(V"+m_pApp->GetModuleVer()+")帮助信息\r\n";
			sRet += "--------------------------------------------------------------\r\n";
			sRet += "help                 显示帮助信息\r\n";
			sRet += "ps                   显示所有登录的进程列表\r\n";
			sRet += "reboot               重新启动当前单元主机\r\n";
			sRet += "halt                 关闭当前单元主机\r\n";
			//sRet += "telnet_reset         清除所有telnetd进程\r\n";
			sRet += "select <PID>         选择指定的进程\r\n";
			sRet += "exit                 退出选择的进程\r\n";
			sRet += "--------------------------------------------------------------\r\n";
			sRet += "cmd [parameter]      执行指定参数的内部命令(参数为空时显示内部命令列表)\r\n";
			sRet += "restart              重新启动当前进程的服务\r\n";
			sRet += "stop                 退出当前进程的服务\r\n";
			sRet += "--------------------------------------------------------------\r\n";
			sRet += "log level none       关闭日志功能                             \r\n";
			sRet += "log level fault      设置日志级别为严重故障级                 \r\n";
			sRet += "log level error      设置日志级别为一般错误级                 \r\n";
			sRet += "log level warn       设置日志级别为警告级                     \r\n";
			sRet += "log level debug      设置日志级别为调试级                     \r\n";
			sRet += "log level basedebug  设置日志级别为基类调试级                 \r\n";
			sRet += "--------------------------------------------------------------\r\n";
			sRet += "log to console on    输出日志到控制台                         \r\n";
			sRet += "log to console off   停止输出日志到控制台                     \r\n";
			sRet += "log to file <path>   输出日志到指定文件                       \r\n";
			sRet += "log to file off      停止日志到文件                           \r\n";
			sRet += "log file size <n>    指定日志文件最大长度                     \r\n";
			sRet += "log to TCP <ip:n>    输出日志到指定IP和端口的TCP服务器        \r\n";
			sRet += "log to TCP off       停止日志到TCP服务器                      \r\n";
			sRet += "log to UDP <ip:n>    输出日志到指定IP和端口的UDP服务器        \r\n";
			sRet += "log to UDP off       停止日志到UDP服务器                      \r\n";
			sRet += "--------------------------------------------------------------\r\n";
			sRet += "quit                 退出\r\n";
		}
	}
	else if(cmd == "ps")
	{
		SRecordset rs;
		rs.SetSize(m_SessionParams.count(),11);
		rs.SetColumnName(0,"PID");
		rs.SetColumnName(1,"CPU%");
		rs.SetColumnName(2,"模块名");
		rs.SetColumnName(3,"版本号");
		rs.SetColumnName(4,"CRC");
		rs.SetColumnName(5,"生成时间");
		rs.SetColumnName(6,"描述");
		rs.SetColumnName(7,"登录IP");
		rs.SetColumnName(8,"登录端口");
		rs.SetColumnName(9,"启动时间");
		rs.SetColumnName(10,"登录时间");
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
		sRet += SString::toFormat("共%d个进程.\r\n",m_SessionParams.count());
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
			sTemp.sprintf("无效的PID=%d, 选择失败!\r\n",pid);
			sRet += sTemp;
		}
		else
		{
			select_pid = pid;
			pSessionParam = pSessionParam2;
			sTemp.sprintf("选择进程(%d:%s,V%s,%s)成功!\r\n",pSessionParam->ProcInfo.pid,pSessionParam->ProcInfo.name,pSessionParam->ProcInfo.version,pSessionParam->ProcInfo.desc);
			sRet += sTemp;
		}
	}
	else if(cmd == "exit")
	{
		if(select_pid == 0)
		{
			sTemp.sprintf("未选择进程, 当前命令被忽略!\r\n");
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
// 			//向自己发送此命令
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
			sTemp.sprintf("未选择进程, 当前命令被忽略!\r\n");
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
			sTemp.sprintf("未选择进程, 当前命令被忽略!\r\n");
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
			//向自己发送此命令
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
		//显示命令列表，不带参数
		//执行内部命令，带参数
		if(select_pid == 0)
		{
			sRet += "请先选择待操作的进程!\r\n";
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
// 描    述:  连接到指定的其他单元
// 作    者:  邵凯田
// 创建时间:  2016-2-23 17:21
// 参数说明:  @pUnit
// 返 回 值:  void
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
// 描    述:  代理间通讯守护进程，定时查找未连接的代理，并进行连接
// 作    者:  邵凯田
// 创建时间:  2016-2-3 11:18
// 参数说明:  @lp为this
// 返 回 值:  NULL
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
			//如果不是本系统对应的软件型号则忽略
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
				//已经存在于当前代理连接中，忽略
				pUnitInfo = units.FetchNext(pos);
				continue;
			}
			//通过CLIENT连接代理
			for(i=0;i<8;i++)
			{
				if(pUnitInfo->unit_ip[i] == 0)
					break;
				sIp = SSocket::Ipv4ToIpStr(pUnitInfo->unit_ip[i]);
				if(SSocket::Ping(sIp.data(),ms,ttl) == 0)
				{
					//PING得通，就用该地址连接
					pThis->m_AgentSessionParams.lock();
					pAgentParam = pThis->SearchAgentParamByIp(pUnitInfo->unit_ip[i]);
					if(pAgentParam != NULL)
					{
						//忽然又找到了，可能是对方主动连接上来，放弃当前连接
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
// 描    述:  根据应用号及实例号检索对应的参数指针
// 作    者:  邵凯田
// 创建时间:  2016-2-3 19:11
// 参数说明:  @appid,instid
// 返 回 值:  stuSessionParam*
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
// 描    述:  根据进程号检索对应的参数指针
// 作    者:  邵凯田
// 创建时间:  2016-2-3 19:11
// 参数说明:  @appid,instid
// 返 回 值:  stuSessionParam*
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
// 描    述:  根据单元编号检索对应的代理参数指针
// 作    者:  邵凯田
// 创建时间:  2016-2-3 18:45
// 参数说明:  @unitid
// 返 回 值:  stuAgentSessionParam*
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
// 描    述:  根据IP地址检索对应的代理参数指针
// 作    者:  邵凯田
// 创建时间:  2016-2-3 13:16
// 参数说明:  @ip
// 返 回 值:  stuAgentSessionParam*
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
// 描    述:  根据会话检索对应的代理参数指针 
// 作    者:  邵凯田
// 创建时间:  2016-2-3 13:19
// 参数说明:  @pSession
// 返 回 值:  stuAgentSessionParam*
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
// 描    述:  登录回调虚函数
// 作    者:  邵凯田
// 创建时间:  2016-2-2 16:44
// 参数说明:  @ip登录客户端的IP
//            @port登录客户端的端口
//            @sLoginHead登录字符串
// 返 回 值:  true表示允许登录,false表示拒绝登录
//////////////////////////////////////////////////////////////////////////
bool CSpUnitAgent::OnLogin(SString ip,int port,SString &sLoginHead)
{
	//客户端是否是合法的身份
	SString sSoftType = SString::GetAttributeValue(sLoginHead,"soft_type");
	if(sSoftType != m_pApp->m_sSoftwareType)
		return false;
	return true;
}


//////////////////////////////////////////////////////////////////////////
// 描    述:  新会话回调虚函数
// 作    者:  邵凯田
// 创建时间:  2016-2-2 16:48
// 参数说明:  @pSession为新创建的会话实例
// 返 回 值:  void
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
			//普通进程登入
			stuSessionParam *pParam = new stuSessionParam();
			memcpy(&pParam->ProcInfo,pPackage->m_pAsduBuffer,sizeof(stuSpUnitAgentProcInfo)-32);//不复制登录信息
			strcpy(pParam->ProcInfo.login_ip, pSession->GetPeerIp().data());
			pParam->ProcInfo.login_port = (DWORD)pSession->GetPeerPort();
			pParam->ProcInfo.login_time = (DWORD)SDateTime::getNowSoc();
			pParam->pSession = pSession;
			pSession->SetUserPtr(pParam);//正常连接放在指针1
			m_SessionParams.append(pParam);

		}
		else if(pPackage->m_wFrameType == SP_UA_MSG_REG_AGENT && pPackage->m_iAsduLen == sizeof(stuMCastUnitInfo))
		{
			//兄弟代理进程登入
			m_AgentSessionParams.lock();
			stuAgentSessionParam *pParam = SearchAgentParamByIp(SSocket::IpStrToIpv4(pSession->GetPeerIp().data()));
			if(pParam != NULL)
			{
				delete pPackage;
				LOGWARN("重复的代理登入(from: %s:%d)!",pSession->GetPeerIp().data(),pSession->GetPeerPort());
				pSession->GetSessionSock()->Close();
				m_AgentSessionParams.unlock();
				return;
			}
			pParam = new stuAgentSessionParam();
			memcpy(&pParam->m_UnitInfo,pPackage->m_pAsduBuffer,sizeof(pParam->m_UnitInfo));
			pParam->pClient = NULL;
			pParam->pSession = pSession;
			pSession->SetUserPtr2(pParam);//代理连接放在指针2
			m_AgentSessionParams.append(pParam,false);
			m_AgentSessionParams.unlock();
		}
		else
		{
			LOGWARN("无效的代理登入(from: %s:%d)!",pSession->GetPeerIp().data(),pSession->GetPeerPort());
			pSession->GetSessionSock()->Close();
		}
		OnRecvFrame(pSession,pPackage);
	}
	if(pPackage != NULL)
		delete pPackage;
}

//////////////////////////////////////////////////////////////////////////
// 描    述:  删除会话的回调虚函数
// 作    者:  邵凯田
// 创建时间:  2016-2-2 16:48
// 参数说明:  @pSession为即将将删除的会话实例
// 返 回 值:  void
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
// 描    述:  处理进程或其它代理发过来的代理消息
// 作    者:  邵凯田
// 创建时间:  2016-2-23 22:27
// 参数说明:  
// 返 回 值:  
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
		//目标为当前单元
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
// 		//仅当明确送达进程的正常报文才给送达信号
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
		//不是代理转发过来的消息，如需要通过代理转发，则转发
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
// 描    述:  会话线程接收到报文后的回调虚函数，派生类通过此函数处理即时消息
// 作    者:  邵凯田
// 创建时间:  2016-2-2 16:48
// 参数说明:  @pSession为会话实例指针，可以通过指针向对端发送数据
//            @pPackage为刚接收到的数据包
// 返 回 值:  true表示已经处理完毕，此报文可以释放了，false表示未处理，此报文应放入接收队列
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
		//代理过来的消息，只需要发给需要匹配的进程，不需要转发给其它代理
		register stuSessionParam *pProc = m_SessionParams.FetchFirst(pos);
		while(pProc)
		{
			if(pProc->ProcInfo.process_id.m_iApplicationId == pMsgHead->m_DstId.m_iApplicationId)
			{
				bSended = true;
				if(pMsgHead->m_DstId.GetApplicationId() == SP_UA_APPNO_AGENT /*&& pPackage->m_wFrameType != SP_UA_MSG_PROC_TXT_CMD_ACK && pPackage->m_wFrameType != SP_UA_MSG_PROC_TXT_CMD_REQ*/)
				{
					m_pApp->AddToRecv(NewPackage(pPackage));
					//m_pApp->ProcessAgentMsg(pPackage->m_wFrameType,pMsgHead,iAsduLen==0?NULL:(pPackage->m_pAsduBuffer+sizeof(stuSpUnitAgentMsgHead)),iAsduLen);//发往代理进程的消息，不需要从网络转发，直接调用
				}
				else
					pProc->pSession->AddToSend(NewPackage(pPackage));
			}
			pProc = m_SessionParams.FetchNext(pos);
		}
		if(pPackage->m_wFrameType > SP_UA_MSG_SEND_ERR && (bSended || pMsgHead->m_DstId.m_iUnitId != 0))
		{
			//发送成功或者不是广播消息，应该给出消息投送结果
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
		//普通进程的消息，除本地处理外，还可能需要转发给其它代理		
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
// 						m_pApp->ProcessAgentMsg(pPackage->m_wFrameType, pMsgHead,iAsduLen==0?NULL:(pPackage->m_pAsduBuffer+sizeof(stuSpUnitAgentMsgHead)),iAsduLen);//发往代理进程的消息，不需要从网络转发，直接调用
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
		//回复送达消息
		if(pPackage->m_wFrameType > SP_UA_MSG_SEND_ERR)
		{
			pMsgHead->m_dwParamReault = (DWORD)pPackage->m_wFrameType;
			stuSpUnitAgentProcessId id;
			memcpy(&id,&pMsgHead->m_SrcId,sizeof(id));
			memcpy(&pMsgHead->m_SrcId,&pMsgHead->m_DstId,sizeof(id));
			memcpy(&pMsgHead->m_DstId,&id,sizeof(id));
			if(!bSendedAgent)
			{
				//没有从通过代理转发				
				((STcpCommBase*)pSession)->SendFrame(pSession->GetSessionSock(),(WORD)(bSended?SP_UA_MSG_SEND_OK:SP_UA_MSG_SEND_ERR),(BYTE*)pMsgHead,sizeof(stuSpUnitAgentMsgHead));
			}
			else if(bSended)
			{
				//有通过代理转发时，如本机已成功投送
				((STcpCommBase*)pSession)->SendFrame(pSession->GetSessionSock(),(WORD)(bSended?SP_UA_MSG_SEND_OK:SP_UA_MSG_SEND_ERR),(BYTE*)pMsgHead,sizeof(stuSpUnitAgentMsgHead));
			}
		}
// 		if(pPackage->m_wFrameType == SP_UA_MSG_PROC_TXT_CMD)
// 			return false;//特定消息需要放到接收队列进行异常处理
	}

#if 0
	switch(pPackage->m_wFrameType)
	{
	case SP_UA_CMD_REGISTER://注册
		if(pPackage->m_iAsduLen == sizeof(pSessionParam->ProcInfo))
			memcpy(&pSessionParam->ProcInfo,pPackage->m_pAsduBuffer,sizeof(pSessionParam->ProcInfo)-32);//不复制登录信息
		break;
	case SP_UA_CMD_PS://列进程列表
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
	case SP_UA_CMD_REBOOT://重启
		LOGWARN("收到客户端[%s->%s:%s]的reboot命令,将重启操作系统!!!",pSessionParam->ProcInfo.login_ip,pSessionParam->ProcInfo.name,pSessionParam->ProcInfo.desc);
		SApi::SystemReboot();
		break;
	case SP_UA_CMD_HALT://关机
		LOGWARN("收到客户端[%s->%s:%s]的halt命令,将关闭操作系统!!!",pSessionParam->ProcInfo.login_ip,pSessionParam->ProcInfo.name,pSessionParam->ProcInfo.desc);
		SApi::SystemReboot();
		break;
	default:
		return false;
	}
#endif
	return true;
}