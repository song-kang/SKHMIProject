#include "math.h"
#include "MMSClient.h"
#include "MMSCLTApp.h"

CSsp_BatchDmlInsert g_batchIns_oe_his_event;
CSsp_BatchDml g_batch_db;
CSsp_BatchDml g_batch_mdb;
bool g_bBatchInited = false;
int g_iClientCnt=0;

MMSClient::MMSClient(MMSCLTApp *app, int nodeNo, int subNo, SString subName)
{
	mms_app = app;
	m_nodeNo = nodeNo;
	m_subNo = subNo;
	m_subName = subName;
	m_iThreads = 0;
	m_reportId = 0;
	m_clientMode = DefaultMode;
	m_settingChangeTime = (int)SDateTime::getNowSoc();
	g_iClientCnt++;
	m_globalConfig = NULL;
	if(g_bBatchInited == false)
	{
		g_bBatchInited = true;
		eSsp_Database_Type dbType = GET_DB_CFG->GetMasterType();
		eSsp_Database_Type mdbType = DB_MDB;
		g_batchIns_oe_his_event.Init(DB,dbType,"t_oe_his_event","st_sn,sub_no,ied_no,cpu_no,group_no,entry,act_soc,act_usec,fun,inf,type,severity_level,rettime,dpi,nof,fan,act_q,time_q,reason",0);
		g_batch_mdb.Init(MDB,mdbType,0,false);
		g_batch_db.Init(DB,dbType,0,false);
		SKT_CREATE_THREAD(ThreadBatchSqlTimeout,this);
	}
}

MMSClient::~MMSClient(void)
{
	g_iClientCnt--;
	Stop();

	for (std::vector<Ied *>::iterator iter = m_ieds.begin(); iter != m_ieds.end(); ++iter)
	{
		delete (*iter);
	}
	m_ieds.clear();
}

bool MMSClient::Start()
{
	netA_state = 0;
	netB_state = 0;

	m_bQuit = false;
	m_reports.setShared(true);
	m_reports.setAutoDelete(true);
	//m_CliReportInfos.setAutoDelete(true);
	//S_CREATE_THREAD(ThreadReport,this);
	//static bool bFirst=true;
	bool ret=true;
	//if(bFirst)
	{
		//bFirst = false;
		ret = SMmsClient::Start();
	}

	return ret;
}

bool MMSClient::Stop()
{
	bool ret = SMmsClient::Stop();

	m_bQuit = true;
	while (m_iThreads > 0)
		SApi::UsSleep(50000);

	return ret;
}

void MMSClient::OnConnected()
{
	bool ret = false;
	////ret = ResetLed();
	////ret = ResetIed();
	////ret = ControlSelect(202,2,8,4,1);
	////ret = ControlCancel(202,2,8,4,1);
	////ret = ControlSelect(202,2,8,1,1);
	////ret = ControlExecute(202,2,8,1,1);
	//int area = 0;
	////ret = GetCurrentArea(area);
	////ret = GetEditArea(area);
	////ret = SetCurrentArea(1);
	////ret = SetEditArea(3,30);
	////ret = ReadCurrentAreaSetting(202,2,1);
	////ret = ReadEditAreaSetting(202,2,1);
	////ret = ReadAppointAreaSetting(202,2,1,3);
	//ret = WriteSettingValue(2,2,1,3,1,"10.2");
	//ret = CureSettingValue(2,2,1);
	//SPtrList<stuSMmsFileInfo> files;
	//files.setAutoDelete(true);
	////ret = GetMvlDirectory("COMTRADE",files,false);
	////ret = DownMvlFile("tffs0/ipacs5751.cid","D:\\1.cid");
	//unsigned long entries = 0;
	////bool deletable = false;
	////ret = GetJournalState("PQMonitorPQM1","lcStatisticData",entries,deletable);
	////ret = InitJournal("PQMonitorPQM1","lcFlickerData",entries);
	//SPtrList<stuJournalData> lstJData;
	//lstJData.setAutoDelete(true);
	//bool follow = false;
	//SDateTime end = SDateTime::currentDateTime();
	//SDateTime start = end.addDays(-60);
	//ret = ReadJournal("JZKJ5911CAMLD0","JZKJ5911CAMLD0",start,end,lstJData,follow);
	//lstJData.clear();
	//ret = ReadJournal("JZKJ5911CAMLD0","JZKJ5911CAMLD0",start,end,lstJData,follow);
	//lstJData.clear();
	//ret = ReadJournal("JZKJ5911CAMLD0","JZKJ5911CAMLD0",start,end,lstJData,follow);

#ifndef C_SMMS_SIGNAL_THREAD
	S_CREATE_THREAD(ThreadTimer,this);
#endif

	if (m_ieds.size() == 1)
		dbCommStateChange(m_ieds.at(0)->iedNo(), 1);
}

void MMSClient::OnConnected_A()
{
	netA_state = 1;
	SetChannelStateA(netA_state);
	SetChannelState(netA_state);
}

void MMSClient::OnConnected_B()
{
	netB_state = 1;
	SetChannelStateB(netB_state);
	SetChannelState(netB_state);
}

void MMSClient::OnDisConnected()
{
	netA_state = 0;
	netB_state = 0;
	SetChannelState(netA_state);
	SetChannelStateA(netA_state);
	SetChannelStateB(netB_state);

	if (m_ieds.size() == 1)
		dbCommStateChange(m_ieds.at(0)->iedNo(), 0);
}

void MMSClient::OnDisConnected_A()
{
	netA_state = 0;
	SetChannelStateA(netA_state);
}

void MMSClient::OnDisConnected_B()
{
	netB_state = 0;
	SetChannelStateB(netB_state);
}

bool MMSClient::checkAnalogChannel()
{
	for (std::vector<Ied *>::iterator iter = m_ieds.begin(); iter != m_ieds.end(); ++iter)
	{
		checkAnalogChannel((*iter)->iedNo());
	}
	
	return true;
}

bool MMSClient::checkAnalogChannel(int ied_no)
{
	Ied *ied = getIed(ied_no);
	if (!ied)
		return false;

	std::vector<AnalogMonitorItem *> analogMonitorList;
	ied->getAnalogMonitorList(analogMonitorList);
	if (analogMonitorList.empty())
		return true;

	bool ret = false;
	unsigned long pos = 0;
	unsigned long ds_pos = 0;
	unsigned long fc_pos = 0;
	SPtrList<SString> dom_name;
	SPtrList<SString> var_name;
	SPtrList<MMSValue> *value;
	SPtrList< SPtrList<MMSValue> > lstValue;
	SPtrList<t_reference> lstFc;
	dom_name.setAutoDelete(true);
	var_name.setAutoDelete(true);
	lstValue.setAutoDelete(true);
	int timeOut = 5000;
	//unsigned long soc = (unsigned long)time(NULL);
	SDateTime currentTime = SDateTime::currentDateTime();
	unsigned int soc = currentTime.soc();

	for (std::vector<AnalogMonitorItem *>::iterator iter = analogMonitorList.begin(); iter != analogMonitorList.end(); ++iter)
	{
		SDateTime lastTime = SDateTime::makeDateTime((*iter)->last_soc);
		if (lastTime.hour() == currentTime.hour())		// 每小时执行一次
			continue;

		// 是否在扫描间隔内
// 		if ((long)(soc - (*iter)->last_soc) < (*iter)->inteval)
// 		{
// 			continue;
// 		}

		(*iter)->last_soc = soc;
		SString *dn = new SString(SString::GetIdAttribute(1, (*iter)->mms_path,"/"));
		dom_name.append(dn);
		SString *vn = new SString(SString::GetIdAttribute(2, (*iter)->mms_path,"/"));
		var_name.append(vn);
		value = new SPtrList<MMSValue>;
		value->setAutoDelete(true);
		lstValue.append(value);
		lstFc.append((*iter)->reference);
		if (++pos >= MAX_READ_VALUES_NUMBER)
		{
			LOGDEBUG("读多值[%d]准备，子站号[%d]装置MMS名称[%s]。",pos,m_subNo, ied->iedName());
			ret = ReadValues(dom_name,var_name,lstValue,timeOut);
			if (ret)
			{
				LOGDEBUG("读多值[%d]完毕，子站号[%d]装置MMS名称[%s]。",pos,m_subNo, ied->iedName());
				dbAnalogValue(lstFc, lstValue, soc);
				LOGDEBUG("多值[%d]入库完毕，子站号[%d]装置MMS名称[%s]。",pos,m_subNo, ied->iedName());
			}
			dom_name.clear();
			var_name.clear();
			lstValue.clear();
			lstFc.clear();
			pos = 0;

			if (!ret)
				break;
		}
	}

	if (dom_name.count() > 0)
	{
		LOGDEBUG("读多值[%d]准备，子站号[%d]装置MMS名称[%s]。",pos,m_subNo, ied->iedName());
		ret = ReadValues(dom_name,var_name,lstValue);
		if (ret)
		{
			LOGDEBUG("读多值[%d]完毕，子站号[%d]装置MMS名称[%s]。",pos,m_subNo, ied->iedName());
			dbAnalogValue(lstFc, lstValue, soc);
			LOGDEBUG("多值[%d]入库完毕，子站号[%d]装置MMS名称[%s]。",pos,m_subNo, ied->iedName());
		}
		dom_name.clear();
		var_name.clear();
		lstValue.clear();
		lstFc.clear();
		pos = 0;
	}

	return true;
}

bool MMSClient::checkParam()
{
	if (!this->IsConnected())
		return false;

	Ied *ied = NULL;
	unsigned int lastCheckSoc = 0;
	unsigned int currentSoc = (unsigned int)::time(NULL);
	bool ret = true;

	for (std::vector<Ied *>::iterator iter = m_ieds.begin(); iter != m_ieds.end(); ++iter)
	{
		ied = *iter;
		if (!ied->isTimeParamCheck(currentSoc))
			continue;

		if (!ReadParamCheckValue(ied->iedNo(), 0, 0, false))
			ret = false;
	}

	return ret;
}

Ied * MMSClient::getIed(int ied_no)
{
	for (std::vector<Ied *>::iterator iter = m_ieds.begin(); iter != m_ieds.end(); ++iter)
	{
		if ((*iter)->iedNo() == ied_no)
			return (*iter);
	}

	return NULL;
}

std::vector<Ied *> MMSClient::getIeds()
{
	return m_ieds;
}

#ifdef C_SMMS_SIGNAL_THREAD
//////////////////////////////////////////////////////////////////////////
// 描    述:  每秒钟一次的回调，由应用层决定执行定时任务，如通过取服务端标识确定通信是否正常
// 作    者:  邵凯田
// 创建时间:  2017-5-26 10:30
// 参数说明:  void
// 返 回 值:  void
//////////////////////////////////////////////////////////////////////////
void MMSClient::OnSecondTimer()
{
	if(IsConnected())
	{
		//连接成功
		if (GetIdentify())
			m_iGetIdErrorTimes = 0;
		else
			m_iGetIdErrorTimes++;
		if (m_iGetIdErrorTimes >= 3)
		{
			DisConnect();
			m_iGetIdErrorTimes = 0;
		}
	}

	OnTimerWork();
}
#else
void* MMSClient::ThreadTimer(void* lp)
{
	unsigned int times = 0;
	unsigned int abort_times = 0;
	const unsigned int sleepTime = 50;  // 50ms
	int timesPerOneSecond = 1000 / 50;
	MMSClient *pThis = (MMSClient*)lp;
	pThis->m_iThreads++;
	while(!pThis->m_bQuit)
	{
		if ((times % (timesPerOneSecond * 30)) == 0)
		{
			if (pThis->GetIdentify())
				abort_times = 0;
			else
				abort_times++;

			if (abort_times >= 3)
			{
				pThis->DisConnect();
				abort_times = 0;
			}
		}

		pThis->OnProcessReport();

		if (times % timesPerOneSecond == 0)
			pThis->OnTimerWork();
		SApi::UsSleep(sleepTime * 1000);	// 100ms
		if (times++ >= 0x7FFFFFFF)
			times = 0;
	}

	pThis->m_iThreads--;
	return NULL;
}
#endif

void MMSClient::OnMmsReportProcess(stuSMmsReport* pMmsRpt)
{
	//m_reports.append(pMmsRpt);

	unsigned long		item_pos;
	stuSMmsReportItem	*item;
	stuSMmsReportInfo	*rptInfo = 0;
	SString				entry_id;

	if (pMmsRpt->EntryID)
	{
		SetReportEntryID(pMmsRpt,&rptInfo,entry_id);
		if (rptInfo)
			setEntryID(rptInfo,entry_id);
	}

	item = pMmsRpt->Items.FetchFirst(item_pos);
	while(item)
	{
		OnMmsReportItemProcess(item);
		item = pMmsRpt->Items.FetchNext(item_pos);
	}

	delete pMmsRpt;
}

void MMSClient::OnProcessReport()
{
	stuSMmsReport *report = NULL;
	unsigned long item_pos;
	stuSMmsReportItem *item = NULL;
	stuSMmsReportInfo	*rptInfo = 0;
	SString				entry_id;
	int processedCount = 0;

	while (m_reports.count() > 0 && processedCount++ < 50)
	{
		report = m_reports.at(0);
		if (report->EntryID)
		{
			SetReportEntryID(report, &rptInfo, entry_id);
			if (rptInfo)
				setEntryID(rptInfo, entry_id);
		}

		item = report->Items.FetchFirst(item_pos);
		while(item)
		{
			OnMmsReportItemProcess(item);
			item = report->Items.FetchNext(item_pos);
		}

		m_reports.remove(0);
	}
}

void MMSClient::OnEnabledRpt(stuSMmsReportInfo *rpt, bool result, bool isLastRpt)
{
	ProcessGi(rpt->rcb_name);
}

void* MMSClient::ThreadBatchSqlTimeout(void* lp)//批量SQL超时检查线程，全局运行一次
{
	while(g_iClientCnt > 0)
	{
		g_batchIns_oe_his_event.CheckTimeOut();
		g_batch_db.CheckTimeOut();
		g_batch_mdb.CheckTimeOut();
		SApi::UsSleep(100000);
	}
	return NULL;
}
//void* MMSClient::ThreadReport(void* lp)
//{
//	//unsigned long		item_pos;
//	stuSMmsReport		*report;
//	//stuSMmsReportItem	*item;
//
//	MMSClient *pThis = (MMSClient*)lp;
//	pThis->m_iThreads++;
//	while(!pThis->m_bQuit)
//	{
//		report = pThis->m_reports[0];
//		if (report == NULL)
//		{
//			SApi::UsSleep(500000);	//500ms
//			continue;
//		}
//
//		//if (report->EntryID)
//		//	pThis->SetReportEntryID(report);
//
//		//item = report->Items.FetchFirst(item_pos);
//		//while(item)
//		//{
//		//	pThis->OnMmsReportItemProcess(item);
//		//	item = report->Items.FetchNext(item_pos);
//		//}
//
//		pThis->m_reports.remove(report);
//	}
//
//	pThis->m_reports.clear();
//	pThis->m_iThreads--;
//	return NULL;
//}

void MMSClient::OnMmsReportItemProcess(stuSMmsReportItem * pMmsRptItem)
{
	SString reference = GetMvlVarText(pMmsRptItem->dataRefName);
	if (reference == NULL)
	{
		LOGFAULT("无法获取报告内Var的reference内容。");
		return;
	}
	SString value = GetMvlVarText(pMmsRptItem->dataValue);
	if (value == NULL)
	{
		LOGFAULT("无法获取报告内Var的value内容。");
		return;
	}
	SString reason = GetMvlVarText(pMmsRptItem->Reason);
	if (reason == NULL)
	{
		LOGFAULT("无法获取报告内Var的reason内容。");
		return;
	}

	SPtrList<MMSValue> vList;
	vList.setAutoDelete(true);
	int ret = GetMvlVarValueList(pMmsRptItem->dataValue,vList);
	if (ret)
	{
		LOGFAULT("无法获取报告内Var的value内容。");
		return;
	}
	LOGDEBUG("收到:%s",reference.data());
#if 1//by skt

	t_reference *refObj = findReference(reference.data());
	//map<string,t_reference*> m_map = mms_app->map_reference;
	//map<string,t_reference*>::iterator iter = mms_app->map_reference.find(reference.data());
	if (refObj)
	{
		LOGDEBUG("收到突发数据，名称=[%s] MMS路径=[%s]。",refObj->desc.data(),reference.data());
		if (refObj->tableName == "t_oe_element_general")
		{
			dbElementGeneral(refObj,vList,reason);
		}
		else if (refObj->tableName == "t_oe_element_state")
		{
			dbElementState(refObj,vList,reason);
		}
	}
	else
	{
		LOGWARN("无法在数据库中对应突发数据，MMS路径=[%s]。",reference.data());
		OnMmsUnknownReport(pMmsRptItem, reference);
	}
#endif
}

