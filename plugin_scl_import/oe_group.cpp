#include "oe_group.h"
#include "oe_cpu.h"
#include "oe_element_state.h"
#include "oe_element_general.h"
#include "view_plugin_scl_import.h"
#include "oe_ied.h"
#include "oe_goose_ctrl_block.h"
#include "oe_smv_ctrl_block.h"

oe_group::oe_group(view_plugin_scl_import * scl,QList<XmlObject*> list,oe_cpu *m_cpu)
{
	ied_no = m_cpu->getIedNo();
	cpu_no = m_cpu->getCpuNo();
	group_no = 0;
	name = "NULL";
	type = 0;
	mms_path = "NULL";
	report_ln = "NULL";
	report_name = "NULL";
	report_datset = "NULL";
	report_intgPd = 0;
	report_rptID = "NULL";
	report_confRev = "NULL";
	report_buffered = "false";
	report_bufTime = 0;
	report_desc = "NULL";
	trgOps_qchg = "false";
	trgOps_period = "false";
	trgOps_dupd = "false";
	trgOps_dchg = "false";
	trgOps_gi = "true";
	optFields_dataRef = "false";
	optFields_reasonCode = "false";
	optFields_configRef = "false";
	optFields_dataSet = "false";
	optFields_entryID = "false";
	optFields_timeStamp = "false";
	optFields_seqNum = "false";
	rptEnabled_max = 1;
	log_ena = "false";
	log_reasonCode = "false";
	log_name = "";
	entryID = "0000000000000000";

	sclImport = scl;
	document = list;
	cpu = m_cpu;
	iedName = m_cpu->getIedName();
	cpu_object = m_cpu->getObject();
}

oe_group::~oe_group(void)
{
}

bool oe_group::execute(QString & error,QString &warnText)
{
	bool b = true;

	groups = cpu_object->findChildrenDeep("DataSet");

	if (groups.count())
	{
		foreach (group_object,groups)
		{
			if (sclImport->getIedGroupType().value(group_object) == GROUP_NO_IMPORT) //´ËDATASET²»µ¼ÈëÊý¾Ý¿â
				continue;

			b = db_insert(group_object,error);
			if (b)
			{
				b = db_element(group_object,error);
				if (!b)
					break;
			}
			else
				break;
		}
	}

	if (b)
		b = control_group(cpu_object,error);
	//if (b)
	//	b = account_group(cpu_object,error);
	if (b)
	{
		oe_goose_ctrl_block gcb(sclImport,document,this);
		b = gcb.execute(error,warnText);
	}
	if (b)
	{
		oe_smv_ctrl_block scb(sclImport,document,this);
		b = scb.execute(error,warnText);
	}

	return b;
}

bool oe_group::db_element(XmlObject * object,QString & error)
{
	S_UNUSED(object);
	S_UNUSED(error);

	bool b = false;

	if (type == GROUP_LOG)
	{
		return true;
	}
	else if (type == GROUP_SWITCH || 
		type == GROUP_SOFT_STRAP ||
		type == GROUP_EVENT || 
		type == GROUP_ALARM ||
		type == GROUP_GOOSE)
	{
		oe_element_state element(sclImport,document,this);
		b = element.execute(error);
	}
	else
	{
		oe_element_general element(sclImport,document,this);
		b = element.execute(error);
	}

	return b;
}

