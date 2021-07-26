#include "MMSClientMainStation.h"
#include "MMSCLTApp.h"


MMSClientMainStation::MMSClientMainStation(MMSCLTApp *mmscltApp, int nodeNo, int substationNo, SString substationName)
	: MMSClient(mmscltApp, nodeNo, substationNo, substationName)
{
	m_clientMode = MMSClient::MainStationMode;
}

MMSClientMainStation::~MMSClientMainStation()
{
}

bool MMSClientMainStation::Init()
{
	if (!InitParam())
		return false;

	if (!InitIeds())
		return false;

	return true;
}

bool MMSClientMainStation::SetChannelState(int state)
{
	SString sql;

	//to t_cim_substation
	sql.sprintf("update t_cim_substation set comstate=%d where sub_no=%d", state, m_subNo);
	if (DB->Execute(sql) && MDB->GetDatabasePool()->GetDatabaseByIdx(0)->GetStatus() != SDatabase::DBERROR && MDB->Execute(sql))
	{
		LOGDEBUG("子站[%d]总通信状态[%d]更新成功。", m_subNo, state);
	}
	else
	{
		LOGDEBUG("子站[%d]总通信状态[%d]更新失败。", m_subNo, state);
	}

	dbSubStateSyslog("总通信状态",state);

	//to t_oe_ied
	sql.sprintf("update t_oe_ied set comstate=%d where model='AGENT_MONI' and sub_no=%d", state, m_subNo);
	if (DB->Execute(sql))
	{
		LOGDEBUG("子站[%d]总通信状态[%d]更新成功。", m_subNo, state);
	}
	else
	{
		LOGDEBUG("子站[%d]总通信状态[%d]更新失败。", m_subNo, state);
	}

	return true;
}

void MMSClientMainStation::setEntryID(stuSMmsReportInfo *rptInfo,SString entryId)
{
	for (std::vector<Ied *>::iterator iter = m_ieds.begin(); iter != m_ieds.end(); ++iter)
	{
		stuReportInfo *info = (*iter)->findReportInfo(rptInfo);
		if (info)
		{
			SetEntryID(info, entryId);
			return;
		}
	}

	LOGERROR("未找到报告控制块%s/%s", rptInfo->dom_name.data(), rptInfo->rcb_name.data());
}

bool MMSClientMainStation::InitParam()
{
	SString		ied_ip_a = "";
	SString		ied_ip_b = "";
	SString		sql;
	SRecordset	rs;

	sql = SString::toFormat("select param_name,current_val from t_oe_module_param "
		"where node_no=%d and module_name='%s'", this->m_nodeNo, MODULE_NAME);
	int iRet = DB->Retrieve(sql,rs);
	if (iRet < 0)
	{
		LOGFAULT("数据库语句操作失败，SQL：%s",sql.data());
		return false;
	}
	else if (iRet == 0)
	{
		LOGWARN("数据库t_oe_module_param表中未找到节点号=%d，程序名=%s的数据。", this->m_nodeNo, MODULE_NAME);
		return false;
	}
	else if (iRet > 0)
	{
		for (int i = 0; i < rs.GetRows(); i++)
		{
			SString param = rs.GetValue(i,0);
			if (param == "ip_addr_a")
				this->m_iedIPA = rs.GetValue(i,1);
			else if (param == "ip_addr_b")
				this->m_iedIPB = rs.GetValue(i,1);
		}
		if (m_iedIPA.isEmpty() && m_iedIPB.isEmpty())	//A、B网地址均无，不创建此装置
		{
			LOGWARN("数据库t_oe_module_param表中未找到节点号=%d，程序名=%s的A或B网地址配置。", this->m_nodeNo, MODULE_NAME);
			return false;
		}
	}

	SetMmsServerAddr(m_iedIPA.data(), m_iedIPB.data());

	return true;
}

bool MMSClientMainStation::InitIeds()
{
	SRecordset rs;
	SString sql = SString::toFormat("select ied_no, name, mms_path from t_oe_ied where sub_no=%d", this->m_subNo);
	int iRet = DB->Retrieve(sql, rs);
	if (iRet < 0)
	{
		LOGFAULT("数据库语句操作失败，SQL：%s", sql.data());
		return false;
	}
	else if (iRet == 0)
	{
		LOGWARN("数据库t_oe_ied表中未找到子站号 = %d 的数据。", m_subNo);
		return false;
	}

	for (int i = 0; i < rs.GetRows(); ++i)
	{
		int iedNo = rs.GetValue(i, 0).toInt();
		SString name = rs.GetValue(i, 1);
		SString iedName = rs.GetValue(i, 2);

		Ied *ied = new Ied(this, iedNo, name, iedName);
		if (!ied->Init())
		{
			LOGWARN("初始化IED(%d:%s)失败，忽略该IED设备", iedNo, iedName.data());
			delete ied;
			continue;
		}

		LOGDEBUG("初始化IED(%d:%s)成功。", iedNo, iedName.data());
		m_ieds.push_back(ied);
	}

	return true;
}

t_reference * MMSClientMainStation::findReference(std::string reference)
{
	for (std::vector<Ied *>::iterator iter = m_ieds.begin(); iter != m_ieds.end(); ++iter)
	{
		if(reference.find((*iter)->iedName()) != 0)
			continue;
		t_reference *ref = (*iter)->findReference(reference);
		if(ref != NULL)
			return ref;
	}
	return NULL;
}