bool MMSClient::dbElementGeneral(t_reference *reference,SPtrList<MMSValue> &vList,SString reason)
{
	SString		sql;
	SRecordset	rs;
	SString		value;
	float		fValue = 0.0;
	int soc = 0;
	int usec = 0;
	unsigned char tq = 0;
	unsigned short q = 0;
	int			i = 0;

	for (i = 0; i < vList.count(); i++)
	{
		switch (vList.at(i)->getType())
		{
		case TYPE_BOOL:
		case TYPE_INT8:
		case TYPE_INT16:
		case TYPE_INT32:
		case TYPE_UINT8:
		case TYPE_UINT16:
		case TYPE_UINT32:
			fValue = (float)vList.at(i)->getInt32Value();
			value.sprintf("%s",vList.at(i)->getStrValue().data());
			break;
		case TYPE_FLOAT:
			fValue = vList.at(i)->getFloatValue();
		case TYPE_UTF8_STRING:
			value.sprintf("%s",vList.at(i)->getStrValue().data());
			break;
		default:
			continue;
		}

		break;
	}

	if (i == vList.count())
		return false;

	SDateTime now = SDateTime::currentDateTime();
	if (abs(reference->dateTime.msecsTo(now)) <= reference->smooth)
	{
#ifndef NDEBUG
		LOGDEBUG("装置[%d,%d,%d,%d]的[%s]数值[%s]更替[%f]间隔[%dms]未跨出平滑时间[%dms]，被屏蔽。\n",
			reference->ied_no,reference->cpu_no,reference->group_no,reference->entry,reference->desc.data(),
			value.data(),reference->value,reference->dateTime.msecsTo(now),reference->smooth);
#endif
		return true;
	}
	else
		reference->dateTime = now;

	//if (vList.at(i)->getType() == TYPE_FLOAT && fabsf(fValue-reference->value) > reference->threshold)
	if (fabsf(fValue-reference->value) > reference->threshold)
	{
		reference->value = fValue;
	}
	else
	{
#ifndef NDEBUG
		LOGDEBUG("装置[%d,%d,%d,%d]的[%s]数值[%s]更替[%f]未跨出死区值[%.3f]，被屏蔽。\n",reference->ied_no,reference->cpu_no,
			reference->group_no,reference->entry,reference->desc.data(),value.data(),reference->value,reference->threshold);
#endif
		return true;
	}

	for (i = 0; i < vList.count(); i++)
	{
		if (vList.at(i)->getType() == TYPE_UTC_TIME)
		{
			soc = vList.at(i)->getUint32Value();
			usec = vList.at(i)->getInt32Value();
			tq = vList.at(i)->getUint8Value();
		}
		else if (vList.at(i)->getName() == "q")
		{
			q = vList.at(i)->getUint16Value();
		}
	}

	sql.sprintf("update t_oe_element_general set current_val='%s' where ied_no=%d and cpu_no=%d and group_no=%d and entry=%d",
		value.data(),reference->ied_no,reference->cpu_no,reference->group_no,reference->entry);
	if (DB->Execute(sql) && MDB->GetDatabasePool()->GetDatabaseByIdx(0)->GetStatus() != SDatabase::DBERROR && MDB->Execute(sql))
	{
		//LOGWARN("装置[%d,%d,%d,%d]的[%s]数值[%s]更新失败。",reference->ied_no,reference->cpu_no,
		//	reference->group_no,reference->entry,reference->desc.data(),value.data());

		LOGDEBUG("装置[%d,%d,%d,%d]的[%s]数值[%s]品质[%d]时间[%s.%d(%d)]原因[%s]更新成功。",
			reference->ied_no,reference->cpu_no,reference->group_no,reference->entry,reference->desc.data(),value.data(),q,
			SDateTime::makeDateTime(soc).toString("yyyy-MM-dd hh:mm:ss").data(),usec,tq,reason.data());
	}
	else
	{
		//LOGWARN("装置[%d,%d,%d,%d]的[%s]数值[%s]更新失败。",reference->ied_no,reference->cpu_no,
		//	reference->group_no,reference->entry,reference->desc.data(),value.data());

		LOGDEBUG("装置[%d,%d,%d,%d]的[%s]数值[%s]品质[%d]时间[%s.%d(%d)]原因[%s]更新失败。",
			reference->ied_no,reference->cpu_no,reference->group_no,reference->entry,reference->desc.data(),value.data(),q,
			SDateTime::makeDateTime(soc).toString("yyyy-MM-dd hh:mm:ss").data(),usec,tq,reason.data());

		return false;
	}

	return true;
}

bool MMSClient::dbElementState(t_reference *reference,SPtrList<MMSValue> &vList,SString reason)
{
	SString		sql;
	SRecordset	rs;
	int			val_type = 0;

	if (_getElementStateValueType(reference,val_type))
	{
		switch (val_type)
		{
		case 1:
		case 2:
			dbElementStateDigital(reference,val_type,vList,reason);
			break;
		case 3:
			LOGWARN("读取值类型=%d，枚举类型暂不处理",val_type);
			break;
		case 4:
			dbElementStateMeasure(reference,vList,reason);
			break;
		default:
			LOGWARN("读取值类型=%d不在类型范围内，ied=%d,cpu=%d,group=%d,entry=%d。",
				val_type,reference->ied_no,reference->cpu_no,reference->group_no,reference->entry);
			return false;
		}
	}

	return true;
}

bool MMSClient::_getElementStateValueType(t_reference *reference,int & val_type)
{
	//MEMO: 预加载，不需要动态取 [2017-6-1 14:16 邵凯田]
	val_type = reference->val_type;
	return true;
#if 0
	SString		sql;
	SRecordset	rs;

	sql.sprintf("select val_type from t_oe_element_state where ied_no=%d and cpu_no=%d and group_no=%d and entry=%d",
		reference->ied_no,reference->cpu_no,reference->group_no,reference->entry);
	int iRet = DB->Retrieve(sql,rs);
	if (iRet < 0)
	{
		LOGFAULT("数据库语句操作失败，SQL：%s",sql.data());
		return false;
	}
	else if (iRet == 0)
	{
		LOGWARN("在t_oe_element_state表中未发现值类型，ied=%d,cpu=%d,group=%d,entry=%d。",
			reference->ied_no,reference->cpu_no,reference->group_no,reference->entry);
		return false;
	}
	else if (iRet > 0)
	{
		val_type = rs.GetValue(0,0).toInt();
	}

	return true;
#endif
}

bool MMSClient::dbElementStateDigital(t_reference *reference,int type,SPtrList<MMSValue> &vList,SString reason)
{
	SString	sql;
	int i = 0;
	int val = 0;
	int soc = 0;
	int usec = 0;
	unsigned char tq = 0;
	unsigned short q = 0;
	bool bFind = false;
	SString report;
	bool isReportHaveReason = false;

	for (i = 0; i < vList.count(); i++)
	{
		if (vList.at(i)->getType() == TYPE_BOOL)
		{
			val = vList.at(i)->getUint8Value();
			if (type == 1)//双点
				val += 1;
			bFind = true;
			break;
		}
		else if (vList.at(i)->getType() == TYPE_UTF8_STRING)
		{
			SString s1 = vList.at(i)->getStrValue();
			SString s = SString::Utf82Gb(vList.at(i)->getStrValue().data(), vList.at(i)->getStrValue().length());
			report = s1.toGb2312();
			LOGDEBUG("%s", vList.at(i)->getName().data());
			LOGDEBUG("%s", vList.at(i)->getStrValue().data());
			LOGDEBUG("%s", s.data());
			LOGDEBUG("%s", report.data());
			bFind = true;
			isReportHaveReason = true;
			break;
		}
	}

	if (!bFind)
	{
		LOGWARN("在值列表中未发现可用数据，ied=%d,cpu=%d,group=%d,entry=%d。",
			reference->ied_no,reference->cpu_no,reference->group_no,reference->entry);
		return false;
	}

	for (i = 0; i < vList.count(); i++)
	{
		if (vList.at(i)->getType() == TYPE_UTC_TIME)
		{
			soc = vList.at(i)->getUint32Value();
			usec = vList.at(i)->getInt32Value();
			tq = vList.at(i)->getUint8Value();
		}
		else if (vList.at(i)->getName() == "q")
		{
			q = vList.at(i)->getUint16Value();
		}
	}

	SString quality = "品质:";
	if (QualityIsInvalid(q))
		quality += "无效|";
	if (QualityIsQuestionable(q))
		quality += "可疑|";
	if (QualityIsOverFlow(q))
		quality += "溢出|";
	if (QualityIsOutOfRange(q))
		quality += "超值域|";
	if (QualityIsBadReference(q))
		quality += "坏基准值|";
	if (QualityIsOscillatory(q))
		quality += "抖动|";
	if (QualityIsFailure(q))
		quality += "故障|";
	if (QualityIsOldData(q))
		quality += "旧数据|";
	if (QualityIsInconsistent(q))
		quality += "不一致|";
	if (QualityIsSubstituted(q))
		quality += "取代|";
	if (QualityIsTest(q))
		quality += "测试|";
	if (QualityIsOperatorBlocked(q))
		quality += "闭锁|";
	if (quality == "品质:")
		quality += "正常";
	else
		quality = quality.Left(quality.size()-1);

	SString strReason = "原因码:";
	if (ReasonIsDchg(reason))
		strReason += "数据变化|";
	if (ReasonIsQchg(reason))
		strReason += "品质变化|";
	if (ReasonIsDupd(reason))
		strReason += "数据更新|";
	if (ReasonIsPeriod(reason))
		strReason += "周期变化|";
	if (ReasonIsGi(reason))
		strReason += "总召唤|";
	if (strReason == "原因码:")
		strReason += "无";
	else
		strReason = strReason.Left(strReason.size()-1);

	SString details = quality + "; " + strReason;
	if (isReportHaveReason)
	{
		SString str = SString::toFormat("%s", report.data());
		//MEMO: 为什么要写syslog? [2017-6-1 14:37 邵凯田]
		//dbSyslog(str, 100);
		details += "; ";
		details += str;
	}

	sql.sprintf("update t_oe_element_state set current_val=%d,soc=%d,usec=%d,details='%s',is_confirm=0 "
		"where ied_no=%d and cpu_no=%d and group_no=%d and entry=%d",
		val,soc,usec,details.data(),reference->ied_no,reference->cpu_no,reference->group_no,reference->entry);
#if 1
	g_batch_db.AddSql(sql);
#else
	if (DB->Execute(sql))
	{
		LOGDEBUG("装置[%d,%d,%d,%d]的[%s]数值[%d]品质[%d]时间[%s.%d(%d)]原因[%s]更新成功。",
			reference->ied_no,reference->cpu_no,reference->group_no,reference->entry,reference->desc.data(),val,q,
			SDateTime::makeDateTime(soc).toString("yyyy-MM-dd hh:mm:ss").data(),usec,tq,reason.data());
	}
	else
	{
		LOGWARN("装置[%d,%d,%d,%d]的[%s]数值[%d]品质[%d]时间[%s.%d(%d)]原因[%s]更新失败。",
			reference->ied_no,reference->cpu_no,reference->group_no,reference->entry,reference->desc.data(),val,q,
			SDateTime::makeDateTime(soc).toString("yyyy-MM-dd hh:mm:ss").data(),usec,tq,reason.data());
		return false;
	}
#endif
	if (!ReasonIsGi(reason))
	{
#if 1
		g_batch_mdb.AddSql(sql);
#else
		if (MDB->GetDatabasePool()->GetDatabaseByIdx(0)->GetStatus() != SDatabase::DBERROR && MDB->Execute(sql))
		{
			LOGDEBUG("装置[%d,%d,%d,%d]的[%s]数值[%d]品质[%d]时间[%s.%d(%d)]原因[%s]内存库更新成功。",
				reference->ied_no,reference->cpu_no,reference->group_no,reference->entry,reference->desc.data(),val,q,
				SDateTime::makeDateTime(soc).toString("yyyy-MM-dd hh:mm:ss").data(),usec,tq,reason.data());
		}
#endif
		if(reference->cpu_no != 255)
		{
			sql.sprintf("update t_oe_ied set is_confirm=0 where ied_no=%d",reference->ied_no);
			DB->Execute(sql);
			MDB->Execute(sql);
		}

		dbElementStateDigitalHistory(reference,val,soc,usec,tq,q,reason);
	}

	return true;
}

bool MMSClient::dbElementStateDigitalHistory(t_reference *reference,int val,int soc,int usec,unsigned char tq,
	unsigned short q,SString reason)
{
#if 1

	g_batchIns_oe_his_event.AddInsertValues(SString::toFormat("%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,'%s'",
		reference->st_sn,m_subNo,reference->ied_no,reference->cpu_no,reference->group_no,reference->entry,
		soc,usec,
		reference->fun,
		reference->inf,
		reference->type,
		reference->level,
		0,val,0,0,q,tq,reason.data()));
#else
	SString		sql;
	SRecordset	rs;

	sql.sprintf("select fun,inf,name,type,evt_cls,val_type,severity_level,current_val,soc,usec from t_oe_element_state "
		"where ied_no=%d and cpu_no=%d and group_no=%d and entry=%d",
		reference->ied_no,reference->cpu_no,reference->group_no,reference->entry);
	int cnt = DB->Retrieve(sql,rs);
	if (cnt < 0)
	{
		LOGFAULT("数据库语句操作失败，SQL：%s",sql.data());
		return false;
	}
	else if (cnt == 0)
	{
		LOGWARN("数据库t_oe_element_state表中未找到信息点[%d,%d,%d,%d]的数据。",
			reference->ied_no,reference->cpu_no,reference->group_no,reference->entry);
		return false;
	}
	else if (cnt > 0)
	{
		SRecordset	rs1;
		sql.sprintf("select sub_no from t_oe_ied where ied_no=%d",reference->ied_no);
		int cnt1 = DB->Retrieve(sql,rs1);
		if (cnt1 < 0)
		{
			LOGFAULT("数据库语句操作失败，SQL：%s",sql.data());
			return false;
		}
		else if (cnt1 == 0)
		{
			LOGWARN("数据库t_oe_ied表中未找到装置[%d]的数据。",reference->ied_no);
			return false;
		}
		sql.sprintf("insert into t_oe_his_event (sub_no,ied_no,cpu_no,group_no,entry,"
			"act_soc,act_usec,fun,inf,type,severity_level,rettime,dpi,nof,fan,act_q,time_q,reason) values "
			"(%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,'%s')",
			rs1.GetValue(0,0).toInt(),reference->ied_no,reference->cpu_no,reference->group_no,reference->entry,
			soc,usec,
			rs.GetValue(0,0).toInt(),
			rs.GetValue(0,1).toInt(),
			rs.GetValue(0,3).toInt(),
			rs.GetValue(0,6).toInt(),
			0,val,0,0,q,tq,reason.data());
		if (!DB->Execute(sql))
		{
			LOGWARN("装置[%d,%d,%d,%d]的[%s]数值[%d]历史表更新失败。",reference->ied_no,reference->cpu_no,
				reference->group_no,reference->entry,reference->desc.data(),val);
			LOGFAULT("数据库语句操作失败，SQL：%s",sql.data());
			return false;
		}

	}
#endif
	return true;
}

bool MMSClient::dbElementStateDigitalHistory(int st_sn,int ied_no,int cpu_no,int group_no,int entry,int val)
{
	SString		sql;
	SRecordset	rs;

	sql.sprintf("select fun,inf,name,type,evt_cls,val_type,level,current_val,soc,usec from t_oe_element_state "
		"where ied_no=%d and cpu_no=%d and group_no=%d and entry=%d",ied_no,cpu_no,group_no,entry);
	int cnt = DB->Retrieve(sql,rs);
	if (cnt < 0)
	{
		LOGFAULT("数据库语句操作失败，SQL：%s",sql.data());
		return false;
	}
	else if (cnt == 0)
	{
		LOGWARN("数据库t_oe_element_state表中未找到信息点[%d,%d,%d,%d]的数据。",ied_no,cpu_no,group_no,entry);
		return false;
	}
	else if (cnt > 0)
	{
		int soc,usec;
		SDateTime::getSystemTime(soc,usec);
		sql.sprintf("insert into t_oe_his_event (sub_no,ied_no,cpu_no,group_no,entry,"
			"act_soc,act_usec,fun,inf,type,level,rettime,dpi,nof,fan,act_q,time_q,reason,st_sn) values "
			"(%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,'%s',%d)",
			1,ied_no,cpu_no,group_no,entry,
			soc,usec,
			rs.GetValue(0,0).toInt(),
			rs.GetValue(0,1).toInt(),
			rs.GetValue(0,3).toInt(),
			rs.GetValue(0,6).toInt(),
			0,val,0,0,0,0,"",st_sn);
		if (!DB->Execute(sql))
		{
			LOGWARN("装置[%d,%d,%d,%d]的[%s]数值[%d]历史表更新失败。",ied_no,cpu_no,group_no,entry,rs.GetValue(0,1).data(),val);
			LOGFAULT("数据库语句操作失败，SQL：%s",sql.data());
			return false;
		}
	}

	return true;
}

bool MMSClient::dbElementStateMeasure(t_reference *reference,SPtrList<MMSValue> &vList,SString reason)
{
	SString	sql;
	int val = 0;

	for (int i = 0; i < vList.count(); i++)
	{
		if (vList.at(i)->getType() == TYPE_BOOL)
			val = vList.at(i)->getUint8Value();
	}

	if(reference->cpu_no != 255)
	{
		sql.sprintf("update t_oe_ied set is_confirm=0 where ied_no=%d",reference->ied_no);
		DB->Execute(sql);
		MDB->Execute(sql);
	}

	sql.sprintf("update t_oe_element_state set measure_val=%d,is_confirm=0 where ied_no=%d and cpu_no=%d and group_no=%d and entry=%d",
		val,reference->ied_no,reference->cpu_no,reference->group_no,reference->entry);

	if (DB->Execute(sql) && MDB->GetDatabasePool()->GetDatabaseByIdx(0)->GetStatus() != SDatabase::DBERROR && MDB->Execute(sql))
	{
		LOGDEBUG("装置[%d,%d,%d,%d]的[%s]数值[%f]更新成功。",reference->ied_no,reference->cpu_no,
			reference->group_no,reference->entry,reference->desc.data(),val);
	}
	else
	{
		LOGWARN("装置[%d,%d,%d,%d]的[%s]数值[%f]更新失败。",reference->ied_no,reference->cpu_no,
			reference->group_no,reference->entry,reference->desc.data(),val);
		return false;
	}

	return true;
}

