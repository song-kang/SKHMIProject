#include "oe_element_general.h"
#include "oe_ied.h"
#include "oe_group.h"
#include "oe_cpu.h"
#include "view_plugin_scl_import.h"

oe_element_general::oe_element_general(view_plugin_scl_import * scl,QList<XmlObject*> list,oe_group *m_group)
{
	ied_no = 0;
	cpu_no = 0;
	group_no = 0;
	entry = 0;
	name = "NULL";
	val_type = 0;
	dime = "NULL";
	maxval = 0.0;
	minval = 0.0;;
	stepval = 0.0;;
	precision_n = 0;
	precision_m = 0;
	itemtype = 0;
	factor = 0.0;;
	offset = 0.0;;
	threshold = 0.0;;
	smooth = 0.0;;
	current_val = "NULL";
	reference_val = "NULL";
	mms_path = "NULL";
	isHaveAng = false;

	sclImport = scl;
	document = list;
	group = m_group;
	ied = group->getCpu()->getIed();
	iedName = m_group->getIedName();
	group_object = m_group->getObject();
	dataTypeTemplates = document.at(0)->findChild("DataTypeTemplates");
}

oe_element_general::~oe_element_general(void)
{
}

bool oe_element_general::execute(QString & error)
{
	bool b = false;

	elements = group_object->findChildren("FCDA");

	if (elements.count())
	{
		sql = "";
		sqlMdb = "";
		switch (DB_MASTER_TYPE)
		{
		case DB_MYSQL:
			sql += "insert into t_oe_element_general (ied_no,cpu_no,group_no,entry,name,valtype,dime,maxval,minval,"
			"stepval,precision_n,precision_m,itemtype,factor,offset,threshold,smooth,current_val,reference_val,mms_path,da_name) values ";
			sqlMdb += "insert into t_oe_element_general (ied_no,cpu_no,group_no,entry,valtype,itemtype,"
			"current_val,reference_val,mms_path) values ";
			break;

		case DB_ORACLE:
			sql += "insert all ";
			sqlMdb += "insert all ";
			break;

		default:
			return false;
		}

		foreach (element_object,elements)
		{
			b = db_insert(element_object);
			if (!b)
			{
				error = "表t_oe_element_general数据组织错误";
				break;
			}
		}

		if (b)
		{
			switch (DB_MASTER_TYPE)
			{
			case DB_MYSQL:
				sql = sql.left(sql.size()-1);	//去除最后的逗号
				sqlMdb = sqlMdb.left(sqlMdb.size()-1);	//去除最后的逗号
				break;

			case DB_ORACLE:
				sql += SString::toFormat("SELECT 1 FROM DUAL");
				sqlMdb += SString::toFormat("SELECT 1 FROM DUAL");
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
					b = MDB->Execute(sqlMdb);
					if (!b)
						error = "SQL语句内存库执行错误：" + sqlMdb;
				}
			}
		}
	}
	else
		return true; //无FCDA的情况，返回true，会出现空组情况

	return b;
}

