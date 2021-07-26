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

#ifndef __UK9010_APPLICATION_H__
#define __UK9010_APPLICATION_H__

#include "SApplication.h"
#include "sp_config_mgr.h"
#include "ssp_base.h"
#include "ssp_database.h"
#include "sp_unit_config.h"

#define C_SOFTWARE_TYPE "UK9010"

//数据库执行异步同步到外网时的标记
#define C_SQL_ASYNC_RUN "/*async*/"

#ifndef DWORD
# define DWORD uint32_t
#endif

#ifndef WORD
# define WORD uint16_t
#endif

#ifndef BYTE
# define BYTE uint8_t
#endif

enum eUK9010AppNo
{
	UK9010_APP_HMI = SP_UA_APPNO_USER,	//101 图形工作站程序
	UK9010_APP_MMSCLT,					//102 MS客户端接入
	UK9010_APP_IA_MAIN,					//103 智能告警主程序
	UK9010_APP_NAI_MAIN,				//104 网分接口及网络管理主程序
	UK9010_APP_NPM_MAIN,				//105 网络报文存储模块
	UK9010_APP_MDBPLUGIN,				//106 内存库初始化插件
	UK9010_APP_OMS_MAIN,				//107 运维主站主控制台程序
	UK9010_APP_NAM_IEC104,				//108 浙江网分主站IEC104接入模块
	UK9010_APP_NAM_MAIN,				//109 网分主站主控制台程序
	UK9010_APP_NAM104CLT,				//110 网分主站IEC104接入模块 (废弃)
	UK9010_APP_SVG_FACTORY,				//111 SVG统一生成程序模块
	UK9010_APP_CHECK_SELF,				//112 网分主站自检模块
	UK9010_APP_MMSSVR,					//113 MMS服务端
	UK9010_APP_IA_DL476,				//114 智能告警DL476通信模块
	UK9010_APP_RMP_103,					//115 103保护通信
	UK9010_APP_FIW,						//116 故障智能预警模块
    UK9010_APP_ELE_FENCE,               //117 电子围栏采集模块
    UK9010_APP_MOUBUS_RTU,              //118 MOUBUS_RTU协议模块
	UK9010_APP_AIR_CONDITION,           //119 空调
	UK9010_APP_AIR_CONDITION_JDRK,      //120 建大仁科空调
    UK9010_APP_MODBUS_SIM,              //121 虚拟采集
	UK9010_APP_MJ_WEIGEN,               //122 微耕门禁
	UK9010_APP_LOCALDEV,                //123 本地设备信息
};

//////////////////////////////////////////////////////////////////////////
// 名    称:  eUK9010SysLogType
// 作    者:  邵凯田
// 创建时间:  2016-2-6 10:42
// 描    述:  系统日志类型
//////////////////////////////////////////////////////////////////////////
enum eUK9010SysLogType
{
	UK9010_LT_COMM_CONN = SP_UA_APPNO_USER,	//101 厂站通讯恢复
	UK9010_LT_COMM_DISC,				//102 厂站通讯断开
	UK9010_LT_CALL_SETTING,				//103 召唤定值
	UK9010_LT_CALL_ANALOG,				//104 召唤量测量
	UK9010_LT_CALL_STATE,				//105 召唤状态量
	UK9010_LT_CALL_RYABAN,				//106 召唤软压板
	UK9010_LT_EDIT_SETTING,				//107 编辑定值
	UK9010_LT_EDIT_SETTING_SUCC,		//108 编辑定值成功
	UK9010_LT_EDIT_SETTING_FAIL,		//109 编辑定值失败
	UK9010_LT_EDIT_RYABAN_SUCC,			//110 编辑软压板成功
	UK9010_LT_EDIT_RYABAN_FAIL,			//111 编辑软压板失败
	UK9010_LT_EDIT_SECTOR,				//112 编辑定值区				
	UK9010_LT_EDIT_SECTOR_SUCC,			//113 编辑定值区成功
	UK9010_LT_EDIT_SECTOR_FAIL,			//114 编辑定值区失败
	UK9010_LT_CTRL,						//115 控制操作
	UK9010_LT_CTRL_DIR_SUCC,			//116 直接控制成功
	UK9010_LT_CTRL_DIR_FAIL,			//117 直接控制失败
	UK9010_LT_CTRL_SEL_SUCC,			//118 选择控制成功
	UK9010_LT_CTRL_SEL_FAIL,			//119 选择控制失败
	UK9010_LT_SCD_IMP,					//120 厂站SCD模型导入
	UK9010_LT_SCD_CHG,					//121 厂站SCD文件变更
	UK9010_LT_SCD_HIS_DEL,				//122 厂站SCD文件历史文件自动清理
	UK9010_LT_EFILE_DIFF,				//123 远动E文件差异检出
	UK9010_LT_DEV_MMSLOG,				//124 设备MMS日志
	UK9010_LT_FILE,						//125 文件操作
	UK9010_LT_JOURNAL,					//126 日志操作
	UK9010_LT_NAM_SETTIME_SUCC,			//127 网分主站对下校时成功
	UK9010_LT_NAM_SETTIME_FAIL,			//127 网分主站对下校时失败

};