void MMSClient::loopSetting()
{
	unsigned long pos = 0;
	unsigned long ds_pos;
	unsigned long fc_pos;
	SPtrList<SString> dom_name;
	SPtrList<SString> var_name;
	SPtrList<MMSValue> *value;
	SPtrList< SPtrList<MMSValue> > lstValue;
	SPtrList<t_reference> lstFc;
	dom_name.setAutoDelete(true);
	var_name.setAutoDelete(true);
	lstValue.setAutoDelete(true);
	
	CSMmsDataset *ds = m_Datasets.FetchFirst(ds_pos);
	while(ds)
	{
		if (ds->m_iDsType == DATASET_SETTING)
		{
			pos = 0;
			stuSMmsDataNode *fc = ds->m_Items.FetchFirst(fc_pos);
			while(fc)
			{
				SString *dn = new SString(SString::GetIdAttribute(1,fc->sMmsPath,"/"));
				dom_name.append(dn);
				SString *vn = new SString(SString::GetIdAttribute(2,fc->sMmsPath,"/"));
				var_name.append(vn);
				value = new SPtrList<MMSValue>;
				value->setAutoDelete(true);
				lstValue.append(value);
				lstFc.append((t_reference*)fc->user);
				if (++pos >= MAX_READ_VALUES_NUMBER)
				{
					LOGDEBUG("读多值[%d]准备，子站号[%d]装置MMS名称[%s]。",pos,m_subNo,ds->m_sIedName.data());
					ReadValues(dom_name,var_name,lstValue);
					LOGDEBUG("读多值[%d]完毕，子站号[%d]装置MMS名称[%s]。",pos,m_subNo,ds->m_sIedName.data());
					dbSettingValue(lstFc,lstValue);
					LOGDEBUG("多值[%d]入库完毕，子站号[%d]装置MMS名称[%s]。",pos,m_subNo,ds->m_sIedName.data());
					dom_name.clear();
					var_name.clear();
					lstValue.clear();
					lstFc.clear();
					pos = 0;
				}
				
				fc = ds->m_Items.FetchNext(fc_pos);
			}
			if (pos > 0)
			{
				LOGDEBUG("读多值[%d]准备，子站号[%d]装置MMS名称[%s]。",pos,m_subNo,ds->m_sIedName.data());
				ReadValues(dom_name,var_name,lstValue);
				LOGDEBUG("读多值[%d]完毕，子站号[%d]装置MMS名称[%s]。",pos,m_subNo,ds->m_sIedName.data());
				dbSettingValue(lstFc,lstValue);
				LOGDEBUG("多值[%d]入库完毕，子站号[%d]装置MMS名称[%s]。",pos,m_subNo,ds->m_sIedName.data());
				dom_name.clear();
				var_name.clear();
				lstValue.clear();
				lstFc.clear();
				pos = 0;
			}
		}

		ds = m_Datasets.FetchNext(ds_pos);
	}
}

bool MMSClient::dbSettingValue(SPtrList<t_reference> &lstFc,SPtrList< SPtrList<MMSValue> > &lstValue,bool isEditArea)
{
	SString	sql;

	if (!lstValue.count())
		return false;

	for (int i = 0; i < lstValue.count(); i++)
	{
		t_reference * reference = (t_reference*)lstFc.at(i);
		if (!lstValue.at(i)->at(0))
			continue;

		SString val = "";
		for (int j = 0; j < lstValue.at(i)->count(); j++)
		{
			val = lstValue.at(i)->at(j)->getStrValue();
			if (val != "{")
				break;
		}
		
		if (!isEditArea)//当前区
		{
			if (reference->strValue == val)
			{
				LOGDEBUG("装置[%d,%d,%d,%d]的[%s]数值[%s]与内存数值[%s]相同，忽略更新。",reference->ied_no,reference->cpu_no,
					reference->group_no,reference->entry,reference->desc.data(),val.data(),reference->strValue.data());
				continue;
			}
		}
		if (!isEditArea)//当前区
		{
			sql.sprintf("update %s set current_val='%s' where ied_no=%d and cpu_no=%d and group_no=%d and entry=%d",
				reference->tableName.data(),val.data(),reference->ied_no,reference->cpu_no,reference->group_no,reference->entry);
		}
		else//编辑区或指定区
		{
			sql.sprintf("update %s set reference_val='%s' where ied_no=%d and cpu_no=%d and group_no=%d and entry=%d",
				reference->tableName.data(),val.data(),reference->ied_no,reference->cpu_no,reference->group_no,reference->entry);
		}
		if (DB->Execute(sql) && MDB->GetDatabasePool()->GetDatabaseByIdx(0)->GetStatus() != SDatabase::DBERROR && MDB->Execute(sql))
		{
			LOGDEBUG("装置[%d,%d,%d,%d]的[%s]数值[%s]替代[%s]更新成功。",reference->ied_no,reference->cpu_no,
				reference->group_no,reference->entry,reference->desc.data(),val.data(),reference->strValue.data());
			if (!isEditArea)//当前区
			{
				//dbSettingChange(reference,val);//定值变化告警信息
				reference->strValue = val;
			}
		}
		else
		{
			LOGWARN("装置[%d,%d,%d,%d]的[%s]数值[%s]替代[%s]更新失败。",reference->ied_no,reference->cpu_no,
				reference->group_no,reference->entry,reference->desc.data(),val.data(),reference->strValue.data());
			return false;
		}
	}

	return true;
}

bool MMSClient::dbAnalogValue(SPtrList<t_reference> &lstFc,SPtrList< SPtrList<MMSValue> > &lstValue, int soc)
{
	bool ret = dbSettingValue(lstFc, lstValue, false);
	if (!ret)
		return false;

	SPtrList<MMSValue> *valList = NULL;
	SDateTime dt = SDateTime::makeDateTime(soc);
	unsigned int cnt_soc = SDateTime::makeSOC(dt.year(), dt.month(), dt.day(), 0, 0, 0);
	MMSValue *mmsval = NULL;

	t_reference *lastReference = NULL;
	static unsigned int lastCntSoc = 0;

	for (int row = 0; row < lstFc.count(); ++row)
	{
		t_reference *reference = (t_reference *)lstFc.at(row);
		valList = lstValue.at(row);
		if (valList == NULL || valList->count() == 0)
			continue;
		mmsval = (MMSValue *)valList->at(0);

		SString sql = SString::toFormat("insert into t_oe_his_analog(gen_sn, soc, current_val) values(%d, %d, %6.3f)", 
			reference->gen_sn, soc, mmsval->getFloatValue());
		DB->Execute(sql);

		if (lastReference != reference || lastCntSoc != cnt_soc)	// 减少查询数据库次数 and 防止只统计一台设备，当时间更新后不插入新记录
		{
			sql = SString::toFormat("select gen_sn from t_oe_his_analog_hcnt where gen_sn=%d and cnt_soc=%d", reference->gen_sn, cnt_soc);
			SRecordset rs;
			DB->Retrieve(sql, rs);
			if (rs.GetRows() == 0)
			{
				//sql = SString::toFormat("insert into t_oe_his_analog_hcnt(gen_sn,sub_no,ied_no,cpu_no,group_no,entry,cnt_soc) 
				sql = SString::toFormat("insert into t_oe_his_analog_hcnt(gen_sn,cnt_soc) \
										values(%d,%d)",
										reference->gen_sn, cnt_soc);
				if (!DB->Execute(sql))
				{
					LOGERROR("执行SQL失败,sql=%s", sql.data());
					continue;
				}
			}
		}

		// 0-1点数据存在val1中;23-24点数据存在val24中
		sql = SString::toFormat("update t_oe_his_analog_hcnt set val%d=%0.3f where gen_sn=%d and cnt_soc=%d", 
			dt.hour() + 1, mmsval->getFloatValue(), reference->gen_sn, cnt_soc);
		DB->Execute(sql);

		if (dt.hour() == 23)
		{
			// 计算一天的平均值

			// 统计存表 t_oe_his_analog_dcnt
		}

		lastReference = reference;
		lastCntSoc = cnt_soc;
	}

	return ret;
}

bool MMSClient::dbParamCheckValue(std::vector<ParamCheckItem *> reqList, SPtrList<t_reference> &lstFc,SPtrList< SPtrList<MMSValue> > &lstValue, bool isManualRequest, bool &isParamDiff)
{
	if (reqList.size() != lstFc.count() || reqList.size() != lstValue.count())
		return false;

	bool ret = true;
	SRecordset rs;
	t_reference *reference = NULL;
	t_reference *alarmRef = NULL;
	ParamCheckItem *reqItem = NULL;
	MMSValue *mmsval = NULL;
	Ied *ied = NULL;
	isParamDiff = false;

	unsigned int soc = (unsigned int)time(NULL);
	for (int row = 0; row < lstFc.count(); ++row)
	{
		reference = (t_reference *)lstFc.at(row);
		reqItem = (ParamCheckItem *)reqList.at(row);
		reqItem->last_val = reqItem->current_val;
		reqItem->last_soc = soc;
		ied = getIed(reqItem->ied_no);

		SPtrList<MMSValue> *valList = lstValue.at(row);
		if (ied == NULL || valList == NULL || valList->count() == 0)
			continue;

		alarmRef = ied->getParamChangeRef();
		mmsval = (MMSValue *)valList->at(0);
		SString currentValue;
		SString contrastValue;
		bool isSameValue = true;

		if (m_globalConfig->paramchk_type == 0)
			contrastValue = reqItem->reference_val;
		else
			contrastValue = reqItem->last_val;

		switch(reqList.at(row)->valtype)
		{
		case 3:
		case 9:
			currentValue = SString::toFormat("%d", mmsval->getInt32Value());
			if (mmsval->getInt32Value() != contrastValue.toInt())
			{
				isSameValue = false;
			}
			break;
		case 4:
			currentValue = SString::toFormat("%d", mmsval->getUint32Value());
			if (mmsval->getUint32Value() != contrastValue.toUInt())
			{
				isSameValue = false;
			}
			break;
		case 7:
			currentValue = SString::toFormat("%6.3f", mmsval->getFloatValue());
			if (abs(mmsval->getFloatValue() - contrastValue.toFloat()) > reference->threshold)
			{
				isSameValue = false;
			}
			break;
		default:
			currentValue = SString::toFormat("%s", mmsval->getStrValue().data());
			if (currentValue != contrastValue)
			{
				isSameValue = false;
			}
		}
		reqItem->current_val = currentValue;

		if (!isSameValue && alarmRef)
		{
			isParamDiff = true;
		}

		if (isManualRequest)		// 手动模式先确定数据库中有记录，没有则新增
		{
			SString sql = SString::toFormat("select current_val from t_oe_param_check_val where ied_no=%d and cpu_no=%d and group_no=%d and entry=%d and type=%d",
				reference->ied_no, reference->cpu_no, reference->group_no, reference->entry, isManualRequest ? 1 : 0);
			DB->Retrieve(sql, rs);
			if (rs.GetRows() == 0)
			{
				sql = SString::toFormat("insert into t_oe_param_check_val(gen_sn, ied_no, cpu_no, group_no, entry, current_val, soc, type) \
										values(%d, %d, %d, %d, %d, '%s', %d, %d)",
										reference->gen_sn, reference->ied_no, reference->cpu_no, reference->group_no, reference->entry,
										currentValue.data(), soc, isManualRequest ? 1 : 0);
				if (!DB->Execute(sql))
				{
					LOGERROR("插入定值巡检记录失败，sql=%s", sql.data());
					ret = false;
				}
				else
					continue;
			}
		}

		SString sql = SString::toFormat("update t_oe_param_check_val set current_val='%s', soc=%d where ied_no=%d and cpu_no=%d and group_no=%d and entry=%d and type=%d", 
			currentValue.data(), soc, reference->ied_no, reference->cpu_no, reference->group_no, reference->entry, isManualRequest ? 1 : 0);
		if (!DB->Execute(sql))
		{
			// 检查条目是否存在，不存在则插入记录
			sql = SString::toFormat("select current_val from t_oe_param_check_val where ied_no=%d and cpu_no=%d and group_no=%d and entry=%d and type=%d",
				reference->ied_no, reference->cpu_no, reference->group_no, reference->entry, isManualRequest ? 1 : 0);
			DB->Retrieve(sql, rs);
			if (rs.GetRows() == 0)
			{
				sql = SString::toFormat("insert into t_oe_param_check_val(gen_sn, ied_no, cpu_no, group_no, entry, current_val, soc, type) \
										values(%d, %d, %d, %d, %d, '%s', %d, %d)",
										reference->gen_sn, reference->ied_no, reference->cpu_no, reference->group_no, reference->entry,
										currentValue.data(), soc, isManualRequest ? 1 : 0);
				if (!DB->Execute(sql))
				{
					LOGERROR("插入定值巡检记录失败，sql=%s", sql.data());
					ret = false;
				}
			}
		}
	}

	return ret;
}

bool MMSClient::ReadParamCheckValue(int ied_no, int cpu_no, int group_no, bool isManual, int timeOut)
{
	Ied *ied = this->getIed(ied_no);
	if (!ied)
	{
		LOGERROR("请求定值校核设备号(%d)不存在", ied_no);
		return false;
	}

	std::vector<ParamCheckItem *> paramCheckItem;
	ied->getParamCheckList(paramCheckItem);

	// 开始定值巡检
	bool ret = false;
	unsigned long pos = 0;
	unsigned long ds_pos = 0;
	unsigned long fc_pos = 0;
	SPtrList<SString> dom_name;
	SPtrList<SString> var_name;
	SPtrList<MMSValue> *value;
	SPtrList< SPtrList<MMSValue> > lstValue;
	SPtrList<t_reference> lstFc;
	dom_name.setAutoDelete(true);
	var_name.setAutoDelete(true);
	lstValue.setAutoDelete(true);
	std::vector<ParamCheckItem *> reqList;
	unsigned int current_soc = (unsigned int)::time(NULL);
	bool isParamDiff = false;		// 是否存在定值变化

	for (std::vector<ParamCheckItem *>::iterator iter = paramCheckItem.begin(); iter != paramCheckItem.end(); ++iter)
	{
		if (cpu_no > 0 && (*iter)->cpu_no != cpu_no)
			continue;
		
		if (group_no > 0 && (*iter)->group_no != group_no)
			continue;

		SString *dn = new SString(SString::GetIdAttribute(1, (*iter)->mms_path,"/"));
		dom_name.append(dn);
		SString *vn = new SString(SString::GetIdAttribute(2, (*iter)->mms_path,"/"));
		var_name.append(vn);
		value = new SPtrList<MMSValue>;
		value->setAutoDelete(true);
		lstValue.append(value);
		lstFc.append((*iter)->reference);
		reqList.push_back(*iter);

		if (++pos >= MAX_READ_VALUES_NUMBER)
		{
			LOGDEBUG("读多值[%d]准备，子站号[%d]装置MMS名称[%s]。",pos,m_subNo, getIed(ied_no)->iedName());
			ret = ReadValues(dom_name,var_name,lstValue,timeOut);
			if (ret)
			{
				LOGDEBUG("读多值[%d]完毕，子站号[%d]装置MMS名称[%s]。",pos,m_subNo, getIed(ied_no)->iedName());
				bool isDiff = false;
				dbParamCheckValue(reqList, lstFc,lstValue, isManual, isDiff);
				if (isDiff)
					isParamDiff = true;
				LOGDEBUG("多值[%d]入库完毕，子站号[%d]装置MMS名称[%s]。",pos,m_subNo, getIed(ied_no)->iedName());
			}
			dom_name.clear();
			var_name.clear();
			lstValue.clear();
			lstFc.clear();
			reqList.clear();
			pos = 0;

			//if (ret && lstValue.count() == reqList.size())
			//{
			//	for (size_t row = 0; row < reqList.size(); ++row)
			//	{
			//		t_reference *reference = (t_reference *)lstFc.at(row);
			//		MMSValue *mmsval = (MMSValue *)lstValue.at(row);

			//		if (reference->gen_sn == reqList[row]->gen_sn)
			//		{
			//			reqList[row]->current_val = mmsval->getStrValue();

			//			// 判断当前值和参考值 或 当前值和上一次召唤值
			//		}
			//	}
			//}

			if (!ret)
				break;
		}
	}

	if (dom_name.count() > 0)
	{
		LOGDEBUG("读多值[%d]准备，子站号[%d]装置MMS名称[%s]。",pos,m_subNo, getIed(ied_no)->iedName());
		ret = ReadValues(dom_name,var_name,lstValue,timeOut);
		if (ret)
		{
			LOGDEBUG("读多值[%d]完毕，子站号[%d]装置MMS名称[%s]。",pos,m_subNo, getIed(ied_no)->iedName());
			bool isDiff = false;
			dbParamCheckValue(reqList, lstFc,lstValue, isManual, isDiff);
			if (isDiff)
				isParamDiff = true;
			LOGDEBUG("多值[%d]入库完毕，子站号[%d]装置MMS名称[%s]。",pos,m_subNo, getIed(ied_no)->iedName());
		}

		//if (ret && lstValue.count() == reqList.size())
		//{
		//	for (size_t row = 0; row < reqList.size(); ++row)
		//	{
		//		t_reference *reference = (t_reference *)lstFc.at(row);
		//		MMSValue *mmsval = (MMSValue *)lstValue.at(row);

		//		if (reference->gen_sn == reqList[row]->gen_sn)
		//		{
		//			reqList[row]->current_val = mmsval->getStrValue();
		//		}
		//	}
		//}

		dom_name.clear();
		var_name.clear();
		lstValue.clear();
		lstFc.clear();
		reqList.clear();
		pos = 0;
	}

	if (!isManual)
		ied->updateParamCheckSoc(current_soc);

	if (isParamDiff)
	{
		t_reference *alarmRef = ied->getParamChangeRef();
		if (alarmRef)
		{
			// 告警
			SString sql;
			
			if(alarmRef->cpu_no != 255)
			{
				sql.sprintf("update t_oe_ied set is_confirm=0 where ied_no=%d",alarmRef->ied_no);
				DB->Execute(sql);
				MDB->Execute(sql);
			}

			 sql.sprintf("update t_oe_element_state set current_val='%d', soc=%d,is_confirm=0 where ied_no=%d and cpu_no=%d and group_no=%d and entry=%d"
				, 2, current_soc, alarmRef->ied_no, alarmRef->cpu_no, alarmRef->group_no, alarmRef->entry);
			if (!DB->Execute(sql))
			{
				LOGERROR("执行SQL语句错误,sql=%s", sql.data());
			}
			MDB->ExecuteSQL(sql);

			sql = SString::toFormat("insert into t_oe_his_event(sub_no,ied_no,cpu_no,group_no,entry,"
				"act_soc,act_usec,fun,inf,type,"
				"severity_level,rettime,dpi,nof,fan,"
				"act_q,time_q,reason,st_sn) values "
				"(%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,'%s',%d)",
				m_subNo, alarmRef->ied_no, alarmRef->cpu_no, alarmRef->group_no, alarmRef->entry,
				current_soc, 0, alarmRef->fun, alarmRef->inf, alarmRef->type, 
				alarmRef->level, 0, 2, 0, 0,
				0, 0, "", alarmRef->st_sn);
			if (!DB->Execute(sql))
			{
				LOGERROR("执行SQL语句错误,sql=%s", sql.data());
			}
		}
	}
	onParamTimerChecked(isManual, isParamDiff);

	return true;
}