bool oe_group::db_insert(XmlObject * object,QString & error)
{
	bool b = true;

	if (!get_ied_no(object))
		return false;
	if (!get_cpu_no(object))
		return false;
	if (!get_group_no(object,error))
		return false;
	if (!get_name(object))
		return false;
	if (!get_type(object))
		return false;
	if (!get_mms_path(object))
		return false;

	int cnt = 0;
	if (get_log_control(object))
	{
		cnt++;
		SString sql = SString::toFormat("INSERT INTO t_oe_group VALUES (%d,%d,%d,'%s',%d,'%s',"
			"'%s','%s','%s',%d,'%s','%s','%s',%d,"
			"'%s','%s','%s','%s','%s','%s','%s','%s','%s','%s','%s','%s','%s',%d,'%s','%s','%s','%s')",
			ied_no,
			cpu_no,
			group_no,
			name.data(),
			type,
			mms_path.data(),
			report_ln.data(),
			report_name.data(),
			report_datset.data(),
			report_intgPd,
			report_rptID.data(),
			report_confRev.data(),
			report_buffered.data(),
			report_bufTime,
			report_desc.data(),
			trgOps_qchg.data(),
			trgOps_period.data(),
			trgOps_dupd.data(),
			trgOps_dchg.data(),
			trgOps_gi.data(),
			optFields_dataRef.data(),
			optFields_reasonCode.data(),
			optFields_configRef.data(),
			optFields_dataSet.data(),
			optFields_entryID.data(),
			optFields_timeStamp.data(),
			optFields_seqNum.data(),
			rptEnabled_max,
			log_ena.data(),
			log_reasonCode.data(),
			log_name.data(),
			entryID.data());

		if (!DB->Execute(sql))
		{
			error = "SQLÓï¾äÖ´ÐÐ´íÎó£º" + sql;
			return false;
		}

		if (m_bMDB && !MDB->Execute(sql))
		{
			error = "ÄÚ´æ¿âSQLÓï¾äÖ´ÐÐ´íÎó£º" + sql;
			return false;
		}

		if (!get_group_no(object,error))
			return false;
	}

	if (get_report_control(object))
	{
		cnt++;
		SString sql = SString::toFormat("INSERT INTO t_oe_group VALUES (%d,%d,%d,'%s',%d,'%s',"
			"'%s','%s','%s',%d,'%s','%s','%s',%d,"
			"'%s','%s','%s','%s','%s','%s','%s','%s','%s','%s','%s','%s','%s',%d,'%s','%s','%s','%s')",
			ied_no,
			cpu_no,
			group_no,
			name.data(),
			type,
			mms_path.data(),
			report_ln.data(),
			report_name.data(),
			report_datset.data(),
			report_intgPd,
			report_rptID.data(),
			report_confRev.data(),
			report_buffered.data(),
			report_bufTime,
			report_desc.data(),
			trgOps_qchg.data(),
			trgOps_period.data(),
			trgOps_dupd.data(),
			trgOps_dchg.data(),
			trgOps_gi.data(),
			optFields_dataRef.data(),
			optFields_reasonCode.data(),
			optFields_configRef.data(),
			optFields_dataSet.data(),
			optFields_entryID.data(),
			optFields_timeStamp.data(),
			optFields_seqNum.data(),
			rptEnabled_max,
			log_ena.data(),
			log_reasonCode.data(),
			log_name.data(),
			entryID.data());

		if (!DB->Execute(sql))
		{
			error = "SQLÓï¾äÖ´ÐÐ´íÎó£º" + sql;
			return false;
		}

		if (m_bMDB && !MDB->Execute(sql))
		{
			error = "ÄÚ´æ¿âSQLÓï¾äÖ´ÐÐ´íÎó£º" + sql;
			return false;
		}
	}
	else if (cnt > 0)
		group_no--; //½â¾öÄ³DataSetÖ»ÓÐlogcontrol£¬µ¼ÖÂgroup_no¶à¼ÓÒ»¸ö£¬elementÖÐgroup_no²»¶ÔµÄÎÊÌâ

	if (cnt == 0)
	{
		report_ln = "NULL";
		report_name = "NULL";
		report_datset = "NULL";
		report_intgPd = 0;
		report_rptID = "NULL";
		report_confRev = "NULL";
		report_buffered = "false";
		report_bufTime = 0;
		report_desc = "NULL";
		trgOps_qchg = "false";
		trgOps_period = "false";
		trgOps_dupd = "false";
		trgOps_dchg = "false";
		trgOps_gi = "true";
		optFields_dataRef = "false";
		optFields_reasonCode = "false";
		optFields_configRef = "false";
		optFields_dataSet = "false";
		optFields_entryID = "false";
		optFields_timeStamp = "false";
		optFields_seqNum = "false";
		rptEnabled_max = 1;
		log_ena = "false";
		log_reasonCode = "false";
		log_name = "";
		entryID = "0000000000000000";
		
		SString sql = SString::toFormat("INSERT INTO t_oe_group VALUES (%d,%d,%d,'%s',%d,'%s',"
			"'%s','%s','%s',%d,'%s','%s','%s',%d,"
			"'%s','%s','%s','%s','%s','%s','%s','%s','%s','%s','%s','%s','%s',%d,'%s','%s','%s','%s')",
			ied_no,
			cpu_no,
			group_no,
			name.data(),
			type,
			mms_path.data(),
			report_ln.data(),
			report_name.data(),
			report_datset.data(),
			report_intgPd,
			report_rptID.data(),
			report_confRev.data(),
			report_buffered.data(),
			report_bufTime,
			report_desc.data(),
			trgOps_qchg.data(),
			trgOps_period.data(),
			trgOps_dupd.data(),
			trgOps_dchg.data(),
			trgOps_gi.data(),
			optFields_dataRef.data(),
			optFields_reasonCode.data(),
			optFields_configRef.data(),
			optFields_dataSet.data(),
			optFields_entryID.data(),
			optFields_timeStamp.data(),
			optFields_seqNum.data(),
			rptEnabled_max,
			log_ena.data(),
			log_reasonCode.data(),
			log_name.data(),
			entryID.data());

		if (!DB->Execute(sql))
		{
			error = "SQLÓï¾äÖ´ÐÐ´íÎó£º" + sql;
			b = false;
		}

		if (m_bMDB && !MDB->Execute(sql))
		{
			error = "ÄÚ´æ¿âSQLÓï¾äÖ´ÐÐ´íÎó£º" + sql;
			b = false;
		}
	}

	return b;
}

bool oe_group::get_ied_no(XmlObject * object)
{
	S_UNUSED(object);

	bool b = true;

	ied_no = cpu->getIedNo();

	return b;
}

bool oe_group::get_cpu_no(XmlObject * object)
{
	S_UNUSED(object);

	bool b = true;

	cpu_no = cpu->getCpuNo();

	return b;
}

