/**
 *
 * �� �� �� : AgentEx.h
 * �������� : 2016-2-2 11:28
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

#ifndef __SP_UNIT_AGENT_SERVER_H__
#define __SP_UNIT_AGENT_SERVER_H__

#include "SApplication.h"
#include "STcpServerBase.h"
#include "mcast_founder.h"
#include "SDatabase.h"

class CSpUnitAgent;
class CSpAgentClientProx : public CSpAgentClient
{
public:
	friend class CSpUnitAgent;
	CSpAgentClientProx();
	virtual ~CSpAgentClientProx();

	//////////////////////////////////////////////////////////////////////////
	// ��    ��:  �Ͽ�����˺�Ļص�
	// ��    ��:  �ۿ���
	// ����ʱ��:  2016-2-16 16:18
	// ����˵��:  void
	// �� �� ֵ:  void
	//////////////////////////////////////////////////////////////////////////
	virtual void OnClose();

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
	virtual bool ProcessAgentMsg(WORD wMsgType,stuSpUnitAgentMsgHead *pMsgHead,SString &sHeadStr,BYTE* pBuffer=NULL,int iLength=0);

	CSpUnitAgent *m_pUnitAgent;
};

//////////////////////////////////////////////////////////////////////////
// ��    ��:  CSpUnitAgent
// ��    ��:  �ۿ���
// ����ʱ��:  2016-2-2 16:34
// ��    ��:  ��Ԫ����ͨѶ������
//////////////////////////////////////////////////////////////////////////
class CSpUnitAgent : public STcpServerBase
{
public:
	friend class CSpAgentClientProx;
	struct stuSessionParam
	{
		stuSpUnitAgentProcInfo ProcInfo;
		STcpServerSession *pSession;
	};

	struct stuAgentSessionParam
	{
		STcpServerSession *pSession;//��������
		CSpAgentClientProx *pClient;//��������
		stuMCastUnitInfo m_UnitInfo;//��Ӧ��Ԫ��Ϣ��ͨ���鲥�Է���
	};
	struct stuTelnetParam
	{
		CSpUnitAgent *pAgent;
		SSocket *pSocket;
	};
	CSpUnitAgent();
	virtual ~CSpUnitAgent();

	////////////////////////////////////////////////////////////////////////
	// ��    ��:  ��������
	// ��    ��:  �ۿ���
	// ����ʱ��:  2016-2-3 11:13
	// ����˵��:  @port��ʾ�˿�,@ip��ʾ�����IP��ַ
	// �� �� ֵ:  true��ʾ���������ɹ�,false��ʾ��������ʧ��
	//////////////////////////////////////////////////////////////////////////
	virtual bool Start(int port,SString ip="");

	//////////////////////////////////////////////////////////////////////////
	// ��    ��:  �µ����ӳ�ʱ���󣬿ɽ�SOCKET����������;
	// ��    ��:  �ۿ���
	// ����ʱ��:  2016-2-5 17:17
	// ����˵��:  @pSocketΪSOCKETָ��
	// �� �� ֵ:  true��ʾSOCKET���ɹ��йܣ�Ӧ�ӷ����з��룬������Ҫ������ά����SOCKET����false��ʾ���йܣ�Ӧ�����ͷŸ�����
	//////////////////////////////////////////////////////////////////////////
	virtual bool OnNewTimeoutSocket(SSocket* &pSocket);

	////////////////////////////////////////////////////////////////////////
	// ��    ��:  ��¼�ص��麯��
	// ��    ��:  �ۿ���
	// ����ʱ��:  2016-2-2 16:44
	// ����˵��:  @ip��¼�ͻ��˵�IP
	//            @port��¼�ͻ��˵Ķ˿�
	//            @sLoginHead��¼�ַ���
	// �� �� ֵ:  true��ʾ�����¼,false��ʾ�ܾ���¼
	//////////////////////////////////////////////////////////////////////////
	virtual bool OnLogin(SString ip,int port,SString &sLoginHead);

	//////////////////////////////////////////////////////////////////////////
	// ��    ��:  �»Ự�ص��麯��
	// ��    ��:  �ۿ���
	// ����ʱ��:  2016-2-2 16:48
	// ����˵��:  @pSessionΪ�´����ĻỰʵ��
	// �� �� ֵ:  void
	//////////////////////////////////////////////////////////////////////////
	virtual void OnNewSession(STcpServerSession *pSession);

	//////////////////////////////////////////////////////////////////////////
	// ��    ��:  ɾ���Ự�Ļص��麯��
	// ��    ��:  �ۿ���
	// ����ʱ��:  2016-2-2 16:48
	// ����˵��:  @pSessionΪ������ɾ���ĻỰʵ��
	// �� �� ֵ:  void
	//////////////////////////////////////////////////////////////////////////
	virtual void OnDeleteSession(STcpServerSession *pSession);

	////////////////////////////////////////////////////////////////////////
	// ��    ��:  �Ự�߳̽��յ����ĺ�Ļص��麯����������ͨ���˺�������ʱ��Ϣ
	// ��    ��:  �ۿ���
	// ����ʱ��:  2016-2-2 16:48
	// ����˵��:  @pSessionΪ�Ựʵ��ָ�룬����ͨ��ָ����Զ˷�������
	//            @pPackageΪ�ս��յ������ݰ�
	// �� �� ֵ:  true��ʾ�Ѿ�������ϣ��˱��Ŀ����ͷ��ˣ�false��ʾδ�����˱���Ӧ������ն���
	//////////////////////////////////////////////////////////////////////////
	virtual bool OnRecvFrame(STcpServerSession *pSession,stuSTcpPackage *pPackage);

	//////////////////////////////////////////////////////////////////////////
	// ��    ��:  ������̻��������������Ĵ�����Ϣ
	// ��    ��:  �ۿ���
	// ����ʱ��:  2016-2-23 22:27
	// ����˵��:  
	// �� �� ֵ:  
	//////////////////////////////////////////////////////////////////////////
	void ProcessAgentMsg(/*stuSessionParam *pSessionParam,stuAgentSessionParam *pAgentSessionParam,*/bool bFromAgent,stuSTcpPackage *pPackage);


	inline void AgentMsgAddToSend(stuSessionParam *pSessionParam,stuAgentSessionParam *pAgentSessionParam,stuSTcpPackage *pPackage)
	{
		if(pSessionParam != NULL)
			pSessionParam->pSession->AddToSend(pPackage);
		else if(pAgentSessionParam->pSession != NULL)
			pAgentSessionParam->pSession->AddToSend(pPackage);
		else if(pAgentSessionParam->pClient != NULL)
			pAgentSessionParam->pClient->AddToSend(pPackage);
		else
			delete pPackage;
	}

	//////////////////////////////////////////////////////////////////////////
	// ��    ��:  ȡ���еĴ���Ԫ�б�
	// ��    ��:  �ۿ���
	// ����ʱ��:  2016-2-12 9:10
	// ����˵��:  @UnitInfos��ʾ���÷��صĵ�Ԫ��Ϣ�б�
	// �� �� ֵ:  ����Ԫ����
	//////////////////////////////////////////////////////////////////////////
	int GetUnitList(SPtrList<stuSpUnitAgentUnitInfo> &UnitInfos);

	//////////////////////////////////////////////////////////////////////////
	// ��    ��:  ȡ��ǰ��Ԫ������Ӧ���б�
	// ��    ��:  �ۿ���
	// ����ʱ��:  2016-2-12 9:54
	// ����˵��:  @ProcInfos��ʾӦ�ý��̵��б����÷��أ�
	// �� �� ֵ:  Ӧ������
	//////////////////////////////////////////////////////////////////////////
	int GetProcList(SPtrList<stuSpUnitAgentProcInfo> &ProcInfos);

	void SetMCastFounderPtr(CMCastFounder *p){m_pMCastFounder = p;};


	//////////////////////////////////////////////////////////////////////////
	// ��    ��:  ���ӵ�ָ����������Ԫ
	// ��    ��:  �ۿ���
	// ����ʱ��:  2016-2-23 17:21
	// ����˵��:  @pUnit
	// �� �� ֵ:  void
	//////////////////////////////////////////////////////////////////////////
	void ConnectAgent(CSpUnit *pUnit);
