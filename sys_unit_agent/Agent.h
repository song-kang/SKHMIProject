/**
 *
 * �� �� �� : Agent.h
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
	int m_iPid;//���̺�
	SString m_sModName;//ģ����
	SString m_sModVer;//ģ��汾
	SString m_sModDesc;//ģ������
	SString m_sModCrc;//ģ��CRC
	SString m_sModCompileTime;//ģ�����ʱ��
	SDateTime m_dtLoginTime;//��¼ʱ��
	SString m_sLoginIp;//�ͻ���IP
	int m_iLoginPort;//�ͻ��˶˿�
	SPtrList<CAgentPackage> m_SendPacket;//�����͵İ�
	SPtrList<CAgentPackage> m_RecvPacket;//���յ��İ�
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

	SPtrList<CAgentSession> m_SessionList;//�ͻ����б�
	SLock m_SessionLock;//�߳��б���

private:
	bool NewClientLogin(SSocket *pSocket,SString &sMsg);	
	CAgentSession* SearchSession(int pid);
	static void ChkSelfAnalogChg(void *cbParam,stuUnitCheckSelfAnalog *pAnalog);
	static void ChkSelfStateChg(void *cbParam,stuUnitCheckSelfState *pState);

	static void* ThreadListen(void *lp);
	static void* ThreadTelnet(void *lp);
	SString ProcessTelnetCmd(SString &cmd);
	int m_iAgentPort;//����˿ڣ�Ĭ��Ϊ6666
	CUnitCheckSelfSvr *m_pUnitChkSelfSvr;

	bool m_bQuit;//�Ƿ�Ҫ�˳�Ӧ�ó���
	SSocket m_ListenSocket;//����SOCKET
	SString m_sExePath;//ִ�г���·��
	int m_iSelectPid;//��ǰѡ��Ľ��̺�
};
#endif
#endif // !defined(AFX_AGENT_H__F09D4109_9E2E_45BE_AEDA_722F43D725D3__INCLUDED_)