bool MMSClient::ReadValueSetting(int ied_no,int cpu_no,int group_no,int timeOut/*=10*/)
{
	bool ret = false;
	unsigned long pos = 0;
	unsigned long ds_pos;
	unsigned long fc_pos;
	SPtrList<SString> dom_name;
	SPtrList<SString> var_name;
	SPtrList<MMSValue> *value;
	SPtrList< SPtrList<MMSValue> > lstValue;
	SPtrList<t_reference> lstFc;
	dom_name.setAutoDelete(true);
	var_name.setAutoDelete(true);
	lstValue.setAutoDelete(true);

	SString		sql;
	SRecordset	rs;
	int			cnt;
	SString		iedMmsPath;
	SString     groupTypeDesc;
	SString     groupName;

	sql.sprintf("select mms_path from t_oe_ied where ied_no=%d",ied_no);
	cnt = DB->Retrieve(sql,rs);
	if (cnt < 0)
	{
		LOGFAULT("数据库语句操作失败，SQL：%s",sql.data());
		return false;
	}
	else if (cnt == 0)
	{
		LOGWARN("读取值信息，未发现装置[%d]。",ied_no);
		return false;
	}
	else if (cnt > 0)
	{
		iedMmsPath = rs.GetValue(0,0);
	}

	sql.sprintf("select name,type,mms_path from t_oe_group where ied_no=%d and cpu_no=%d and group_no=%d",ied_no,cpu_no,group_no);
	cnt = DB->Retrieve(sql,rs);
	if (cnt < 0)
	{
		LOGFAULT("数据库语句操作失败，SQL：%s",sql.data());
		return false;
	}
	else if (cnt == 0)
	{
		LOGWARN("读取当前定值区定值，未发现装置有定值组[%d,%d,%d]。",ied_no,cpu_no,group_no);
		return false;
	}
	else if (cnt > 0)
	{
		groupName = rs.GetValue(0, 0);
		int type = rs.GetValue(0,1).toInt();
		if (type == DATASET_SETTING)
			groupTypeDesc = "定值组";
		else if (type == DATASET_RELAY_AIN)
			groupTypeDesc = "遥测组";
		else if (type == DATASET_RELAY_DIN)
			groupTypeDesc = "开关组";
		else if (type == DATASET_RELAY_ENA)
			groupTypeDesc = "压板组";
		else
		{
			LOGWARN("读取当前值数据内容，装置组[%d,%d,%d]不是有效数据组。",ied_no,cpu_no,group_no);
			return false;
		}

		SString mmsPath = rs.GetValue(0,2);
		CSMmsDataset *ds = m_Datasets.FetchFirst(ds_pos);
		while(ds)
		{
			if (ds->m_sDsPath == mmsPath.data() && ds->m_sIedName == iedMmsPath.data())
			{
				pos = 0;
				stuSMmsDataNode *fc = ds->m_Items.FetchFirst(fc_pos);
				while(fc)
				{
					SString *dn = new SString(SString::GetIdAttribute(1,fc->sMmsPath,"/"));
					dom_name.append(dn);
					SString *vn = new SString(SString::GetIdAttribute(2,fc->sMmsPath,"/"));
					var_name.append(vn);
					value = new SPtrList<MMSValue>;
					value->setAutoDelete(true);
					lstValue.append(value);
					lstFc.append((t_reference*)fc->user);
					if (++pos >= MAX_READ_VALUES_NUMBER)
					{
						LOGDEBUG("读多值[%d]准备，子站号[%d]装置MMS名称[%s]。",pos,m_subNo,ds->m_sIedName.data());
						ret = ReadValues(dom_name,var_name,lstValue,timeOut);
						if (ret)
						{
							LOGDEBUG("读多值[%d]完毕，子站号[%d]装置MMS名称[%s]。",pos,m_subNo,ds->m_sIedName.data());
							dbSettingValue(lstFc,lstValue);
							LOGDEBUG("多值[%d]入库完毕，子站号[%d]装置MMS名称[%s]。",pos,m_subNo,ds->m_sIedName.data());
						}
						dom_name.clear();
						var_name.clear();
						lstValue.clear();
						lstFc.clear();
						pos = 0;

						if (!ret)
							break;	//while(fc)
					}

					fc = ds->m_Items.FetchNext(fc_pos);
				}
				if (pos > 0)
				{
					LOGDEBUG("读多值[%d]准备，子站号[%d]装置MMS名称[%s]。",pos,m_subNo,ds->m_sIedName.data());
					ret = ReadValues(dom_name,var_name,lstValue,timeOut);
					if (ret)
					{
						LOGDEBUG("读多值[%d]完毕，子站号[%d]装置MMS名称[%s]。",pos,m_subNo,ds->m_sIedName.data());
						dbSettingValue(lstFc,lstValue);
						LOGDEBUG("多值[%d]入库完毕，子站号[%d]装置MMS名称[%s]。",pos,m_subNo,ds->m_sIedName.data());
					}
					dom_name.clear();
					var_name.clear();
					lstValue.clear();
					lstFc.clear();
					pos = 0;
				}

				break;	//while(ds)
			}

			ds = m_Datasets.FetchNext(ds_pos);
		}
	}

	return ret;
}

bool MMSClient::GetLatestModelFile(int ied_no, ModelFileFormat fmt, SString &localFile, int timeOut)
{
	SString directory = "/";
	SString ext = getModelFileFormatName(fmt);
	if (ext.isEmpty())
	{
		LOGERROR("未知的文件后缀类型(%d),将不进行列文件目录操作", ext.data());
		return false;
	}
	
	SPtrList<stuSMmsFileInfo> slFiles;
	SString lastFileName;
	unsigned int lastFileSoc = 0;
	stuSMmsFileInfo *fileInfo = NULL;

	if (!this->GetMvlDirectory(directory, slFiles))
	{
		LOGERROR("列装置(%d)目录(%s)文件失败", ied_no, directory.data());
		return false;
	}

	for (int i = 0; i < slFiles.count(); ++i)
	{
		fileInfo = slFiles.at(i);
		if (fileInfo->filename.right(3).CompareNoCase(ext) == 0)
		{
			if (fileInfo->mtime > lastFileSoc)
			{
				lastFileName = fileInfo->filename;
				lastFileSoc = (unsigned int)fileInfo->mtime;
			}
		}
	}

	if (lastFileName.isEmpty())
	{
		LOGERROR("未找到设备(%d)最新的CID文件", ied_no);
		return false;
	}

	// 下载最新的文件
	SString remoteFile = SString::toFormat("%s%s", directory.data(), lastFileName.data());
	SString localDirectory = GetIedDataDir(ied_no);
	localFile = SString::toFormat("%s/%s", localDirectory.data(), lastFileName.data());

	if (!this->DownMvlFile(remoteFile, localFile))
	{
		LOGERROR("下载文件(%s)到本地(%s)失败", remoteFile.data(), localFile.data());
		return false;
	}

	return true;
}

SString MMSClient::GetIedDataDir(int ied_no)
{
	SString directory = SString::toFormat("%s/data/%d/%d/", SBASE_SAPP->GetHomePath().data(), m_subNo, ied_no);
	if (!SDir::dirExists(directory))
		SDir::createDir(directory);

	return directory;
}

const char * MMSClient::getModelFileFormatName(ModelFileFormat fmt)
{
	switch (fmt)
	{
	case CID:
		return "CID";

	case ICD:
		return "ICD";

	case SCD:
		return "SCD";

	case CCD:
		return "CCD";

	default:
		return "";
	}
}

bool MMSClient::dbSettingChange(t_reference *reference,SString new_val)
{
	SString	sql;
	int		soc;
	int		usec;
	SDateTime::getSystemTime(soc,usec);
	SString details = SString::toFormat("[%s]数值[%s]变化为[%s]",
		reference->desc.data(),reference->strValue.data(),new_val.data());

	sql.sprintf("update t_oe_element_state set current_val=1,soc=%d,usec=%d,details='%s',is_confirm=0 "
		"where ied_no=%d and cpu_no=255 and group_no=%d and entry=%d",
		soc,usec,details.data(),
		reference->ied_no,SETTING_CHANGE_GROUP,SETTING_CHANGE_ELEMENT);

	if (DB->Execute(sql) && MDB->GetDatabasePool()->GetDatabaseByIdx(0)->GetStatus() != SDatabase::DBERROR && MDB->Execute(sql))
	{
		LOGDEBUG("装置[%d,255,%d,%d]的[定值变化]数值[1]更新成功。定值变化内容:[%d,%d,%d,%d]的[%s]数值[%s]变化为[%s]。",
			reference->ied_no,SETTING_CHANGE_GROUP,SETTING_CHANGE_ELEMENT,
			reference->ied_no,reference->cpu_no,reference->group_no,reference->entry,
			reference->desc.data(),reference->strValue.data(),new_val.data());
		dbElementStateDigitalHistory(reference->st_sn,reference->ied_no,255,SETTING_CHANGE_GROUP,SETTING_CHANGE_ELEMENT,1);
		m_settingChangeTime = soc;
	}
	else
	{
		LOGWARN("装置[%d,255,%d,%d]的[定值变化]数值[1]更新失败。",
			reference->ied_no,SETTING_CHANGE_GROUP,SETTING_CHANGE_ELEMENT);
		return false;
	}

	return true;
}

bool MMSClient::dbSettingChangeReset()
{
	SString		sql;
	SRecordset	rs;
	int			soc;
	int			usec;
	SDateTime::getSystemTime(soc,usec);
	SString details = "定值变化自复归";

	//sql.sprintf("select ied_no,cpu_no,group_no,entry from t_oe_element_state "
	//	"where ied_no=%d and cpu_no=255 and group_no=%d and entry=%d and current_val=1",
	//	m_iedNo,SETTING_CHANGE_GROUP,SETTING_CHANGE_ELEMENT);
	int cnt = MDB->Retrieve(sql,rs);
	if (cnt < 0)
	{
		LOGFAULT("数据库语句操作失败，SQL：%s",sql.data());
		return false;
	}
	else if (cnt > 0)
	{
		for (int i = 0; i < cnt; i++)
		{
			if(rs.GetValue(i,1).toInt() != 255)
			{
				sql.sprintf("update t_oe_ied set is_confirm=0 where ied_no=%d",rs.GetValue(i,0).toInt());
				DB->Execute(sql);			
				MDB->Execute(sql);
			}

			sql.sprintf("update t_oe_element_state set current_val=0,soc=%d,usec=%d,details='%s',is_confirm=0 "
				"where ied_no=%d and cpu_no=%d and group_no=%d and entry=%d",
				soc,usec,details.data(),
				rs.GetValue(i,0).toInt(),rs.GetValue(i,1).toInt(),rs.GetValue(i,2).toInt(),rs.GetValue(i,3).toInt());
			if (DB->Execute(sql) && 
				MDB->GetDatabasePool()->GetDatabaseByIdx(0)->GetStatus() != SDatabase::DBERROR && MDB->Execute(sql))
			{
				LOGDEBUG("装置[%d,%d,%d,%d]的[定值变化]复位数值[0]更新成功。",
					rs.GetValue(i,0).toInt(),rs.GetValue(i,1).toInt(),rs.GetValue(i,2).toInt(),rs.GetValue(i,3).toInt());
				//dbElementStateDigitalHistory(rs.GetValue(i,0).toInt(),255,SETTING_CHANGE_GROUP,SETTING_CHANGE_ELEMENT,0);
			}
			else
			{
				LOGWARN("装置[%d,%d,%d,%d]的[定值变化]复位数值[0]更新失败。",
					rs.GetValue(i,0).toInt(),rs.GetValue(i,1).toInt(),rs.GetValue(i,2).toInt(),rs.GetValue(i,3).toInt());
			}
		}
	}

	return true;
}

bool MMSClient::dbCommStateChange(int ied_no, int state)
{
	SString	sql;
	int		soc;
	int		usec;
	SDateTime::getSystemTime(soc,usec);

	//add by skt IED通信状态写入syslog
	SString ied_name = DB->SelectInto(SString::toFormat("select name from t_oe_ied where ied_no=%d",ied_no));
	sql.sprintf("insert into t_ssp_syslog(soc,usec,usr_sn,monitor_sn,log_type,log_level,id1,id2,id3,id4,log_text) values(%d,%d,0,0,%d,%d,0,0,0,0,'IED(%d-%s)通信%s!')",
		soc,usec,state==0?12:11,2,ied_no,ied_name.data(),state==0?"断开":"恢复");
	DB->Execute(sql);
	if(MDB != NULL)
		MDB->Execute(sql);

	SString details = SString::toFormat("设备[%d]通信状态变化为[%d]",
		ied_no, state);

	sql.sprintf("update t_oe_element_state set current_val=%d,soc=%d,usec=%d,details='%s',is_confirm=0 "
		"where ied_no=%d and cpu_no=255 and group_no=%d and entry=%d",
		state,soc,usec,details.data(),
		ied_no,SETTING_CHANGE_GROUP,CHANNEL_STATE);

	return DB->ExecuteSQL(sql);
}

bool MMSClient::ResetLed(int iedNo)
{
	bool		ret = false;
	SString		sql;
	SRecordset	rs;
	int			m_iedNo = iedNo;

	sql.sprintf("select mms_path from t_oe_element_control where ied_no=%d and mms_path like '%%$CO$LEDRs'",m_iedNo);
	int cnt = DB->Retrieve(sql,rs);
	if (cnt < 0)
	{
		LOGFAULT("数据库语句操作失败，SQL：%s",sql.data());
		return false;
	}
	else if (cnt == 0)
	{
		LOGWARN("装置[%d]信号复归，未发现控制点。",m_iedNo);
	}
	else if (cnt > 0)
	{
		for (int i = 0; i < cnt; i++)
		{
			SString mms = rs.GetValue(i,0).data();
			SString dn = SString::GetIdAttribute(1,mms,"/");
			SString vn = SString::GetIdAttribute(2,mms,"/");
			vn += "$Oper";
			ret = Reset(dn.data(),vn.data());
		}
	}

	return ret;
}

bool MMSClient::ResetIed(int iedNo)
{
	bool		ret = false;
	SString		sql;
	SRecordset	rs;
	int			m_iedNo = iedNo;

	sql.sprintf("select mms_path from t_oe_element_control where ied_no=%d and mms_path like '%%$CO$IEDRs'",m_iedNo);
	int cnt = DB->Retrieve(sql,rs);
	if (cnt < 0)
	{
		LOGFAULT("数据库语句操作失败，SQL：%s",sql.data());
		return false;
	}
	else if (cnt == 0)
	{
		LOGWARN("装置[%d]设备复位，未发现控制点。",m_iedNo);
	}
	else if (cnt > 0)
	{
		for (int i = 0; i < cnt; i++)
		{
			SString mms = rs.GetValue(i,0).data();
			SString dn = SString::GetIdAttribute(1,mms,"/");
			SString vn = SString::GetIdAttribute(2,mms,"/");
			vn += "$Oper";
			ret = Reset(dn.data(),vn.data());
		}
	}

	return ret;
}

bool MMSClient::ControlDirect(int ied_no,int cpu_no,int group_no,int entry,int value,int timeOut)
{
	bool		ret = false;
	SString		sql;
	SRecordset	rs;
	int			model = 0;

	sql.sprintf("select model,mms_path from t_oe_element_control where ied_no=%d and cpu_no=%d and group_no=%d and entry=%d",
		ied_no,cpu_no,group_no,entry);
	int cnt = DB->Retrieve(sql,rs);
	if (cnt < 0)
	{
		LOGFAULT("数据库语句操作失败，SQL：%s",sql.data());
		return false;
	}
	else if (cnt == 0)
	{
		LOGWARN("装置直控，子站[%d]未发现直控点[%d,%d,%d,%d]。",m_subNo,ied_no,cpu_no,group_no,entry);
	}
	else if (cnt > 0)
	{
		if (cnt > 1)
		{
			LOGWARN("装置直控，子站[%d]直控点[%d,%d,%d,%d]有重复，请检查。",m_subNo,ied_no,cpu_no,group_no,entry);
		}
		else
		{
			SString mms = rs.GetValue(0,1).data();
			SString dn = SString::GetIdAttribute(1,mms,"/");
			SString vn = SString::GetIdAttribute(2,mms,"/");
			vn += "$Oper";
			ret = ProcessControl(dn.data(),vn.data(),value,false,timeOut);
			if (ret)
			{
				LOGDEBUG("装置直控，子站[%d]直控点[%d,%d,%d,%d]直控成功。",m_subNo,ied_no,cpu_no,group_no,entry,model);
			}
			else
			{
				LOGWARN("装置直控，子站[%d]直控点[%d,%d,%d,%d]直控失败。",m_subNo,ied_no,cpu_no,group_no,entry,model);
			}
		}
	}

	return ret;
}

