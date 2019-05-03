#include "oe_element_input_extref.h"
#include "view_plugin_scl_import.h"
#include "oe_ied.h"

oe_element_input_extref::oe_element_input_extref(view_plugin_scl_import * scl,QList<XmlObject*> list,oe_ied *ied)
{
	sclImport = scl;
	document = list;
	m_ied = ied;
	int_no = 0; //此项自增加，不可反复初始化
}

oe_element_input_extref::~oe_element_input_extref(void)
{
}

void oe_element_input_extref::initParam()
{
	ied_no = 0;
	int_addr = "";
	int_desc = "";
	ied_name = "";
	do_name = "";
	ln_inst = "";
	ln_class = "";
	da_name = "";
	ld_inst = "";
	prefix = "";
	type = 0;
	link_ied_no = 0;
	link_cpu_no = 0;
	link_group_no = 0;
	link_entry = 0;
	strap = "";
	link_strap = "";
	port = "";
	link_port = "";
}

bool oe_element_input_extref::execute(QString & error,QString &warnText)
{
	SString sql;
	XmlObject *ied_obj = m_ied->getObject();

	switch (DB_MASTER_TYPE)
	{
	case DB_MYSQL:
		sql = "insert ";
		break;

	case DB_ORACLE:
		sql = "insert all ";
		break;

	default:
		return false;
	}
	
	QList<XmlObject*> lstInputs = ied_obj->findChildrenDeep("Inputs");
	foreach (XmlObject *inputs_obj, lstInputs)
	{
		QList<XmlObject*> lstExtRef = inputs_obj->findChildren("ExtRef");
		foreach (XmlObject *extref_obj, lstExtRef)
		{
			initParam();
			if (!insertExtRef(extref_obj,sql,error,warnText))
				return false;

			switch (DB_MASTER_TYPE)
			{
			case DB_MYSQL:
				if (!DB->Execute(sql))
				{
					error = "SQL语句执行错误：" + sql;
					return false;
				}

				if (m_bMDB && !MDB->Execute(sql))
				{
					error = "内存库SQL语句执行错误：" + sql;
					return false;
				}
				sql = "insert ";
				break;

			case DB_ORACLE:
				if (int_no > 0 && !(int_no % 100))
				{
					sql += "SELECT 1 FROM DUAL";
					if (!DB->Execute(sql))
					{
						error = "SQL语句执行错误：" + sql;
						return false;
					}

					if (m_bMDB && !MDB->Execute(sql))
					{
						error = "内存库SQL语句执行错误：" + sql;
						return false;
					}
					sql = "insert all ";
				}
				break;
			}
		}
	}

	if (sql == "insert all " || sql == "insert ") //此IED无ExtRef
		return true;

	sql += "SELECT 1 FROM DUAL";
	if (!DB->Execute(sql))
	{
		error = "SQL语句执行错误：" + sql;
		return false;
	}

	if (m_bMDB && !MDB->Execute(sql))
	{
		error = "内存库SQL语句执行错误：" + sql;
		return false;
	}

	return true;
}

bool oe_element_input_extref::insertExtRef(XmlObject * object,SString &sql,QString & error,QString &warnText)
{
	if (!get_ied_no(object,error,warnText))
		return false;
	if (!get_int_no(object,error,warnText))
		return false;
	if (!get_int_addr(object,error,warnText))
		return false;
	if (!get_int_desc(object,error,warnText))
		return false;
	if (!get_ied_name(object,error,warnText))
		return false;
	if (!get_do_name(object,error,warnText))
		return false;
	if (!get_ln_inst(object,error,warnText))
		return false;
	if (!get_ln_class(object,error,warnText))
		return false;
	if (!get_da_name(object,error,warnText))
		return false;
	if (!get_ld_inst(object,error,warnText))
		return false;
	if (!get_prefix(object,error,warnText))
		return false;
	if (!get_type(object,error,warnText))
		return false;
	if (!get_link_ied_no(object,error,warnText))
		return false;
	if (!get_link_cpu_no(object,error,warnText))
		return false;
	if (!get_link_group_no(object,error,warnText))
		return false;
	if (!get_link_entry(object,error,warnText))
		return false;
	if (!get_strap(object,error,warnText))
		return false;
	if (!get_link_strap(object,error,warnText))
		return false;
	if (!get_port(object,error,warnText))
		return false;
	if (!get_link_port(object,error,warnText))
		return false;

	if (ied_name.isEmpty())
	{
		int_no--;
		return true;
	}

	sql += SString::toFormat("into t_oe_element_input_extref (ied_no,int_no,int_addr,int_desc,ied_name,do_name,ln_inst,ln_class,"
		"da_name,ld_inst,prefix,type,link_ied_no,link_cpu_no,link_group_no,link_entry,strap,link_strap,port,link_port) "
		"values (%d,%d,'%s','%s','%s','%s','%s','%s','%s','%s','%s',%d,%d,%d,%d,%d,'%s','%s','%s','%s') ",
		ied_no,int_no,int_addr.data(),int_desc.data(),ied_name.data(),do_name.data(),ln_inst.data(),ln_class.data(),
		da_name.data(),ld_inst.data(),prefix.data(),type,link_ied_no,link_cpu_no,link_group_no,link_entry,strap.data(),link_strap.data(),port.data(),link_port.data());

	return true;
}

