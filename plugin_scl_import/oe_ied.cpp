#include "oe_ied.h"
#include "view_plugin_scl_import.h"

oe_ied::oe_ied(view_plugin_scl_import *scl,QList<XmlObject*> list,QString ied,int no,int subNo)
{
	ied_no = no;
	sub_no = subNo;
	device_no = 0;
	name = "NULL";
	type = 0;
	pri_code = 0;
	uri = "NULL";
	model = "NULL";
	version = "NULL";
	crc = "NULL";
	version_time = "NULL";
	mms_path = "NULL";

	sclImport = scl;
	document = list;
	iedName = ied;
	object = document.at(0)->findChildDeep("IED","name",iedName);
	goose_cb_no = 0;
	smv_cb_no = 0;
}

oe_ied::~oe_ied(void)
{
}

bool oe_ied::execute(QString & error)
{
	bool b = false;

	if (!object)
	{
		LOGFAULT("表t_oe_ied插入操作时，装置对象未发现：%s.", iedName.toLatin1().data());
		error = QObject::tr("表t_oe_ied插入操作时，装置对象未发现：%s.").arg(iedName.toLatin1().data());
		return false;
	}
	if (!get_ied_no(error))
		return false;
	if (!get_sub_no())
		return false;
	if (!get_device_no())
		return false;
	if (!get_name())
		return false;
	if (!get_type())
		return false;
	if (!get_pri_code())
		return false;
	if (!get_uri())
		return false;
	if (!get_model())
		return false;
	if (!get_version())
		return false;
	if (!get_crc())
		return false;
	if (!get_version_time())
		return false;
	if (!get_mms_path())
		return false;

	SString sql = SString::toFormat("insert into t_oe_ied (ied_no,sub_no,device_no,name,type,pri_code,uri,model,version,crc,"
		"version_time,mms_path,comstate) values (%d,%d,%d,'%s',%d,%d,'%s','%s','%s','%s','%s','%s',%d)",
		ied_no,
		sub_no,
		device_no,
		name.data(),
		type,
		pri_code,
		uri.data(),
		model.data(),
		version.data(),
		crc.data(),
		version_time.data(),
		mms_path.data(),
		0);

	b = DB->Execute(sql);
	if (!b)
	{
		error = "SQL语句执行错误：" + sql;
		return b;
	}

	if (m_bMDB)
	{
		b = MDB->Execute(sql);
		if (!b)
			error = "内存库SQL语句执行错误：" + sql;
	}

	SetElementStateUnknown();

	return b;
}

bool oe_ied::get_ied_no(QString & error)
{
	bool		b = true;
	SRecordset	rs;

	if (ied_no != 0)	//ied_no是外部装置后将删除装置号传入，复用原有装置号
		return b;

	SString sql = "SELECT MAX(ied_no) FROM t_oe_ied";
	int iRet = DB->Retrieve(sql,rs);
	if (iRet < 0)
	{
		error = "SQL语句执行错误：" + sql;
		b = false;
	}
	else if (iRet == 0)
	{
		ied_no = 1;
		b = true;
	}
	else if (iRet > 0)
	{
		ied_no = rs.GetValue(0,0).toInt();
		ied_no++;
		b = true;
	}
	
	return b;
}

bool oe_ied::get_sub_no()
{
	bool b = true;

	sub_no = sub_no;

	return b;
}

bool oe_ied::get_device_no()
{
	bool b = true;

	device_no = 0;

	return b;
}

bool oe_ied::get_name()
{
	bool b = true;

	name = object->attrib("desc").toStdString();
	
	return b;
}

bool oe_ied::get_type()
{
	type = 0;
	QList<device_type_t> list = sclImport->deviceTypeList;
	if (!list.count())
		return true;

	for (int i = 0; i < list.count(); i++)
	{
		if (list.at(i).wildcard.isEmpty())
			continue;

		QRegExp rx(list.at(i).wildcard);
		if (rx.indexIn(name.data()) >= 0)
		{
			type = list.at(i).type_no;
			break;
		}
	}

	return true;
}

bool oe_ied::get_pri_code()
{
	bool b = true;

	pri_code = 0;

	return b;
}

bool oe_ied::get_uri()
{
	bool b = true;

	uri = "NULL";

	return b;
}

bool oe_ied::get_model()
{
	bool b = true;

	model = object->attrib("type").toStdString();

	return b;
}

bool oe_ied::get_version()
{
	bool b = true;

	version = object->attrib("configVersion").toStdString();

	return b;
}

bool oe_ied::get_crc()
{
	bool b = true;

	crc = "NULL";

	return b;
}

bool oe_ied::get_version_time()
{
	bool b = true;

	version_time = "NULL";

	return b;
}

bool oe_ied::get_mms_path()
{
	bool b = true;

	mms_path = iedName.toStdString();

	return b;
}

void oe_ied::SetElementStateUnknown()
{
	SRecordset	rs;
	SString		sql;

	sql.sprintf("select t_oe_element_state.ied_no,t_oe_element_state.cpu_no,t_oe_element_state.group_no,t_oe_element_state.entry,"
		"t_oe_element_state.name from t_oe_ied,t_oe_group,t_oe_element_state where "
		"t_oe_ied.sub_no=%d and "
		"t_oe_ied.model='AGENT_MONI' and "
		"t_oe_ied.mms_path='AGENT_MONI' and "
		"t_oe_ied.ied_no=t_oe_group.ied_no and "
		"t_oe_group.reportcontrol_datSet='dsIEDCommSt' and "
		"t_oe_group.ied_no=t_oe_element_state.ied_no and "
		"t_oe_group.cpu_no=t_oe_element_state.cpu_no and "
		"t_oe_group.group_no=t_oe_element_state.group_no and "
		"t_oe_element_state.unknown_desc='未知'",sub_no);
	int cnt = DB->Retrieve(sql,rs);
	if (cnt > 0)
	{
		for (int i = 0; i < cnt; i++)
		{
			QString element_name = rs.GetValue(i,4).data();
			QString ied_name = name.data();
			QString ied_mms = mms_path.data();
			if (element_name.contains(ied_name) || element_name.contains(ied_mms))
			{
				sql.sprintf("update t_oe_element_state set unknown_desc='%s' "
					"where ied_no=%d and cpu_no=%d and group_no=%d and entry=%d",
					SString::toFormat("%d").data(),ied_no,
					rs.GetValue(i,0).toInt(),
					rs.GetValue(i,1).toInt(),
					rs.GetValue(i,2).toInt(),
					rs.GetValue(i,3).toInt());
				DB->Execute(sql);
				if (m_bMDB)
					MDB->Execute(sql);
				break;
			}
		}
	}
}
