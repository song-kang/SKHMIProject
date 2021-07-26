/**
 *
 * 文 件 名 : NaModel.h
 * 创建日期 : 2017-8-24 11:09
 * 作    者 : 邵凯田(skt001@163.com)
 * 修改日期 : $Date: $
 * 当前版本 : $Revision: $
 * 功能描述 : 网络监视模型
 * 修改记录 : 
 *            $Log: $
 *
 * Ver  Date        Author  Comments
 * ---  ----------  ------  -------------------------------------------
 * 001	2017-8-24	邵凯田　创建文件
 *
 **/

#ifndef __NA_MODEL_H__
#define __NA_MODEL_H__

#include "SString.h"
#include "SList.h"
#include "SXmlConfig.h"
#include "SFile.h"
#include "SDateTime.h"
#include "ssp_base.h"
#include <map>

class CNaModelMgr;
class CNaBaseModel
{
public:
	CNaBaseModel();
	~CNaBaseModel();

	//////////////////////////////////////////////////////////////////////////
	// 描    述:  从数据库备份网络信息到指定目录，当内容未改变时不更新
	// 作    者:  邵凯田
	// 创建时间:  2017-8-24 14:45
	// 参数说明:  @sPath为目录名称
	// 返 回 值:  int, >=0表示备份表数量，<0表示失败
	//////////////////////////////////////////////////////////////////////////
	int BackupFromDb(SString sPath);

	//////////////////////////////////////////////////////////////////////////
	// 描    述:  还原网络信息数据到数据库，如文件修改时间较上一次操作未变则忽略还原
	// 作    者:  邵凯田
	// 创建时间:  2017-8-24 14:33
	// 参数说明:  @sPath为目录名称
	// 返 回 值:  int,>=0表示成功还原表数量，<0表示失败
	//////////////////////////////////////////////////////////////////////////
	int ChkRestoreToDb(SString sPath);

	//////////////////////////////////////////////////////////////////////////
	// 描    述:  还原网络信息数据到数据库，
	// 作    者:  邵凯田
	// 创建时间:  2017-8-24 14:33
	// 参数说明:  @sPath为目录名称
	// 返 回 值:  int,>=0表示成功还原表数量，<0表示失败
	//////////////////////////////////////////////////////////////////////////
	int RestoreToDb(SString sPathFile);

	SString m_sTableName;
	SString m_sFieldNames;
	SString m_sFieldTypes;//int,str,float,blob
	SString m_sOrderBy;
	int m_iLastRestoreTime;//最后一次恢复XML的时间
	SString m_sLastBackupXml;//最后一次备份的XML文件
	SXmlConfig m_Config;
	CNaModelMgr *m_pMgr;
};

class CNaModelMgr
{
public:
	CNaModelMgr();
	~CNaModelMgr();

	//////////////////////////////////////////////////////////////////////////
	// 描    述:  重新建立NETID的映射记录
	// 作    者:  邵凯田
	// 创建时间:  2017-8-24 16:22
	// 参数说明:  @num表示记录数量
	// 返 回 值:  void
	//////////////////////////////////////////////////////////////////////////
	//void NewNetIdMap(int num);


	//////////////////////////////////////////////////////////////////////////
	// 描    述:  添加NETID的子站到主站映射
	// 作    者:  邵凯田
	// 创建时间:  2018-6-1 17:01
	// 参数说明:  
	// 返 回 值:  
	//////////////////////////////////////////////////////////////////////////
	void AddNewNetId(int sub_net_id,int main_net_id);

	//////////////////////////////////////////////////////////////////////////
	// 描    述:  添加COMID的子站到主站映射
	// 作    者:  邵凯田
	// 创建时间:  2018-6-1 17:01
	// 参数说明:  
	// 返 回 值:  
	//////////////////////////////////////////////////////////////////////////
	void AddNewComId(int sub_com_id,int main_com_id);

	//////////////////////////////////////////////////////////////////////////
	// 描    述:  取主站NETID
	// 作    者:  邵凯田
	// 创建时间:  2018-6-1 17:02
	// 参数说明:  
	// 返 回 值:  
	//////////////////////////////////////////////////////////////////////////
	int GetMainNetId(int sub_net_id);

	//////////////////////////////////////////////////////////////////////////
	// 描    述:  取主站COMID
	// 作    者:  邵凯田
	// 创建时间:  2018-6-1 17:02
	// 参数说明:  
	// 返 回 值:  
	//////////////////////////////////////////////////////////////////////////
	int GetMainComId(int sub_com_id);

	//////////////////////////////////////////////////////////////////////////
	// 描    述:  从数据库备份网络信息到指定目录，当内容未改变时不更新
	// 作    者:  邵凯田
	// 创建时间:  2017-8-24 14:32
	// 参数说明:  @sPath为目录名称
	// 返 回 值:  int, >=0表示备份表数量，<0表示失败
	//////////////////////////////////////////////////////////////////////////
	int BackupFromDb(SString sPath);

	//////////////////////////////////////////////////////////////////////////
	// 描    述:  还原网络信息数据到数据库，如文件修改时间较上一次操作未变则忽略还原
	// 作    者:  邵凯田
	// 创建时间:  2017-8-24 14:33
	// 参数说明:  @sPath为目录名称
	// 返 回 值:  int,>=0表示成功还原表数量，<0表示失败
	//////////////////////////////////////////////////////////////////////////
	int RestoreToDb(SString sPath);

	CNaBaseModel m_na_sub_network;			//net_id
	CNaBaseModel m_na_comm_device;			//com_id
	CNaBaseModel m_na_comm_device_port;		//net_id,dev_cls,dev_id,port_id
	CNaBaseModel m_na_comm_connect_line;	//net_id_a,dev_cls_a,dev_id_a,port_id_a,net_id_b,dev_cls_b,dev_id_b,port_id_b
	CNaBaseModel m_na_iec104_session;		//sub_no,client_ip,server_ip,client_port,server_port
	CNaBaseModel m_na_mms_session;			//sub_no,client_ip,server_ip,client_port,server_port
// 	CNaBaseModel m_na_unit;					//na_no
// 	CNaBaseModel m_na_capture_port;			//na_no,port_no
// 	CNaBaseModel m_na_comm_mac_addr;		//na_no,ip_addr,mac_addr
// 	CNaBaseModel m_na_gse_dataset_item;		//sub_no,appid,chn_no
// 	CNaBaseModel m_na_gse_event_cnt;
// 	CNaBaseModel m_na_gse_realflow;
// 	CNaBaseModel m_na_port_event_cnt;
// 	CNaBaseModel m_na_port_realflow;
// 	CNaBaseModel m_na_sv_event_cnt;
// 	CNaBaseModel m_na_sv_realflow;
// 	CNaBaseModel m_na_switch_port_mac;
	int m_iSubNo;//主站侧的对应子站号
/*
	int m_iNetIdNum;//net_id数量
	int* m_piSubNetId;//子站侧的net_id数组
	int* m_piMainNetId;//主站侧的net_id数组*/
	std::map<int, int> m_mapNetId;//子站侧ID-->主站侧ID
	std::map<int, int> m_mapComId;//子站侧ID-->主站侧ID

};

#endif//__NA_MODEL_H__