bool MMSClient::ControlSelect(int ied_no,int cpu_no,int group_no,int entry,int value,int timeOut)
{
	bool		ret = false;
	SString		sql;
	SRecordset	rs;
	int			model = 0;

	sql.sprintf("select model,mms_path from t_oe_element_control where ied_no=%d and cpu_no=%d and group_no=%d and entry=%d",
		ied_no,cpu_no,group_no,entry);
	int cnt = DB->Retrieve(sql,rs);
	if (cnt < 0)
	{
		LOGFAULT("数据库语句操作失败，SQL：%s",sql.data());
		return false;
	}
	else if (cnt == 0)
	{
		LOGWARN("装置遥控，子站[%d]未发现遥控点[%d,%d,%d,%d]。",m_subNo,ied_no,cpu_no,group_no,entry);
	}
	else if (cnt > 0)
	{
		if (cnt > 1)
		{
			LOGWARN("装置遥控，子站[%d]遥控点[%d,%d,%d,%d]有重复，请检查。",m_subNo,ied_no,cpu_no,group_no,entry);
		}
		else
		{
			model = rs.GetValue(0,0).toInt();
			if (model == 1 || model == 2 || model == 3)
			{
				LOGWARN("装置遥控，子站[%d]遥控点[%d,%d,%d,%d]非增强型操作前选择控制，请检查。",
					m_subNo,ied_no,cpu_no,group_no,entry);
			}
			else
			{
				SString mms = rs.GetValue(0,1).data();
				SString dn = SString::GetIdAttribute(1,mms,"/");
				SString vn = SString::GetIdAttribute(2,mms,"/");
				vn += "$SBOw";
				ret = ProcessControl(dn.data(),vn.data(),value,true,timeOut);
				if (ret)
				{
					LOGDEBUG("装置遥控，子站[%d]遥控点[%d,%d,%d,%d]类型[%d]遥控选择[%s]成功。",
						m_subNo,ied_no,cpu_no,group_no,entry,model,value==1?"合":"分");
				}
				else
				{
					LOGWARN("装置遥控，子站[%d]遥控点[%d,%d,%d,%d]类型[%d]遥控选择[%s]失败。",
						m_subNo,ied_no,cpu_no,group_no,entry,model,value==1?"合":"分");
				}
			}
		}
	}

	return ret;
}

bool MMSClient::ControlExecute(int ied_no,int cpu_no,int group_no,int entry,int value,int timeOut)
{
	bool		ret = false;
	SString		sql;
	SRecordset	rs;

	sql.sprintf("select model,mms_path from t_oe_element_control where ied_no=%d and cpu_no=%d and group_no=%d and entry=%d",
		ied_no,cpu_no,group_no,entry);
	int cnt = DB->Retrieve(sql,rs);
	if (cnt < 0)
	{
		LOGFAULT("数据库语句操作失败，SQL：%s",sql.data());
		return false;
	}
	else if (cnt == 0)
	{
		LOGWARN("装置遥控，子站[%d]未发现遥控点[%d,%d,%d,%d]。",m_subNo,ied_no,cpu_no,group_no,entry);
	}
	else if (cnt > 0)
	{
		if (cnt > 1)
		{
			LOGWARN("装置遥控，子站[%d]遥控点[%d,%d,%d,%d]有重复，请检查。",m_subNo,ied_no,cpu_no,group_no,entry);
		}
		else
		{
			int model = rs.GetValue(0,0).toInt();
			if (model == 1 || model == 2 || model == 3)
			{
				LOGWARN("装置遥控，子站[%d]遥控点[%d,%d,%d,%d]非增强型操作前选择控制，请检查。",
					m_subNo,ied_no,cpu_no,group_no,entry);
			}
			else
			{
				SString mms = rs.GetValue(0,1).data();
				SString dn = SString::GetIdAttribute(1,mms,"/");
				SString vn = SString::GetIdAttribute(2,mms,"/");
				vn += "$Oper";
				ret = ProcessControl(dn.data(),vn.data(),value,true,timeOut);
				if (ret)
				{
					LOGDEBUG("装置遥控，子站[%d]遥控点[%d,%d,%d,%d]类型[%d]遥控执行[%s]成功。",
						m_subNo,ied_no,cpu_no,group_no,entry,model,value==1?"合":"分");
				}
				else
				{
					LOGWARN("装置遥控，子站[%d]遥控点[%d,%d,%d,%d]类型[%d]遥控执行[%s]失败。",
						m_subNo,ied_no,cpu_no,group_no,entry,model,value==1?"合":"分");
				}
			}
		}
	}

	return ret;
}

bool MMSClient::ControlCancel(int ied_no,int cpu_no,int group_no,int entry,int value,int timeOut)
{
	bool		ret = false;
	SString		sql;
	SRecordset	rs;

	sql.sprintf("select model,mms_path from t_oe_element_control where ied_no=%d and cpu_no=%d and group_no=%d and entry=%d",
		ied_no,cpu_no,group_no,entry);
	int cnt = DB->Retrieve(sql,rs);
	if (cnt < 0)
	{
		LOGFAULT("数据库语句操作失败，SQL：%s",sql.data());
		return false;
	}
	else if (cnt == 0)
	{
		LOGWARN("装置遥控，子站[%d]未发现遥控点[%d,%d,%d,%d]。",m_subNo,ied_no,cpu_no,group_no,entry);
	}
	else if (cnt > 0)
	{
		if (cnt > 1)
		{
			LOGWARN("装置遥控，子站[%d]遥控点[%d,%d,%d,%d]有重复，请检查。",m_subNo,ied_no,cpu_no,group_no,entry);
		}
		else
		{
			int model = rs.GetValue(0,0).toInt();
			if (model == 1 || model == 2 || model == 3)
			{
				LOGWARN("装置遥控，子站[%d]遥控点[%d,%d,%d,%d]非增强型操作前选择控制，请检查。",
					m_subNo,ied_no,cpu_no,group_no,entry);
			}
			else
			{
				SString mms = rs.GetValue(0,1).data();
				SString dn = SString::GetIdAttribute(1,mms,"/");
				SString vn = SString::GetIdAttribute(2,mms,"/");
				vn += "$Cancel";
				ret = ProcessControl(dn.data(),vn.data(),value,true,timeOut);
				if (ret)
				{
					LOGDEBUG("装置遥控，子站[%d]遥控点[%d,%d,%d,%d]类型[%d]遥控撤消[%s]成功。",
						m_subNo,ied_no,cpu_no,group_no,entry,model,value==1?"合":"分");
				}
				else
				{
					LOGWARN("装置遥控，子站[%d]遥控点[%d,%d,%d,%d]类型[%d]遥控撤消[%s]失败。",
						m_subNo,ied_no,cpu_no,group_no,entry,model,value==1?"合":"分");
				}
			}
		}
	}

	return ret;
}

bool MMSClient::dbSubStateSyslog(SString text,int state)
{
	SString		sql;
	SRecordset	rs;

	int soc,usec;
	SDateTime::getSystemTime(soc,usec);
	SString log;
	if (state)
		log = m_subName+text+"-连接";
	else
		log = m_subName+text+"-中断";

	sql.sprintf("insert into t_ssp_syslog (soc,usec,usr_sn,log_type,log_level,id1,id2,id3,id4,log_text) values "
		"(%d,%d,%d,%d,%d,%d,%d,%d,%d,'%s')",
		soc,usec,
		0,
		state == 1 ? UK9010_LT_COMM_CONN : UK9010_LT_COMM_DISC,
		0,
		m_subNo,0,0,0,
		log.data());
	if (!DB->Execute(sql))
	{
		LOGFAULT("数据库语句操作失败，SQL：%s",sql.data());
		return false;
	}

	return true;
}

void MMSClient::SetEntryID(stuReportInfo *info, SString entryId)
{
	SString		sql;
	SRecordset	rs;
	sql.sprintf("update t_oe_group set entryID='%s' where ied_no=%d and cpu_no=%d and group_no=%d",
		entryId.data(),info->ied_no,info->cpu_no,info->group_no);
	if (DB->Execute(sql))
	{
		LOGDEBUG("数据库EntryID设置成功，[%d,%d,%d]->entryID[%s]。",info->ied_no,info->cpu_no,info->group_no,entryId.data());
	}
	else
	{
		LOGWARN("数据库EntryID设置失败，[%d,%d,%d]->entryID[%s]。",info->ied_no,info->cpu_no,info->group_no,entryId.data());
	}
}

bool MMSClient::dbSyslog( SString text,int log_type )
{
	SString		sql;
	SRecordset	rs;

	int soc,usec;
	SDateTime::getSystemTime(soc,usec);
	SString log;

	sql.sprintf("insert into t_ssp_syslog (soc,usec,usr_sn,log_type,log_level,id1,id2,id3,id4,log_text) values "
		"(%d,%d,%d,%d,%d,%d,%d,%d,%d,'%s')",
		soc,usec,
		0,
		log_type,
		0,
		m_subNo,0,0,0,
		log.data());
	if (!DB->Execute(sql))
	{
		LOGFAULT("数据库语句操作失败，SQL：%s",sql.data());
		return false;
	}

	return true;
}

bool MMSClient::GetCurrentArea(int iedNo,int &area,int timeOut)
{
	SString		sql;
	SRecordset	rs;
	bool		ret = false;
	SString		es;
	int			m_iedNo = iedNo;

	if (DB->GetDbType() ==  DB_MYSQL)
		sql.sprintf("select mms_path from t_oe_element_general where ied_no=%d and mms_path like '%%$SG$%%' group by cpu_no",m_iedNo);
	else if (DB->GetDbType() ==  DB_ORACLE)
		sql.sprintf("select min(mms_path) from t_oe_element_general where ied_no=%d and mms_path like '%%$SG$%%' group by cpu_no",m_iedNo);
	int cnt = DB->Retrieve(sql,rs);
	if (cnt < 0)
	{
		LOGFAULT("数据库语句操作失败，SQL：%s",sql.data());
		return false;
	}
	else if (cnt == 0)
	{
		if (DB->GetDbType() ==  DB_MYSQL)
			sql.sprintf("select mms_path from t_oe_element_general where ied_no=%d and mms_path like '%%$SP$%%' group by cpu_no",m_iedNo);
		else if (DB->GetDbType() ==  DB_ORACLE)
			sql.sprintf("select min(mms_path) from t_oe_element_general where ied_no=%d and mms_path like '%%$SP$%%' group by cpu_no",m_iedNo);
		cnt = DB->Retrieve(sql,rs);
		if (cnt < 0)
		{
			LOGFAULT("数据库语句操作失败，SQL：%s",sql.data());
			return false;
		}
		else if (cnt == 0)
		{
			LOGWARN("读取当前定值区，未发现装置[%d]有定值项。",m_iedNo);
			return false;
		}
	}
	if (cnt > 0)
	{
		for (int i = 0; i < cnt; i++)
		{
			SString mms = rs.GetValue(i,0).data();
			mms = SString::GetIdAttribute(1,mms,"/");
			if (ReadCurrentSettingArea(mms.data(),area,timeOut))
			{
				LOGDEBUG("读取装置[%d:%s]当前定值区号[%d]成功。",m_iedNo,mms.data(),area);
				ret = true;
				break;
			}
			else
			{
				es = es + mms + ",";//将读取不到定值的LD一同打出
				ret = false;
			}
		}
	}

	if (!ret)
		LOGWARN("读取装置[%d:%s]当前定值区号失败。",m_iedNo,es.data());

	return ret;
}

bool MMSClient::GetEditArea(int iedNo,int &area,int timeOut)
{
	SString		sql;
	SRecordset	rs;
	bool		ret = false;
	SString		es;
	int			m_iedNo = iedNo;

	if (DB->GetDbType() ==  DB_MYSQL)
		sql.sprintf("select mms_path from t_oe_element_general where ied_no=%d and mms_path like '%%$SG$%%' group by cpu_no",m_iedNo);
	else if (DB->GetDbType() ==  DB_ORACLE)
		sql.sprintf("select min(mms_path) from t_oe_element_general where ied_no=%d and mms_path like '%%$SG$%%' group by cpu_no",m_iedNo);
	int cnt = DB->Retrieve(sql,rs);
	if (cnt < 0)
	{
		LOGFAULT("数据库语句操作失败，SQL：%s",sql.data());
		return false;
	}
	else if (cnt == 0)
	{
		if (DB->GetDbType() ==  DB_MYSQL)
			sql.sprintf("select mms_path from t_oe_element_general where ied_no=%d and mms_path like '%%$SP$%%' group by cpu_no",m_iedNo);
		else if (DB->GetDbType() ==  DB_ORACLE)
			sql.sprintf("select min(mms_path) from t_oe_element_general where ied_no=%d and mms_path like '%%$SP$%%' group by cpu_no",m_iedNo);
		cnt = DB->Retrieve(sql,rs);
		if (cnt < 0)
		{
			LOGFAULT("数据库语句操作失败，SQL：%s",sql.data());
			return false;
		}
		else if (cnt == 0)
		{
			LOGWARN("读取当前定值区，未发现装置[%d]有定值项。",m_iedNo);
			return false;
		}
	}
	if (cnt > 0)
	{
		for (int i = 0; i < cnt; i++)
		{
			SString mms = rs.GetValue(i,0).data();
			mms = SString::GetIdAttribute(1,mms,"/");
			if (ReadEditSettingArea(mms.data(),area,timeOut))
			{
				LOGDEBUG("读取装置[%d:%s]编辑定值区号[%d]成功。",m_iedNo,mms.data(),area);
				ret = true;
				break;
			}
			else
			{
				es = es + mms + ",";
				ret = false;
			}
		}
	}

	if (!ret)
		LOGWARN("读取装置[%d:%s]编辑定值区号失败。",m_iedNo,es.data());

	return ret;
}

bool MMSClient::SetCurrentArea(int iedNo,int area,int timeOut)
{
	SString		sql;
	SRecordset	rs;
	bool		ret = false;
	int			m_iedNo = iedNo;

	int m_area;
	ret = GetCurrentArea(m_iedNo,m_area,timeOut);
	if (!ret)
	{
		LOGWARN("切换当前定值区[%d]时，获取装置[%d]当前定值区失败。",area,m_iedNo);
		return false;
	}

	if (m_area == area)
	{
		LOGDEBUG("获取装置[%d]当前定值区[%d]与切换当前定值区[%d]一致，不切换。",m_area,area);
		return true;
	}

	if (DB->GetDbType() ==  DB_MYSQL)
		sql.sprintf("select mms_path from t_oe_element_general where ied_no=%d and mms_path like '%%$SG$%%' group by cpu_no",m_iedNo);
	else if (DB->GetDbType() ==  DB_ORACLE)
		sql.sprintf("select min(mms_path) from t_oe_element_general where ied_no=%d and mms_path like '%%$SG$%%' group by cpu_no",m_iedNo);
	int cnt = DB->Retrieve(sql,rs);
	if (cnt < 0)
	{
		LOGFAULT("数据库语句操作失败，SQL：%s",sql.data());
		return false;
	}
	else if (cnt == 0)
	{
		if (DB->GetDbType() ==  DB_MYSQL)
			sql.sprintf("select mms_path from t_oe_element_general where ied_no=%d and mms_path like '%%$SP$%%' group by cpu_no",m_iedNo);
		else if (DB->GetDbType() ==  DB_ORACLE)
			sql.sprintf("select min(mms_path) from t_oe_element_general where ied_no=%d and mms_path like '%%$SP$%%' group by cpu_no",m_iedNo);
		cnt = DB->Retrieve(sql,rs);
		if (cnt < 0)
		{
			LOGFAULT("数据库语句操作失败，SQL：%s",sql.data());
			return false;
		}
		else if (cnt == 0)
		{
			LOGWARN("读取当前定值区，未发现装置[%d]有定值项。",m_iedNo);
			return false;
		}
	}
	if (cnt > 0)
	{
		SString mms = rs.GetValue(0,0).data();
		mms = SString::GetIdAttribute(1,mms,"/");
		if (WriteCurrentSettingArea(mms.data(),area,timeOut))
		{
			LOGDEBUG("修改装置[%d:%s]当前定值区[%d]成功。",m_iedNo,mms.data(),area);
			ret = true;
		}
		else
		{
			LOGWARN("修改装置[%d:%s]当前定值区[%d]失败。",m_iedNo,mms.data(),area);
			ret = false;
		}
	}

	return ret;
}

bool MMSClient::SetEditArea(int iedNo,int area,int timeOut)
{
	SString		sql;
	SRecordset	rs;
	bool		ret = false;
	int			m_iedNo = iedNo;

	//int m_area;
	//ret = GetEditArea(m_iedNo,m_area,timeOut);
	//if (!ret)
	//{
	//	LOGWARN("切换编辑定值区[%d]时，获取装置[%d]编辑定值区失败。",area,m_iedNo);
	//	return false;
	//}

	//if (m_area == area)
	//{
	//	LOGDEBUG("获取装置[%d]，编辑定值区[%d]与切换编辑定值区[%d]一致，不切换。",iedNo,m_area,area);
	//	return true;
	//}

	if (DB->GetDbType() ==  DB_MYSQL)
		sql.sprintf("select mms_path from t_oe_element_general where ied_no=%d and mms_path like '%%$SG$%%' group by cpu_no",m_iedNo);
	else if (DB->GetDbType() ==  DB_ORACLE)
		sql.sprintf("select min(mms_path) from t_oe_element_general where ied_no=%d and mms_path like '%%$SG$%%' group by cpu_no",m_iedNo);
	int cnt = DB->Retrieve(sql,rs);
	if (cnt < 0)
	{
		LOGFAULT("数据库语句操作失败，SQL：%s",sql.data());
		return false;
	}
	else if (cnt == 0)
	{
		if (DB->GetDbType() ==  DB_MYSQL)
			sql.sprintf("select mms_path from t_oe_element_general where ied_no=%d and mms_path like '%%$SP$%%' group by cpu_no",m_iedNo);
		else if (DB->GetDbType() ==  DB_ORACLE)
			sql.sprintf("select min(mms_path) from t_oe_element_general where ied_no=%d and mms_path like '%%$SP$%%' group by cpu_no",m_iedNo);
		cnt = DB->Retrieve(sql,rs);
		if (cnt < 0)
		{
			LOGFAULT("数据库语句操作失败，SQL：%s",sql.data());
			return false;
		}
		else if (cnt == 0)
		{
			LOGWARN("读取当前定值区，未发现装置[%d]有定值项。",m_iedNo);
			return false;
		}
	}
	else if (cnt > 0)
	{
		SString mms = rs.GetValue(0,0).data();
		mms = SString::GetIdAttribute(1,mms,"/");
		if (WriteEditSettingArea(mms.data(),area,timeOut))
		{
			LOGDEBUG("修改装置[%d:%s]编辑定值区[%d]成功。",m_iedNo,mms.data(),area);
			ret = true;
		}
		else
		{
			LOGWARN("修改装置[%d:%s]编辑定值区[%d]失败。",m_iedNo,mms.data(),area);
			ret = false;
		}
	}

	return ret;
}

