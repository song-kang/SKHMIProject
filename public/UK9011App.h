/**
 *
 * 文 件 名 : UK9010App.h
 * 创建日期 : 2016-2-6 9:38
 * 作    者 : 邵凯田(skt001@163.com)
 * 修改日期 : $Date: $
 * 当前版本 : $Revision: $
 * 功能描述 : UK9010运维主站应用基础类
 * 修改记录 : 
 *            $Log: $
 *
 * Ver  Date        Author  Comments
 * ---  ----------  ------  -------------------------------------------
 * 001	2016-2-6	邵凯田　创建文件
 *
 **/

#ifndef __UK9011_APPLICATION_H__
#define __UK9011_APPLICATION_H__

#include "SApplication.h"
#include "sp_config_mgr.h"
#include "ssp_base.h"
#include "ssp_database.h"
#include "sp_unit_config.h"
//#include "mib_window_cfg.h"
//#include "switch_cfg_fun.h"
//#include "switch_cfg.h"
#define C_SOFTWARE_TYPE "UK9011"

#ifndef OMS_MYSQL_DB
#define OMS_MYSQL_DB
#endif

#define OMCCFG_MIBWND SPCFG_USER
#define OMCCFG_MIBFUN SPCFG_USER2

enum eUK9011AppNo
{
	UK9011_APP_HMI = SP_UA_APPNO_USER,	//101 图形工作站程序
	UK9011_APP_MMSCLT,					//102 MS客户端接入
	UK9011_APP_IA_MAIN,					//103 智能告警主程序
	UK9011_APP_NAI_MAIN,				//104 网分接口及网络管理主程序
	UK9011_APP_NPM_MAIN,				//105 网络报文存储模块
	UK9011_APP_MDBPLUGIN,				//106 内存库初始化插件
	UK9011_APP_OMS_MAIN,				//107 运维主站主控制台程序
	UK9011_APP_NAM_IEC104,				//108 浙江网分主站IEC104接入模块
	UK9011_APP_NAM_MAIN,				//109 网分主站主控制台程序
	UK9011_APP_NAM104CLT,				//110 网分主站IEC104接入模块
	UK9011_APP_SVG_FACTORY,				//111 SVG统一生成程序模块
	UK9011_APP_SMART_RTU104				//112 智能远动机一体化配置扩展104模块
};

//////////////////////////////////////////////////////////////////////////
// 名    称:  eUK9010SysLogType
// 作    者:  邵凯田
// 创建时间:  2016-2-6 10:42
// 描    述:  系统日志类型
//////////////////////////////////////////////////////////////////////////
enum eUK9011SysLogType
{
	UK9011_LT_COMM_CONN = SP_UA_APPNO_USER,	//101 交换机通讯恢复
	UK9011_LT_COMM_DISC,				//102 交换机通讯断开
	UK9011_LT_SWITCH_CFG_UPD,			//103 交换机配置下装
	UK9011_LT_SWITCH_CFG_DIFF,			//104 交换机配置不一致
	UK9011_LT_SWITCH_WARN,				//105 交换机告警

	UK9011_LT_CALL_RYABAN,				//106 召唤软压板
	UK9011_LT_EDIT_SETTING,				//107 编辑定值
	UK9011_LT_EDIT_SETTING_SUCC,		//108 编辑定值成功
	UK9011_LT_EDIT_SETTING_FAIL,		//109 编辑定值失败
	UK9011_LT_EDIT_RYABAN_SUCC,			//110 编辑软压板成功
	UK9011_LT_EDIT_RYABAN_FAIL,			//111 编辑软压板失败
	UK9011_LT_EDIT_SECTOR,				//112 编辑定值区				
	UK9011_LT_EDIT_SECTOR_SUCC,			//113 编辑定值区成功
	UK9011_LT_EDIT_SECTOR_FAIL,			//114 编辑定值区失败
	UK9011_LT_CTRL,						//115 控制操作
	UK9011_LT_CTRL_DIR_SUCC,			//116 直接控制成功
	UK9011_LT_CTRL_DIR_FAIL,			//117 直接控制失败
	UK9011_LT_CTRL_SEL_SUCC,			//118 选择控制成功
	UK9011_LT_CTRL_SEL_FAIL,			//119 选择控制失败
	UK9011_LT_SCD_IMP,					//120 厂站SCD模型导入
	UK9011_LT_SCD_CHG,					//121 厂站SCD文件变更
	UK9011_LT_SCD_HIS_DEL,				//122 厂站SCD文件历史文件自动清理
	UK9011_LT_EFILE_DIFF,				//123 远动E文件差异检出
	UK9011_LT_DEV_MMSLOG,				//124 设备MMS日志
	UK9011_LT_FILE,						//125 文件操作
	UK9011_LT_JOURNAL,					//126 日志操作
	UK9011_LT_NAM_SETTIME_SUCC,			//127 网分主站对下校时成功
	UK9011_LT_NAM_SETTIME_FAIL,			//127 网分主站对下校时失败
	UK9011_LT_YD_MAINTENANCE = 201,     // 201 广东远动设备一体化运维配置日志
	UK9011_LT_HT_MAINTENANCE,           // 202 广东后台设备一体化运维配置日志
	UK9011_LT_CK_MAINTENANCE,           // 203 广东测控设备一体化运维配置日志

};