bool oe_group::get_group_no(XmlObject * object,QString & error)
{
	S_UNUSED(object);
	S_UNUSED(error);

	bool		b = true;
	SRecordset	rs;

	SString sql = SString::toFormat("SELECT MAX(group_no) FROM t_oe_group where ied_no=%d and cpu_no=%d",ied_no,cpu_no);
	int iRet = DB->Retrieve(sql,rs);
	if (iRet < 0)
	{
		error = "SQLÓï¾äÖ´ÐÐ´íÎó£º" + sql;
		b = false;
	}
	else if (iRet == 0)
	{
		group_no = 1;
		b = true;
	}
	else if (iRet > 0)
	{
		group_no = rs.GetValue(0,0).toInt();
		group_no++;
		b = true;
	}

	return b;
}

bool oe_group::get_name(XmlObject * object)
{
	S_UNUSED(object);

	bool b = true;

	name = object->attrib("desc").toStdString();

	return b;
}

bool oe_group::get_type(XmlObject * object)
{
	S_UNUSED(object);

	bool b = true;
	QMap<QString,int> map;
	QString name = object->attrib("name");

	if (sclImport->bSmartImport)
	{
		if (name.contains("dsAin"))
			type = GROUP_ANALOGUE;
		else if (name.contains("dsDin"))
			type = GROUP_SWITCH;
		else if (name.contains("dsAlarm"))
			type = GROUP_EVENT;
		else if (name.contains("dsWarning"))
			type = GROUP_ALARM;
		else if (name.contains("dsCommState"))
			type = GROUP_SWITCH;
		else if (name.contains("dsParameter"))
			type = GROUP_SETTING;
		else if (name.contains("dsTripInfo"))
			type = GROUP_EVENT;
		else if (name.contains("dsRelayDin"))
			type = GROUP_SWITCH;
		else if (name.contains("dsRelayEna"))
			type = GROUP_SOFT_STRAP;
		else if (name.contains("dsRelayRec"))
			type = GROUP_RELAY_REC;
		else if (name.contains("dsRelayAin"))
			type = GROUP_ANALOGUE;
		else if (name.contains("dsSetting"))
			type = GROUP_SETTING;
		else if (name.contains("dsGOOSE"))
			type = GROUP_GOOSE;
		else if (name.contains("dsSV"))
			type = GROUP_SV;
		else if (name.contains("dsLog"))
			type = GROUP_LOG;
		else
		{
			map = sclImport->getMapGroupType();
			QMap<QString,int>::iterator iter = map.find(name);
			if (iter != map.end()) 
				type = iter.value();
			else
				type = GROUP_UNKNOWN;
		}	
	}
	else
	{
		QMap<XmlObject*,int> obj_map = sclImport->getIedGroupType();
		type = obj_map.value(object);
	}

	return b;
}

bool oe_group::get_mms_path(XmlObject * object)
{
	S_UNUSED(object);

	bool b = true;

	mms_path = cpu->getCpuMmsPath() + "$" + object->attrib("name").toStdString();

	return b;
}

bool oe_group::get_report_control(XmlObject * object)
{
	bool b = false;

	XmlObject * obj = cpu_object->findChildDeep("ReportControl","datSet",object->attrib("name"));
	if (obj)
	{
		report_ln = obj->parent->attrib("lnClass").toStdString();
		report_name = obj->attrib("name").toStdString();
		report_datset = obj->attrib("datSet").toStdString();
		report_intgPd = obj->attrib("intgPd").toInt();
		report_rptID = obj->attrib("rptID").toStdString(); 
		report_confRev = obj->attrib("confRev").toStdString();
		report_buffered = obj->attrib("buffered").isEmpty() ? "false" : obj->attrib("buffered").toStdString();
		report_bufTime = obj->attrib("bufTime").isEmpty() ? 0 : obj->attrib("bufTime").toInt();
		report_desc = obj->attrib("desc").toStdString();

		XmlObject * obj_ex = obj->findChild("TrgOps");
		if (obj_ex)
		{
			trgOps_qchg = obj_ex->attrib("qchg").isEmpty() ? "false" : obj_ex->attrib("qchg").toStdString();
			trgOps_period = obj_ex->attrib("period").isEmpty() ? "false" : obj_ex->attrib("period").toStdString();
			trgOps_dupd = obj_ex->attrib("dupd").isEmpty() ? "false" : obj_ex->attrib("dupd").toStdString();
			trgOps_dchg = obj_ex->attrib("dchg").isEmpty() ? "false" : obj_ex->attrib("dchg").toStdString();
			trgOps_dchg = obj_ex->attrib("gi").isEmpty() ? "true" : obj_ex->attrib("gi").toStdString();
		}
		else
		{
			trgOps_qchg = "false";
			trgOps_period = "false";
			trgOps_dupd = "false";
			trgOps_dchg = "false";
			trgOps_gi = "true";
		}

		obj_ex = obj->findChild("OptFields");
		if (obj_ex)
		{
			optFields_dataRef = obj_ex->attrib("dataRef").isEmpty() ? "false" : obj_ex->attrib("dataRef").toStdString();
			optFields_reasonCode = obj_ex->attrib("reasonCode").isEmpty() ? "false" : obj_ex->attrib("reasonCode").toStdString();
			optFields_configRef = obj_ex->attrib("configRef").isEmpty() ? "false" : obj_ex->attrib("configRef").toStdString();
			optFields_dataSet = obj_ex->attrib("dataSet").isEmpty() ? "false" : obj_ex->attrib("dataSet").toStdString();
			optFields_entryID = obj_ex->attrib("entryID").isEmpty() ? "false" : obj_ex->attrib("entryID").toStdString();
			optFields_timeStamp = obj_ex->attrib("timeStamp").isEmpty() ? "false" : obj_ex->attrib("timeStamp").toStdString();
			optFields_seqNum = obj_ex->attrib("seqNum").isEmpty() ? "false" : obj_ex->attrib("seqNum").toStdString();
		}
		else
		{
			optFields_dataRef = "false";
			optFields_reasonCode = "false";
			optFields_configRef = "false";
			optFields_dataSet = "false";
			optFields_entryID = "false";
			optFields_timeStamp = "false";
			optFields_seqNum = "false";
		}

		obj_ex = obj->findChild("RptEnabled");
		if (obj_ex)
			rptEnabled_max = obj_ex->attrib("max").toInt();
		else
			rptEnabled_max = 1;

		log_ena = "false";
		log_reasonCode = "false";
		log_name = "";

		b = true;
	}

	return b;
}