bool oe_element_general::db_insert(XmlObject * object)
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
	if (!get_val_type(object))
		return false;
	if (!get_dime(object))
		return false;
	if (!get_maxval(object))
		return false;
	if (!get_minval(object))
		return false;
	if (!get_stepval(object))
		return false;
	if (!get_precision_n(object))
		return false;
	if (!get_precision_m(object))
		return false;
	if (!get_itemtype(object))
		return false;
	if (!get_factor(object))
		return false;
	if (!get_offset(object))
		return false;
	if (!get_threshold(object))
		return false;
	if (!get_smooth(object))
		return false;
	if (!get_current_val(object))
		return false;
	if (!get_reference_val(object))
		return false;
	if (!get_mms_path(object))
		return false;

	switch (DB_MASTER_TYPE)
	{
	case DB_MYSQL:
		{
			sql += SString::toFormat("(%d,%d,%d,%d,'%s',%d,'%s',%f,%f,%f,%d,%d,%d,%f,%f,%f,%f,'%s','%s','%s','%s'),",
				ied_no,
				cpu_no,
				group_no,
				entry,
				name.data(),
				val_type,
				dime.data(),
				maxval,
				minval,
				stepval,
				precision_n,
				precision_m,
				itemtype,
				factor,
				offset,
				threshold,
				smooth,
				current_val.data(),
				reference_val.data(),
				mms_path.data(),
				da_name.data());

			if (isHaveAng)
			{
				entry++;
				name = name + "-相角";
				//mms_path = mms_path.replace("$mag$","$ang$");
				da_name = da_name.replace("mag$","ang$");
				sql += SString::toFormat("(%d,%d,%d,%d,'%s',%d,'%s',%f,%f,%f,%d,%d,%d,%f,%f,%f,%f,'%s','%s','%s','%s'),",
					ied_no,
					cpu_no,
					group_no,
					entry,
					name.data(),
					val_type,
					dime.data(),
					maxval,
					minval,
					stepval,
					precision_n,
					precision_m,
					itemtype,
					factor,
					offset,
					threshold,
					smooth,
					current_val.data(),
					reference_val.data(),
					mms_path.data(),
					da_name.data());
			}

			sqlMdb += SString::toFormat("(%d,%d,%d,%d,%d,%d,'%s','%s','%s'),",
				ied_no,
				cpu_no,
				group_no,
				entry,
				val_type,
				itemtype,
				current_val.data(),
				reference_val.data(),
				mms_path.data());
		}
		break;

	case DB_ORACLE:
		sql += SString::toFormat("into t_oe_element_general (ied_no,cpu_no,group_no,entry,name,valtype,dime,maxval,minval,"
			"stepval,precision_n,precision_m,itemtype,factor,offset,threshold,smooth,current_val,reference_val,mms_path,da_name) values "
			"(%d,%d,%d,%d,'%s',%d,'%s',%f,%f,%f,%d,%d,%d,%f,%f,%f,%f,'%s','%s','%s') ",
			ied_no,
			cpu_no,
			group_no,
			entry,
			name.data(),
			val_type,
			dime.data(),
			maxval,
			minval,
			stepval,
			precision_n,
			precision_m,
			itemtype,
			factor,
			offset,
			threshold,
			smooth,
			current_val.data(),
			reference_val.data(),
			mms_path.data(),
			da_name.data());

		sqlMdb += SString::toFormat("into t_oe_element_general (ied_no,cpu_no,group_no,entry,valtype,itemtype,"
			"current_val,reference_val,mms_path) values (%d,%d,%d,%d,%d,%d,'%s','%s','%s') ",
			ied_no,
			cpu_no,
			group_no,
			entry,
			val_type,
			itemtype,
			current_val.data(),
			reference_val.data(),
			mms_path.data());
		break;
	}

	return b;
}

bool oe_element_general::get_ied_no(XmlObject * object)
{
	S_UNUSED(object);
	bool b = true;

	ied_no = group->getIedNo();

	return b;
}

bool oe_element_general::get_cpu_no(XmlObject * object)
{
	S_UNUSED(object);
	bool b = true;

	cpu_no = group->getCpuNo();

	return b;
}

bool oe_element_general::get_group_no(XmlObject * object)
{
	S_UNUSED(object);
	bool b = true;

	group_no = group->getGroupNo();

	return b;
}

bool oe_element_general::get_entry(XmlObject * object)
{
	S_UNUSED(object);
	bool b = true;

	entry++;

	return b;
}

bool oe_element_general::get_name(XmlObject * object)
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
	else if (object->attributes["daName"] == "q")
		name += "(品质)";

	return b;
}