//////////////////////////////////////////////////////////////////////////
// 名    称:  eUK9010MsgType
// 作    者:  邵凯田
// 创建时间:  2016-2-6 10:42
// 描    述:  代理消息类型，各消息对应的ASDU加‘stu’前缀
//////////////////////////////////////////////////////////////////////////
enum eUK9010MsgType
{
	UK9010_MSG_REQ_SETTING =SP_UA_MSG_USER,		//101 申请定值
	UK9010_MSG_RES_SETTING,						//102 申请定值回复
	UK9010_MSG_REQ_SETTING_AREA,				//103 申请定值区
	UK9010_MSG_RES_SETTING_AREA,				//104 申请定值区回复
	UK9010_MSG_REQ_ANALOG,						//105 申请模拟量
	UK9010_MSG_RES_ANALOG,						//106 申请模拟量回复
	UK9010_MSG_REQ_SWITCH,						//107 申请开关量
	UK9010_MSG_RES_SWITCH,						//108 申请开关量回复
	UK9010_MSG_REQ_RYABAN,						//109 申请软压板
	UK9010_MSG_RES_RYABAN,						//110 申请软压板回复
	UK9010_MSG_REQ_EDIT_SETTING,				//111 申请修改定值
	UK9010_MSG_RES_EDIT_SETTING,				//112 申请修改定值回复
	UK9010_MSG_REQ_EDIT_SETTING_AREA,			//113 申请修改定值区
	UK9010_MSG_RES_EDIT_SETTING_AREA,			//114 申请修改定值区回复
	UK9010_MSG_REQ_CURE_SETTING,				//115 申请固化定值
	UK9010_MSG_RES_CURE_SETTING,				//116 申请固化定值回复
	UK9010_MSG_REQ_CTRL_SELECT,					//117 控制选择
	UK9010_MSG_RES_CTRL_SELECT,					//118 控制选择回复
	UK9010_MSG_REQ_CTRL_EXECUTE,				//119 控制执行
	UK9010_MSG_RES_CTRL_EXECUTE,				//120 控制执行回复
	UK9010_MSG_REQ_CTRL_CANCEL,					//121 控制撤消
	UK9010_MSG_RES_CTRL_CANCEL,					//122 控制撤消回复
	UK9010_MSG_REQ_CTRL_DIRECT,					//123 直接控制
	UK9010_MSG_RES_CTRL_DIRECT,					//124 直接控制回复
	UK9010_MSG_REQ_FILE_LIST,					//125 申请文件列表
	UK9010_MSG_RES_FILE_LIST,					//126 申请文件列表回复
	UK9010_MSG_REQ_FILE_READ,					//127 申请文件读取
	UK9010_MSG_RES_FILE_READ,					//128 申请文件列表回复
	UK9010_MSG_REQ_JOURNAL_VMD_SPEC,			//129 申请日志值名
	UK9010_MSG_RES_JOURNAL_VMD_SPEC,			//130 申请日志值名回复
	UK9010_MSG_REQ_JOURNAL_READ,				//131 申请日志读取
	UK9010_MSG_RES_JOURNAL_READ,				//132 申请日志读取回复
	UK9010_MSG_REQ_JOURNAL_STATE_READ,			//133 申请日志状态
	UK9010_MSG_RES_JOURNAL_STATE_READ,			//134 申请日志状态回复