bool oe_group::get_log_control(XmlObject * object)
{
	bool b = false;

	XmlObject *obj = cpu_object->findChildDeep("LogControl","datSet",object->attrib("name"));
	if (obj)
	{
		report_ln = obj->parent->attrib("lnClass").toStdString();
		report_name = obj->attrib("name").toStdString();
		report_datset = obj->attrib("datSet").toStdString();
		report_intgPd = obj->attrib("intgPd").toInt();
		report_rptID = "NULL";
		report_confRev = "NULL";
		report_buffered = "false";
		report_bufTime = 0;
		report_desc = obj->attrib("desc").toStdString();

		log_ena = obj->attrib("logEna").isEmpty() ? "true" : obj->attrib("logEna").toStdString();
		log_reasonCode = obj->attrib("reasonCode").isEmpty() ? "false" : obj->attrib("reasonCode").toStdString();
		log_name = obj->attrib("logName").toStdString();

		XmlObject * obj_ex = obj->findChild("TrgOps");
		if (obj_ex)
		{
			trgOps_qchg = obj_ex->attrib("qchg").isEmpty() ? "false" : obj_ex->attrib("qchg").toStdString();
			trgOps_period = obj_ex->attrib("period").isEmpty() ? "false" : obj_ex->attrib("period").toStdString();
			trgOps_dupd = obj_ex->attrib("dupd").isEmpty() ? "false" : obj_ex->attrib("dupd").toStdString();
			trgOps_dchg = obj_ex->attrib("dchg").isEmpty() ? "false" : obj_ex->attrib("dchg").toStdString();
			trgOps_dchg = obj_ex->attrib("gi").isEmpty() ? "true" : obj_ex->attrib("gi").toStdString();
		}
		else
		{
			trgOps_qchg = "false";
			trgOps_period = "false";
			trgOps_dupd = "false";
			trgOps_dchg = "false";
			trgOps_gi = "true";
		}

		optFields_dataRef = "false";
		optFields_reasonCode = "false";
		optFields_configRef = "false";
		optFields_dataSet = "false";
		optFields_entryID = "false";
		optFields_timeStamp = "false";
		optFields_seqNum = "false";
		rptEnabled_max = 1;

		b = true;
	}

	return b;
}

////////////////////////////  Ò£¿Ø×é¼°µãÑ¡È¡  //////////////////////////////////////////
bool oe_group::control_group(XmlObject * object,QString & error)
{
	S_UNUSED(object);

	int	entry = 0;
	bool isExistCtl = false;
	bool b = control_group_insert(error);
	if (!b)
		return false;

	SString	sql;
	switch (DB_MASTER_TYPE)
	{
	case DB_MYSQL:
		sql += "insert into t_oe_element_control (ied_no,cpu_no,group_no,entry,name,type,model,inver,mms_path,"
		"link_ied_no,link_cpu_no,link_group_no,link_entry) values ";
		break;

	case DB_ORACLE:
		sql += "insert all ";
		break;

	default:
		return false;
	}

	QList<XmlObject*> m_LNs = cpu_object->findChildrenDeep("LN0");
	m_LNs += cpu_object->findChildrenDeep("LN");
	if (m_LNs.count())
	{
		foreach (XmlObject *ln,m_LNs)
		{
			QList<XmlObject*> m_DOIs = ln->findChildrenDeep("DOI");
			if (m_DOIs.count())
			{
				foreach (XmlObject *doi,m_DOIs)
				{
					b = control_element(sql,entry,isExistCtl,doi,error);
					if (!b)
						return false;
				}
			}
		}
	}

	if (isExistCtl)
	{
		switch (DB_MASTER_TYPE)
		{
		case DB_MYSQL:
			sql = sql.left(sql.size()-1);	//È¥³ý×îºóµÄ¶ººÅ
			break;

		case DB_ORACLE:
			sql += SString::toFormat("SELECT 1 FROM DUAL");
			break;
		}

		if (!DB->Execute(sql))
		{
			error = "SQLÓï¾äÖ´ÐÐ´íÎó£º" + sql;
			return false;
		}

		if (m_bMDB && !MDB->Execute(sql))
		{
			error = "ÄÚ´æ¿âSQLÓï¾äÖ´ÐÐ´íÎó£º" + sql;
			return false;
		}
	}

	SRecordset	rs;
	sql.sprintf("select count(*) from t_oe_element_control where ied_no=%d and cpu_no=%d and group_no=%d",
		ied_no,cpu_no,group_no);
	int iRet = DB->Retrieve(sql,rs);
	if (iRet < 0)
	{
		error = "SQLÓï¾äÖ´ÐÐ´íÎó£º" + sql;
		return false;
	}
	else if (iRet > 0)
	{
		int count = rs.GetValue(0,0).toInt();
		if (count == 0)
		{
			sql.sprintf("delete from t_oe_group where ied_no=%d and cpu_no=%d and group_no=%d",ied_no,cpu_no,group_no);
			if (!DB->Execute(sql))
			{
				error = "SQLÓï¾äÖ´ÐÐ´íÎó£º" + sql;
				return false;
			}
			if (m_bMDB && !MDB->Execute(sql))
			{
				error = "ÄÚ´æ¿âSQLÓï¾äÖ´ÐÐ´íÎó£º" + sql;
				return false;
			}
		}
	}

	return true;
}