bool oe_element_general::get_val_type(XmlObject * object)
{
	bool b = true;

	QStringList	attrNameList;
	QStringList	valueList;

	XmlObject *cpu_object = group->getCpuObject();

	attrNameList.append("lnClass");
	attrNameList.append("inst");
	attrNameList.append("prefix");
	valueList.append(object->attrib("lnClass"));
	valueList.append(object->attrib("lnInst"));
	valueList.append(object->attrib("prefix"));
	//XmlObject *ln_object = cpu_object->findChildDeep("LN0","lnClass",object->attrib("lnClass"));
	XmlObject *ln_object = cpu_object->findChildDeepWithAttribs("LN0",attrNameList,valueList);
	if (!ln_object)
	{
		//ln_object = cpu_object->findChildDeep("LN","lnClass",object->attrib("lnClass"));
		ln_object = cpu_object->findChildDeepWithAttribs("LN",attrNameList,valueList);
		if (!ln_object)
		{
			val_type = 7;	//没查到情况下默认为浮点数
			return true;
		}
	}

	attrNameList.clear();
	attrNameList.append("lnClass");
	attrNameList.append("id");
	valueList.clear();
	valueList.append(ln_object->attributes["lnClass"]);
	valueList.append(ln_object->attributes["lnType"]);
	XmlObject *lntype_object = dataTypeTemplates->findChildDeepWithAttribs("LNodeType",attrNameList,valueList);
	if (!lntype_object)
	{
		val_type = 7;	//没查到情况下默认为浮点数
		return true;
	}

	XmlObject *dotype_object = getDoTypeObject(lntype_object,object->attrib("doName"));
	if (!dotype_object)
	{
		val_type = 7;	//没查到情况下默认为浮点数
		return true;
	}

	QString daName = object->attrib("daName");
	if (daName.isEmpty())	
	{
		daName = "stVal";
	}

	QString bdaName;
	QString bType = getDAbTypeObject(dotype_object,daName,bdaName);
	if (bType.isEmpty())
		bType = getDAbTypeObject(dotype_object,"setVal",bdaName);
	if (bType == "Unicode255" || bType.contains("String"))
	{
		val_type = VAL_TYPE_STRING;
	}
	else if (bType == "BOOLEAN" || bType == "Quality" || bType.contains("INT"))
	{
		val_type = VAL_TYPE_INTEGER;
	}
	else
	{
		val_type = VAL_TYPE_FLOAT;
	}

	return b;
}

bool oe_element_general::get_dime(XmlObject * object)
{
	S_UNUSED(object);
	bool b = true;
	XmlObject *dai_Object;
	XmlObject *val_Object;

	dime = "";
	if (doi_Object)
	{	// 使用Q／GDW 396-2009定义单位规则
		dai_Object = doi_Object->findChildDeep("DAI","name","multiplier");
		if (dai_Object)
		{
			val_Object = dai_Object->findChildDeep("Val");
			if (val_Object)
			{
				QString val = val_Object->attrib("value");
				XmlObject *enum_object = dataTypeTemplates->findChildDeep("EnumType","id","multiplier");
				if (enum_object)
				{
					val_Object = enum_object->findChildDeep("EnumVal","ord",val);
					if (val_Object)
						dime = val_Object->attrib("value").toStdString();
				}
			}
		}

		dai_Object = doi_Object->findChildDeep("DAI","name","SIUnit");
		if (dai_Object)
		{
			val_Object = dai_Object->findChildDeep("Val");
			if (val_Object)
			{
				QString val = val_Object->attrib("value");
				XmlObject *enum_object = dataTypeTemplates->findChildDeep("EnumType","id","SIUnit");
				if (enum_object)
				{
					val_Object = enum_object->findChildDeep("EnumVal","ord",val);
					if (val_Object)
						dime += val_Object->attrib("value").toStdString();
				}
			}
		}
	}

	return b;
}

bool oe_element_general::get_maxval(XmlObject * object)
{
	S_UNUSED(object);
	bool b = true;
	XmlObject *dai_Object;
	XmlObject *val_Object;

	if (doi_Object)
	{
		dai_Object = doi_Object->findChildDeep("SDI","name","max");
		if (dai_Object)
		{
			val_Object = dai_Object->findChildDeep("Val");
			if (val_Object)
			{
				maxval = dai_Object->attrib("value").toFloat();
				return true;
			}
		}

		dai_Object = doi_Object->findChildDeep("SDI","name","maxVal");
		if (dai_Object)
		{
			val_Object = dai_Object->findChildDeep("Val");
			if (val_Object)
			{
				maxval = dai_Object->attrib("value").toFloat();
				return true;
			}
		}
	}

	maxval = 65535.0;

	return b;
}

bool oe_element_general::get_minval(XmlObject * object)
{
	S_UNUSED(object);
	bool b = true;
	XmlObject *dai_Object;
	XmlObject *val_Object;

	if (doi_Object)
	{
		dai_Object = doi_Object->findChildDeep("SDI","name","min");
		if (dai_Object)
		{
			val_Object = dai_Object->findChildDeep("Val");
			if (val_Object)
			{
				minval = dai_Object->attrib("value").toFloat();
				return true;
			}
		}

		dai_Object = doi_Object->findChildDeep("SDI","name","minVal");
		if (dai_Object)
		{
			val_Object = dai_Object->findChildDeep("Val");
			if (val_Object)
			{
				minval = dai_Object->attrib("value").toFloat();
				return true;
			}
		}
	}

	minval = 0.0;

	return b;
}