	UK9010_MSG_REQ_LIST_SCDFILE,				//135 取指定厂站SCD文件信息列表
	UK9010_MSG_RES_LIST_SCDFILE,				//136 取指定厂站SCD文件信息列表回复
	UK9010_MSG_REQ_NEW_SCDFILE,					//137 新版本SCD文件存入请求（给出厂站标识）
	UK9010_MSG_RES_NEW_SCDFILE,					//138 新版本SCD文件存入回复（回复新文件的目录及文件名）
	UK9010_MSG_NEWSCD_CHECK,					//139 新版本SCD文件差异比对（在通过代理成功上传后）
	UK9010_MSG_NEWSCD_CHECK_START,				//140 SCD差异比对任务启动（Result为0表示成功，1表示失败）
	UK9010_MSG_NEWSCD_CHECK_END,				//141 SCD差异比对任务终止（Result为0表示成功，1表示失败）
	UK9010_MSG_DEL_SCDFILE,						//142 删除指定厂站及文件名的SCD文件版本信息

	UK9010_MSG_REQ_CATCH_PACKAGE_START,			//143 请求网分模块开始抓包开始
	UK9010_MSG_RES_CATCH_PACKAGE_START,			//144 响应网分模块开始抓包开始
	UK9010_MSG_REQ_CATCH_PACKAGE_STOP,			//145 请求停止网分模块抓包
	UK9010_MSG_RES_CATCH_PACKAGE_STOP,			//146 响应停止网分模块抓包
	UK9010_MSG_CATCH_PACKAGE_ABORT,				//147 中止网分模块抓包
	UK9010_MSG_REAL_CATCHED_PACKAGE,			//148 回复实时抓包的报文内容
	UK9010_MSG_REQ_SEARCH_PACKAGE_PCAP,			//149 请求检索指定条件的报文到PCAP文件
	UK9010_MSG_RES_SEARCH_PACKAGE_PCAP,			//150 响应检索指定条件的报文到PCAP文件

	UK9010_MSG_NAM_104_STARTCAPTURE,			//151 浙江网分IEC104采集通讯报文开始捕获,sHeadS为：sub_no=厂站号;
	UK9010_MSG_NAM_104_STOPCAPTURE,				//152 浙江网分IEC104采集通讯报文停止捕获,sHeadS为：sub_no=厂站号;
	UK9010_MSG_NAM_104_PACKAGE,					//153 浙江网分IEC104采集通讯实时报文,sHead为：sub_no=厂站号;mip=主站IP;mport=主站端口;sip=子站IP;sport=子站端口;way=send/recv;
	UK9010_MSG_NAM_REQ_SETTIME,					//154 对子站校时请求,sHead为：[sub_no=厂站号];[time=yyyy-MM-dd hh:mm:ss.zzz];time不输入时使用本地时间对厂站校时
	UK9010_MSG_NAM_RES_SETTIME,					//155 对子站校时回复,sHead为：[sub_no=厂站号];result=0/1;desc=操作结果描述  sub_no表示对应的厂站,desc表示操作结果的文本描述
	UK9010_MSG_NAM_REQ_GI,						//156 对子站总召唤,sHead为：[sub_no=厂站号];
	UK9010_MSG_NAM_REQ_HISQUERY,				//157 对子站检索历史信息,sHead为：sub_no=厂站号;cl_id=通信链接编号;point_id=信息点号;type=0/1/2/3;from=yyyy-MM-dd hh:mm:ss;to=yyyy-MM-dd hh:mm:ss;
																					//为通信链接的全局唯一编号
																					//point_id表示信息点号，0表示表示召唤该类信息的全部条目
																					//type表示召唤信息类型:      0：召唤“状态”、“事件”和“统计”类信息；
																											// 	1：召唤“状态”类信息；
																											// 	2：召唤“事件”类信息；
																											// 	3：召唤“统计”类信息；
																					//from/to表示检索的开始结束时间段
	UK9010_MSG_NAM_RES_HISQUERY_ITEM,			//158 回复子站历史信息,m_dwParamResult中存放记录数量，记录为stuUK9010_MSG_NAM_RES_HISQUERY_ITEM数组，可以携带1条或多条
	UK9010_MSG_NAM_RES_HISQUERY_FINISHED,		//159 回复子站历史信息检索结束

