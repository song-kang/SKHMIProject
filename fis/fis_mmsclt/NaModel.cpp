/**
 *
 * 文 件 名 : NaModel.cpp
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

#include "NaModel.h"

CNaBaseModel::CNaBaseModel()
{
	m_iLastRestoreTime = 0;
	m_pMgr = NULL;
}

CNaBaseModel::~CNaBaseModel()
{

}


//////////////////////////////////////////////////////////////////////////
// 描    述:  从数据库备份网络信息到指定目录，当内容未改变时不更新
// 作    者:  邵凯田
// 创建时间:  2017-8-24 14:45
// 参数说明:  @sPath为目录名称
// 返 回 值:  int, >=0表示备份表数量，<0表示失败
//////////////////////////////////////////////////////////////////////////
int CNaBaseModel::BackupFromDb(SString sPath)
{
	sPath += m_sTableName+".xml";
	SString sql;
	SRecordset rs;
	SRecord *pRec;
	sql = "select "+m_sFieldNames+" from "+m_sTableName;
	if(m_sOrderBy.length() > 0)
		sql += " order by "+m_sOrderBy;
	DB->Retrieve(sql,rs);
	int i,cnt = rs.GetRows();
	m_Config.clear();
	m_Config.SetNodeName("na_model");
	SBaseConfig *model_info = m_Config.AddChildNode("model_info");
	model_info->SetAttribute("table",m_sTableName);
	model_info->SetAttribute("fields",m_sFieldNames);
	model_info->SetAttribute("types",m_sFieldTypes);
	model_info->SetAttribute("orderby",m_sOrderBy);
	SBaseConfig *pRow;

	int j,cols = SString::GetAttributeCount(m_sFieldNames,",");
	for(i=0;i<cnt;i++)
	{
		pRec = rs.GetRecord(i);
		pRow = m_Config.AddChildNode("row");
		for(j=0;j<cols;j++)
		{
			pRow->SetAttribute(SString::toFormat("c%d",j+1),pRec->GetValue(j));
		}
	}
	SString xml;
	m_Config.SaveConfigToText(xml);
	if(xml.compare(m_sLastBackupXml.data()) == 0)
	{
		return 0;//文件内容未改变，忽略保存文件
	}
	m_sLastBackupXml = xml;
	SFile f(sPath);
	if(!f.open(IO_Truncate))
		return -1;
	char* pStr = xml.data();
	int max_block = 65536;
	int len = xml.length();
	while(len > 0)
	{
		int blen = len;
		if(blen > max_block)
			blen = max_block;
		f.writeBlock((BYTE*)pStr,blen);
		len -= blen;
		pStr += blen;
	}
	f.close();
	return 1;
}

//////////////////////////////////////////////////////////////////////////
// 描    述:  还原网络信息数据到数据库，如文件修改时间较上一次操作未变则忽略还原
// 作    者:  邵凯田
// 创建时间:  2017-8-24 14:33
// 参数说明:  @sPath为目录名称
// 返 回 值:  int,>=0表示成功还原表数量，<0表示失败
//////////////////////////////////////////////////////////////////////////
int CNaBaseModel::ChkRestoreToDb(SString sPath)
{
	//查看指定目录
	sPath += m_sTableName+".xml";
	SDateTime dtModiTime = SFile::filetime(sPath);
	if(dtModiTime.soc() != m_iLastRestoreTime)
	{
		m_iLastRestoreTime = dtModiTime.soc();
		return RestoreToDb(sPath);
	}
	return 0;
}

//////////////////////////////////////////////////////////////////////////
// 描    述:  还原网络信息数据到数据库，
// 作    者:  邵凯田
// 创建时间:  2017-8-24 14:33
// 参数说明:  @sPath为目录名称
// 返 回 值:  int,>=0表示成功还原表数量，<0表示失败
//////////////////////////////////////////////////////////////////////////
int CNaBaseModel::RestoreToDb(SString sPathFile)
{
	m_Config.clear();
	if(!m_Config.ReadConfig(sPathFile))
	{
		LOGERROR("文件打开失败!file:%s", sPathFile.data());
		return false;
	}
	int soc =(int)SDateTime::getNowSoc();
	if(m_Config.SearchNodeAttribute("model_info","table") != m_sTableName)
		return -1;//文件不匹配
	if(m_Config.SearchNodeAttribute("model_info","fields") != m_sFieldNames)
		return -2;//文件不匹配
	SDatabase *pDB = GET_DB_CFG->GetHisDbOper()->GetDatabasePool()->GetDatabase();
	pDB->BeginTrans();
	CSsp_BatchDmlInsert batch;
	batch.Init(pDB,m_sTableName.data(),m_sFieldNames.data(),1);
	SString vals;
	bool bSubNetwork = false;
	SRecordset rsMainNetId,rsComDevPort;
	if(m_sTableName == "t_na_sub_network")
	{
		//子网表，保存子网号映射
		int nets = m_Config.GetChildCount("row");
		//m_pMgr->NewNetIdMap(nets);
		bSubNetwork = true;
		pDB->Retrieve(SString::toFormat("select net_id from t_na_sub_network where sub_no=%d order by net_id",m_pMgr->m_iSubNo),rsMainNetId);
	}
	bool bComDevice = false;
	if(m_sTableName == "t_na_comm_device")
	{
		bComDevice = true;
	}

	bool bComDevPort = false;
	if(m_sTableName == "t_na_comm_device_port")
	{
		bComDevPort = true;
		pDB->Retrieve(SString::toFormat("select port_sn from t_na_comm_device_port where sub_no=%d order by port_sn",m_pMgr->m_iSubNo),rsComDevPort);
	}
	pDB->Execute(SString::toFormat("delete from %s where sub_no=%d",m_sTableName.data(),m_pMgr->m_iSubNo));

	int j,cols = SString::GetAttributeCount(m_sFieldNames,",");
	SString stype,field,val;
	SString s_sub_no;
	s_sub_no.sprintf("%d",m_pMgr->m_iSubNo);
	int idx = 0;
	unsigned long pos;
	int max_net_id=-1,max_com_id=-1,max_port_sn=-1;
	SBaseConfig *pRow = m_Config.GetChildPtr()->FetchFirst(pos);
	SString sPrevColVal;
	while(pRow)
	{
		if(pRow->GetNodeName() == "row")
		{
			sPrevColVal = "";
			vals = "";
			for(j=0;j<cols;j++)
			{
				field = SString::GetIdAttribute(j+1,m_sFieldNames,",");
				stype = SString::GetIdAttribute(j+1,m_sFieldTypes,",");
				val = pRow->GetAttribute(SString::toFormat("c%d",j+1));
				if(field == "sub_no")
					val = s_sub_no;
				else if(bSubNetwork && field == "net_id")
				{
					if(max_net_id < 0)
						max_net_id = pDB->SelectIntoI("select max(net_id) from t_na_sub_network")+1;
					m_pMgr->AddNewNetId(val.toInt(),max_net_id);
					val.sprintf("%d",max_net_id);
					max_net_id++;
					/*
					m_pMgr->m_piSubNetId[idx] = val.toInt();
					if(idx<rsMainNetId.GetRows())
						m_pMgr->m_piMainNetId[idx] = rsMainNetId.GetValueInt(idx,0);
					else
					{
						//主站库中没有原记录时
						if(max_net_id < 0)
							max_net_id = pDB->SelectIntoI("select max(net_id) from t_na_sub_network")+1;
						m_pMgr->m_piMainNetId[idx] = max_net_id++;
					}
					val.sprintf("%d",m_pMgr->m_piMainNetId[idx]);*/
				}
				else if(bComDevice && field == "com_id")
				{
					if(max_com_id < 0)
						max_com_id = pDB->SelectIntoI("select max(com_id) from t_na_comm_device")+1;
					m_pMgr->AddNewComId(val.toInt(),max_com_id);
					val.sprintf("%d",max_com_id);
					max_com_id++;
				}
				else if(field == "net_id" || field == "net_id_a" || field == "net_id_b")
				{
					//根据子站的net_id找主站的net_id
					int sub_net_id = val.toInt();
					int main_net_id = m_pMgr->GetMainNetId(sub_net_id);
					val.sprintf("%d",main_net_id);
					/*for(int i=0;i<m_pMgr->m_iNetIdNum;i++)
					{
						if(m_pMgr->m_piSubNetId[i] == sub_net_id)
						{
							val.sprintf("%d",m_pMgr->m_piMainNetId[i]);
							break;
						}
					}*/
				}
				else if(sPrevColVal.toInt()==2 && (field == "dev_id" || field == "dev_id_a" || field == "dev_id_b"))
				{
					int sub_com_id = val.toInt();
					int main_com_id = m_pMgr->GetMainComId(sub_com_id);
					val.sprintf("%d",main_com_id);
				}
				else if(bComDevPort && field == "port_sn")
				{
					if(idx<rsComDevPort.GetRows())
						val = rsComDevPort.GetValue(idx,0);
					else
					{
						//主站库中没有原记录时
						if(max_port_sn < 0)
							max_port_sn = pDB->SelectIntoI("select max(port_sn) from t_na_comm_device_port")+1;
						val.sprintf("%d",max_port_sn++);
					}					
				}
				if(vals.length() > 0)
					vals += ",";
				if(stype == "str")
					vals += "'"+val+"'";
				else if(val.length() == 0)
					vals += "null";
				else
					vals += val;

				sPrevColVal = val;
			}
			batch.AddInsertValues(vals);
			idx ++;
		}
		pRow = m_Config.GetChildPtr()->FetchNext(pos);
	}
	batch.CheckTimeOut(soc+100);

	pDB->Commit();
	GET_DB_CFG->GetHisDbOper()->GetDatabasePool()->Release(pDB);
	return 1;
}