bool oe_element_general::get_stepval(XmlObject * object)
{
	S_UNUSED(object);
	bool b = true;
	XmlObject *dai_Object;
	XmlObject *val_Object;

	if (doi_Object)
	{
		dai_Object = doi_Object->findChildDeep("SDI","name","stepSize");
		if (dai_Object)
		{
			val_Object = dai_Object->findChildDeep("Val");
			if (val_Object)
			{
				stepval = dai_Object->attrib("value").toFloat();
				return true;
			}
		}
	}

	minval = 0.0;

	return b;
}

bool oe_element_general::get_precision_n(XmlObject * object)
{
	S_UNUSED(object);
	bool b = true;

	precision_n = 6;

	return b;
}

bool oe_element_general::get_precision_m(XmlObject * object)
{
	S_UNUSED(object);
	bool b = true;

	precision_m = 3;

	return b;
}

bool oe_element_general::get_itemtype(XmlObject * object)
{
	S_UNUSED(object);
	itemtype = 0;
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
				itemtype = list.at(i).type_no;
				break;
			}
		}
	}

	return true;
}

bool oe_element_general::get_factor(XmlObject * object)
{
	S_UNUSED(object);
	bool b = true;

	factor = 1.0;

	return b;
}

bool oe_element_general::get_offset(XmlObject * object)
{
	S_UNUSED(object);
	bool b = true;

	offset = 0.0;

	return b;
}

bool oe_element_general::get_threshold(XmlObject * object)
{
	S_UNUSED(object);
	bool b = true;
	if(group && group->getGroupType() == GROUP_ANALOGUE)
		threshold = 1.0;
	else
		threshold = 0.0;
	return b;
	/*
	SString sql;
	SRecordset rs;

	sql.sprintf("select type from t_oe_group where ied_no=%d and cpu_no=%d and group_no=%d",ied_no,cpu_no,group_no);
	int cnt = DB->Retrieve(sql,rs);
	if(cnt > 0 && rs.GetValue(0,0).toInt() == GROUP_ANALOGUE)
		threshold = 1.0;
	else
		threshold = 0.0;

	return b;
	*/
}

bool oe_element_general::get_smooth(XmlObject * object)
{
	S_UNUSED(object);
	bool b = true;
	if(group && group->getGroupType() == GROUP_ANALOGUE)
		threshold = 1.0;
	else
		threshold = 0.0;
	return b;
	/*
	SString sql;
	SRecordset rs;

	sql.sprintf("select type from t_oe_group where ied_no=%d and cpu_no=%d and group_no=%d",ied_no,cpu_no,group_no);
	int cnt = DB->Retrieve(sql,rs);
	if(cnt > 0 && rs.GetValue(0,0).toInt() == GROUP_ANALOGUE)
		smooth = 1.0;
	else
		smooth = 0.0;

	return b;
	*/
}

bool oe_element_general::get_current_val(XmlObject * object)
{
	S_UNUSED(object);
	bool b = true;

	current_val = "0.0";

	return b;
}

bool oe_element_general:: get_reference_val(XmlObject * object)
{
	S_UNUSED(object);
	bool b = true;

	reference_val = "0.0";

	return b;
}

bool oe_element_general::get_mms_path(XmlObject * object)
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
	else if (object->attrib("fc") == "SG" || object->attrib("fc") == "SP" ||
		object->attrib("fc") == "MX" || object->attrib("fc") == "ST")
	{
		QString err;
		QString daName = GetDaName(object,err);
		if (!daName.isEmpty())
			mms += "$" + daName; //定值类将da放在mms中，不独立出来
		else
			LOGWARN("未找到定值FCDA[%s]的DA，错误原因[%s]。",mms.toLocal8Bit().data(),err.toLocal8Bit().data());
	}
	//else if (object->attrib("fc") == "MX" || object->attrib("fc") == "ST")
	//{
	//	QString err;
	//	QString daName = GetDaNameByFC(object,object->attrib("fc"),err);
	//	if (!daName.isEmpty())
	//	{
	//		//mms += daName;
	//		da_name = daName.toStdString();
	//		da_name = da_name.replace(".","$");
	//	}
	//	else
	//		LOGWARN("未找到定值FCDA[%s]的DA，错误原因[%s]。",mms.toLocal8Bit().data(),err.toLocal8Bit().data());
	//}

	mms = mms.replace(".","$");

	mms_path = mms.toStdString();

	return b;
}

XmlObject * oe_element_general::getDoiObjectByFcda(XmlObject * obj)
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

