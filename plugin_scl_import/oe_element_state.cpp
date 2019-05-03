#include "oe_element_state.h"
#include "oe_ied.h"
#include "oe_group.h"
#include "oe_cpu.h"
#include "view_plugin_scl_import.h"

oe_element_state::oe_element_state(view_plugin_scl_import * scl,QList<XmlObject*> list,oe_group *m_group)
{
	ied_no = 0;
	cpu_no = 0;
	group_no = 0;
	entry = 0;
	name = "NULL";
	fun = 0;
	inf = 0;
	type = 0;
	evt_cls = 0;
	val_type = 0;
	level = 0;
	on_desc = "NULL";
	off_desc = "NULL";
	unknown_desc = "NULL";
	inver = 0;
	val = 0;
	measure_val = "NULL";
	soc = 0;
	usec = 0;
	mms_path = "NULL";

	sclImport = scl;
	document = list;
	group = m_group;
	ied = group->getCpu()->getIed();
	iedName = m_group->getIedName();
	group_object = m_group->getObject();
	init_fun();
}

oe_element_state::~oe_element_state(void)
{
}

void oe_element_state::init_fun()
{
	SRecordset	rs;

	SString sql = SString::toFormat("select max(fun),count(*) from t_oe_element_state where ied_no=%d",group->getIedNo());
	int iRet = DB->Retrieve(sql,rs);
	if (iRet < 0)
		funBase = 100;
	else if (iRet == 0)
		funBase = 100;
	else if (iRet > 0)
	{
		if (rs.GetValue(0,1).toInt() == 0)	//空数据表
			funBase = 100;
		else
		{
			funBase = rs.GetValue(0,0).toInt();
			funBase++;
		}
	}
}

bool oe_element_state::execute(QString & error)
{
	bool b = false;

	elements = group_object->findChildren("FCDA");

	if (elements.count())
	{
		sql = "";

		switch (DB_MASTER_TYPE)
		{
		case DB_MYSQL:
			sql += "insert into t_oe_element_state (ied_no,cpu_no,group_no,entry,fun,inf,name,type,evt_cls,val_type,severity_level,"
				"on_dsc,off_dsc,unknown_desc,inver,current_val,measure_val,soc,usec,mms_path,da_name,details) values ";
			break;

		case DB_ORACLE:
			sql += "insert all ";
			break;

		default:
			return false;
		}

		foreach (element_object,elements)
		{
			b = db_insert(element_object);
			if (!b)
			{
				error = "表t_oe_element_state数据组织错误";
				break;
			}
		}

		if (b)
		{
			switch (DB_MASTER_TYPE)
			{
			case DB_MYSQL:
				sql = sql.left(sql.size()-1);	//去除最后的逗号
				break;

			case DB_ORACLE:
				sql += SString::toFormat("SELECT 1 FROM DUAL");
				break;
			}

			b = DB->Execute(sql);
			if (!b)
			{
				error = "SQL语句执行错误：" + sql;
			}
			else
			{
				if (m_bMDB)
				{
					b = MDB->Execute(sql);
					if (!b)
						error = "SQL语句内存库执行错误：" + sql;
				}
			}
		}
	}
	else
		return true; //无FCDA的情况，返回true，会出现空组情况

	return b;
}