bool oe_element_input_extref::get_ied_no(XmlObject * object,QString & error,QString &warnText)
{
	S_UNUSED(object);
	S_UNUSED(error);
	S_UNUSED(warnText);

	ied_no = m_ied->getIedNo();

	return true;
}

bool oe_element_input_extref::get_int_no(XmlObject * object,QString & error,QString &warnText)
{
	S_UNUSED(object);
	S_UNUSED(error);
	S_UNUSED(warnText);

	int_no++;

	return true;
}

bool oe_element_input_extref::get_int_addr(XmlObject * object,QString & error,QString &warnText)
{
	S_UNUSED(object);
	S_UNUSED(error);
	S_UNUSED(warnText);

	QString addr = object->attrib("intAddr");
	if (addr.split(":").count() > 1)
	{
		int_addr = addr.split(":").at(1).toStdString().data();
	}
	else
	{
		int_addr = object->attrib("intAddr").toStdString().data();
	}

	return true;
}

bool oe_element_input_extref::get_int_desc(XmlObject * object,QString & error,QString &warnText)
{
	S_UNUSED(object);
	S_UNUSED(error);
	S_UNUSED(warnText);

	XmlObject *ied_obj = m_ied->getObject();
	QString intAddr = object->attrib("intAddr");
	if (intAddr.isEmpty())
		return true;
	QStringList l1 = intAddr.split("/");
	if (l1.count() != 2)
		return true;
	QStringList l2 = l1.at(1).split(".");
	if (l2.count() < 2)
		return true;

	QList<XmlObject*> lstLN = ied_obj->findChildrenDeep("LN0");
	lstLN += ied_obj->findChildrenDeep("LN");
	foreach (XmlObject *ln_obj, lstLN)
	{
		QString ln = ln_obj->attrib("prefix") + ln_obj->attrib("lnClass") + ln_obj->attrib("inst");
		if (ln != l2.at(0))
			continue;

		XmlObject *doi_obj = ln_obj->findChildDeep("DOI","name",l2.at(1));
		if (doi_obj)
		{
			int_desc = doi_obj->attrib("desc").toStdString();
			if (int_desc.isEmpty())	//如果DOI的desc为空或没有，找DAI中的du
			{
				XmlObject * dai_Object = doi_obj->findChildDeep("DAI","name","dU");
				if (dai_Object)
				{
					XmlObject * val_Object = dai_Object->findChild("Val");
					if (val_Object)
					{
						int_desc = dai_Object->attrib("value").toStdString();
						if (!int_desc.isEmpty())
							break;
					}
				}
			}
			else
				break;
		}

		//DOI中未找到，在DataTypeTemplates中查找
		XmlObject *temp_obj = document.at(0)->findChild("DataTypeTemplates");
		if (temp_obj)
		{
			XmlObject *lnType_obj = temp_obj->findChild("LNodeType","id",ln_obj->attrib("lnType"));
			if (lnType_obj)
			{
				XmlObject *do_obj = lnType_obj->findChild("DO","name",l2.at(1));
				if (do_obj)
				{
					int_desc = do_obj->attrib("desc").toStdString();
					if (!int_desc.isEmpty())
						break;
				}
			}
		}
	}

	if (!int_desc.isEmpty())
	{
		for (int i = 2; i < l2.count(); i++)
		{
			int_desc += ".";
			int_desc += l2.at(i).toStdString();
		}
	}

	return true;
}

bool oe_element_input_extref::get_ied_name(XmlObject * object,QString & error,QString &warnText)
{
	S_UNUSED(error);
	S_UNUSED(warnText);
	ied_name = object->attrib("iedName").toLocal8Bit().data();

	return true;
}

bool oe_element_input_extref::get_do_name(XmlObject * object,QString & error,QString &warnText)
{
	S_UNUSED(object);
	S_UNUSED(error);
	S_UNUSED(warnText);

	do_name = object->attrib("doName").toLocal8Bit().data();

	return true;
}