bool MMSClient::ReadCurrentAreaSetting(int ied_no,int cpu_no,int group_no,int timeOut)
{
	bool ret = false;
	unsigned long pos = 0;
	unsigned long ds_pos;
	unsigned long fc_pos;
	SPtrList<SString> dom_name;
	SPtrList<SString> var_name;
	SPtrList<MMSValue> *value;
	SPtrList< SPtrList<MMSValue> > lstValue;
	SPtrList<t_reference> lstFc;
	dom_name.setAutoDelete(true);
	var_name.setAutoDelete(true);
	lstValue.setAutoDelete(true);

	SString		sql;
	SRecordset	rs;
	int			cnt;
	SString		iedMmsPath;

	sql.sprintf("select mms_path from t_oe_ied where ied_no=%d",ied_no);
	cnt = DB->Retrieve(sql,rs);
	if (cnt < 0)
	{
		LOGFAULT("数据库语句操作失败，SQL：%s",sql.data());
		return false;
	}
	else if (cnt == 0)
	{
		LOGWARN("读取当前定值区定值，未发现装置[%d]。",ied_no);
		return false;
	}
	else if (cnt > 0)
	{
		iedMmsPath = rs.GetValue(0,0);
	}

	sql.sprintf("select name,type,mms_path from t_oe_group where ied_no=%d and cpu_no=%d and group_no=%d",ied_no,cpu_no,group_no);
	cnt = DB->Retrieve(sql,rs);
	if (cnt < 0)
	{
		LOGFAULT("数据库语句操作失败，SQL：%s",sql.data());
		return false;
	}
	else if (cnt == 0)
	{
		LOGWARN("读取当前定值区定值，未发现装置有定值组[%d,%d,%d]。",ied_no,cpu_no,group_no);
		return false;
	}
	else if (cnt > 0)
	{
		int type = rs.GetValue(0,1).toInt();
		if (type != DATASET_SETTING)
		{
			LOGWARN("读取当前定值区定值，装置组[%d,%d,%d]不是定值组。",ied_no,cpu_no,group_no);
			return false;
		}

		SString mmsPath = rs.GetValue(0,2);
		CSMmsDataset *ds = m_Datasets.FetchFirst(ds_pos);
		while(ds)
		{
			if (ds->m_iDsType == DATASET_SETTING && ds->m_sDsPath == mmsPath.data() && ds->m_sIedName == iedMmsPath.data())
			{
				pos = 0;
				stuSMmsDataNode *fc = ds->m_Items.FetchFirst(fc_pos);
				while(fc)
				{
					SString *dn = new SString(SString::GetIdAttribute(1,fc->sMmsPath,"/"));
					dom_name.append(dn);
					SString *vn = new SString(SString::GetIdAttribute(2,fc->sMmsPath,"/"));
					var_name.append(vn);
					value = new SPtrList<MMSValue>;
					value->setAutoDelete(true);
					lstValue.append(value);
					lstFc.append((t_reference*)fc->user);
					if (++pos >= MAX_READ_VALUES_NUMBER)
					{
						LOGDEBUG("读多值[%d]准备，子站号[%d]装置MMS名称[%s]。",pos,m_subNo,ds->m_sIedName.data());
						ret = ReadValues(dom_name,var_name,lstValue,timeOut);
						if (ret)
						{
							LOGDEBUG("读多值[%d]完毕，子站号[%d]装置MMS名称[%s]。",pos,m_subNo,ds->m_sIedName.data());
							dbSettingValue(lstFc,lstValue);
							LOGDEBUG("多值[%d]入库完毕，子站号[%d]装置MMS名称[%s]。",pos,m_subNo,ds->m_sIedName.data());
						}
						dom_name.clear();
						var_name.clear();
						lstValue.clear();
						lstFc.clear();
						pos = 0;

						if (!ret)
							break;	//while(fc)
					}

					fc = ds->m_Items.FetchNext(fc_pos);
				}
				if (pos > 0)
				{
					LOGDEBUG("读多值[%d]准备，子站号[%d]装置MMS名称[%s]。",pos,m_subNo,ds->m_sIedName.data());
					ret = ReadValues(dom_name,var_name,lstValue,timeOut);
					if (ret)
					{
						LOGDEBUG("读多值[%d]完毕，子站号[%d]装置MMS名称[%s]。",pos,m_subNo,ds->m_sIedName.data());
						dbSettingValue(lstFc,lstValue);
						LOGDEBUG("多值[%d]入库完毕，子站号[%d]装置MMS名称[%s]。",pos,m_subNo,ds->m_sIedName.data());
					}
					dom_name.clear();
					var_name.clear();
					lstValue.clear();
					lstFc.clear();
					pos = 0;
				}

				break;	//while(ds)
			}

			ds = m_Datasets.FetchNext(ds_pos);
		}
	}

	return ret;
}

bool MMSClient::ReadEditAreaSetting(int ied_no,int cpu_no,int group_no,int timeOut)
{
	bool ret = false;
	unsigned long pos = 0;
	unsigned long ds_pos;
	unsigned long fc_pos;
	SPtrList<SString> dom_name;
	SPtrList<SString> var_name;
	SPtrList<MMSValue> *value;
	SPtrList< SPtrList<MMSValue> > lstValue;
	SPtrList<t_reference> lstFc;
	dom_name.setAutoDelete(true);
	var_name.setAutoDelete(true);
	lstValue.setAutoDelete(true);

	SString		sql;
	SRecordset	rs;
	int			cnt;
	SString		iedMmsPath;

	sql.sprintf("select mms_path from t_oe_ied where ied_no=%d",ied_no);
	cnt = DB->Retrieve(sql,rs);
	if (cnt < 0)
	{
		LOGFAULT("数据库语句操作失败，SQL：%s",sql.data());
		return false;
	}
	else if (cnt == 0)
	{
		LOGWARN("读取当前定值区定值，未发现装置[%d]。",ied_no);
		return false;
	}
	else if (cnt > 0)
	{
		iedMmsPath = rs.GetValue(0,0);
	}

	sql.sprintf("select name,type,mms_path from t_oe_group where ied_no=%d and cpu_no=%d and group_no=%d",ied_no,cpu_no,group_no);
	cnt = DB->Retrieve(sql,rs);
	if (cnt < 0)
	{
		LOGFAULT("数据库语句操作失败，SQL：%s",sql.data());
		return false;
	}
	else if (cnt == 0)
	{
		LOGWARN("读取编辑定值区定值，未发现装置有定值组[%d,%d,%d]。",ied_no,cpu_no,group_no);
		return false;
	}
	else if (cnt > 0)
	{
		int type = rs.GetValue(0,1).toInt();
		if (type != DATASET_SETTING)
		{
			LOGWARN("读取编辑定值区定值，装置组[%d,%d,%d]不是定值组。",ied_no,cpu_no,group_no);
			return false;
		}

		SString mmsPath = rs.GetValue(0,2);
		CSMmsDataset *ds = m_Datasets.FetchFirst(ds_pos);
		while(ds)
		{
			if (ds->m_iDsType == DATASET_SETTING && ds->m_sDsPath == mmsPath.data() && ds->m_sIedName == iedMmsPath.data())
			{
				pos = 0;
				stuSMmsDataNode *fc = ds->m_Items.FetchFirst(fc_pos);
				while(fc)
				{
					SString *dn = new SString(SString::GetIdAttribute(1,fc->sMmsPath,"/"));
					dom_name.append(dn);
					SString *vn = new SString(SString::GetIdAttribute(2,fc->sMmsPath,"/"));
					vn->replace("$SG$","$SE$");
					var_name.append(vn);
					value = new SPtrList<MMSValue>;
					value->setAutoDelete(true);
					lstValue.append(value);
					lstFc.append((t_reference*)fc->user);
					if (++pos >= MAX_READ_VALUES_NUMBER)
					{
						LOGDEBUG("读多值[%d]准备，子站号[%d]装置MMS名称[%s]。",pos,m_subNo,ds->m_sIedName.data());
						ret = ReadValues(dom_name,var_name,lstValue,timeOut);
						if (ret)
						{
							LOGDEBUG("读多值[%d]完毕，子站号[%d]装置MMS名称[%s]。",pos,m_subNo,ds->m_sIedName.data());
							dbSettingValue(lstFc,lstValue,true);
							LOGDEBUG("多值[%d]入库完毕，子站号[%d]装置MMS名称[%s]。",pos,m_subNo,ds->m_sIedName.data());
						}
						dom_name.clear();
						var_name.clear();
						lstValue.clear();
						lstFc.clear();
						pos = 0;

						if (!ret)
							break;	//while(fc)
					}

					fc = ds->m_Items.FetchNext(fc_pos);
				}
				if (pos > 0)
				{
					LOGDEBUG("读多值[%d]准备，子站号[%d]装置MMS名称[%s]。",pos,m_subNo,ds->m_sIedName.data());
					ret = ReadValues(dom_name,var_name,lstValue);
					if (ret)
					{
						LOGDEBUG("读多值[%d]完毕，子站号[%d]装置MMS名称[%s]。",pos,m_subNo,ds->m_sIedName.data());
						dbSettingValue(lstFc,lstValue,true);
						LOGDEBUG("多值[%d]入库完毕，子站号[%d]装置MMS名称[%s]。",pos,m_subNo,ds->m_sIedName.data());
					}
					dom_name.clear();
					var_name.clear();
					lstValue.clear();
					lstFc.clear();
					pos = 0;
				}

				break;	//while(ds)
			}

			ds = m_Datasets.FetchNext(ds_pos);
		}
	}

	return ret;
}

bool MMSClient::ReadAppointAreaSetting(int ied_no,int cpu_no,int group_no,int area,int timeOut)
{
	bool ret = false;
	unsigned long pos = 0;
	unsigned long ds_pos;
	unsigned long fc_pos;
	SPtrList<SString> dom_name;
	SPtrList<SString> var_name;
	SPtrList<MMSValue> *value;
	SPtrList< SPtrList<MMSValue> > lstValue;
	SPtrList<t_reference> lstFc;
	dom_name.setAutoDelete(true);
	var_name.setAutoDelete(true);
	lstValue.setAutoDelete(true);

	int m_area = 0;
	ret = GetEditArea(ied_no,m_area,timeOut);
	if (!ret)
	{
		LOGWARN("读取指定定值区[%d]定值，获取装置[%d]编辑定值区时失败。",area,ied_no);
		return false;
	}
	if (m_area != area)//获取的编辑定值区和指定定值区不一致时，将指定定值区切换至编辑定值区
	{
		ret = SetEditArea(ied_no,area,timeOut);
		if (!ret)
		{
			LOGWARN("读取指定定值区[%d]定值，切换装置[%d]定值区时失败。",area,ied_no);
			return false;
		}
	}

	SString		sql;
	SRecordset	rs;
	int			cnt;
	SString		iedMmsPath;

	sql.sprintf("select mms_path from t_oe_ied where ied_no=%d",ied_no);
	cnt = DB->Retrieve(sql,rs);
	if (cnt < 0)
	{
		LOGFAULT("数据库语句操作失败，SQL：%s",sql.data());
		return false;
	}
	else if (cnt == 0)
	{
		LOGWARN("读取当前定值区定值，未发现装置[%d]。",ied_no);
		return false;
	}
	else if (cnt > 0)
	{
		iedMmsPath = rs.GetValue(0,0);
	}

	sql.sprintf("select name,type,mms_path from t_oe_group where ied_no=%d and cpu_no=%d and group_no=%d",ied_no,cpu_no,group_no);
	cnt = DB->Retrieve(sql,rs);
	if (cnt < 0)
	{
		LOGFAULT("数据库语句操作失败，SQL：%s",sql.data());
		return false;
	}
	else if (cnt == 0)
	{
		LOGWARN("读取编辑定值区定值，未发现装置有定值组[%d,%d,%d]。",ied_no,cpu_no,group_no);
		return false;
	}
	else if (cnt > 0)
	{
		int type = rs.GetValue(0,1).toInt();
		if (type != DATASET_SETTING)
		{
			LOGWARN("读取编辑定值区定值，装置组[%d,%d,%d]不是定值组。",ied_no,cpu_no,group_no);
			return false;
		}

		SString mmsPath = rs.GetValue(0,2);
		CSMmsDataset *ds = m_Datasets.FetchFirst(ds_pos);
		while(ds)
		{
			if (ds->m_iDsType == DATASET_SETTING && ds->m_sDsPath == mmsPath.data() && ds->m_sIedName == iedMmsPath.data())
			{
				pos = 0;
				stuSMmsDataNode *fc = ds->m_Items.FetchFirst(fc_pos);
				while(fc)
				{
					SString *dn = new SString(SString::GetIdAttribute(1,fc->sMmsPath,"/"));
					dom_name.append(dn);
					SString *vn = new SString(SString::GetIdAttribute(2,fc->sMmsPath,"/"));
					vn->replace("$SG$","$SE$");
					var_name.append(vn);
					value = new SPtrList<MMSValue>;
					value->setAutoDelete(true);
					lstValue.append(value);
					lstFc.append((t_reference*)fc->user);
					if (++pos >= MAX_READ_VALUES_NUMBER)
					{
						LOGDEBUG("读多值[%d]准备，子站号[%d]装置MMS名称[%s]。",pos,m_subNo,ds->m_sIedName.data());
						ret = ReadValues(dom_name,var_name,lstValue,timeOut);
						if (ret)
						{
							LOGDEBUG("读多值[%d]完毕，子站号[%d]装置MMS名称[%s]。",pos,m_subNo,ds->m_sIedName.data());
							dbSettingValue(lstFc,lstValue,true);
							LOGDEBUG("多值[%d]入库完毕，子站号[%d]装置MMS名称[%s]。",pos,m_subNo,ds->m_sIedName.data());
						}
						dom_name.clear();
						var_name.clear();
						lstValue.clear();
						lstFc.clear();
						pos = 0;

						if (!ret)
							break;	//while(fc)
					}

					fc = ds->m_Items.FetchNext(fc_pos);
				}
				if (pos > 0)
				{
					LOGDEBUG("读多值[%d]准备，子站号[%d]装置MMS名称[%s]。",pos,m_subNo,ds->m_sIedName.data());
					ret = ReadValues(dom_name,var_name,lstValue);
					if (ret)
					{
						LOGDEBUG("读多值[%d]完毕，子站号[%d]装置MMS名称[%s]。",pos,m_subNo,ds->m_sIedName.data());
						dbSettingValue(lstFc,lstValue,true);
						LOGDEBUG("多值[%d]入库完毕，子站号[%d]装置MMS名称[%s]。",pos,m_subNo,ds->m_sIedName.data());
					}
					dom_name.clear();
					var_name.clear();
					lstValue.clear();
					lstFc.clear();
					pos = 0;
				}

				break;	//while(ds)
			}

			ds = m_Datasets.FetchNext(ds_pos);
		}
	}

	return ret;
}

bool MMSClient::WriteSettingValue(int ied_no,int cpu_no,int group_no,int entry,int area,SString value,int timeOut)
{
	bool ret = false;

	//int m_area = 0;
	//ret = GetEditArea(ied_no,m_area,timeOut);
	//if (!ret)
	//{
	//	LOGWARN("读取指定定值区[%d]定值，获取装置[%d]编辑定值区时失败。",area,ied_no);
	//	return false;
	//}
	//if (m_area != area)//获取的编辑定值区和指定定值区不一致时，将指定定值区切换至编辑定值区
	//{
	//	ret = SetEditArea(ied_no,area,timeOut);
	//	if (!ret)
	//	{
	//		LOGWARN("读取指定定值区[%d]定值，切换装置[%d]定值区时失败。",area,ied_no);
	//		return false;
	//	}
	//}

	SString		sql;
	SRecordset	rs;
	sql.sprintf("select name,valtype,mms_path from t_oe_element_general where ied_no=%d and cpu_no=%d and group_no=%d and entry=%d",
		ied_no,cpu_no,group_no,entry);
	int cnt = DB->Retrieve(sql,rs);
	if (cnt < 0)
	{
		LOGFAULT("数据库语句操作失败，SQL：%s",sql.data());
		return false;
	}
	else if (cnt == 0)
	{
		LOGWARN("修改编辑定值区定值，未发现装置定值项[%d,%d,%d,%d]。",ied_no,cpu_no,group_no,entry);
		return false;
	}
	else if (cnt > 0)
	{
		int   iVal = 0;
		float fVal = 0.0;
		char data[256] = {'\0'};
		int m_valtype = rs.GetValue(0,1).toInt();
		switch (m_valtype)
		{
		case 1://字符串
			memcpy(data,value.data(),value.size());
			break;
		case 3://整型
			iVal = value.toInt();
			memcpy(data,&iVal,sizeof(int));
			break;
		case 7://浮点型
			fVal = value.toFloat();
			memcpy(data,&fVal,sizeof(float));
			break;
		default:
			LOGWARN("修改编辑定值区定值，装置定值项[%d,%d,%d,%d]无法识别类型[%d]。",ied_no,cpu_no,group_no,entry,m_valtype);
			return false;
		}

		SString mms = rs.GetValue(0,2).data();
		SString dom = SString::GetIdAttribute(1,mms,"/");
		SString var = SString::GetIdAttribute(2,mms,"/");
		var.replace("$SG$","$SE$");
		ret = WriteValue(dom.data(),var.data(),data);
		if (ret)
		{
			LOGDEBUG("修改编辑定值区[%d]装置定值项[%d,%d,%d,%d:%s]值[%s]成功。",
				area,ied_no,cpu_no,group_no,entry,rs.GetValue(0,0).data(),value.data());
		}
		else
		{
			LOGWARN("修改编辑定值区[%d]装置定值项[%d,%d,%d,%d:%s]值[%s]失败。",
				area,ied_no,cpu_no,group_no,entry,rs.GetValue(0,0).data(),value.data());
		}
	}

	return ret;
}

