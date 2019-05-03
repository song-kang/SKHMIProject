#include "oe_comm_system.h"
#include "view_na_zj.h"

oe_comm_system::oe_comm_system(view_na_zj *nam)
{
	comm_id = 0;
	sub_no = 0;
	dev_type = 0;
	com_name = "";
	model = "";
	manufacturer = "";
	mgr_ip = 0;
	mgr_ip2 = 0;
	mgr_ip3 = 0;
	mgr_ip4 = 0;
	mgr_port = 0;
	version = "";
	comm_addr = 0;
	weight = 0.0;
	com_desc = "";
	ips = "";

	pNam = nam;
	document = nam->getDocument();
	substationNo = nam->getSubStationNo();
}

oe_comm_system::~oe_comm_system(void)
{
}

bool oe_comm_system::execute(QString & error)
{
	if (!init_device_type(error))
		return false;

	QList<XmlObject*> lstHost = document.at(0)->findChildrenDeep("host");
	foreach (XmlObject * host_obj, lstHost)
	{
		if (!insertHost(host_obj,error))
			return false;
	}

	return true;
}

bool oe_comm_system::init_device_type(QString & error)
{
	SString sql;
	SRecordset rs;
	
	sql.sprintf("select type_no,dev_cls,score_cls,wildcard from t_oe_device_type");
	int cnt = DB->Retrieve(sql,rs);
	if (cnt < 0)
	{
		error = "SQLÓï¾äÖ´ÐÐ´íÎó£º" + sql;
		return false;
	}
	else if (cnt == 0)
	{
		error = "Êý¾Ý¿âÀàÐÍ·ÖÀà±ít_oe_device_typeÎª¿Õ£¬oe_comm_systemÅäÖÃÊ§°Ü¡£";
		return false;
	}
	else if (cnt > 0)
	{
		lstNaDeviceType.clear();
		for (int i = 0; i < cnt; i++)
		{
			na_device_type_t *ndt = new na_device_type_t;
			ndt->type_no = rs.GetValue(i,0).toInt();
			ndt->dev_cls = rs.GetValue(i,1).toInt();
			ndt->score_cls = rs.GetValue(i,2).toInt();
			ndt->wildcard = rs.GetValue(i,3).data();
			lstNaDeviceType.append(ndt);
		}
	}

	return true;
}

bool oe_comm_system::insertHost(XmlObject *object,QString &error)
{
	if (!get_comm_id(object,error))
		return false;
	if (!get_sub_no(object,error))
		return false;
	if (!get_dev_type(object,error))
		return false;
	if (!get_com_name(object,error))
		return false;
	if (!get_model(object,error))
		return false;
	if (!get_manufacturer(object,error))
		return false;
	if (!get_mgr_ip(object,error))
		return false;
	if (!get_mgr_ip2(object,error))
		return false;
	if (!get_mgr_ip3(object,error))
		return false;
	if (!get_mgr_ip4(object,error))
		return false;
	if (!get_mgr_port(object,error))
		return false;
	if (!get_version(object,error))
		return false;
	if (!get_comm_addr(object,error))
		return false;
	if (!get_weight(object,error))
		return false;
	if (!get_com_desc(object,error))
		return false;
	if (!get_ips(object,error))
		return false;

	bool isExist = false;
	if (!systemIsExist(isExist,error))
		return false;

	SString sql;
	if (isExist)
	{
		sql.sprintf("update t_oe_comm_system set "
			"dev_type=%d,model='%s',manufacturer='%s',version='%s',comm_addr=%d,com_desc='%s',ips='%s' "
			"where sub_no=%d and com_name='%s'",
			dev_type,model.data(),manufacturer.data(),version.data(),comm_addr,com_desc.data(),ips.data(),
			sub_no,com_name.data());
	}
	else
	{
		sql.sprintf("insert into t_oe_comm_system (comm_id,sub_no,dev_type,com_name,model,manufacturer,"
			"mgr_ip,mgr_ip2,mgr_ip3,mgr_ip4,mgr_port,version,comm_addr,weight,com_desc,ips) "
			"values (%d,%d,%d,'%s','%s','%s',%d,%d,%d,%d,%d,'%s',%d,%f,'%s','%s')",
			comm_id,sub_no,dev_type,com_name.data(),model.data(),manufacturer.data(),
			mgr_ip,mgr_ip2,mgr_ip3,mgr_ip4,mgr_port,version.data(),comm_addr,weight,com_desc.data(),ips.data());
	}

	if (!DB->Execute(sql))
	{
		error = "SQLÓï¾äÖ´ÐÐ´íÎó£º" + sql;
		return false;
	}

	if (!MDB->Execute(sql))
	{
		error = "ÄÚ´æ¿âSQLÓï¾äÖ´ÐÐ´íÎó£º" + sql;
		return false;
	}

	return true;
}

