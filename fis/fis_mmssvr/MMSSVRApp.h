#pragma once

#include "MMSServer.h"
#include "public/UK9010App.h"
#include "public/mdb_oe_def.h"
#include "public/mdb_na_def.h"
#define MODULE_NAME "mst_mmssvr"


class CSMmsDataset;
class MMSSVRApp : public CUK9010Application
{
public:
	struct Ied
	{
		SString name;
		SString ipa;
		SString ipb;
	};

public:
	MMSSVRApp(void);
	virtual ~MMSSVRApp(void);

	//////////////////////////////////////////////////////////////////////////
	// 描    述:  启动服务，开启所有应用的服务，该函数必须由派生类实现，且启动服务完成后必须返回
	// 作    者:  邵凯田
	// 创建时间:  2010-7-1 9:15
	// 参数说明:  
	// 返 回 值:  null
	//////////////////////////////////////////////////////////////////////////
	virtual bool Start();

	//////////////////////////////////////////////////////////////////////////
	// 描    述:  停止服务，关闭所有应用的服务，
	// 作    者:  邵凯田
	// 创建时间:  2010-7-1 9:18
	// 参数说明:  
	// 返 回 值:  null
	//////////////////////////////////////////////////////////////////////////
	virtual bool Stop();

	////////////////////////////////////////////////////////////////////////
	// 描    述:  代理消息处理接口，由派生类实现，处理函数必须尽量短小，快速返回
	// 作    者:  邵凯田
	// 创建时间:  2016-2-3 15:20
	// 参数说明:  @wMsgType表示消息类型
	//         :  @pMsgHead为消息头
	//         :  @pBuffer为消息携带的二进制数据内容，NULL表示无二进制数据
	//         :  @iLength为二进制数据长度
	// 返 回 值:  true表示处理成功，false表示处理失败或未处理
	//////////////////////////////////////////////////////////////////////////
	virtual bool ProcessAgentMsg(unsigned short wMsgType,stuSpUnitAgentMsgHead *pMsgHead,SString &sHeadStr,unsigned char* pBuffer=NULL,int iLength=0);

	//////////////////////////////////////////////////////////////////////////
	// 描    述:  处理文本命令
	// 作    者:  邵凯田
	// 创建时间:  2016-2-13 15:12
	// 参数说明:  @sCmd为命令内容，空字符串表示取表命令列表
	//         :  @sResult为返回结果，纯文本
	// 返 回 值:  true表示成功，false表示失败
	//////////////////////////////////////////////////////////////////////////
	virtual bool ProcessTxtCmd(SString &sCmd,SString &sResult);

	inline void lock(){m_Lock.lock();}
	inline void unlock(){m_Lock.unlock();}

	// Description: 处理状态量突发
	// Author:      qianchenglin
	// Date:        2016-01-01
	// Parameter:   @data[in]数据内容指针
	//              @rows[in]数据条目数
	// Return:      处理成功返回true，否则返回false
	bool ProcessStateChanged(unsigned char *data, int rows);

	// Description: 处理模拟量突发
	// Author:      qianchenglin
	// Date:        2016-01-01
	// Parameter:   @data[in]数据内容指针
	//              @rows[in]数据条目数
	// Return:      处理成功返回true，否则返回false
	bool ProcessGeneralChanged(unsigned char *data, int rows);

	// Description: 处理通信设备控制数据变化突发
	// Author:      qianchenglin
	// Date:        2016-01-01
	// Parameter:   @data[in]数据内容指针
	//              @rows[in]数据条目数
	// Return:      处理成功返回true，否则返回false
	bool ProcessControlChanged(unsigned char *data, int rows);

	// Description: 处理IED设备数据变化突发
	// Author:      qianchenglin
	// Date:        2016-01-01
	// Parameter:   @data[in]数据内容指针
	//              @rows[in]数据条目数
	// Return:      处理成功返回true，否则返回false
	bool ProcessIedChanged(unsigned char *data, int rows);

	// Description: 事件突发回调
	static unsigned char* OnRealEventCallback(void* cbParam, SString &sTable, eMdbTriggerType eType,
		int iTrgRows,int iRowSize,unsigned char *pTrgData);

	// Description: 主事件循环线程
	// Author:      qianchenglin
	// Date:        2016-01-01
	// Parameter:   @lp[in]参数指针
	static void* ThreadMainLoop(void* lp);

protected:
	bool initStation();
	bool initScl(std::vector<Ied> &iedList);	// 根据模型文件初始化
	bool useModelType(std::string &filename);		// 返回true表示cid

private:
	std::map<std::string, std::vector<MMSServer*> > m_mmsServerList;
	SString m_sclFile;
	bool m_isScdMode;
	CSsp_Database *m_pDatabase;	//数据库配置文件
	int	node_no;//节点号
	bool is_period;//周期上送是否数据生效
	int	report_id;//注册报告号
	int m_stationNo;//子站号
	SString m_stationName;
	SLock m_Lock;
};