bool MMSClient::CureSettingValue(int ied_no,int cpu_no,int group_no,int timeOut)
{
	bool		ret = false;
	SString		sql;
	SRecordset	rs;

	sql.sprintf("select t_oe_ied.mms_path,t_oe_cpu.mms_path from t_oe_ied,t_oe_cpu "
		"where t_oe_ied.ied_no=%d and t_oe_cpu.ied_no=%d and t_oe_cpu.cpu_no=%d",ied_no,ied_no,cpu_no);
	int cnt = DB->Retrieve(sql,rs);
	if (cnt < 0)
	{
		LOGFAULT("数据库语句操作失败，SQL：%s",sql.data());
		return false;
	}
	else if (cnt == 0)
	{
		LOGWARN("固化定值，未发现装置定值CPU[%d,%d]。",ied_no,cpu_no);
		return false;
	}
	else if (cnt > 0)
	{
		SString dom = rs.GetValue(0,0).data();
		dom += rs.GetValue(0,1).data();
		ret = CureSetting(dom.data());
		if (ret)
		{
			LOGDEBUG("固化定值，装置定值组[%d,%d,%d:%s]成功。",ied_no,cpu_no,group_no,dom.data());
		}
		else
		{
			LOGWARN("固化定值，装置定值组[%d,%d,%d:%s]失败。",ied_no,cpu_no,group_no,dom.data());
		}
	}

	return ret;
}

bool MMSClient::ReadLeafValue(SString dom, SString var, int &valType, SString &value, int timeOut)
{
	SPtrList<MMSValue> lstValue;
	if (!ReadValue(dom.data(), var.data(), lstValue, timeOut))
	{
		LOGERROR("ReadValue failed, dom(%s), var(%s)\n", dom.data(), var.data());
		return false;
	}

	if (lstValue.count() == 0)
		return false;

	valType = lstValue.at(0)->getType();
	value = lstValue.at(0)->getStrValue();

	LOGDEBUG("read %s/%s success, valtype=%d, value=%s\n", dom.data(), var.data(), valType, value.data());
	return true;
}

Ied::Ied(MMSClient *mmsClient, int iedNo, SString name, SString iedName)
	: m_mmsClient(mmsClient)
	, m_iedNo(iedNo)
	, m_name(name)
	, m_iedName(iedName)
{
	m_lastParamCheckSoc = 0;
	m_paramCheckInteval = 60;
	m_paramChangeAlarmRef = NULL;
}

Ied::~Ied()
{
	if (m_paramChangeAlarmRef)
	{
		delete m_paramChangeAlarmRef;
		m_paramChangeAlarmRef = NULL;
	}
}

bool Ied::Init()
{
	SString				sql;
	SRecordset			rs;

	sql = SString::toFormat("select mms_path from t_oe_ied where ied_no=%d", this->m_iedNo);
	int iRet = DB->Retrieve(sql,rs);
	if (iRet < 0)
	{
		LOGFAULT("数据库语句操作失败，SQL：%s",sql.data());
		return false;
	}
	else if (iRet == 0)
	{
		LOGWARN("数据库t_oe_ied表中未找到装置号 = %d 的数据。", m_iedNo);
		return false;
	}
	else if (iRet > 0)
	{
		this->m_iedName = rs.GetValue(0,0);
	}

	// RCD对应的CPU序号
	sql = SString::toFormat("select cpu_no from t_oe_cpu where ied_no=%d and mms_path like 'RCD'", m_iedNo);
	DB->Retrieve(sql, rs);
	if (rs.GetRows() > 0)
		m_iedParam.m_RCDCpuNo = rs.GetValueInt(0, 0);

	if (!InitIedParam())
		return false;

	if (!InitGroup())
		return false;

	return true;
}

t_reference * Ied::findReference(std::string reference)
{
	std::map<std::string, t_reference *>::iterator iter = m_mapReference.find(reference);
	if (iter != m_mapReference.end())
		return iter->second;
	else
		return NULL;
}

stuReportInfo * Ied::findReportInfo(stuSMmsReportInfo *rptInfo)
{
	for (std::vector<stuReportInfo *>::iterator iter = m_reportInfo.begin(); iter != m_reportInfo.end(); ++iter)
	{
		if ((*iter)->sMmsReportInfo == rptInfo)
			return (*iter);
	}

	return NULL;
}

bool Ied::InitIedParam()
{
	SString		ied_ip_a = "";
	SString		ied_ip_b = "";
	SString		sql;
	SRecordset	rs;

	sql = SString::toFormat("select param_name, current_val from t_oe_ied_param "
		"where ied_no=%d", this->m_iedNo);
	int iRet = DB->Retrieve(sql,rs);
	if (iRet < 0)
	{
		LOGFAULT("数据库语句操作失败，SQL：%s",sql.data());
		return false;
	}
	else if (iRet == 0)
	{
		LOGWARN("数据库t_oe_ied_param表中未找到节点号=%d，程序名=%s的数据。", this->m_iedNo, MODULE_NAME);
		return false;
	}
	else if (iRet > 0)
	{
		for (int i = 0; i < rs.GetRows(); i++)
		{
			SString param = rs.GetValue(i, 0);
			SString value = rs.GetValue(i, 1);
			if (param == "net_a_ip")
				this->m_ipA = rs.GetValue(i, 1);
			else if (param == "net_b_ip")
				this->m_ipB = rs.GetValue(i, 1);
			else if (param == "req_wave_time_period")
				m_iedParam.m_isSupportReqWaveByTimePeriod = rs.GetValue(i,1).toInt() == 1 ? true : false;
			else if (param == "comtrade_path")
				m_iedParam.m_comtradePath = rs.GetValue(i,1);

			m_iedParam.setAttr(param.data(), value.data());
		}

		if (this->m_mmsClient->clientMode() == MMSClient::SubStationMode)
		{
			if (this->m_ipA.isEmpty() && this->m_ipB.isEmpty())	//A、B网地址均无，不创建此装置
			{
				LOGWARN("数据库t_oe_ied_param表中未找到节点号=%d，程序名=%s的A或B网地址配置。", this->m_iedNo, MODULE_NAME);
				return false;
			}	
		}
	}

	return true;
}

bool Ied::InitGroup()
{
	SRecordset	rs;

// 	SString sql = SString::toFormat("select cpu_no,reportcontrol_ln,reportcontrol_buffered,reportcontrol_name,"
// 		"trgOps_qchg,trgOps_period,trgOps_dupd,trgOps_dchg,name,mms_path,type,group_no,trgOps_gi,"
// 		"reportcontrol_intgPd,reportcontrol_rptID,entryID from t_oe_group "
// 		"where ied_no=%d and logcontrol_logEna='false' and (logcontrol_logName='' or logcontrol_logName is null)", this->m_iedNo);
	SString sql = SString::toFormat("select cpu_no,reportcontrol_ln,reportcontrol_buffered,reportcontrol_name,"
		"trgOps_qchg,trgOps_period,trgOps_dupd,trgOps_dchg,name,mms_path,type,group_no,trgOps_gi,"
		"reportcontrol_intgPd,reportcontrol_rptID,entryID from t_oe_group "
		"where ied_no=%d", this->m_iedNo);
	int iRet = DB->Retrieve(sql,rs);
	if (iRet < 0)
	{
		LOGFAULT("数据库语句操作失败，SQL：%s",sql.data());
		return false;
	}
	else if (iRet == 0)
	{
		LOGWARN("数据库t_oe_group表中未找到装置号 = %d 的数据。", this->m_iedNo);
		return false;
	}
	else if (iRet > 0)
	{
		for (int i = 0; i < rs.GetRows(); i++)
		{
			int cpu_no = rs.GetValue(i,0).toInt();
			int group_no = rs.GetValue(i,11).toInt();

			SString sql = SString::toFormat("select (select count(1) from t_oe_element_state where ied_no=%d and cpu_no=%d and group_no=%d) + "
				"(select count(1) from t_oe_element_general where ied_no=%d and cpu_no=%d and group_no=%d) + "
				"(select count(1) from t_oe_element_control where  ied_no=%d and cpu_no=%d and group_no=%d) from dual",
				m_iedNo, cpu_no, group_no,
				m_iedNo, cpu_no, group_no,
				m_iedNo, cpu_no, group_no);
			int itemCount = DB->SelectIntoI(sql);
			if (itemCount == 0)
				continue;

			AddReportInfo(rs, i, this->m_iedNo, cpu_no,group_no, this->m_iedName);

			SString ds_desc = rs.GetValue(i,8);
			SString mms_path = rs.GetValue(i,9);
			int ds_type = rs.GetValue(i,10).toInt();
			CSMmsDataset * mmsDataset = m_mmsClient->AddDataset(m_iedName, mms_path, ds_desc, ds_type);
			if (mmsDataset)
			{
				if (!InitElementFromGeneral(m_iedNo,cpu_no,group_no,mmsDataset))
				{
					if (!InitElementFromState(m_iedNo,cpu_no,group_no,mmsDataset))
					{
						if (!InitElementFromControl(m_iedNo,cpu_no,group_no,mmsDataset))
						{
							LOGFAULT("未找到[ied = %d,cpu = %d,group = %d] 中的点信息。", m_iedNo, cpu_no, group_no);
							return false;
						}
					}
				}
			}
			else
			{
				LOGFAULT("内存添加数据集[ied = %d,mms = %s,desc=%s]失败！", m_iedNo, mms_path.data(), ds_desc.data());
				return false;
			}
		}
	}

	return true;
}

bool Ied::AddReportInfo(SRecordset &rs, int i, int iedNo, int cpuNo, int groupNo, SString iedName)
{
	SString report = "";
	BYTE	trgOps = 0;
	SString	ld_name  = "";

	if (!GetLD(cpuNo, ld_name))
		return false;

	SString ln = rs.GetValue(i,1);
	if (ln == "NULL" || ln.isEmpty())
		return false;
	else
		report += ln;

	SString buffered = rs.GetValue(i,2);
	if (buffered == "true")
		report += "$BR$";
	else if (buffered == "false")
		report += "$RP$";
	else
		return false;

	SString name = rs.GetValue(i,3);
	if (name == "NULL" || name.isEmpty())
		return false;
	else
	{
		if (name.left(4).CompareNoCase("urcb") == 0)
		{
			SString attr = this->getIedParam()->getAttrValueString("enabled_urcb");
			if (attr.CompareNoCase("TRUE") == 0 || attr.toInt() == 1)
			{
				// 使能报告控制块
			}
			else
				return false;
		}
		
		report += name;
	}

	SString trg = rs.GetValue(i,4);
	if (trg == "true")
		trgOps |= C_SMMS_TRGOPT_Q;

	trg = rs.GetValue(i,5);
	if (trg == "true" && m_mmsClient->isPeriod())	//配置的基础上，程序也可控制周期上送
		trgOps |= C_SMMS_TRGOPT_ZQ;

	trg = rs.GetValue(i,6);
	if (trg == "true")
		trgOps |= C_SMMS_TRGOPT_REF;

	trg = rs.GetValue(i,7);
	if (trg == "true")
		trgOps |= C_SMMS_TRGOPT_DATA;

	trg = rs.GetValue(i,12);
	if (trg == "true")
		trgOps |= C_SMMS_TRGOPT_ZZ;

	int intgPd = rs.GetValue(i,13).toInt();
	intgPd = intgPd < 60000 ? 60000 : intgPd;

	SString rptID = rs.GetValue(i,14);

	SString entryID = rs.GetValue(i,15);

	stuSMmsReportInfo *sMmsReportInfo = m_mmsClient->AddReportInfo(this->m_iedName + ld_name, rptID, buffered, report, m_mmsClient->reportId(), entryID, intgPd, trgOps);
	if (sMmsReportInfo)
	{
		stuReportInfo *reportInfo = new stuReportInfo;
		reportInfo->ied_no = iedNo;
		reportInfo->cpu_no = cpuNo;
		reportInfo->group_no = groupNo;
		reportInfo->sMmsReportInfo = sMmsReportInfo;
		m_reportInfo.push_back(reportInfo);
		//m_mmsClient->AddCliReportInfo(this->m_iedNo, cpuNo, groupNo, sMmsReportInfo);
	}

	return true;
}

bool Ied::GetLD(int cpuNo, SString &ldName)
{
	SString				sql;
	SRecordset			rs;

	sql = SString::toFormat("select mms_path from t_oe_cpu where ied_no=%d and cpu_no=%d", this->m_iedNo, cpuNo);
	int iRet = DB->Retrieve(sql,rs);
	if (iRet < 0)
	{
		LOGFAULT("数据库语句操作失败，SQL：%s",sql.data());
		return false;
	}
	else if (iRet == 0)
	{
		LOGWARN("数据库t_oe_ied_cpu表中未找到装置号 = %d 的数据。", this->m_iedNo);
		return false;
	}
	else if (iRet > 0)
	    ldName = rs.GetValue(0,0);

	return true;
}

bool Ied::InitElementFromGeneral(int ied_no,int cpu_no,int group_no,CSMmsDataset *mmsDataset)
{
	SRecordset	rs;

	SString sql = SString::toFormat("select entry,name,mms_path,threshold,smooth,current_val,gen_sn from t_oe_element_general "
		"where ied_no=%d and cpu_no=%d and group_no=%d",ied_no,cpu_no,group_no);
	int iRet = DB->Retrieve(sql,rs);
	if (iRet < 0)
	{
		LOGFAULT("数据库语句操作失败，SQL：%s",sql.data());
		return false;
	}
	else if (iRet == 0)
	{
		return false;
	}
	else if (iRet > 0)
	{
		for (int i = 0; i < rs.GetRows(); i++)
		{
			int		entry = rs.GetValue(i,0).toInt();
			SString elem_desc = rs.GetValue(i,1);
			SString mms_path = rs.GetValue(i,2);
			float	threshold = rs.GetValue(i,3).toFloat();
			int		smooth	  = rs.GetValue(i,4).toInt();
			SString current_val = rs.GetValue(i,5).data();
			int		gen_sn      = rs.GetValue(i,6).toInt();
			if (mms_path.isEmpty())
				continue;
			stuSMmsDataNode *element = mmsDataset->AddItem(elem_desc,mms_path);
			t_reference *ref = new t_reference;
			if (ref)
			{
				ref->tableName = "t_oe_element_general";
				ref->gen_sn     = gen_sn;
				ref->ied_no = ied_no;
				ref->cpu_no = cpu_no;
				ref->group_no = group_no;
				ref->entry = entry;
				ref->desc = elem_desc;
				ref->value = current_val.toFloat();
				ref->strValue = current_val;
				ref->threshold = threshold;
				ref->smooth = smooth;
				ref->fun = 0;
				ref->inf = 0;
				ref->type = 0;
				ref->level = 0;
				ref->val_type = 0;
				ref->st_sn = 0;
				ref->dateTime = SDateTime::currentDateTime();
			}
			element->user = ref;
			//LOGDEBUG("General Ref:%s",mms_path.data());
			m_mapReference.insert(pair<string,t_reference*>(mms_path.data(),ref));
		}
	}

	return true;
}

bool Ied::InitElementFromState(int ied_no,int cpu_no,int group_no,CSMmsDataset *mmsDataset)
{
	SRecordset	rs;

	SString sql = SString::toFormat("select entry,name,mms_path,current_val,fun,inf,type,severity_level,val_type,st_sn from t_oe_element_state "
		"where ied_no=%d and cpu_no=%d and group_no=%d",ied_no,cpu_no,group_no);
	int iRet = DB->Retrieve(sql,rs);
	SRecord *pR;
	if (iRet < 0)
	{
		LOGFAULT("数据库语句操作失败，SQL：%s",sql.data());
		return false;
	}
	else if (iRet == 0)
	{
		return false;
	}
	else if (iRet > 0)
	{
		for (int i = 0; i < rs.GetRows(); i++)
		{
			pR = rs.GetRecord(i);
			int		entry = pR->GetValueInt(0);
			SString elem_desc = pR->GetValue(1);
			SString mms_path = pR->GetValue(2);
			SString current_val = rs.GetValue(i,3).data();
			if (mms_path.isEmpty())
				continue;
			stuSMmsDataNode *element = mmsDataset->AddItem(elem_desc,mms_path);
			t_reference *ref = new t_reference;
			if (ref)
			{
				ref->tableName = "t_oe_element_state";
				ref->gen_sn = 0;
				ref->ied_no = ied_no;
				ref->cpu_no = cpu_no;
				ref->group_no = group_no;
				ref->entry = entry;
				ref->desc = elem_desc;
				ref->value = current_val.toFloat();
				ref->strValue = current_val;
				ref->threshold = 0.0;
				ref->smooth = 0;
				ref->dateTime = SDateTime::currentDateTime();
				ref->fun = pR->GetValueInt(4);
				ref->inf = pR->GetValueInt(5);
				ref->type = pR->GetValueInt(6);
				ref->level = pR->GetValueInt(7);
				ref->val_type = pR->GetValueInt(8);
				ref->st_sn = pR->GetValueInt(9);
			}
			element->user = ref;
			//LOGDEBUG("State Ref:%s",mms_path.data());
			m_mapReference.insert(pair<string,t_reference*>(mms_path.data(),ref));
		}
	}

	return true;
}