bool oe_comm_system::systemIsExist(bool &isExist,QString &error)
{
	SString sql;
	SRecordset rs;

	sql.sprintf("select count(*) from t_oe_comm_system where sub_no=%d and com_name='%s'",sub_no,com_name.data());
	int cnt = DB->Retrieve(sql,rs);
	if (cnt < 0)
	{
		error = "SQLÓï¾äÖ´ÐÐ´íÎó£º" + sql;
		return false;
	}
	else if (cnt > 0)
	{
		int num = rs.GetValue(0,0).toInt();
		if (num == 0)
			isExist = false;
		else
			isExist = true;
	}

	return true;
}

bool oe_comm_system::get_comm_id(XmlObject *object,QString &error)
{
	SString sql;
	SRecordset rs;

	sql.sprintf("select max(comm_id) from t_oe_comm_system");
	int cnt = DB->Retrieve(sql,rs);
	if (cnt < 0)
	{
		error = "SQLÓï¾äÖ´ÐÐ´íÎó£º" + sql;
		return false;
	}
	else if (cnt == 0)
	{
		comm_id = 1;
	}
	else if (cnt > 0)
	{
		comm_id = rs.GetValue(0,0).toInt();
		comm_id++;
	}

	return true;
}

bool oe_comm_system::get_sub_no(XmlObject *object,QString &error)
{
	sub_no = substationNo;

	return true;
}

bool oe_comm_system::get_dev_type(XmlObject *object,QString &error)
{
	dev_type = 0;

	for (int i = 0; i < lstNaDeviceType.count(); i++)
	{
		if (lstNaDeviceType.at(i)->wildcard.isEmpty())
			continue;

		QRegExp rx(lstNaDeviceType.at(i)->wildcard);
		if (rx.indexIn(object->attrib("desc")) >= 0 )
		{
			dev_type = lstNaDeviceType.at(i)->type_no;
			break;
		}
		else if (rx.indexIn(object->attrib("name")) >= 0)
		{
			dev_type = lstNaDeviceType.at(i)->type_no;
			break;
		}
	}

	return true;
}

bool oe_comm_system::get_com_name(XmlObject *object,QString &error)
{
	com_name = object->attrib("name").toStdString().data();

	return true;
}

bool oe_comm_system::get_model(XmlObject *object,QString &error)
{
	model = object->attrib("type").toStdString().data();

	return true;
}

bool oe_comm_system::get_manufacturer(XmlObject *object,QString &error)
{
	manufacturer = object->attrib("manufacturer").toStdString().data();

	return true;
}

bool oe_comm_system::get_mgr_ip(XmlObject *object,QString &error)
{
	mgr_ip = 0;

	return true;
}

bool oe_comm_system::get_mgr_ip2(XmlObject *object,QString &error)
{
	mgr_ip2 = 0;

	return true;
}

bool oe_comm_system::get_mgr_ip3(XmlObject *object,QString &error)
{
	mgr_ip3 = 0;

	return true;
}

bool oe_comm_system::get_mgr_ip4(XmlObject *object,QString &error)
{
	mgr_ip4 = 0;

	return true;
}

bool oe_comm_system::get_mgr_port(XmlObject *object,QString &error)
{
	mgr_port = 0;

	return true;
}

bool oe_comm_system::get_version(XmlObject *object,QString &error)
{
	version = "1.0";

	return true;
}

bool oe_comm_system::get_comm_addr(XmlObject *object,QString &error)
{
	QStringList attrList;
	QStringList valueList;
	attrList.append("name");
	valueList.append(com_name.data());

	XmlObject * rcu_obj = document.at(0)->findChildDeepWithAttribs("rcu",attrList,valueList);
	if (rcu_obj)
	{
		comm_addr = rcu_obj->attrib("addr").toInt();
		return true;
	}

	//´Ë´¦¼æÈÝV1°æ±¾xmlÎÄ¼þ
	QString addr = object->attrib("addr");
	if (!addr.isEmpty())
		comm_addr = addr.toInt();

	return true;
}

bool oe_comm_system::get_weight(XmlObject *object,QString &error)
{
	weight = 0.0;

	return true;
}

bool oe_comm_system::get_com_desc(XmlObject *object,QString &error)
{
	com_desc = object->attrib("desc").toStdString().data();

	return true;
}

bool oe_comm_system::get_ips(XmlObject *object,QString &error)
{
	QString m_ips;
	QList<XmlObject*> lstSubnet = object->findChildren("subnet");
	foreach (XmlObject *subnet_obj, lstSubnet)
	{
		QString ip = subnet_obj->attrib("ip");
		if (!ip.isEmpty())
			m_ips += ip + ",";
	}

	QStringList attrList;
	QStringList valueList;
	attrList.append("name");
	valueList.append(com_name.data());
	lstSubnet.clear();
	XmlObject * rcu_obj = document.at(0)->findChildDeepWithAttribs("rcu",attrList,valueList);
	if (rcu_obj)
	{
		lstSubnet = rcu_obj->findChildren("subnet");
		foreach (XmlObject *subnet_obj, lstSubnet)
		{
			QString ip = subnet_obj->attrib("ip");
			if (!ip.isEmpty())
				m_ips += ip + ",";
		}
	}

	ips = m_ips.toStdString().data();

	return true;
}