												//模拟私有事件明细及信息号的命令，每条信息均带具体的时间信息，多条事件可以成批发送，当两个相邻事件时间间隔大于1秒时，实际发送的间隔时间尽可能不大于事件间的时间间隔
	UK9010_MSG_NAM_PRIVATE_EVTDETAIL,			//160 模拟一条私有事件明细信息,sHead为:sub_no=厂站号;缓冲区对应一个stuUK9010_MSG_NAM_PRIVATE_EVTDETAIL结构和yk_steps数量的stuUK9010_MSG_NAM_PRIVATE_EVTDETAIL_YKSTEP结构，m_dwParamResult中存放步骤记录的数量
	UK9010_MSG_NAM_PRIVATE_POINT,				//161 模拟一条私有的遥信点、MMS事件点明细信息，,sHead为:sub_no=厂站号;缓冲区为stuUK9010_MSG_NAM_PRIVATE_POINT数组，可携带一条或多条，m_dwParamResult中存放记录数量，
	UK9010_MSG_NAM_DB_SYN_LINK_STATE,			//162 数据库同步通信状态，sHead为:state=1/2; (1表示通信正常，2表示通信断开)
	UK9010_MSG_NAM_SUBSTATION_COMM_START,		//163 启动厂站通信（如果已运行则重启），sHead为:sub_no=厂站号;  可用于HMI发送给通信程序表示启动命令，也可用于通信程序发送给HMI表示启动成功
	UK9010_MSG_NAM_SUBSTATION_COMM_STOP,		//164 停止厂站通信，sHead为:sub_no=厂站号;  可用于HMI发送给通信程序表示启动命令，也可用于通信程序发送给HMI表示启动成功
	UK9010_MSG_NAM_RES_HISQUERY_FAILED,			//165 回复子站历史信息检索失败，sHead为: sub_no=厂站号;err=检索失败原因描述;

	UK9010_MSG_REQ_LEAF_VALUE,					// 请求节点路径值
	UK9010_MSG_RES_LEAF_VALUE,					// 回复节点路径值
	UK9010_MSG_REQ_ENTRY_VALUE,					// 请求条目值
	UK9010_MSG_RES_ENTRY_VALUE,					// 回复条目值
	UK9010_MSG_REQ_SETTING_CHECK,				// 申请定值校核
	UK9010_MSG_RES_SETTING_CHECK,				// 申请定值校核回复
	UK9010_MSG_REQ_LATEST_CID,					// 请求最新的CID文件
	UK9010_MSG_RES_LATEST_CID,					// 请求最新的CID文件回复

	UK9010_MSG_REQ_SET_VALUE,					//设值
	UK9010_MSG_RES_SET_VALUE,					//设值回复
	UK9010_MSG_REQ_GET_VALUE,					//取值
	UK9010_MSG_RES_GET_VALUE,					//取值回复
};