bool oe_element_state::db_insert(XmlObject * object)
{
	bool b = true;

	if (!get_ied_no(object))
		return false;
	if (!get_cpu_no(object))
		return false;
	if (!get_group_no(object))
		return false;
	if (!get_entry(object))
		return false;
	if (!get_name(object))
		return false;
	if (!get_fun(object))
		return false;
	if (!get_inf(object))
		return false;
	if (!get_type(object))
		return false;
	if (!get_evt_cls(object))
		return false;
	if (!get_val_type(object))
		return false;
	if (!get_level(object))
		return false;
	if (!get_on_desc(object))
		return false;
	if (!get_off_desc(object))
		return false;
	if (!get_unknown_desc(object))
		return false;
	if (!get_val(object))
		return false;
	if (!get_measure_val(object))
		return false;
	if (!get_mms_path(object))
		return false;
	if (!get_detail(object))
		return false;

	switch (DB_MASTER_TYPE)
	{
	case DB_MYSQL:
		sql += SString::toFormat("(%d,%d,%d,%d,%d,%d,'%s',%d,%d,%d,%d,'%s','%s','%s',%d,%d,'%s',%d,%d,'%s','%s','%s'),",
			ied_no,
			cpu_no,
			group_no,
			entry,
			fun,
			inf,
			name.data(),
			type,
			evt_cls,
			val_type,
			level,
			on_desc.data(),
			off_desc.data(),
			unknown_desc.data(),
			inver,
			val,
			measure_val.data(),
			soc,
			usec,
			mms_path.data(),
			da_name.data(),
			detail.data());
		break;

	case DB_ORACLE:
		sql += SString::toFormat("into t_oe_element_state (ied_no,cpu_no,group_no,entry,fun,inf,name,type,evt_cls,val_type,severity_level,"
			"on_dsc,off_dsc,unknown_desc,inver,current_val,measure_val,soc,usec,mms_path,da_name,details) values "
			"(%d,%d,%d,%d,%d,%d,'%s',%d,%d,%d,%d,'%s','%s','%s',%d,%d,'%s',%d,%d,'%s','%s','%s') ",
			ied_no,
			cpu_no,
			group_no,
			entry,
			fun,
			inf,
			name.data(),
			type,
			evt_cls,
			val_type,
			level,
			on_desc.data(),
			off_desc.data(),
			unknown_desc.data(),
			inver,
			val,
			measure_val.data(),
			soc,
			usec,
			mms_path.data(),
			da_name.data(),
			detail.data());
		break;
	}

	return b;
}

bool oe_element_state::get_ied_no(XmlObject * object)
{
	S_UNUSED(object);

	bool b = true;

	ied_no = group->getIedNo();

	return b;
}

bool oe_element_state::get_cpu_no(XmlObject * object)
{
	S_UNUSED(object);

	bool b = true;

	cpu_no = group->getCpuNo();

	return b;
}

bool oe_element_state::get_group_no(XmlObject * object)
{
	S_UNUSED(object);

	bool b = true;
	
	group_no = group->getGroupNo();

	return b;
}

bool oe_element_state::get_entry(XmlObject * object)
{
	S_UNUSED(object);

	bool b = true;
	
	entry++;

	return b;
}

bool oe_element_state::get_name(XmlObject * object)
{
	bool b = true;

	doi_Object = getDoiObjectByFcda(object);
	if (doi_Object)
	{
		name = doi_Object->attrib("desc").toStdString();
		if (name.isEmpty())	//如果DOI的desc为空或没有，找DAI中的du
		{
			XmlObject * dai_Object = doi_Object->findChildDeep("DAI","name","dU");
			if (dai_Object)
			{
				XmlObject * val_Object = dai_Object->findChild("Val");
				if (val_Object)
					name = dai_Object->attrib("value").toStdString();
			}
		}
	}

	if (name.isEmpty())
		name = "未知";

	return b;
}

bool oe_element_state::get_fun(XmlObject * object)
{
	S_UNUSED(object);

	bool b = true;
	
	fun = funBase + (entry / 200);

	return b;
}

bool oe_element_state::get_inf(XmlObject * object)
{
	S_UNUSED(object);

	bool b = true;

	if ((entry / 200) > 0)
		inf = (entry % 200) + 1;
	else
		inf = entry % 200;

	return b;
}

bool oe_element_state::get_type(XmlObject * object)
{
	S_UNUSED(object);

	bool b = true;

	type = group->getGroupType();

	return b;
}

bool oe_element_state::get_evt_cls(XmlObject * object)
{
	S_UNUSED(object);

	evt_cls = 0;
	QList<element_type_t> list = sclImport->elementTypeList;
	if (!list.count())
		return true;

	for (int i = 0; i < list.count(); i++)
	{
		if (list.at(i).wildcard.isEmpty())
			continue;

		int ied_type = ied->getIedType();
		if (list.at(i).device_type_no == 0 || (ied_type != 0 && ied_type == list.at(i).device_type_no))
		{
			QRegExp rx(list.at(i).wildcard);
			if (rx.indexIn(name.data()) >= 0)
			{
				evt_cls = list.at(i).type_no;
				break;
			}
		}
	}

	return true;
}