XmlObject * oe_element_general::getDoTypeObject(XmlObject * lntype_object,QString doName)
{
	QString		type;
	XmlObject * do_object = 0;
	XmlObject * dotype_object = 0;
	XmlObject * sdo_object = 0;
	XmlObject * result = 0;
	QStringList l = doName.split(".");

	for (int i = 0; i < l.count(); i++)
	{
		if (i == 0)
		{
			do_object = lntype_object->findChildDeep("DO","name",l.at(i));	//获取LNodeType下DO
			if (do_object)
			{
				type = do_object->attrib("type");	//获取DO内type
				if (type.isEmpty())
					break;
			}
			else
				break;
		}
		else if (i >= 1)
		{
			dotype_object = dataTypeTemplates->findChildDeep("DOType","id",type);	//获取DataTypeTemplates下DOType
			if (dotype_object)
			{
				sdo_object = dotype_object->findChild("SDO","name",l.at(i));	//获取DOType下SDO
				if (sdo_object)
				{
					type = sdo_object->attrib("type");	//获取SDO内type
					if (type.isEmpty())
						break;
				}
				else
					break;
			}
			else
				break;
		}
	}

	if (!type.isEmpty())
		result = dataTypeTemplates->findChildDeep("DOType","id",type);

	return result;
}

QString oe_element_general::getDAbTypeObject(XmlObject * dotype_object,QString daName,QString &bdaName)
{
	QString		type;
	QString		result;
	XmlObject * da_object = 0;
	XmlObject * datype_object = 0;
	XmlObject * bda_object = 0;
	QStringList l;
	if (daName.contains("."))
		l = daName.split(".");
	else if (daName.contains("$"))
		l = daName.split("$");
	else
		l.append(daName);

	for (int i = 0; i < l.count(); i++)
	{
		if (i == 0)
		{
			da_object = dotype_object->findChildDeep("DA","name",l.at(i));	//获取DOType下DA
			if (da_object)
			{
				type = da_object->attrib("type");	//获取DA内type
				if (type.isEmpty())
				{
					result = da_object->attrib("bType");
					bdaName = da_object->attrib("name");
					break;
				}
			}
			else
				break;
		}
		else if (i >= 1)
		{
			datype_object = dataTypeTemplates->findChildDeep("DAType","id",type);	//获取DataTypeTemplates下DAType
			if (datype_object)
			{
				bda_object = datype_object->findChild("BDA","name",l.at(i));	//获取DAType下BDA
				if (bda_object)
				{
					type = bda_object->attrib("type");	//获取BDA内type
					if (type.isEmpty())
					{
						result = bda_object->attrib("bType");
						bdaName = bda_object->attrib("name");
						break;
					}
				}
				else
					break;
			}
			else
				break;
		}
	}

	return result;
}

QString	oe_element_general::GetDaName(XmlObject * fcda_object,QString &err)
{
	QString name = QString::null;

	XmlObject *ld_object = group->getCpu()->getObject();
	if (!ld_object)
	{
		err = QString("%1").arg("获取LDevice Object异常");
		return QString::null;
	}

	QStringList	attrNameList;
	QStringList	valueList;
	attrNameList.append("lnClass");
	attrNameList.append("inst");
	attrNameList.append("prefix");
	valueList.append(fcda_object->attrib("lnClass"));
	valueList.append(fcda_object->attrib("lnInst"));
	valueList.append(fcda_object->attrib("prefix"));
	XmlObject *ln_object = ld_object->findChildDeepWithAttribs("LN0",attrNameList,valueList);
	if (!ln_object)
	{
		ln_object = ld_object->findChildDeepWithAttribs("LN",attrNameList,valueList);
		if (!ln_object)
		{
			err = QString("%1").arg("获取LN Object异常");
			return QString::null;
		}
	}

	XmlObject *dataTypeTemplates = document.at(0)->findChild("DataTypeTemplates");
	if (!dataTypeTemplates)
	{
		err = QString("%1").arg("获取DataTypeTemplates Object异常");
		return QString::null;
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
		return QString::null;
	}

	XmlObject *DO_object = lnType_object->findChild("DO","name",fcda_object->attrib("doName"));
	if (!DO_object)
	{
		err = QString("%1").arg("获取DO Object异常");
		return QString::null;
	}

	XmlObject *DOType_object = dataTypeTemplates->findChild("DOType","id",DO_object->attrib("type"));
	if (!DOType_object)
	{
		err = QString("%1").arg("获取DOType Object异常");
		return QString::null;
	}

	XmlObject *DA = DOType_object->findChild("DA","fc",fcda_object->attrib("fc"));
	if (!DA)
	{
		err = QString("%1").arg("获取DA Object异常");
		return QString::null;
	}

	name += DA->attrib("name");
	if (DA->attrib("bType") == "Struct")
	{
		XmlObject *DAType_object = dataTypeTemplates->findChild("DAType","id",DA->attrib("type"));
		if (!DAType_object)
		{
			err = QString("%1").arg("获取DAType_object Object异常");
			return QString::null;
		}

		QList<XmlObject*> lst_BDA_object = DAType_object->findChildren("BDA");
		for (int i = 0; i < lst_BDA_object.count(); i++)
		{
			XmlObject *BDA_object = lst_BDA_object.at(i);
			if (BDA_object->attrib("bType") == "Unicode255" ||
				BDA_object->attrib("bType") == "BOOLEAN" ||
				BDA_object->attrib("bType").contains("String") ||
				BDA_object->attrib("bType").contains("INT") ||
				BDA_object->attrib("bType").contains("FLOAT"))
			{
				name += "$" + BDA_object->attrib("name");
			}
		}
	}

	return name;
}

