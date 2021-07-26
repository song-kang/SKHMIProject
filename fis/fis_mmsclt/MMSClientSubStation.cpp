#include "MMSClientSubStation.h"


MMSClientSubStation::MMSClientSubStation(MMSCLTApp *mmscltApp, int nodeNo, int substationNo, SString substationName, int iedNo, SString iedDesc, SString iedName)
	: MMSClient(mmscltApp, nodeNo, substationNo, substationName)
{
	m_clientMode = MMSClient::SubStationMode;
	Ied *ied = new Ied(this, iedNo, iedDesc, iedName);
	m_ieds.push_back(ied);
}

MMSClientSubStation::~MMSClientSubStation()
{
}

bool MMSClientSubStation::Start()
{
//	this->SetMultiThreadMode(true);
	return MMSClient::Start();
}

bool MMSClientSubStation::Init()
{
	bool ret = getIed()->Init();
	getIed()->InitElementAnalogMonitor(&this->m_Datasets);
	getIed()->InitElementParamCheck(&this->m_Datasets, m_globalConfig);

	SetMmsServerAddr(getIed()->ipA().data(), getIed()->ipB().data());

	return ret;
}

t_reference * MMSClientSubStation::findReference(std::string reference)
{
	return getIed()->findReference(reference);
}

// bool MMSClientSubStation::checkAnalogChannel()
// {
// 	std::vector<AnalogMonitorItem *> analogMonitorList;
// 	getIed()->getAnalogMonitorList(analogMonitorList);
// 	if (analogMonitorList.empty())
// 		return true;
// 
// 	bool ret = false;
// 	unsigned long pos = 0;
// 	unsigned long ds_pos = 0;
// 	unsigned long fc_pos = 0;
// 	SPtrList<SString> dom_name;
// 	SPtrList<SString> var_name;
// 	SPtrList<MMSValue> *value;
// 	SPtrList< SPtrList<MMSValue> > lstValue;
// 	SPtrList<t_reference> lstFc;
// 	dom_name.setAutoDelete(true);
// 	var_name.setAutoDelete(true);
// 	lstValue.setAutoDelete(true);
// 	int timeOut = 5000;
// 	unsigned long soc = (unsigned long)time(NULL);
// 
// 	for (std::vector<AnalogMonitorItem *>::iterator iter = analogMonitorList.begin(); iter != analogMonitorList.end(); ++iter)
// 	{
// 		// 是否在扫描间隔内
// 		if ((long)(soc - (*iter)->last_soc) < (*iter)->inteval)
// 		{
// 			continue;
// 		}
// 
// 		(*iter)->last_soc = soc;
// 		SString *dn = new SString(SString::GetIdAttribute(1, (*iter)->mms_path,"/"));
// 		dom_name.append(dn);
// 		SString *vn = new SString(SString::GetIdAttribute(2, (*iter)->mms_path,"/"));
// 		var_name.append(vn);
// 		value = new SPtrList<MMSValue>;
// 		value->setAutoDelete(true);
// 		lstValue.append(value);
// 		lstFc.append((*iter)->reference);
// 		if (++pos >= MAX_READ_VALUES_NUMBER)
// 		{
// 			LOGDEBUG("读多值[%d]准备，子站号[%d]装置MMS名称[%s]。",pos,m_subNo, getIed()->iedName());
// 			ret = ReadValues(dom_name,var_name,lstValue,timeOut);
// 			if (ret)
// 			{
// 				LOGDEBUG("读多值[%d]完毕，子站号[%d]装置MMS名称[%s]。",pos,m_subNo, getIed()->iedName());
// 				dbAnalogValue(lstFc,lstValue);
// 				LOGDEBUG("多值[%d]入库完毕，子站号[%d]装置MMS名称[%s]。",pos,m_subNo, getIed()->iedName());
// 			}
// 			dom_name.clear();
// 			var_name.clear();
// 			lstValue.clear();
// 			lstFc.clear();
// 			pos = 0;
// 
// 			if (!ret)
// 				break;
// 		}
// 	}
// 
// 	if (dom_name.count() > 0)
// 	{
// 		LOGDEBUG("读多值[%d]准备，子站号[%d]装置MMS名称[%s]。",pos,m_subNo, getIed()->iedName());
// 		ret = ReadValues(dom_name,var_name,lstValue);
// 		if (ret)
// 		{
// 			LOGDEBUG("读多值[%d]完毕，子站号[%d]装置MMS名称[%s]。",pos,m_subNo, getIed()->iedName());
// 			dbAnalogValue(lstFc,lstValue);
// 			LOGDEBUG("多值[%d]入库完毕，子站号[%d]装置MMS名称[%s]。",pos,m_subNo, getIed()->iedName());
// 		}
// 		dom_name.clear();
// 		var_name.clear();
// 		lstValue.clear();
// 		lstFc.clear();
// 		pos = 0;
// 	}
// 	
// 	return true;
// }

Ied * MMSClientSubStation::getIed()
{
	if (m_ieds.empty())
		return NULL;

	return m_ieds.at(0);
}

void MMSClientSubStation::setEntryID(stuSMmsReportInfo *rptInfo,SString entryId)
{
	stuReportInfo *info = getIed()->findReportInfo(rptInfo);
	if (info)
	{
		SetEntryID(info, entryId);
	}
	else
	{
		LOGERROR("未找到报告控制块%s/%s", rptInfo->dom_name.data(), rptInfo->rcb_name.data());
	}
}

bool MMSClientSubStation::SetChannelStateA(int state)
{
	SString sql = SString::toFormat("update t_oe_ied set comstate = %d where ied_no=%d", state, getIed()->iedNo());

	if (DB->Execute(sql) && MDB->GetDatabasePool()->GetDatabaseByIdx(0)->GetStatus() != SDatabase::DBERROR && MDB->Execute(sql))
	{
		LOGDEBUG("子站[%d]设备[%d]A网通信状态[%d]更新成功。", m_subNo, getIed()->iedNo(), state);
	}
	else
	{
		LOGDEBUG("子站[%d]设备[%d]A网通信状态[%d]更新失败。", m_subNo, getIed()->iedNo(), state);
	}

	dbSubStateSyslog("A网通信状态", state);

	return true;
}

bool MMSClientSubStation::SetChannelStateB(int state)
{
	SString sql = SString::toFormat("update t_oe_ied set comstate = %d where ied_no=%d", state, getIed()->iedNo());

	if (DB->Execute(sql) && MDB->GetDatabasePool()->GetDatabaseByIdx(0)->GetStatus() != SDatabase::DBERROR && MDB->Execute(sql))
	{
		LOGDEBUG("子站[%d]设备[%d]B网通信状态[%d]更新成功。", m_subNo, getIed()->iedNo(), state);
	}
	else
	{
		LOGDEBUG("子站[%d]设备[%d]B网通信状态[%d]更新失败。", m_subNo, getIed()->iedNo(), state);
	}

	dbSubStateSyslog("B网通信状态", state);

	return true;
}