CNaModelMgr::CNaModelMgr()
{
	m_na_sub_network.m_pMgr = this;
	m_na_sub_network.m_sTableName	= "t_na_sub_network";
	m_na_sub_network.m_sFieldNames	= "net_id,sub_no,net_name,net_type,net_desc,net_bitrate,uiwnd_sn,comm_addr,weight,netmasks";
	m_na_sub_network.m_sFieldTypes	= "int,int,str,int,str,int,int,int,float,str";
	m_na_sub_network.m_sOrderBy		= "net_id";

	m_na_comm_device.m_pMgr = this;
	m_na_comm_device.m_sTableName	= "t_na_comm_device";
	m_na_comm_device.m_sFieldNames	= "com_id,sub_no,net_id,dev_type,com_name,mgr_prot,mgr_ip,mgr_port,mgr_state,state_confirm,dev_factory,dev_model,dev_version,dev_crc,up_time,ext_attr,comm_addr,weight";
	m_na_comm_device.m_sFieldTypes	= "int,int,int,int,str,int,str,int,int,int,str,str,str,str,str,str,int,float";
	m_na_comm_device.m_sOrderBy		= "com_id";

	m_na_comm_device_port.m_pMgr = this;
	m_na_comm_device_port.m_sTableName	= "t_na_comm_device_port";
	m_na_comm_device_port.m_sFieldNames	= "net_id,dev_cls,dev_id,port_id,sub_no,port_name,port_type,if_type,ip_addr,mac_addr,mcast_mac_addr,port_state,state_confirm,port_flow,port_inflow,port_outflow,in_bytes,In_pkgs,In_bcast_pkgs,In_mcast_pkgs,sum_in_pkts_64,sum_in_pkts_65_127,sum_in_pkts_128_255,sum_in_pkts_256_511,sum_in_pkts_512_1023,sum_in_pkts_1024_1518,port_sn";
	m_na_comm_device_port.m_sFieldTypes	= "int,int,int,int,int,str,int,int,str,str,str,...";
	m_na_comm_device_port.m_sOrderBy	= "net_id,dev_cls,dev_id,port_id";

	m_na_comm_connect_line.m_pMgr = this;
	m_na_comm_connect_line.m_sTableName	= "t_na_comm_connect_line";
	m_na_comm_connect_line.m_sFieldNames= "net_id_a,dev_cls_a,dev_id_a,port_id_a,net_id_b,dev_cls_b,dev_id_b,port_id_b,sub_no,line_type,line_stat,state_confirm,soc";
	m_na_comm_connect_line.m_sFieldTypes= "int,int,int,int,int,int,int,int,int,int,int,int";
	m_na_comm_connect_line.m_sOrderBy	= "net_id_a,dev_cls_a,dev_id_a,port_id_a,net_id_b,dev_cls_b,dev_id_b,port_id_b";

	m_na_iec104_session.m_pMgr = this;
	m_na_iec104_session.m_sTableName	= "t_na_iec104_session";
	m_na_iec104_session.m_sFieldNames	= "sub_no,client_ip,server_ip,client_port,server_port,port_no,soc,real_pkgs,real_bytes,state,connect_time";
	m_na_iec104_session.m_sFieldTypes	= "int,int,int,int,int,int,int,int,int,int,int";
	m_na_iec104_session.m_sOrderBy		= "sub_no,client_ip,server_ip,client_port,server_port";

	m_na_mms_session.m_pMgr = this;
	m_na_mms_session.m_sTableName		= "t_na_mms_session";
	m_na_mms_session.m_sFieldNames		= "sub_no,client_ip,server_ip,client_port,server_port,port_no,soc,real_pkgs,real_bytes,state,connect_time";
	m_na_mms_session.m_sFieldTypes		= "int,int,int,int,int,int,int,int,int,int,int";
	m_na_mms_session.m_sOrderBy			= "sub_no,client_ip,server_ip,client_port,server_port";

// 	m_iNetIdNum = 0;
// 	m_piSubNetId = m_piMainNetId = NULL;
	m_iSubNo = 0;
}