bool oe_element_state::get_val_type(XmlObject * object)
{
	bool b = true;

	QString fc = object->attrib("fc");
	if (fc == "MX")
	{
		val_type = STATE_MEASURE;
	}
	else
	{
		val_type = GetSPS_DPS(object);
	}

	return b;
}

bool oe_element_state::get_level(XmlObject * object)
{
	S_UNUSED(object);

	bool b = true;

	if (evt_cls == 0)
	{
		level = 0;
	}
	else
	{
		SRecordset	rs;
		SString sql = SString::toFormat("select severity_level from t_oe_element_type where type_no=%d",evt_cls);
		int iRet = DB->Retrieve(sql,rs);
		if (iRet <= 0)
			level = 0;
		else if (iRet > 0)
			level = rs.GetValue(0,0).toInt();
	}

	return b;
}

bool oe_element_state::get_on_desc(XmlObject * object)
{
	S_UNUSED(object);

	bool b = true;

	on_desc = "合";

	return b;
}

bool oe_element_state::get_off_desc(XmlObject * object)
{
	S_UNUSED(object);

	bool b = true;

	off_desc = "分";

	return b;
}

bool oe_element_state::get_unknown_desc(XmlObject * object)
{
	S_UNUSED(object);

	unknown_desc = "未知";
	return true;
	/*
	SRecordset	rs;
	SString		sql;

	unknown_desc = "";
	int sub_no = ied->getSubNo();
	sql.sprintf("select ied_no,name,mms_path from t_oe_ied where sub_no=%d and name!='AGENT_MONI' and mms_path!='AGENT_MONI'",sub_no);
	int cnt = DB->Retrieve(sql,rs);
	if (cnt > 0)
	{
		QString element_name = name;
		QString ied_name;
		QString ied_mms;
		for (int i = 0; i < cnt; i++)
		{
			ied_name = rs.GetValue(i,1).data();
			ied_mms = rs.GetValue(i,2).data();
			if (element_name.contains(ied_name) || element_name.contains(ied_mms))
			{
				unknown_desc = SString::toFormat("%d",rs.GetValue(i,0).toInt());
				break;
			}
		}
	}

	if (unknown_desc.isEmpty())
		unknown_desc = "未知";
	
	return true;
	*/
}

bool oe_element_state::get_inver(XmlObject * object)
{
	S_UNUSED(object);

	bool b = true;

	inver = 0;	//不取反

	return b;
}

bool oe_element_state::get_val(XmlObject * object)
{
	S_UNUSED(object);

	bool b = true;

	val = 3; //无论单点还是双点，3均为无效值

	return b;
}

bool oe_element_state::get_measure_val(XmlObject * object)
{
	S_UNUSED(object);

	bool b = true;

	measure_val = "0.0";

	return b;
}

bool oe_element_state::get_soc(XmlObject * object)
{
	S_UNUSED(object);

	bool b = true;

	soc = 0;

	return b;
}

bool oe_element_state::get_usec(XmlObject * object)
{
	S_UNUSED(object);

	bool b = true;

	usec = 0;

	return b;
}

bool oe_element_state::get_mms_path(XmlObject * object)
{
	bool b = true;

	QString mms = iedName + group->getCpu()->getCpuMmsPath()+"/";

	mms += object->attrib("prefix") + 
		object->attrib("lnClass") +
		object->attrib("lnInst") +
		"$" +
		object->attrib("fc") +
		"$" +
		object->attrib("doName");

	if (!object->attrib("daName").isEmpty())
	{
		//mms += "$" + object->attrib("daName");
		da_name = object->attrib("daName").toStdString();
		da_name = da_name.replace(".","$");
	}
	else if (object->attrib("fc") == "ST")
	{
		//mms += "$stVal";
		da_name = "$stVal";
	}

	mms = mms.replace(".","$");

	mms_path = mms.toStdString();

	return b;
}