#pragma pack(push)
#pragma pack(1)

struct stuUK9010_MSG_REQ_LIST_SCDFILE
{
	int iSubNo;//表示厂站号
};

struct stuUK9010_MSG_RES_LIST_SCDFILE
{
	char sPathFile[128];//文件全路径
	int iFileSize;//文件大小（字节）
};

struct stuUK9010_MSG_REQ_NEW_SCDFILE
{
	int iSubNo;//表示厂站号
};

struct stuUK9010_MSG_RES_NEW_SCDFILE
{
	int iSubNo;//表示厂站号
	char sPathFile[128];//新文件的
};

struct stuUK9010_MSG_REQ_SETTING
{
	int iSubNo;//表示厂站号
	int iDevNo;//表示装置号
	int iCpuNo;//表示CPU号，-1表示所有CPU
	int iSectorNo;//表示定值区号，-1表示当前定值区
	int iGroupNo;//表示组号，-1表示所有组
	int iEntryNo;//表示条目号，-1表示所有条目
};

struct stuUK9010_MSG_RES_SETTING
{
	int iSubNo;//表示厂站号
	int iDevNo;//表示装置号
	int iCpuNo;//表示CPU号，-1表示所有CPU
	int iSectorNo;//表示定值区号，-1表示当前定值区
	int iGroupNo;//表示组号，-1表示所有组
	int iEntryNo;//表示条目号，-1表示所有条目
	char visable_value[64];//可视的值内容
};

struct stuUK9010_MSG_REQ_SETTING_AREA
{
	int iSubNo;//表示厂站号
	int iDevNo;//表示装置号
	int iCpuNo;//表示CPU号，-1表示所有CPU
	int iSectorNo;//表示定值区号，-1表示当前定值区
};

struct stuUK9010_MSG_RES_SETTING_AREA
{
	int iSubNo;//表示厂站号
	int iDevNo;//表示装置号
	int iCpuNo;//表示CPU号，-1表示所有CPU
	int iSectorNo;//表示定值区号，-1表示当前定值区
	char visable_value[64];//可视的值内容
};

struct stuUK9010_MSG_REQ_EDIT_SETTING
{
	int iSubNo;//表示厂站号
	int iDevNo;//表示装置号
	int iCpuNo;//表示CPU号
	int iSectorNo;//表示定值区号，-1表示当前定值区
	int iGroupNo;//表示组号
	int iEntryNo;//表示条目号
	char visable_value[64];//可视的值内容
};

struct stuUK9010_MSG_RES_EDIT_SETTING
{
	int iRet;	// 1成功；0失败
};

struct stuUK9010_MSG_REQ_EDIT_SETTING_AREA
{
	int iSubNo;//表示厂站号
	int iDevNo;//表示装置号
	int iCpuNo;//表示CPU号，-1表示所有CPU
	int iSectorNo;//表示定值区号，-1表示当前定值区
	int iArea;//写定值区内容
};

struct stuUK9010_MSG_REQ_CURE_SETTING
{
	int iSubNo;//表示厂站号
	int iDevNo;//表示装置号
	int iCpuNo;//表示CPU号
	int iGroupNo;//表示组号
};

struct stuUK9010_MSG_REQ_CTRL
{
	int iSubNo;//表示厂站号
	int iDevNo;//表示装置号
	int iCpuNo;//表示CPU号
	int iGroupNo;//表示组号
	int iEntryNo;//表示条目号
	char visable_value[64];//可视的值内容
};

struct stuUK9010_MSG_RES_CTRL
{
	int result; // 1成功；0失败
};

struct stuUK9010_MSG_REQ_SET_VALUE
{
	int iSubNo;//表示厂站号
	int iDevNo;//表示装置号
	int iCpuNo;//表示CPU号
	int iGroupNo;//表示组号
	int iEntryNo;//表示条目号
	char visable_value[64];//可视的值内容
};