bool oe_element_input_extref::get_ln_inst(XmlObject * object,QString & error,QString &warnText)
{
	S_UNUSED(object);
	S_UNUSED(error);
	S_UNUSED(warnText);

	ln_inst = object->attrib("lnInst").toLocal8Bit().data();

	return true;
}

bool oe_element_input_extref::get_ln_class(XmlObject * object,QString & error,QString &warnText)
{
	S_UNUSED(object);
	S_UNUSED(error);
	S_UNUSED(warnText);

	ln_class = object->attrib("lnClass").toLocal8Bit().data();

	return true;
}

bool oe_element_input_extref::get_da_name(XmlObject * object,QString & error,QString &warnText)
{
	S_UNUSED(object);
	S_UNUSED(error);
	S_UNUSED(warnText);

	da_name = object->attrib("daName").toLocal8Bit().data();

	return true;
}

bool oe_element_input_extref::get_ld_inst(XmlObject * object,QString & error,QString &warnText)
{
	S_UNUSED(object);
	S_UNUSED(error);
	S_UNUSED(warnText);

	ld_inst = object->attrib("ldInst").toLocal8Bit().data();

	return true;
}

bool oe_element_input_extref::get_prefix(XmlObject * object,QString & error,QString &warnText)
{
	S_UNUSED(object);
	S_UNUSED(error);
	S_UNUSED(warnText);

	prefix = object->attrib("prefix").toLocal8Bit().data();

	return true;
}

bool oe_element_input_extref::get_type(XmlObject * object,QString & error,QString &warnText)
{
	S_UNUSED(object);
	S_UNUSED(error);
	S_UNUSED(warnText);

	return true;
}

bool oe_element_input_extref::get_link_ied_no(XmlObject * object,QString & error,QString &warnText)
{
	S_UNUSED(object);
	S_UNUSED(error);
	S_UNUSED(warnText);

	return true;
}

bool oe_element_input_extref::get_link_cpu_no(XmlObject * object,QString & error,QString &warnText)
{
	S_UNUSED(object);
	S_UNUSED(error);
	S_UNUSED(warnText);

	return true;
}

bool oe_element_input_extref::get_link_group_no(XmlObject * object,QString & error,QString &warnText)
{
	S_UNUSED(object);
	S_UNUSED(error);
	S_UNUSED(warnText);

	return true;
}

bool oe_element_input_extref::get_link_entry(XmlObject * object,QString & error,QString &warnText)
{
	S_UNUSED(object);
	S_UNUSED(error);
	S_UNUSED(warnText);

	return true;
}

bool oe_element_input_extref::get_strap(XmlObject * object,QString & error,QString &warnText)
{
	S_UNUSED(object);
	S_UNUSED(error);
	S_UNUSED(warnText);

	return true;
}

bool oe_element_input_extref::get_link_strap(XmlObject * object,QString & error,QString &warnText)
{
	S_UNUSED(object);
	S_UNUSED(error);
	S_UNUSED(warnText);

	return true;
}

bool oe_element_input_extref::get_port(XmlObject * object,QString & error,QString &warnText)
{
	S_UNUSED(object);
	S_UNUSED(error);
	S_UNUSED(warnText);

	QString int_addr = object->attrib("intAddr");
	if (int_addr.split(":").count() > 1)
	{
		port = int_addr.split(":").at(0).toStdString().data();
	}

	return true;
}

bool oe_element_input_extref::get_link_port(XmlObject * object,QString & error,QString &warnText)
{
	S_UNUSED(object);
	S_UNUSED(error);
	S_UNUSED(warnText);

	QString p = port.data();
	if (!port.isEmpty())
	{
		QStringList lstPort = p.split("/");
		foreach (QString sPort, lstPort)
		{
			XmlObject * obj = object;
			while(obj)
			{
				if (obj->name == "AccessPoint")
				{
					QStringList	attrNameList;
					QStringList	valueList;
					attrNameList.append("iedName");
					attrNameList.append("apName");
					valueList.append(m_ied->getIedMmsPath());
					valueList.append(obj->attrib("name"));
					XmlObject *connectedapObj = document.at(0)->findChildDeepWithAttribs("ConnectedAP",attrNameList,valueList);
					if (connectedapObj)
					{
						QList<XmlObject*> list = connectedapObj->findChildrenDeep("P","type","Port");
						foreach (XmlObject *o, list)
						{
							if (o->text == sPort)
							{
								XmlObject *rem_port = o->parent->findChild("Private","type","RemDev-Port");
								if (rem_port)
								{
									link_port += rem_port->text.toStdString().data();
									link_port += "/";
								}
							}
						}
					}
					break;
				}
				obj = obj->parent;
			}
		}

		link_port = link_port.left(link_port.count()-1);
	}

	return true;
}