//////////////////////////////////////////////////////////////////////////
// 名    称:  eUK9010MsgType
// 作    者:  邵凯田
// 创建时间:  2016-2-6 10:42
// 描    述:  代理消息类型，各消息对应的ASDU加‘stu’前缀
//////////////////////////////////////////////////////////////////////////
enum eUK9011MsgType
{
	UK9011_MSG_REQ_SETTING =SP_UA_MSG_USER,		//101 申请定值
	UK9011_MSG_RES_SETTING,						//102 申请定值回复
	UK9011_MSG_REQ_SETTING_AREA,				//103 申请定值区
	UK9011_MSG_RES_SETTING_AREA,				//104 申请定值区回复
	UK9011_MSG_REQ_ANALOG,						//105 申请模拟量
	UK9011_MSG_RES_ANALOG,						//106 申请模拟量回复
	UK9011_MSG_REQ_SWITCH,						//107 申请开关量
	UK9011_MSG_RES_SWITCH,						//108 申请开关量回复
	UK9011_MSG_REQ_RYABAN,						//109 申请软压板
	UK9011_MSG_RES_RYABAN,						//110 申请软压板回复
	UK9011_MSG_REQ_EDIT_SETTING,				//111 申请修改定值
	UK9011_MSG_RES_EDIT_SETTING,				//112 申请修改定值回复
	UK9011_MSG_REQ_EDIT_SETTING_AREA,			//113 申请修改定值区
	UK9011_MSG_RES_EDIT_SETTING_AREA,			//114 申请修改定值区回复
	UK9011_MSG_REQ_CURE_SETTING,				//115 申请固化定值
	UK9011_MSG_RES_CURE_SETTING,				//116 申请固化定值回复
	UK9011_MSG_REQ_CTRL_SELECT,					//117 控制选择
	UK9011_MSG_RES_CTRL_SELECT,					//118 控制选择回复
	UK9011_MSG_REQ_CTRL_EXECUTE,				//119 控制执行
	UK9011_MSG_RES_CTRL_EXECUTE,				//120 控制执行回复
	UK9011_MSG_REQ_CTRL_CANCEL,					//121 控制撤消
	UK9011_MSG_RES_CTRL_CANCEL,					//122 控制撤消回复
	UK9011_MSG_REQ_CTRL_DIRECT,					//123 直接控制
	UK9011_MSG_RES_CTRL_DIRECT,					//124 直接控制回复
	UK9011_MSG_REQ_FILE_LIST,					//125 申请文件列表
	UK9011_MSG_RES_FILE_LIST,					//126 申请文件列表回复
	UK9011_MSG_REQ_FILE_READ,					//127 申请文件读取
	UK9011_MSG_RES_FILE_READ,					//128 申请文件列表回复
	UK9011_MSG_REQ_JOURNAL_VMD_SPEC,			//129 申请日志值名
	UK9011_MSG_RES_JOURNAL_VMD_SPEC,			//130 申请日志值名回复
	UK9011_MSG_REQ_JOURNAL_READ,				//131 申请日志读取
	UK9011_MSG_RES_JOURNAL_READ,				//132 申请日志读取回复
	UK9011_MSG_REQ_JOURNAL_STATE_READ,			//133 申请日志状态
	UK9011_MSG_RES_JOURNAL_STATE_READ,			//134 申请日志状态回复

	UK9011_MSG_SWITCH_TOPO_UPDATE,				//135 交换机拓扑发生改变的通知
	UK9011_MSG_REQ_SMART_RTU104,				//136 智能远动机一体化配置问询
	UK9011_MSG_RES_SMART_RTU104,				//136 智能远动机一体化配置回复
};


#pragma pack(push)
#pragma pack(1)



#pragma pack(4)
#pragma pack(pop)


class CUK9011Application : public SApplication
{
public:
	CUK9011Application();
	virtual ~CUK9011Application();

	//////////////////////////////////////////////////////////////////////////
	// 描    述:  启动服务，开启所有应用的服务，该函数必须由派生类实现，且启动服务完成后必须返回
	// 作    者:  邵凯田
	// 创建时间:  2015-7-30 16:05
	// 参数说明:  void
	// 返 回 值:  true/false
	//////////////////////////////////////////////////////////////////////////
	virtual bool Start();

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
	// 描    述:  将SQL语句中日期类型转为字符串类型字段
	// 作    者:  邵凯田
	// 创建时间:  2016-5-6 16:23
	// 参数说明:  @sField为字段名称
	// 返 回 值:  根据不同的数据库类型而不同
	//////////////////////////////////////////////////////////////////////////
	static SString Date2String(SString sField);

	//////////////////////////////////////////////////////////////////////////
	// 描    述:  将SQL语句中字符串改为日期类型
	// 作    者:  邵凯田
	// 创建时间:  2016-5-6 16:25
	// 参数说明:  @sDateTime格式为:yyyy-MM-dd hh:mm:ss
	// 返 回 值:  根据不同的数据库类型而不同
	//////////////////////////////////////////////////////////////////////////
	static SString String2Date(SString sDateTime);

	CUnitConfig m_UnitConfig;

};

#endif//__UK9010_APPLICATION_H__