bool Ied::InitElementFromControl(int ied_no,int cpu_no,int group_no,CSMmsDataset *mmsDataset)
{
	SRecordset	rs;

	SString sql = SString::toFormat("select count(*) from t_oe_element_control "
		"where ied_no=%d and cpu_no=%d and group_no=%d",ied_no,cpu_no,group_no);
	int iRet = DB->Retrieve(sql,rs);
	if (iRet < 0)
	{
		LOGFAULT("数据库语句操作失败，SQL：%s",sql.data());
		return false;
	}
	else if (iRet == 0)
	{
		return false;
	}
	else if (iRet > 0)
	{
		int count = rs.GetValue(0,0).toInt();
		if (count == 0)
			return false;
	}

	return true;
}

bool Ied::InitElementAnalogMonitor(SPtrList<CSMmsDataset> *m_Datasets)
{
	SString sql = SString::toFormat(" \
									select t_oe_element_general_monitor.gen_sn,t_oe_element_general_monitor.ied_no,	\
									t_oe_element_general_monitor.cpu_no,t_oe_element_general_monitor.group_no,	\
									t_oe_element_general_monitor.entry,t_oe_element_general_monitor.enabled,	\
									t_oe_element_general_monitor.inteval,t_oe_element_general_monitor.maxval,	\
									t_oe_element_general_monitor.minval,t_oe_element_general_monitor.mutate,	\
									t_oe_element_general_monitor.last_soc,	t_oe_element_general.mms_path, \
									t_oe_group.mms_path FROM	\
									(t_oe_element_general_monitor INNER JOIN t_oe_element_general ON	\
									t_oe_element_general_monitor.ied_no = t_oe_element_general.ied_no AND	\
									t_oe_element_general_monitor.cpu_no = t_oe_element_general.cpu_no AND	\
									t_oe_element_general_monitor.group_no = t_oe_element_general.group_no AND	\
									t_oe_element_general_monitor.entry = t_oe_element_general.entry AND	\
									t_oe_element_general_monitor.ied_no=%d)	\
									INNER JOIN t_oe_group ON t_oe_group.ied_no = t_oe_element_general.ied_no AND	\
									t_oe_group.cpu_no = t_oe_element_general.cpu_no AND t_oe_group.group_no = t_oe_element_general.group_no	\
		", m_iedNo);
	SRecordset rs;
	DB->Retrieve(sql, rs);

	unsigned long pos = 0;
	unsigned long ds_pos;
	unsigned long fc_pos;
	SPtrList<t_reference> lstFc;

	for (int row = 0; row < rs.GetRows(); ++row)
	{
		AnalogMonitorItem *item = new AnalogMonitorItem();
		int col = 0;
		item->gen_sn = rs.GetValueInt(row, col++);
		item->ied_no = rs.GetValueInt(row, col++);
		item->cpu_no = rs.GetValueInt(row, col++);
		item->group_no = rs.GetValueInt(row, col++);
		item->entry = rs.GetValueInt(row, col++);
		item->enabled = rs.GetValueInt(row, col++) == 1 ? true : false;
		item->inteval = rs.GetValueInt(row, col++);
		item->maxval = rs.GetValueFloat(row, col++);
		item->minval = rs.GetValueFloat(row, col++);
		item->mutate = rs.GetValueFloat(row, col++);
		item->last_soc = rs.GetValueInt(row, col++);
		item->mms_path = rs.GetValue(row, col++);
		item->dsdataset = rs.GetValue(row, col++);
		item->reference = NULL;

		// 与数据集对比，若符合规范则加入监视队列
		CSMmsDataset *ds = m_Datasets->FetchFirst(ds_pos);
		while(ds)
		{
			if (ds->m_sDsPath == item->dsdataset && ds->m_sIedName == this->iedName())
			{
				stuSMmsDataNode *fc = ds->m_Items.FetchFirst(fc_pos);
				while(fc)
				{
					if (fc->sMmsPath == item->mms_path)
					{
						item->reference = (t_reference *)fc->user;
						break;
					}
					fc = ds->m_Items.FetchNext(fc_pos);
				}
			}

			if (item->reference)
				break;

			ds = m_Datasets->FetchNext(ds_pos);
		}

		if (item->reference)
			m_analogMonitorList.push_back(item);
		else
			delete item;
	}

	return true;
}

bool Ied::InitElementParamCheck(SPtrList<CSMmsDataset> *m_Datasets, stuGlobalConfig *globalConfig)
{
	SRecordset rs;
	SString sql = SString::toFormat("select current_val from t_oe_ied_param where ied_no = %d and param_name='paramchk.enabled'", m_iedNo);
	int cnt = DB->Retrieve(sql, rs);
	if (cnt <= 0 && globalConfig->paramchk_enabled == false)			// 有全局配置时装置没配置同样可以执行
	{
		LOGDEBUG("装置(%d)未配置定值巡检参数paramchk.enabled，将不启用定值巡检", m_iedNo);
		return true;
	}

	SString paramchkVal = rs.GetValue(0, 0);	
	if (globalConfig->paramchk_enabled == true && (paramchkVal.CompareNoCase("true") == 0 || paramchkVal.toInt() == 1) || paramchkVal.isEmpty())
	{
		SString val;
		// 获取巡检间隔，定时或间隔执行
		sql = SString::toFormat("select current_val from t_oe_ied_param where ied_no=%d and param_name='paramchk.time'", m_iedNo);
		cnt = DB->Retrieve(sql, rs);
		if (cnt <= 0)
		{
			val = globalConfig->paramchk_time;
		}
		else
			val = rs.GetValue(0, 0);

		if (val.left(1) == "0")
		{
			m_paramCheckInteval = val.right(val.length() - 2).toInt();
			LOGDEBUG("装置(%d)配置了定值巡检功能，将启用定值巡检，间隔%d小时%d分钟", m_iedNo, m_paramCheckInteval / 60, m_paramCheckInteval % 60);
		}
		else if (val.left(1) == "1")
		{
			val = val.right(val.length() - 2);
			cnt = SString::GetAttributeCount(val, ",");
			for (int i = 1; i <= cnt; ++i)
			{
				int hour = SString::GetIdAttributeI(i, val, ",");
				m_paramCheckTimed.push_back(hour);
			}
			LOGDEBUG("装置(%d)配置了定值巡检功能，将启用定值巡检，定时时间%s时", m_iedNo, val.data());
		}
		else
		{
			LOGDEBUG("装置(%d)定值巡检时间间隔参数配置错误(参数=%s)，将不启用定值巡检", m_iedNo, val.data());
			return true;
		}
	}
	else
	{
		LOGDEBUG("装置(%d)禁用了定值巡检功能，将禁用定值巡检", m_iedNo);
		return true;
	}

	sql = SString::toFormat("select current_val from t_oe_ied_param where ied_no = %d and param_name='paramchk.last_soc'");
	m_lastParamCheckSoc = DB->SelectIntoI(sql);

	SString groupCondition;		// 限制组号
	sql = SString::toFormat("select cpu_no, group_no from t_oe_group where ied_no=%d and type=1", m_iedNo);
	cnt = DB->Retrieve(sql, rs);
	for (int row = 0; row < rs.GetRows(); ++row)
	{
		if (groupCondition.isEmpty() == false)
			groupCondition += " OR ";
		groupCondition += SString::toFormat("(t_oe_element_general.cpu_no=%d and t_oe_element_general.group_no=%d)", 
			rs.GetValueInt(row, 0), rs.GetValueInt(row, 1));
	}
	if (groupCondition.isEmpty() == false)
		groupCondition = SString::toFormat("AND (%s)", groupCondition.data());
	 
	// 列出装置下所有定值
	sql = SString::toFormat("\
							select t_oe_element_general.gen_sn, t_oe_element_general.ied_no, t_oe_element_general.cpu_no, \
							t_oe_element_general.group_no, t_oe_element_general.entry, t_oe_element_general.valtype, \
							t_oe_group.mms_path, t_oe_element_general.mms_path,\
							t_oe_element_general.reference_val, t_oe_param_check_val.soc, t_oe_param_check_val.current_val	\
							from (t_oe_element_general INNER JOIN t_oe_group ON	\
							t_oe_element_general.ied_no=t_oe_group.ied_no AND t_oe_element_general.cpu_no=t_oe_group.cpu_no	\
							AND t_oe_element_general.group_no=t_oe_group.group_no AND t_oe_element_general.ied_no=%d %s) " \
							// AND t_oe_element_general.reference_val <> '' AND t_oe_element_general.reference_val <> 0.0)
							"LEFT JOIN t_oe_param_check_val		\
							ON t_oe_element_general.ied_no=t_oe_param_check_val.ied_no AND t_oe_element_general.cpu_no=t_oe_param_check_val.cpu_no AND	\
							t_oe_element_general.group_no=t_oe_param_check_val.group_no AND t_oe_element_general.entry=t_oe_param_check_val.entry	\
							AND t_oe_param_check_val.type=0 \
							", m_iedNo, groupCondition.data());
	cnt = DB->Retrieve(sql, rs);

	unsigned long pos = 0;
	unsigned long ds_pos;
	unsigned long fc_pos;
	SPtrList<t_reference> lstFc;

	// 优化 - 从数据库中查找所有组下面的条目先将信息存储下来
	SRecordset rsParamCheckVal;
	std::map<std::string, int> mapParamCheckItem;
	sql = SString::toFormat("select gen_sn, cpu_no, group_no, entry from t_oe_param_check_val where ied_no=%d  and type=0", m_iedNo);
	DB->Retrieve(sql, rsParamCheckVal);
	for (int row = 0; row < rsParamCheckVal.GetRows(); ++row)
	{
		int gen_sn = rsParamCheckVal.GetValueInt(row, 0);
		int cpu_no = rsParamCheckVal.GetValueInt(row, 1);
		int group_no = rsParamCheckVal.GetValueInt(row, 2);
		int entry = rsParamCheckVal.GetValueInt(row, 3);
		std::string ident = SString::toFormat("%03d%03d%03d", cpu_no, group_no, entry).data();
		mapParamCheckItem[ident] = 1;
	}

	for (int row = 0; row < rs.GetRows(); ++row)
	{
		ParamCheckItem *item = new ParamCheckItem();
		int col = 0;
		item->gen_sn = rs.GetValueInt(row, col++);
		item->ied_no = rs.GetValueInt(row, col++);
		item->cpu_no = rs.GetValueInt(row, col++);
		item->group_no = rs.GetValueInt(row, col++);
		item->entry = rs.GetValueInt(row, col++);
		item->valtype = rs.GetValueInt(row, col++);
		item->dsdataset = rs.GetValue(row, col++);
		item->mms_path = rs.GetValue(row, col++);
		item->reference_val = rs.GetValue(row, col++);
		item->last_soc = rs.GetValueInt(row, col++);
		item->last_val = rs.GetValue(row, col++);
		item->current_val = item->last_val;
		item->reference = NULL;

		// 与数据集对比，若符合规范则加入监视队列
		CSMmsDataset *ds = m_Datasets->FetchFirst(ds_pos);
		while(ds)
		{
			if (ds->m_sDsPath == item->dsdataset && ds->m_sIedName == this->iedName())
			{
				stuSMmsDataNode *fc = ds->m_Items.FetchFirst(fc_pos);
				while(fc)
				{
					if (fc->sMmsPath == item->mms_path)
					{
						item->reference = (t_reference *)fc->user;
						break;
					}
					fc = ds->m_Items.FetchNext(fc_pos);
				}
			}

			if (item->reference)
				break;

			ds = m_Datasets->FetchNext(ds_pos);
		}

		if (item->reference)
			m_paramCheckList.push_back(item);
		else
			delete item;

		// 检查数据库中记录是否存在，不存在则新建，后面直接更新数值
		if (item->last_soc == 0)
		{
			std::string ident = SString::toFormat("%03d%03d%03d", item->cpu_no, item->group_no, item->entry).data();
			std::map<std::string, int>::iterator iterGenSn = mapParamCheckItem.find(ident);
// 			SRecordset valRs;
// 			sql = SString::toFormat("select gen_sn from t_oe_param_check_val where ied_no=%d and cpu_no=%d and group_no=%d and entry=%d and type=0",
// 				item->ied_no, item->cpu_no, item->group_no, item->entry);
// 			DB->Retrieve(sql, valRs);
// 			if (valRs.GetRows() == 0)
			if (iterGenSn == mapParamCheckItem.end())
			{
				sql = SString::toFormat("insert into t_oe_param_check_val(gen_sn, ied_no, cpu_no, group_no, entry, current_val, soc, type) \
										values(%d, %d, %d, %d, %d, '%s', %d, 0)",
										item->gen_sn, item->ied_no, item->cpu_no, item->group_no, item->entry, "", 1);
				if (!DB->Execute(sql))
				{
					LOGERROR("插入定值巡检记录失败，sql=%s", sql.data());
				}
			}
		}
	}

	// 定值巡检告警点
	sql = SString::toFormat("select st_sn, ied_no, cpu_no, group_no, entry, fun, inf, type, severity_level from t_oe_element_state where ied_no=%d and name='定值变化'", m_iedNo);
	DB->Retrieve(sql, rs);
	if (rs.GetRows() > 0)
	{
		int col = 0;
		m_paramChangeAlarmRef = new t_reference();
		m_paramChangeAlarmRef->tableName = "t_oe_element_state";
		m_paramChangeAlarmRef->st_sn = rs.GetValueInt(0, col++);
		m_paramChangeAlarmRef->ied_no = rs.GetValueInt(0, col++);
		m_paramChangeAlarmRef->cpu_no = rs.GetValueInt(0, col++);
		m_paramChangeAlarmRef->group_no = rs.GetValueInt(0, col++);
		m_paramChangeAlarmRef->entry = rs.GetValueInt(0, col++);
		m_paramChangeAlarmRef->fun = rs.GetValueInt(0, col++);
		m_paramChangeAlarmRef->inf = rs.GetValueInt(0, col++);
		m_paramChangeAlarmRef->type = rs.GetValueInt(0, col++);
		m_paramChangeAlarmRef->level = rs.GetValueInt(0, col++);
	}

	return true;
}

void Ied::updateParamCheckSoc(unsigned int soc)
{
	SString sql = SString::toFormat("update t_oe_ied_param set current_val='%d' where ied_no = %d and param_name='paramchk.last_soc'", soc, m_iedNo);
	if (!DB->Execute(sql))
	{
		sql = SString::toFormat("select ied_no from t_oe_ied_param where ied_no=%d and param_name='paramchk.last_soc'", m_iedNo);
		SRecordset rs;
		DB->Retrieve(sql ,rs);
		if (rs.GetRows() > 0)
		{
			sql = SString::toFormat("delete from t_oe_ied_param where ied_no=%d and param_name='paramchk.last_soc'", m_iedNo);
			DB->Execute(sql);
		}
	
		sql = SString::toFormat("insert into t_oe_ied_param(ied_no, param_name, value_type, current_val, dsc) \
								values(%d, 'paramchk.last_soc', %d, '%d', '最后次定值巡检时间')", m_iedNo, 3, soc);
		DB->Execute(sql);
	}
	m_lastParamCheckSoc = soc;
}

bool Ied::isTimeParamCheck(unsigned int currentSoc)
{
	if (m_paramCheckTimed.size() > 0)	// 定时执行
	{
		SDateTime dtCurrent = SDateTime::makeDateTime(currentSoc);
		SDateTime dtLast = SDateTime::makeDateTime(m_lastParamCheckSoc);

		bool canCheck = false;
		for (std::vector<int>::iterator iter = m_paramCheckTimed.begin(); iter != m_paramCheckTimed.end(); ++iter)
		{
			if (dtLast.hour() != dtCurrent.hour() && dtCurrent.hour() == *iter)
			{
				canCheck = true;
				break;
			}
		}

		if (!canCheck)
			return false;
	}
	else if (m_paramCheckInteval > 0)
	{
		if (currentSoc < m_lastParamCheckSoc + m_paramCheckInteval * 60)
			return false;
	}
	else
		return false;

	return true;
}

bool Ied::isAutoDownloadWave()
{
	// 装置是否处在检修状态
	SString sql = SString::toFormat("select current_val from t_oe_ied_param where ied_no=%d and param_name='IsAutoDownloadWave'", m_iedNo);
	SRecordset rs;
	DB->Retrieve(sql, rs);
	if (rs.GetRows() > 0)
	{
		SString value = rs.GetValueInt(0, 0);
		if (value.toInt() == 1 || value.CompareNoCase("true") == 0)
		{
			return true;
		}
	}

	return false;
}

IedParam::IedParam()
{
	m_isSupportReqWaveByTimePeriod = false;
	m_RCDCpuNo = 0;
	lastCommunicationCheckInteval = 60;
}

void IedParam::setAttr(std::string key, std::string value)
{
	m_mapAttrs.insert(std::make_pair(key, value));
}

bool IedParam::isContainAttr(std::string key)
{
	std::map<std::string, std::string>::iterator iter = m_mapAttrs.find(key);
	if (iter != m_mapAttrs.end())
		return true;
	return false;
}

std::string IedParam::getAttrValueString(std::string key)
{
	std::map<std::string, std::string>::iterator iter = m_mapAttrs.find(key);
	if (iter != m_mapAttrs.end())
		return iter->second;

	return "";
}

int IedParam::getAttrValueInt(std::string key)
{
	return atoi(getAttrValueString(key).c_str());
}

float IedParam::getAttrValueFloat(std::string key)
{
	return (float)atof(getAttrValueString(key).c_str());
}