struct stuUK9010_MSG_RES_SET_VALUE
{
	int result; // 1成功；0失败
};

struct stuUK9010_MSG_REQ_GET_VALUE
{
	int iSubNo;//表示厂站号
	int iDevNo;//表示装置号
	int iCpuNo;//表示CPU号
	int iGroupNo;//表示组号
	int iEntryNo;//表示条目号
};

struct stuUK9010_MSG_RES_GET_VALUE
{
	int result;	// 1成功；0失败
};

struct stuUK9010_MSG_REQ_FILE
{
	int iSubNo;//表示厂站号
	int iDevNo;//表示装置号
	char visable_value[512];//可视的值内容
};

struct stuUK9010_MSG_REQ_FILE_LIST
{
	int iSubNo;//表示厂站号
	int iDevNo;//表示装置号
	int socFrom;
	int socTo;
	char visable_value[512];//可视的值内容 
};

struct stuUK9010_MSG_RES_FILE
{
	char local_path[256];//本地存储路径
};

struct stuUK9010_MSG_RES_FILE_LIST
{
	char	fileName[256];//文件名称
	int		fileSize;//文件大小
	time_t	fileTime;//文件时间
	bool	follow;//是否有后继文件,有后续文件true，否则false（不同于61850服务） 
};

struct stuUK9010_MSG_REQ_JOURNAL_VMD_SPEC
{
	int iSubNo;//表示厂站号
	int iDevNo;//表示装置号
	char visable_value[64];//可视的值内容
};

struct stuUK9010_MSG_RES_JOURNAL_VMD_SPEC
{
	char domName[128];//域名
	char varName[128];//值名
	bool follow;
};

struct stuUK9010_MSG_REQ_JOURNAL_READ
{
	int  iSubNo;//表示厂站号
	int iDevNo;//表示装置号
	char domName[128];//域名
	char varName[128];//值名
	unsigned char entry_id[8];//条目序列值
	int	 socStart;//开始时间
	int	 socEnd;//结束时间
};

struct stuUK9010_MSG_RES_JOURNAL_READ
{
	int	 soc;//世纪秒
	int	 usec;//微秒
	char desc[128];//日志信息描述
	char entry_id[8];//条目序列值
	char visable_value[64];//可视的值内容
	bool follow;
};

struct stuUK9010_MSG_REQ_JOURNAL_STATE_READ
{
	int  iSubNo;//表示厂站号
	int iDevNo;//表示装置号
	char domName[128];//域名
	char varName[128];//值名
};

struct stuUK9010_MSG_RES_JOURNAL_STATE_READ
{
	bool ret;
	int entryNum;
	bool deletable;
};


struct stuUK9010_MSG_NAM_RES_HISQUERY_ITEM
{
	int sub_no;//=厂站号;
	int dev_sn;//=设备唯一编号;
	int point_id;//=信息点号(对应状态、事件、统计的主键);
	int time;//=yyyy-MM-dd hh:mm:ss.zzz;
	int usec;
	int val;//=SPI或统计值;
	int count;//=信息点重复次数;
	unsigned char type;//=1/2/3（同上定义）;
	unsigned char iv;//=0/1(品质是否无效);
	unsigned char bl;//=0/1(品质是否闭锁);
};

