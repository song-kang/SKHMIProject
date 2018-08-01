/**
 *
 * 文 件 名 : Agent.h
 * 创建日期 : 2014-4-26 21:39
 * 作    者 : 邵凯田(skt001@163.com)
 * 修改日期 : $Date: $
 * 当前版本 : $Revision: $
 * 功能描述 : 单元代理服务端
 * 修改记录 : 
 *            $Log: $
 *
 * Ver  Date        Author  Comments
 * ---  ----------  ------  -------------------------------------------
 * 001	2014-4-26	邵凯田　创建文件
 *
 **/

#if !defined(AFX_AGENT_H__F09D4109_9E2E_45BE_AEDA_722F43D725D3__INCLUDED_)
#define AFX_AGENT_H__F09D4109_9E2E_45BE_AEDA_722F43D725D3__INCLUDED_

#if 0

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "SApi.h"
#include "SSocket.h"
#include "SString.h"
#include "SApplication.h"
#include "UnitCheckSelfSvr.h"
class CAgent;


class CAgentSession
{
public:
	CAgentSession()
	{
		m_pSocket = NULL;
		m_SendPacket.setShared(true);
		m_SendPacket.setAutoDelete(true);
		m_RecvPacket.setShared(true);
	}
	virtual ~CAgentSession()
	{
		if(m_pSocket != NULL)
		{
			delete m_pSocket;
			m_pSocket = NULL;
		}
	}

	bool Begin();
	static void* ThreadSession(void *lp);
	int ProcessMsg(SSocket *pSocket,SString &sMsg,BYTE* pBuffer=NULL,int iLength=0);
	void AddSendPackage(SString &sMsg,BYTE* pBuffer=NULL,int iLength=0);
	CAgentPackage* SearchResult(SString &id,int timeout_s=2);
	int GetCpuPercent();
	SString GetInfo();

	SSocket *m_pSocket;
	bool m_bGuest;
	int m_iPid;//进程号
	SString m_sModName;//模块名
	SString m_sModVer;//模块版本
	SString m_sModDesc;//模块描述
	SString m_sModCrc;//模块CRC
	SString m_sModCompileTime;//模块编译时间
	SDateTime m_dtLoginTime;//登录时间
	SString m_sLoginIp;//客户端IP
	int m_iLoginPort;//客户端端口
	SPtrList<CAgentPackage> m_SendPacket;//待发送的包
	SPtrList<CAgentPackage> m_RecvPacket;//接收到的包
	CAgent *m_pAgent;
};

class CAgent  
{
public:
	struct stuTelnetParam
	{
		CAgent *pAgent;
		SSocket *pSocket;
	};
	friend class CAgentSession;
	CAgent();
	virtual ~CAgent();

	bool Start(int iAgentPort);
	bool Stop();

	inline void SetExePath(SString path){m_sExePath = path;};
	inline SString GetExePath(){return m_sExePath;};
	void SetUnitChkSelfSvr(CUnitCheckSelfSvr *p){m_pUnitChkSelfSvr = p;};	

	SPtrList<CAgentSession> m_SessionList;//客户端列表
	SLock m_SessionLock;//线程列表锁

private:
	bool NewClientLogin(SSocket *pSocket,SString &sMsg);	
	CAgentSession* SearchSession(int pid);
	static void ChkSelfAnalogChg(void *cbParam,stuUnitCheckSelfAnalog *pAnalog);
	static void ChkSelfStateChg(void *cbParam,stuUnitCheckSelfState *pState);

	static void* ThreadListen(void *lp);
	static void* ThreadTelnet(void *lp);
	SString ProcessTelnetCmd(SString &cmd);
	int m_iAgentPort;//代理端口，默认为6666
	CUnitCheckSelfSvr *m_pUnitChkSelfSvr;

	bool m_bQuit;//是否要退出应用程序
	SSocket m_ListenSocket;//侦听SOCKET
	SString m_sExePath;//执行程序路径
	int m_iSelectPid;//当前选择的进程号
};
#endif
#endif // !defined(AFX_AGENT_H__F09D4109_9E2E_45BE_AEDA_722F43D725D3__INCLUDED_)