QString	oe_element_general::GetDaNameByFC(XmlObject * fcda_object, QString fc, QString &err)
{
	QString name = QString::null;

	XmlObject *ld_object = group->getCpu()->getObject();
	if (!ld_object)
	{
		err = QString("%1").arg("获取LDevice Object异常");
		return QString::null;
	}

	QStringList	attrNameList;
	QStringList	valueList;
	attrNameList.append("lnClass");
	attrNameList.append("inst");
	attrNameList.append("prefix");
	valueList.append(fcda_object->attrib("lnClass"));
	valueList.append(fcda_object->attrib("lnInst"));
	valueList.append(fcda_object->attrib("prefix"));
	XmlObject *ln_object = ld_object->findChildDeepWithAttribs("LN0",attrNameList,valueList);
	if (!ln_object)
	{
		ln_object = ld_object->findChildDeepWithAttribs("LN",attrNameList,valueList);
		if (!ln_object)
		{
			err = QString("%1").arg("获取LN Object异常");
			return QString::null;
		}
	}

	XmlObject *doObject = NULL;
	QStringList doNameList = fcda_object->attrib("doName").split(".");
	for (int i = 0; i < doNameList.count(); i++)
	{
		QString doName = doNameList.at(i);
		attrNameList.clear();
		attrNameList.append("name");
		valueList.clear();
		valueList.append(doName);
		if (i == 0)
			doObject = ln_object->findChildDeepWithAttribs("DOI",attrNameList,valueList);
		else
			doObject = doObject->findChildDeepWithAttribs("SDI",attrNameList,valueList);

		if (doObject == NULL)
		{
			err = QString("未发现实例化的DO[%1]").arg(fcda_object->attrib("doName"));
			return QString::null;
		}
	}

	XmlObject *daObject = NULL;
	attrNameList.clear();
	attrNameList.append("name");
	valueList.clear();
	if (fc == "MX")
	{
		valueList.append("f");
	}
	else if (fc == "ST")
	{
		valueList.append("stVal");
	}
	daObject = doObject->findChildDeepWithAttribs("DAI",attrNameList,valueList);
	if (daObject == NULL)
	{
		valueList.clear();
		valueList.append("posVal");
		daObject = doObject->findChildDeepWithAttribs("DAI",attrNameList,valueList);
		if (daObject == NULL)
		{
			err = QString("未发现实例化的DA[%1]").arg(fcda_object->attrib("doName"));
			return QString::null;
		}
	}

	name = "$" + daObject->attrib("name");
	XmlObject *parentObj = daObject->parent;
	while(parentObj && parentObj->attrib("name") != doNameList.at(doNameList.count()-1))
	{
		QString tmp = name;
		QString tmp1 = "$" + parentObj->attrib("name");
		name = tmp1 + tmp;

		parentObj = parentObj->parent;
	}

	isHaveAng = false;
	attrNameList.clear();
	attrNameList.append("name");
	valueList.clear();
	valueList.append("ang");
	XmlObject *angObj = doObject->findChildDeepWithAttribs("SDI",attrNameList,valueList);
	if (angObj)
		isHaveAng = true;

	return name;
}
