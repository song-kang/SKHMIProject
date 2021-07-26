#ifndef _MMSCLT_APPLICATION_H_
#define _MMSCLT_APPLICATION_H_


#include "MMSClient.h"
#include "public/UK9010App.h"
#define MODULE_NAME "mst_mmsclt"

class MMSCLTApp : public CUK9010Application
{
public:
	enum RunMode
	{
		RunModeUnknown = 0,
		RunModeMainStation = 1,
		RunModeSubStation = 2,
	};

	// 地区规范
	enum RegionalSpecification
	{
		RS_Default = 0,
		RS_NWIR = 1,			// 南网智能录波器
	};

public:
	MMSCLTApp(void);
	virtual ~MMSCLTApp(void);

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
	virtual bool ProcessAgentMsg(WORD wMsgType,stuSpUnitAgentMsgHead *pMsgHead,SString &sHeadStr,BYTE* pBuffer=NULL,int iLength=0);

	inline void lock(){m_Lock.lock();}
	inline void unlock(){m_Lock.unlock();}

	// Description: 获取运行模式。目前分为主站运行模式和子站运行模式
	// Author:      qianchenglin
	// Date:        2016-01-01
	// Parameter:   void
	// Return:      返回RunMode枚举对象
	RunMode GetRunMode() { return m_runMode; }

private:
	CSsp_Database *m_pDatabase;	//数据库配置文件
	SPtrList<MMSClient>	m_MMSMgrs;	//装置管理队列
	int	node_no;//节点号
	bool is_period;//周期上送是否数据生效
	int	report_id;//注册报告号
	int substation_id;//子站号
	RunMode m_runMode;						// 运行模式
	RegionalSpecification m_regionalSpec;	// 地区规范
	SLock m_Lock;
	stuGlobalConfig m_globalConfig;
	SDateTime m_lastGenerateParamReportTime;

	static void* ThreadReadFile(void* lp); //定期读取文件线程
	static void* ThreadChannelMonitor(void* lp);	// 定时定值巡检和遥测刷新
#ifdef TEST_CODE
	static void* ThreadTestReadFile(void* lp);
#endif
private:
	/********************************************  
	* @brief 从运行设备列表中加载设备至内存
	* @author 宋康
	* @date 2015/12/24
	* @return 加载成功或失败 -true 成功 -false 失败
	*********************************************/ 
	bool LoadParam();

	bool LoadSubstation(int sub_no);
		
	bool LoadGlobalConfig();

	MMSClient *GetMmsClientBySubstation(int sub_no);
	MMSClient *GetMmsClientByIedNo(int sub_no, int ied_no);
	MMSClient *GetMmsClient(int sub_no, int ied_no);

	bool SendErrorAgentMsg(WORD wMsgType,stuSpUnitAgentMsgHead *head,SString strMsg="");

	bool ProcessReqSetting(WORD wMsgType,stuSpUnitAgentMsgHead *head,stuUK9010_MSG_REQ_SETTING *req_setting);

	bool ProcessReqSettingArea(WORD wMsgType,stuSpUnitAgentMsgHead *head,stuUK9010_MSG_REQ_SETTING_AREA *req_setting_area);

	bool ProcessReqEditSetting(WORD wMsgType,stuSpUnitAgentMsgHead *head,stuUK9010_MSG_REQ_EDIT_SETTING *req_edit_setting);

	bool ProcessReqEditSettingArea(WORD wMsgType,stuSpUnitAgentMsgHead *head,stuUK9010_MSG_REQ_EDIT_SETTING_AREA *req_edit_setting_area);

	bool ProcessReqCureSetting(WORD wMsgType,stuSpUnitAgentMsgHead *head,stuUK9010_MSG_REQ_CURE_SETTING *req_cure_setting);

	bool ProcessReqCtrlSelect(WORD wMsgType,stuSpUnitAgentMsgHead *head,stuUK9010_MSG_REQ_CTRL *req_ctrl);

	bool ProcessReqCtrlExecute(WORD wMsgType,stuSpUnitAgentMsgHead *head,stuUK9010_MSG_REQ_CTRL *req_ctrl);

	bool ProcessReqCtrlCancel(WORD wMsgType,stuSpUnitAgentMsgHead *head,stuUK9010_MSG_REQ_CTRL *req_ctrl);

	bool ProcessReqCtrlDirect(WORD wMsgType,stuSpUnitAgentMsgHead *head,stuUK9010_MSG_REQ_CTRL *req_ctrl);

	bool ProcessReqFileList(WORD wMsgType,stuSpUnitAgentMsgHead *head,stuUK9010_MSG_REQ_FILE_LIST *req_file);

	bool ProcessReqFileRead(WORD wMsgType,stuSpUnitAgentMsgHead *head,stuUK9010_MSG_REQ_FILE *req_file);

	bool ProcessReqJournalVmd(WORD wMsgType,stuSpUnitAgentMsgHead *head,stuUK9010_MSG_REQ_JOURNAL_VMD_SPEC *req_journal_vmd_spec);

	bool ProcessReqJournalRead(WORD wMsgType,stuSpUnitAgentMsgHead *head,stuUK9010_MSG_REQ_JOURNAL_READ *req_journal_read);

	bool ProcessReqJournalStateRead(WORD wMsgType,stuSpUnitAgentMsgHead *head,stuUK9010_MSG_REQ_JOURNAL_STATE_READ *req_journal_state_read);

	bool ProcessReqReadLeaf(WORD wMsgType,stuSpUnitAgentMsgHead *head, stuUK9010_MSG_REQ_READ_LEAF *req);
	bool ProcessReqReadLeaf(WORD wMsgType,stuSpUnitAgentMsgHead *head, stuUK9010_MSG_REQ_READ_LEAF_EX *req);

	bool ProcessReqSettingCheck(WORD wMsgType,stuSpUnitAgentMsgHead *head, stuUK9010_MSG_REQ_SETTING_CHECK *req);

	bool ProcessReqLatestCid(WORD wMsgType,stuSpUnitAgentMsgHead *head, stuUK9010_MSG_REQ_LATEST_CID *req);

	// 读取模拟量、开关量、压板，使用原读定值方式
	bool ProcessReqValue(WORD wMsgType,stuSpUnitAgentMsgHead *head,stuUK9010_MSG_REQ_SETTING *req_setting);

	SPtrList<stuSMmsFileInfo> m_iReadFiles;
	bool ReadFileList();
	bool ReadFile();
	bool IsDownFile(stuUK9010_MSG_RES_FILE_LIST *file,SString localFileName);
	bool CreateDailyParamCheckReport();
};

#endif	//_MMSCLT_APPLICATION_H_