/**
 *
 * 文 件 名 : AgentEx.h
 * 创建日期 : 2016-2-2 11:28
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
	// 描    述:  断开服务端后的回调
	// 作    者:  邵凯田
	// 创建时间:  2016-2-16 16:18
	// 参数说明:  void
	// 返 回 值:  void
	//////////////////////////////////////////////////////////////////////////
	virtual void OnClose();

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
	virtual bool ProcessAgentMsg(WORD wMsgType,stuSpUnitAgentMsgHead *pMsgHead,SString &sHeadStr,BYTE* pBuffer=NULL,int iLength=0);

	CSpUnitAgent *m_pUnitAgent;
};

//////////////////////////////////////////////////////////////////////////
// 名    称:  CSpUnitAgent
// 作    者:  邵凯田
// 创建时间:  2016-2-2 16:34
// 描    述:  单元代理通讯服务类
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
		STcpServerSession *pSession;//被动连接
		CSpAgentClientProx *pClient;//主动连接
		stuMCastUnitInfo m_UnitInfo;//对应单元信息，通道组播自发现
	};
	struct stuTelnetParam
	{
		CSpUnitAgent *pAgent;
		SSocket *pSocket;
	};
	CSpUnitAgent();
	virtual ~CSpUnitAgent();

	////////////////////////////////////////////////////////////////////////
	// 描    述:  启动服务
	// 作    者:  邵凯田
	// 创建时间:  2016-2-3 11:13
	// 参数说明:  @port表示端口,@ip表示服务端IP地址
	// 返 回 值:  true表示服务启动成功,false表示服务启动失败
	//////////////////////////////////////////////////////////////////////////
	virtual bool Start(int port,SString ip="");

	//////////////////////////////////////////////////////////////////////////
	// 描    述:  新的连接超时请求，可将SOCKET用于其它用途
	// 作    者:  邵凯田
	// 创建时间:  2016-2-5 17:17
	// 参数说明:  @pSocket为SOCKET指针
	// 返 回 值:  true表示SOCKET被成功托管（应从服务中分离，不再需要服务类维护该SOCKET），false表示不托管，应立即释放该连接
	//////////////////////////////////////////////////////////////////////////
	virtual bool OnNewTimeoutSocket(SSocket* &pSocket);

	////////////////////////////////////////////////////////////////////////
	// 描    述:  登录回调虚函数
	// 作    者:  邵凯田
	// 创建时间:  2016-2-2 16:44
	// 参数说明:  @ip登录客户端的IP
	//            @port登录客户端的端口
	//            @sLoginHead登录字符串
	// 返 回 值:  true表示允许登录,false表示拒绝登录
	//////////////////////////////////////////////////////////////////////////
	virtual bool OnLogin(SString ip,int port,SString &sLoginHead);

	//////////////////////////////////////////////////////////////////////////
	// 描    述:  新会话回调虚函数
	// 作    者:  邵凯田
	// 创建时间:  2016-2-2 16:48
	// 参数说明:  @pSession为新创建的会话实例
	// 返 回 值:  void
	//////////////////////////////////////////////////////////////////////////
	virtual void OnNewSession(STcpServerSession *pSession);

	//////////////////////////////////////////////////////////////////////////
	// 描    述:  删除会话的回调虚函数
	// 作    者:  邵凯田
	// 创建时间:  2016-2-2 16:48
	// 参数说明:  @pSession为即将将删除的会话实例
	// 返 回 值:  void
	//////////////////////////////////////////////////////////////////////////
	virtual void OnDeleteSession(STcpServerSession *pSession);

	////////////////////////////////////////////////////////////////////////
	// 描    述:  会话线程接收到报文后的回调虚函数，派生类通过此函数处理即时消息
	// 作    者:  邵凯田
	// 创建时间:  2016-2-2 16:48
	// 参数说明:  @pSession为会话实例指针，可以通过指针向对端发送数据
	//            @pPackage为刚接收到的数据包
	// 返 回 值:  true表示已经处理完毕，此报文可以释放了，false表示未处理，此报文应放入接收队列
	//////////////////////////////////////////////////////////////////////////
	virtual bool OnRecvFrame(STcpServerSession *pSession,stuSTcpPackage *pPackage);

	//////////////////////////////////////////////////////////////////////////
	// 描    述:  处理进程或其它代理发过来的代理消息
	// 作    者:  邵凯田
	// 创建时间:  2016-2-23 22:27
	// 参数说明:  
	// 返 回 值:  
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
	// 描    述:  取所有的代理单元列表
	// 作    者:  邵凯田
	// 创建时间:  2016-2-12 9:10
	// 参数说明:  @UnitInfos表示引用返回的单元信息列表
	// 返 回 值:  代理单元数量
	//////////////////////////////////////////////////////////////////////////
	int GetUnitList(SPtrList<stuSpUnitAgentUnitInfo> &UnitInfos);

	//////////////////////////////////////////////////////////////////////////
	// 描    述:  取当前单元的所有应用列表
	// 作    者:  邵凯田
	// 创建时间:  2016-2-12 9:54
	// 参数说明:  @ProcInfos表示应用进程的列表（引用返回）
	// 返 回 值:  应用数量
	//////////////////////////////////////////////////////////////////////////
	int GetProcList(SPtrList<stuSpUnitAgentProcInfo> &ProcInfos);

	void SetMCastFounderPtr(CMCastFounder *p){m_pMCastFounder = p;};


	//////////////////////////////////////////////////////////////////////////
	// 描    述:  连接到指定的其他单元
	// 作    者:  邵凯田
	// 创建时间:  2016-2-23 17:21
	// 参数说明:  @pUnit
	// 返 回 值:  void
	//////////////////////////////////////////////////////////////////////////
	void ConnectAgent(CSpUnit *pUnit);
private:
	//////////////////////////////////////////////////////////////////////////
	// 描    述:  根据会话检索对应的参数指针
	// 作    者:  邵凯田
	// 创建时间:  2016-2-3 8:55
	// 参数说明:  @pSession
	// 返 回 值:  stuSessionParam*
	//////////////////////////////////////////////////////////////////////////
	inline stuSessionParam* SearchParamBySession(STcpServerSession *pSession){return (stuSessionParam*)pSession->GetUserPtr();};

	//////////////////////////////////////////////////////////////////////////
	// 描    述:  根据应用号及实例号检索对应的参数指针
	// 作    者:  邵凯田
	// 创建时间:  2016-2-3 19:11
	// 参数说明:  @appid,instid
	// 返 回 值:  stuSessionParam*
	//////////////////////////////////////////////////////////////////////////
	stuSessionParam* SearchParamByAppId(WORD appid,WORD instid=0);


	//////////////////////////////////////////////////////////////////////////
	// 描    述:  根据进程号检索对应的参数指针
	// 作    者:  邵凯田
	// 创建时间:  2016-2-3 19:11
	// 参数说明:  @appid,instid
	// 返 回 值:  stuSessionParam*
	//////////////////////////////////////////////////////////////////////////
	stuSessionParam* SearchParamByPid(int pid);


	//////////////////////////////////////////////////////////////////////////
	// 描    述:  根据单元编号检索对应的代理参数指针
	// 作    者:  邵凯田
	// 创建时间:  2016-2-3 18:45
	// 参数说明:  @unitid
	// 返 回 值:  stuAgentSessionParam*
	//////////////////////////////////////////////////////////////////////////
	stuAgentSessionParam* SearchAgentParamByUnitId(DWORD unitid);

	//////////////////////////////////////////////////////////////////////////
	// 描    述:  根据IP地址检索对应的代理参数指针
	// 作    者:  邵凯田
	// 创建时间:  2016-2-3 13:16
	// 参数说明:  @ip
	// 返 回 值:  stuAgentSessionParam*
	//////////////////////////////////////////////////////////////////////////
	stuAgentSessionParam* SearchAgentParamByIp(DWORD ip);

	//////////////////////////////////////////////////////////////////////////
	// 描    述:  根据会话检索对应的代理参数指针 
	// 作    者:  邵凯田
	// 创建时间:  2016-2-3 13:19
	// 参数说明:  @pSession
	// 返 回 值:  stuAgentSessionParam*
	//////////////////////////////////////////////////////////////////////////
	stuAgentSessionParam* SearchAgentParamBySession(STcpServerSession *pSession);

	//////////////////////////////////////////////////////////////////////////
	// 描    述:  代理间通讯守护进程，定时查找未连接的代理，并进行连接
	// 作    者:  邵凯田
	// 创建时间:  2016-2-3 11:18
	// 参数说明:  @lp为this
	// 返 回 值:  NULL
	//////////////////////////////////////////////////////////////////////////
	static void* ThreadAgentComm(void* lp);


	//////////////////////////////////////////////////////////////////////////
	// 描    述:  TELNET服务线程
	// 作    者:  邵凯田
	// 创建时间:  2016-2-5 17:26
	// 参数说明:  @lp为this
	// 返 回 值:  NULL
	//////////////////////////////////////////////////////////////////////////
	static void* ThreadTelnet(void *lp);

	SString ProcessTelnetCmd(int &select_pid,SString &cmd);

	////////////////////////////////////////////////////////////////////////
	// 描    述:  将记录集转换为对齐文本
	// 作    者:  邵凯田
	// 创建时间:  2015-3-31 14:24
	// 参数说明:  @rs为记录集
	//         :  @sText为输出文本
	// 返 回 值:  void
	//////////////////////////////////////////////////////////////////////////
	void RecordsetToText(SRecordset &rs,SString &sText);

	SPtrList<stuSessionParam> m_SessionParams;//活动会话参数（针对普通客户端进程）
	SPtrList<stuSessionParam> m_RemoveSessionParams;//已删除的活动会话参数
	SPtrList<stuAgentSessionParam> m_AgentSessionParams;//活动的其它单元代理拉手连接
	SPtrList<stuAgentSessionParam> m_RemoveAgentSessionParams;//已删除的其它单元代理拉手连接	
	SPtrList<CSpAgentClient> m_AgentClients;//主动连接其它代理的客户端列表，代理间的拉手有主动连接和被动连接两种（任意两个代理间均点对点，为网状结构），代理启动时总是主总连接其它代理，已存在代理的代理则通过被动连接与之通道
	CMCastFounder *m_pMCastFounder;//组播自发现对象实例指针，用于发现兄弟代理单元，并与之建立连接
	SApplication *m_pApp;
public:
	stuSpUnitAgentUnitInfo m_SelfUnitInfo;
};

#endif//__SP_UNIT_AGENT_SERVER_H__