bool oe_element_state::get_detail(XmlObject * object)
{
	S_UNUSED(object);

	detail = "";

	return true;
}

XmlObject * oe_element_state::getDoiObjectByFcda(XmlObject * obj)
{
	XmlObject * result = 0;
	XmlObject * object = obj;
	QStringList	attrNameList;
	QStringList	valueList;

	while(object)
	{
		if (object->name == "IED")	//回溯定位IED
		{
			result = object->findChildDeep("LDevice","inst",obj->attributes["ldInst"]);
			if (result)
			{
				XmlObject * temp = result;
				attrNameList.append("lnClass");
				attrNameList.append("inst");
				attrNameList.append("prefix");
				valueList.append(obj->attributes["lnClass"]);
				valueList.append(obj->attributes["lnInst"]);
				valueList.append(obj->attributes["prefix"]);
				result = temp->findChildDeepWithAttribs("LN0",attrNameList,valueList);
				if (!result)	//如LN0没有查找LN
					result = temp->findChildDeepWithAttribs("LN",attrNameList,valueList);

				if (result)
				{
					QStringList doiName = obj->attributes["doName"].split(".");	//有"."表示doName中有DOI和SDI
					for (int i = 0; i < doiName.count(); i++)
					{
						if (i == 0)
							result = result->findChildDeep("DOI","name",doiName.at(i));
						else if (i == 1)
						{
							if (result)
								result = result->findChildDeep("SDI","name",doiName.at(i));
						}
					}
				}
			}
			break;
		}

		object = object->parent;
	}

	return result;
}

int oe_element_state::GetSPS_DPS(XmlObject *object)
{
	QString err;

	XmlObject *ld_object = group->getCpu()->getObject();
	if (!ld_object)
	{
		err = QString("%1").arg("获取LDevice Object异常");
		return STATE_SPS;
	}

	QStringList	attrNameList;
	QStringList	valueList;
	attrNameList.append("lnClass");
	attrNameList.append("inst");
	attrNameList.append("prefix");
	valueList.append(object->attrib("lnClass"));
	valueList.append(object->attrib("lnInst"));
	valueList.append(object->attrib("prefix"));
	XmlObject *ln_object = ld_object->findChildDeepWithAttribs("LN0",attrNameList,valueList);
	if (!ln_object)
	{
		ln_object = ld_object->findChildDeepWithAttribs("LN",attrNameList,valueList);
		if (!ln_object)
		{
			err = QString("%1").arg("获取LN Object异常");
			return STATE_SPS;
		}
	}

	XmlObject *dataTypeTemplates = document.at(0)->findChild("DataTypeTemplates");
	if (!dataTypeTemplates)
	{
		err = QString("%1").arg("获取DataTypeTemplates Object异常");
		return STATE_SPS;
	}

	attrNameList.clear();
	attrNameList.append("id");
	attrNameList.append("lnClass");
	valueList.clear();
	valueList.append(ln_object->attributes["lnType"]);
	valueList.append(ln_object->attributes["lnClass"]);
	XmlObject *lnType_object = dataTypeTemplates->findChildDeepWithAttribs("LNodeType",attrNameList,valueList);
	if (!lnType_object)
	{
		err = QString("%1").arg("获取LNodeType Object异常");
		return STATE_SPS;
	}

	XmlObject *DO_object = lnType_object->findChild("DO","name",object->attrib("doName"));
	if (!DO_object)
	{
		err = QString("%1").arg("获取DO Object异常");
		return STATE_SPS;
	}

	XmlObject *DOType_object = dataTypeTemplates->findChild("DOType","id",DO_object->attrib("type"));
	if (!DOType_object)
	{
		err = QString("%1").arg("获取DOType Object异常");
		return STATE_SPS;
	}

	QString cdc = DOType_object->attrib("cdc");
	if (cdc == "SPS" || cdc == "SPC")
		return STATE_SPS;
	else if (cdc == "DPS" || cdc == "DPC")
		return STATE_DPS;

	return STATE_SPS;
}