CNaModelMgr::~CNaModelMgr()
{
// 	if(m_piSubNetId != NULL)
// 		delete[] m_piSubNetId;
// 	if(m_piMainNetId != NULL)
// 		delete[] m_piMainNetId;
}


//////////////////////////////////////////////////////////////////////////
// 描    述:  重新建立NETID的映射记录
// 作    者:  邵凯田
// 创建时间:  2017-8-24 16:22
// 参数说明:  @num表示记录数量
// 返 回 值:  void
//////////////////////////////////////////////////////////////////////////
/*
void CNaModelMgr::NewNetIdMap(int num)
{
	if(m_iNetIdNum != num)
	{
		if(m_piSubNetId != NULL)
			delete[] m_piSubNetId;
		if(m_piMainNetId != NULL)
			delete[] m_piMainNetId;
		m_piSubNetId = NULL;
		m_piMainNetId = NULL;
		m_iNetIdNum = num;
		if(num > 0)
		{
			m_piSubNetId = new int[num];
			m_piMainNetId = new int[num];
		}
	}
	if(num > 0)
	{
		memset(m_piSubNetId,0,sizeof(int)*num);
		memset(m_piMainNetId,0,sizeof(int)*num);
	}
}*/


//////////////////////////////////////////////////////////////////////////
// 描    述:  添加NETID的子站到主站映射
// 作    者:  邵凯田
// 创建时间:  2018-6-1 17:01
// 参数说明:  
// 返 回 值:  
//////////////////////////////////////////////////////////////////////////
void CNaModelMgr::AddNewNetId(int sub_net_id,int main_net_id)
{
	m_mapNetId[sub_net_id] = main_net_id;
}