struct stuUK9010_MSG_NAM_PRIVATE_EVTDETAIL
{
	int dev_sn;//网分事件对应的设备唯一编号
	int point_id;//=信息点号(对应事件的主键);
	int time;//=yyyy-MM-dd hh:mm:ss
	int local_soc;
	int usec;
	unsigned char val;//=SPI值;
	unsigned char yk_type;//0表示MMS遥控, 1表示IEC104遥控,2表示主站侧IEC104遥控，3表示主站侧遥信,255表示非遥控
	int link_ied_no;//yk_type为0时对应IED唯一编号
					//yk_type为1时对应t_oe_comm_system ->comm_id
	int link_cpu_no;//yk_type为0时对应IED的CPU号
					//yk_type=1时无效
	int link_group_no;//yk_type为0时对应IED的组号
					//yk_type=1时对应t_nam_yd_point ->group_type
	int link_entry;//yk_type为0时对应IED的条目号
					//yk_type=1时对应t_nam_yd_point ->point_id
	int yk_steps;//遥控步骤数量，遥控使用，其它事件为0
	char ctrl_name[64];//针对遥控事件时表示控点名称，针对其他事件时表示事件对应的通道描述
	//后面跟随yk_steps个stuUK9010_MSG_NAM_PRIVATE_EVTDETAIL_YKSTEP
};

struct stuUK9010_MSG_NAM_PRIVATE_EVTDETAIL_YKSTEP
{
	int step_type;//步骤类型，定义见T_NAM_HIS_YK_DETAIL附表
	int time;//soc =yyyy-MM-dd hh:mm:ss
	int usec;
	int way;//0表示主控侧发给被控侧，1表示被控侧回复主控侧
	int result;//0:无，1：肯定应答，2：否定应答，3：未应答
};

struct stuUK9010_MSG_NAM_PRIVATE_POINT
{
	int link_ied_no;//关联信息点的装置号,哪from=3时对应104点表，下同
	int link_cpu_no;
	int link_group_no;
	int link_entry;
	int time;//=yyyy-MM-dd hh:mm:ss
	int local_soc;
	int usec;
	int from;//1:MMS; 2:GOOSE; 3:IEC104; 10:IEC104(主站侧)
	int dpi;//0:未知，1:分/复归，2:合/动作
	char info_desc[128];//信息的详细描述，含通道名称、当前值信息
};

struct stuUK9010_MSG_REQ_READ_LEAF
{
	int iSubNo;//表示厂站号
	int iDevNo;//表示装置号
	char domName[128];//域名
	char varName[128];//值名
};

struct stuUK9010_MSG_REQ_READ_LEAF_EX
{
	int iCommandNo;//命令序号
	int iSubNo;//表示厂站号
	int iDevNo;//表示装置号
	char domName[128];//域名
	char varName[128];//值名
};

struct stuUK9010_MSG_REQ_READ_ENTRY
{
	int iSubNo;//表示厂站号
	int iDevNo;//表示装置号
	int iCpuNo;
	int iGroupNo;
	int iEntry;
};

struct stuUK9010_MSG_RES_READ_ENTRY
{
	bool ret;
	char value[256];
};

struct stuUK9010_MSG_RES_READ_LEAF
{
	bool ret;
	int type;
	char value[256];
};

struct stuUK9010_MSG_RES_READ_LEAF_EX
{
	int iCommandNo;
	bool ret;
	int type;
	char value[256];
};

struct stuUK9010_MSG_REQ_SETTING_CHECK
{
	int iSubNo;//表示厂站号
	int iDevNo;//表示装置号
	int iCpuNo;//表示CPU号
	int iGroupNo;//表示组号
};

struct stuUK9010_MSG_RES_SETTING_CHECK
{
	bool ret;
};

struct stuUK9010_MSG_REQ_LATEST_CID
{
	int iSubNo;//表示厂站号
	int iDevNo;//表示装置号
	int iType; //0:CID;1:ICD;2:SCD;3:CCD
};

struct stuUK9010_MSG_RES_LATEST_CID
{
	bool ret;
	char path[256];
};

#pragma pack(4)
#pragma pack(pop)


class CUK9010Application : public SApplication
{
public:
	CUK9010Application();
	virtual ~CUK9010Application();

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
	virtual bool ProcessAgentMsg(unsigned short wMsgType,stuSpUnitAgentMsgHead *pMsgHead,SString &sHeadStr,unsigned char* pBuffer=NULL,int iLength=0);

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