bool oe_group::control_group_insert(QString & error)
{
	SString		sql;
	SRecordset	rs;

	sql.sprintf("select group_no from t_oe_group where ied_no=%d and cpu_no=%d and type=%d",ied_no,cpu_no,GROUP_CONTROL);
	int iRet = DB->Retrieve(sql,rs);
	if (iRet < 0)
	{
		error = "SQLÓï¾äÖ´ÐÐ´íÎó£º" + sql;
		return false;
	}
	else if (iRet == 0)
		group_no++;
	else if (iRet > 0)
		group_no = rs.GetValue(0,0).toInt();

	sql.sprintf("insert into t_oe_group (ied_no,cpu_no,group_no,name,type) values (%d,%d,%d,'%s',%d)",
		ied_no,cpu_no,group_no,"¿ØÖÆ×é",GROUP_CONTROL);

	if (!DB->Execute(sql))
	{
		error = "SQLÓï¾äÖ´ÐÐ´íÎó£º" + sql;
		return false;
	}

	if (m_bMDB && !MDB->Execute(sql))
	{
		error = "ÄÚ´æ¿âSQLÓï¾äÖ´ÐÐ´íÎó£º" + sql;
		return false;
	}

	return true;
}

bool oe_group::control_element(SString &sql,int &entry,bool &isExistCtl,XmlObject * object,QString & error)
{
	XmlObject *obj = object->findChild("DAI","name","ctlModel");
	if (!obj)
		return true;//Î´ÕÒµ½£¬²»ÊÇÒ£¿Ø£¬·µ»Ø·ÖÎöÏÂÒ»¸öDOI
	
	QString model;
	XmlObject * val_Object = obj->findChild("Val");
	if (val_Object)
		model = val_Object->attrib("value");
	
	if (model != "1" && model != "2" && model != "3" && model != "4" && 
		model != "direct-with-normal-security" && model != "sbo-with-normal-security" &&
		model != "direct-with-enhanced-security" && model != "sbo-with-enhanced-security")
		return true;//model²»ÔÚ1-4·¶Î§ÄÚ£¬²»¿ÉÒ£¿Ø£¬·µ»Ø·ÖÎöÏÂÒ»¸öDOI

	int iModel = 0;
	if (model == "1" || model == "direct-with-normal-security")
		iModel = 1;
	else if (model == "2" || model == "sbo-with-normal-security")
		iModel = 2;
	else if (model == "3" || model == "direct-with-enhanced-security")
		iModel = 3;
	else if (model == "4" || model == "sbo-with-enhanced-security")
		iModel = 4;

	QString name;
	int		type;
	QString mms_path;
	int		link_ied_no;
	int		link_cpu_no;
	int		link_group_no;
	int		link_entry;
	if (!get_control_entry(entry,error))
		return false;
	if (!get_control_name(object,name))
		return false;
	if (!get_control_type(type))
		return false;
	if (!get_control_mms_path(object,mms_path,error))
		return false;
	if (!get_control_link(mms_path,error,link_ied_no,link_cpu_no,link_group_no,link_entry,type))
		return false;

	//SString sql;
	switch (DB_MASTER_TYPE)
	{
	case DB_MYSQL:
		sql += SString::toFormat("(%d,%d,%d,%d,'%s',%d,%d,%d,'%s',%d,%d,%d,%d),",
			cpu->getIedNo(),cpu->getCpuNo(),group_no,entry,name.toStdString().data(),type,iModel,0,mms_path.toStdString().data(),
			link_ied_no,link_cpu_no,link_group_no,link_entry);
		break;

	case DB_ORACLE:
		sql += SString::toFormat("into t_oe_element_control (ied_no,cpu_no,group_no,entry,name,type,model,inver,mms_path,link_ied_no,link_cpu_no,link_group_no,link_entry) "
			"values (%d,%d,%d,%d,'%s',%d,%d,%d,'%s',%d,%d,%d,%d) ",
			cpu->getIedNo(),cpu->getCpuNo(),group_no,entry,name.toStdString().data(),type,iModel,0,mms_path.toStdString().data(),
			link_ied_no,link_cpu_no,link_group_no,link_entry);
		break;

	default:
		return false;
	}

	//if (!DB->Execute(sql))
	//{
	//	error = "SQLÓï¾äÖ´ÐÐ´íÎó£º" + sql;
	//	return false;
	//}

	isExistCtl = true;

	return true;
}

