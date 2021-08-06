/**
 *
 * 文 件 名 : UnitMgrApplication.h
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

#ifndef __UNIT_MGR_APPLICATION_H__
#define __UNIT_MGR_APPLICATION_H__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "SApplication.h"
#include "AgentEx.h"
#include "mcast_founder.h"
#include "sk_unitconfig.h"

class CUnitMgrApplication : public SApplication  
{
public:
	struct stuPuttingFileParam
	{
		SString sFileName;//文件名
		SFile file;//文件
		int begin_soc;//开始时间
	};
	CUnitMgrApplication();
	virtual ~CUnitMgrApplication();

	////////////////////////////////////////////////////////////////////////
	// 描    述:  启动服务，开启所有应用的服务，该函数必须由派生类实现，且启动服务完成后必须返回
	// 作    者:  邵凯田
	// 创建时间:  2014:4:26 21:46
	// 参数说明:  void
	// 返 回 值:  true/false
	//////////////////////////////////////////////////////////////////////////
	virtual bool Start();

	////////////////////////////////////////////////////////////////////////
	// 描    述:  停止服务，关闭所有应用的服务
	// 作    者:  邵凯田
	// 创建时间:  2014:4:26 21:46
	// 参数说明:  void
	// 返 回 值:  true/false
	//////////////////////////////////////////////////////////////////////////
	virtual bool Stop();

	#ifndef _WITHOUT_AGENT_CHANNEL_
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


	//////////////////////////////////////////////////////////////////////////
	// 描    述:  处理文本命令
	// 作    者:  邵凯田
	// 创建时间:  2016-2-13 15:12
	// 参数说明:  @sCmd为命令内容，空字符串表示取表命令列表
	//         :  @sResult为返回结果，纯文本
	// 返 回 值:  true表示成功，false表示失败
	//////////////////////////////////////////////////////////////////////////
	virtual bool ProcessTxtCmd(SString &sCmd,SString &sResult);

	#endif

	CSKUnitconfig m_UnitConfig;
	CSpUnitAgent m_Agent;
	CMCastFounder m_MCastFounder;
	SPtrList<stuPuttingFileParam> m_PuttingParam;

private:
	inline stuPuttingFileParam* SearchPuttingParam(SString &sFileName)
	{
		register unsigned long pos = 0;
		stuPuttingFileParam *p = m_PuttingParam.FetchFirst(pos);
		while(p)
		{
			if(p->sFileName == sFileName)
				return p;
			p = m_PuttingParam.FetchNext(pos);
		}
		return NULL;
	}

private:
	static void* ThreadLic(void* lp);

public:
	void ExitByLic() {
		exit(-1);
	}

};

#endif //__UNIT_MGR_APPLICATION_H__