private:
	//////////////////////////////////////////////////////////////////////////
	// ��    ��:  ���ݻỰ������Ӧ�Ĳ���ָ��
	// ��    ��:  �ۿ���
	// ����ʱ��:  2016-2-3 8:55
	// ����˵��:  @pSession
	// �� �� ֵ:  stuSessionParam*
	//////////////////////////////////////////////////////////////////////////
	inline stuSessionParam* SearchParamBySession(STcpServerSession *pSession){return (stuSessionParam*)pSession->GetUserPtr();};

	//////////////////////////////////////////////////////////////////////////
	// ��    ��:  ����Ӧ�úż�ʵ���ż�����Ӧ�Ĳ���ָ��
	// ��    ��:  �ۿ���
	// ����ʱ��:  2016-2-3 19:11
	// ����˵��:  @appid,instid
	// �� �� ֵ:  stuSessionParam*
	//////////////////////////////////////////////////////////////////////////
	stuSessionParam* SearchParamByAppId(WORD appid,WORD instid=0);


	//////////////////////////////////////////////////////////////////////////
	// ��    ��:  ���ݽ��̺ż�����Ӧ�Ĳ���ָ��
	// ��    ��:  �ۿ���
	// ����ʱ��:  2016-2-3 19:11
	// ����˵��:  @appid,instid
	// �� �� ֵ:  stuSessionParam*
	//////////////////////////////////////////////////////////////////////////
	stuSessionParam* SearchParamByPid(int pid);


	//////////////////////////////////////////////////////////////////////////
	// ��    ��:  ���ݵ�Ԫ��ż�����Ӧ�Ĵ������ָ��
	// ��    ��:  �ۿ���
	// ����ʱ��:  2016-2-3 18:45
	// ����˵��:  @unitid
	// �� �� ֵ:  stuAgentSessionParam*
	//////////////////////////////////////////////////////////////////////////
	stuAgentSessionParam* SearchAgentParamByUnitId(DWORD unitid);

	//////////////////////////////////////////////////////////////////////////
	// ��    ��:  ����IP��ַ������Ӧ�Ĵ������ָ��
	// ��    ��:  �ۿ���
	// ����ʱ��:  2016-2-3 13:16
	// ����˵��:  @ip
	// �� �� ֵ:  stuAgentSessionParam*
	//////////////////////////////////////////////////////////////////////////
	stuAgentSessionParam* SearchAgentParamByIp(DWORD ip);

	//////////////////////////////////////////////////////////////////////////
	// ��    ��:  ���ݻỰ������Ӧ�Ĵ������ָ�� 
	// ��    ��:  �ۿ���
	// ����ʱ��:  2016-2-3 13:19
	// ����˵��:  @pSession
	// �� �� ֵ:  stuAgentSessionParam*
	//////////////////////////////////////////////////////////////////////////
	stuAgentSessionParam* SearchAgentParamBySession(STcpServerSession *pSession);

	//////////////////////////////////////////////////////////////////////////
	// ��    ��:  �����ͨѶ�ػ����̣���ʱ����δ���ӵĴ�������������
	// ��    ��:  �ۿ���
	// ����ʱ��:  2016-2-3 11:18
	// ����˵��:  @lpΪthis
	// �� �� ֵ:  NULL
	//////////////////////////////////////////////////////////////////////////
	static void* ThreadAgentComm(void* lp);


	//////////////////////////////////////////////////////////////////////////
	// ��    ��:  TELNET�����߳�
	// ��    ��:  �ۿ���
	// ����ʱ��:  2016-2-5 17:26
	// ����˵��:  @lpΪthis
	// �� �� ֵ:  NULL
	//////////////////////////////////////////////////////////////////////////
	static void* ThreadTelnet(void *lp);

	SString ProcessTelnetCmd(int &select_pid,SString &cmd);

	////////////////////////////////////////////////////////////////////////
	// ��    ��:  ����¼��ת��Ϊ�����ı�
	// ��    ��:  �ۿ���
	// ����ʱ��:  2015-3-31 14:24
	// ����˵��:  @rsΪ��¼��
	//         :  @sTextΪ����ı�
	// �� �� ֵ:  void
	//////////////////////////////////////////////////////////////////////////
	void RecordsetToText(SRecordset &rs,SString &sText);

	SPtrList<stuSessionParam> m_SessionParams;//��Ự�����������ͨ�ͻ��˽��̣�
	SPtrList<stuSessionParam> m_RemoveSessionParams;//��ɾ���Ļ�Ự����
	SPtrList<stuAgentSessionParam> m_AgentSessionParams;//���������Ԫ������������
	SPtrList<stuAgentSessionParam> m_RemoveAgentSessionParams;//��ɾ����������Ԫ������������	
	SPtrList<CSpAgentClient> m_AgentClients;//����������������Ŀͻ����б��������������������Ӻͱ����������֣�����������������Ե㣬Ϊ��״�ṹ������������ʱ���������������������Ѵ��ڴ���Ĵ�����ͨ������������֮ͨ��
	CMCastFounder *m_pMCastFounder;//�鲥�Է��ֶ���ʵ��ָ�룬���ڷ����ֵܴ���Ԫ������֮��������
	SApplication *m_pApp;
public:
	stuSpUnitAgentUnitInfo m_SelfUnitInfo;
};

#endif//__SP_UNIT_AGENT_SERVER_H__