bool oe_group::get_control_entry(int &entry,QString & error)
{
	S_UNUSED(error);

	//SString		sql;
	//SRecordset	rs;

	//sql.sprintf("select max(entry) from t_oe_element_control where ied_no=%d and cpu_no=%d and group_no=%d",
	//	cpu->getIedNo(),cpu->getCpuNo(),group_no);
	//int iRet = DB->Retrieve(sql,rs);
	//if (iRet < 0)
	//{
	//	error = "SQLÓï¾äÖ´ÐÐ´íÎó£º" + sql;
	//	return false;
	//}
	//else if (iRet == 0)
	//{
	//	entry = 1;
	//}
	//else if (iRet > 0)
	//{
	//	entry = rs.GetValue(0,0).toInt();
		entry++;
	//}

	return true;
}

bool oe_group::get_control_name(XmlObject * object,QString &name)
{
	name = object->attrib("desc");
	if (!name.isEmpty())
		return true;

	XmlObject * obj = object->findChildDeep("DAI","name","dU");
	if (obj)
	{
		XmlObject * val_Object = obj->findChild("Val");
		if (val_Object)
			name = obj->attrib("value");
	}

	return true;
}

bool oe_group::get_control_type(int &type)
{
	type = 1;

	return true;
}

bool oe_group::get_control_mms_path(XmlObject * object,QString &mms_path,QString & error)
{
	S_UNUSED(error);

	mms_path = cpu->ied->getIedMmsPath()+cpu->getCpuMmsPath()+"/";

	XmlObject *ln_object = object->parent;
	if (!ln_object)
		return false;

	mms_path += ln_object->attrib("prefix");
	mms_path += ln_object->attrib("lnClass");
	mms_path += ln_object->attrib("inst");
	mms_path += "$CO$";
	mms_path += object->attrib("name");

	return true;
}

bool oe_group::get_control_link(QString mms_path,QString & error,int &link_ied_no,int &link_cpu_no,int &link_group_no,int &link_entry,int &type)
{
	SString		sql;
	SRecordset	rs;
	QString		temp_mms = mms_path.replace("$CO$","$ST$");

	sql.sprintf("select ied_no,cpu_no,group_no,entry,val_type from t_oe_element_state "
		"where ied_no=%d and cpu_no=%d and mms_path like '%s%%'",cpu->getIedNo(),cpu->getCpuNo(),temp_mms.toLocal8Bit().data());
	int iRet = DB->Retrieve(sql,rs);
	if (iRet < 0)
	{
		error = "SQLÓï¾äÖ´ÐÐ´íÎó£º" + sql;
		return false;
	}
	else if (iRet == 0)
	{
		link_ied_no = 0;
		link_cpu_no = 0;
		link_group_no = 0;
		link_entry = 0;
	}
	else if (iRet == 1)
	{
		link_ied_no = rs.GetValue(0,0).toInt();
		link_cpu_no = rs.GetValue(0,1).toInt();
		link_group_no = rs.GetValue(0,2).toInt();
		link_entry = rs.GetValue(0,3).toInt();
		int val_type = rs.GetValue(0,4).toInt();
		if (val_type == 2)
			type = 1;
		else if (val_type == 1)
			type = 2;
	}
	else
	{
		link_ied_no = -1;
		link_cpu_no = -1;
		link_group_no = -1;
		link_entry = -1;
	}

	return true;
}

////////////////////////////  Ì¨ÕÊ×é¼°µãÑ¡È¡  //////////////////////////////////////////
bool oe_group::account_group(XmlObject * object,QString & error)
{
	XmlObject *obj_LPHD = object->findChildDeep("LN","lnClass","LPHD");
	if (obj_LPHD)
	{
		account_LPHD(obj_LPHD,error);
	}

	return true;
}