//////////////////////////////////////////////////////////////////////////
// 描    述:  添加COMID的子站到主站映射
// 作    者:  邵凯田
// 创建时间:  2018-6-1 17:01
// 参数说明:  
// 返 回 值:  
//////////////////////////////////////////////////////////////////////////
void CNaModelMgr::AddNewComId(int sub_com_id,int main_com_id)
{
	m_mapComId[sub_com_id] = main_com_id;
}

//////////////////////////////////////////////////////////////////////////
// 描    述:  取主站NETID
// 作    者:  邵凯田
// 创建时间:  2018-6-1 17:02
// 参数说明:  
// 返 回 值:  
//////////////////////////////////////////////////////////////////////////
int CNaModelMgr::GetMainNetId(int sub_net_id)
{
	if(m_mapNetId.find(sub_net_id) == m_mapNetId.end())
		return 0;
	return m_mapNetId[sub_net_id];
}

//////////////////////////////////////////////////////////////////////////
// 描    述:  取主站COMID
// 作    者:  邵凯田
// 创建时间:  2018-6-1 17:02
// 参数说明:  
// 返 回 值:  
//////////////////////////////////////////////////////////////////////////
int CNaModelMgr::GetMainComId(int sub_com_id)
{
	if(m_mapComId.find(sub_com_id) == m_mapComId.end())
		return 0;
	return m_mapComId[sub_com_id];
}

//////////////////////////////////////////////////////////////////////////
// 描    述:  从数据库备份网络信息到指定目录，当内容未改变时不更新
// 作    者:  邵凯田
// 创建时间:  2017-8-24 14:32
// 参数说明:  @sPath为目录名称
// 返 回 值:  int, >=0表示备份表数量，<0表示失败
//////////////////////////////////////////////////////////////////////////
int CNaModelMgr::BackupFromDb(SString sPath)
{
	int ret = 0;
	if(sPath.right(1) != "\\" && sPath.right(1) != "/")
		sPath += SDIR_SEPARATOR;

	m_na_sub_network.BackupFromDb(sPath);
	m_na_comm_device.BackupFromDb(sPath);
	m_na_comm_device_port.BackupFromDb(sPath);
	m_na_comm_connect_line.BackupFromDb(sPath);
	m_na_iec104_session.BackupFromDb(sPath);
	m_na_mms_session.BackupFromDb(sPath);

	return ret;
}

//////////////////////////////////////////////////////////////////////////
// 描    述:  还原网络信息数据到数据库，如文件修改时间较上一次操作未变则忽略还原
// 作    者:  邵凯田
// 创建时间:  2017-8-24 14:33
// 参数说明:  @sPath为目录名称
// 返 回 值:  int,>=0表示成功还原表数量，<0表示失败
//////////////////////////////////////////////////////////////////////////
int CNaModelMgr::RestoreToDb(SString sPath)
{
	int ret = 0;
	if(sPath.right(1) != "\\" && sPath.right(1) != "/")
		sPath += SDIR_SEPARATOR;

	m_na_sub_network.ChkRestoreToDb(sPath);
	m_na_comm_device.ChkRestoreToDb(sPath);
	m_na_comm_device_port.ChkRestoreToDb(sPath);
	m_na_comm_connect_line.ChkRestoreToDb(sPath);
	m_na_iec104_session.ChkRestoreToDb(sPath);
	m_na_mms_session.ChkRestoreToDb(sPath);

	return ret;
}