bool oe_group::account_LPHD(XmlObject * object,QString & error)
{
	SString		sql;
	SRecordset	rs;
	int			iRet;
	bool		ret;
	int			plugin_no=1;

	QString vendor = QString::null;
	QString hwRev = QString::null;
	QString swRev = QString::null;
	QString serNum = QString::null;
	QString model = QString::null;
	QString DevDescr = QString::null;
	QString MfgDate = QString::null;
	QString UseDate = QString::null;
	QString PlateNum = QString::null;

	sql.sprintf("select name,model,manufacturer,version,crc,factory_time,run_time,plugin_cnt "
		"from t_oe_account where dev_id=%d",cpu->getIedNo());
	iRet = DB->Retrieve(sql,rs);
	if (iRet < 0)
	{
		error = "SQLÓï¾äÖ´ÐÐ´íÎó£º" + sql;
		return false;
	}
	else if (iRet == 0)
	{
		error = QString("Î´»ñÈ¡µ½×°ÖÃ[%1]µÄÌ¨ÕÊÊý¾Ý¡£").arg(cpu->getIedNo());
		return false;
	}
	else if (iRet > 0)
	{
		DevDescr = rs.GetValue(0,0).data();
		model = rs.GetValue(0,1).data();
		vendor = rs.GetValue(0,2).data();
		swRev = rs.GetValue(0,3).data();
		serNum = rs.GetValue(0,4).data();
		MfgDate = rs.GetValue(0,5).data();
		UseDate = rs.GetValue(0,6).data();
		PlateNum = rs.GetValue(0,7).data();
	}

	sql.sprintf("delete from t_oe_account_plugin where dev_cls=1 and dev_id=%d",cpu->getIedNo());
	ret = DB->Execute(sql);
	if (!ret)
	{
		error = "SQLÓï¾äÖ´ÐÐ´íÎó£º" + sql;
		return false;
	}
	if (m_bMDB && !MDB->Execute(sql))
	{
		error = "ÄÚ´æ¿âSQLÓï¾äÖ´ÐÐ´íÎó£º" + sql;
		return false;
	}

	sql.sprintf("delete from t_oe_account_comm where dev_cls=1 and dev_id=%d",cpu->getIedNo());
	ret = DB->Execute(sql);
	if (!ret)
	{
		error = "SQLÓï¾äÖ´ÐÐ´íÎó£º" + sql;
		return false;
	}
	if (m_bMDB && !MDB->Execute(sql))
	{
		error = "ÄÚ´æ¿âSQLÓï¾äÖ´ÐÐ´íÎó£º" + sql;
		return false;
	}

	QList<XmlObject *> lstObj = object->findChildrenDeep("DOI");
	for (int i = 0; i < lstObj.count(); i++)
	{
		XmlObject *obj_doi = lstObj.at(i);
		QString name = obj_doi->attrib("name");
		if (name == "PhyNam")
		{
			XmlObject *obj_PhyNam_vendor = obj_doi->findChildDeep("DAI","name","vendor");
			if (obj_PhyNam_vendor)
			{
				XmlObject *obj_val = obj_PhyNam_vendor->findChild("Val");
				if (obj_val)
					vendor = obj_val->text;
				else
					vendor = obj_PhyNam_vendor->attrib("desc");
			}

			XmlObject *obj_PhyNam_hwRev = obj_doi->findChildDeep("DAI","name","hwRev");
			if (obj_PhyNam_hwRev)
			{
				XmlObject *obj_val = obj_PhyNam_hwRev->findChild("Val");
				if (obj_val)
					hwRev = obj_val->text;
				else
					hwRev = obj_PhyNam_hwRev->attrib("desc");
			}

			XmlObject *obj_PhyNam_swRev = obj_doi->findChildDeep("DAI","name","swRev");
			if (obj_PhyNam_swRev)
			{
				XmlObject *obj_val = obj_PhyNam_swRev->findChild("Val");
				if (obj_val)
					swRev = obj_val->text;
				else
					swRev = obj_PhyNam_swRev->attrib("desc");
			}

			XmlObject *obj_PhyNam_serNum = obj_doi->findChildDeep("DAI","name","serNum");
			if (obj_PhyNam_serNum)
			{
				XmlObject *obj_val = obj_PhyNam_serNum->findChild("Val");
				if (obj_val)
					serNum = obj_val->text;
				else
					serNum = obj_PhyNam_serNum->attrib("desc");
			}

			XmlObject *obj_PhyNam_model = obj_doi->findChildDeep("DAI","name","model");
			if (obj_PhyNam_model)
			{
				XmlObject *obj_val = obj_PhyNam_model->findChild("Val");
				if (obj_val)
					model = obj_val->text;
				else
					model = obj_PhyNam_model->attrib("desc");
			}
		}
		else if (name == "DevDescr")
		{
			XmlObject *obj_DevDescr_setVal = obj_doi->findChildDeep("DAI","name","setVal");
			if (obj_DevDescr_setVal)
			{
				XmlObject *obj_val = obj_DevDescr_setVal->findChild("Val");
				if (obj_val)
					DevDescr = obj_val->text;
			}
		}
		else if (name == "MfgDate")
		{
			XmlObject *obj_MfgDate_setVal = obj_doi->findChildDeep("DAI","name","setVal");
			if (obj_MfgDate_setVal)
			{
				XmlObject *obj_val = obj_MfgDate_setVal->findChild("Val");
				if (obj_val)
					MfgDate = obj_val->text;
			}
		}
		else if (name == "UseDate")
		{
			XmlObject *obj_UseDate_setVal = obj_doi->findChildDeep("DAI","name","setVal");
			if (obj_UseDate_setVal)
			{
				XmlObject *obj_val = obj_UseDate_setVal->findChild("Val");
				if (obj_val)
					UseDate = obj_val->text;
			}
		}
		else if (name == "PlateNum")
		{
			XmlObject *obj_PlateNum_setVal = obj_doi->findChildDeep("DAI","name","setVal");
			if (obj_PlateNum_setVal)
			{
				XmlObject *obj_val = obj_PlateNum_setVal->findChild("Val");
				if (obj_val)
					PlateNum = obj_val->text;
			}
		}
		else if (name.contains("PlateTyp"))
		{
			QString plateName = QString::null;
			QString plateModel = QString::null;
			if (!obj_doi->attrib("desc").isEmpty())
			{
				plateName = obj_doi->attrib("desc");
			}
			else
			{
				XmlObject *obj_PlateType_dU = obj_doi->findChildDeep("DAI","name","dU");
				if (obj_PlateType_dU)
				{
					XmlObject *obj_val = obj_PlateType_dU->findChild("Val");
					if (obj_val)
						plateName = obj_val->text;
				}
			}

			XmlObject *obj_PlateType_model = obj_doi->findChildDeep("DAI","name","model");
			if (obj_PlateType_model)
			{
				XmlObject *obj_val = obj_PlateType_model->findChild("Val");
				if (obj_val)
					plateModel = obj_val->text;
			}
			if (plateModel.isEmpty())
			{
				XmlObject *obj_PlateType_setVal = obj_doi->findChildDeep("DAI","name","setVal");
				if (obj_PlateType_setVal)
				{
					XmlObject *obj_val = obj_PlateType_setVal->findChild("Val");
					if (obj_val)
						plateModel = obj_val->text;
				}
			}

			sql.sprintf("insert into t_oe_account_plugin (dev_cls,dev_id,plugin_no,plugin_model,plugin_name) "
				"values (%d,%d,%d,'%s','%s')",
				1,
				cpu->getIedNo(),
				plugin_no++,
				plateModel.isEmpty() ? "NULL" : plateModel.toLocal8Bit().data(),
				plateName.isEmpty() ? "NULL" : plateName.toLocal8Bit().data());
			ret = DB->Execute(sql);
			if (!ret)
			{
				error = "SQLÓï¾äÖ´ÐÐ´íÎó£º" + sql;
				return false;
			}
		}
		else if (name.contains("IPAddr"))
		{
			QString ipaddrName = QString::null;
			QString ipaddrValue = QString::null;
			if (!obj_doi->attrib("desc").isEmpty())
			{
				ipaddrName = obj_doi->attrib("desc");
			}
			else
			{
				XmlObject *obj_IPAddr_dU = obj_doi->findChildDeep("DAI","name","dU");
				if (obj_IPAddr_dU)
				{
					XmlObject *obj_val = obj_IPAddr_dU->findChild("Val");
					if (obj_val)
						ipaddrName = obj_val->text;
				}
			}

			XmlObject *obj_IPAddr_setVal = obj_doi->findChildDeep("DAI","name","setVal");
			if (obj_IPAddr_setVal)
			{
				XmlObject *obj_val = obj_IPAddr_setVal->findChild("Val");
				if (obj_val)
					ipaddrValue = obj_val->text;
			}

			sql.sprintf("insert into t_oe_account_comm (dev_cls,dev_id,comm_name,comm_value) "
				"values (%d,%d,'%s','%s')",
				1,
				cpu->getIedNo(),
				ipaddrName.isEmpty() ? "NULL" : ipaddrName.toLocal8Bit().data(),
				ipaddrValue.isEmpty() ? "NULL" : ipaddrValue.toLocal8Bit().data());
			ret = DB->Execute(sql);
			if (!ret)
			{
				error = "SQLÓï¾äÖ´ÐÐ´íÎó£º" + sql;
				return false;
			}
		}
		else if (name.contains("MACAddr"))
		{
			QString macName = QString::null;
			QString macValue = QString::null;
			if (!obj_doi->attrib("desc").isEmpty())
			{
				macName = obj_doi->attrib("desc");
			}
			else
			{
				XmlObject *obj_IPAddr_dU = obj_doi->findChildDeep("DAI","name","dU");
				if (obj_IPAddr_dU)
				{
					XmlObject *obj_val = obj_IPAddr_dU->findChild("Val");
					if (obj_val)
						macName = obj_val->text;
				}
			}

			XmlObject *obj_IPAddr_setVal = obj_doi->findChildDeep("DAI","name","setVal");
			if (obj_IPAddr_setVal)
			{
				XmlObject *obj_val = obj_IPAddr_setVal->findChild("Val");
				if (obj_val)
					macValue = obj_val->text;
			}

			sql.sprintf("insert into t_oe_account_comm (dev_cls,dev_id,comm_name,comm_value) "
				"values (%d,%d,'%s','%s')",
				1,
				cpu->getIedNo(),
				macName.isEmpty() ? "NULL" : macName.toLocal8Bit().data(),
				macValue.isEmpty() ? "NULL" : macValue.toLocal8Bit().data());
			ret = DB->Execute(sql);
			if (!ret)
			{
				error = "SQLÓï¾äÖ´ÐÐ´íÎó£º" + sql;
				return false;
			}
		}
	}

	sql.sprintf("update t_oe_account set name='%s',model='%s',manufacturer='%s',"
		"version='%s',crc='%s',factory_time='%s',run_time='%s',plugin_cnt=%d where dev_id=%d",
		DevDescr.toLocal8Bit().data(),
		model.toLocal8Bit().data(),
		vendor.toLocal8Bit().data(),
		swRev.toLocal8Bit().data(),
		serNum.toLocal8Bit().data(),
		MfgDate.toLocal8Bit().data(),
		UseDate.toLocal8Bit().data(),
		PlateNum.toInt(),
		cpu->getIedNo());
	ret = DB->Execute(sql);
	if (!ret)
	{
		error = "SQLÓï¾äÖ´ÐÐ´íÎó£º" + sql;
		return false;
	}

	return true;
}
