#include "oe_ied_param.h"
#include "oe_ied.h"
#include "view_plugin_scl_import.h"

oe_ied_param::oe_ied_param(QList<XmlObject*> list,oe_ied *m_ied)
{
	current_val = "";
	mmsAName = "MMS-A";
	mmsBName = "MMS-B";

	document = list;
	ied = m_ied;
	iedName = ied->getIedName();
	ied_object = ied->getObject();
}

oe_ied_param::~oe_ied_param(void)
{
}

bool oe_ied_param::execute(QString & error)
{
	db_insert_net_a_ip(error);
	db_insert_net_b_ip(error);

	return true;
}

bool oe_ied_param::db_insert_net_a_ip(QString & error)
{
	bool b = true;

	if (!get_net_a_ip())
	{
		current_val = "";
		int_val = 0;
	}

	SString sql = SString::toFormat("INSERT INTO t_oe_ied_param (ied_no,param_name,value_type,"
		"current_val,int_val,val_fmt,def_val,dsc) VALUES (%d,'%s',%d,'%s',%d,'%s','%s','%s')",
		ied->getIedNo(),
		"net_a_ip",
		TYPE_STRING,
		current_val.data(),
		int_val,
		"maxlen=16",
		"0.0.0.0",
		"装置A网IP地址");

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

	return b;
}

bool oe_ied_param::db_insert_net_b_ip(QString & error)
{
	bool b = true;

	if (!get_net_b_ip())
	{
		current_val = "";
		int_val = 0;
	}

	SString sql = SString::toFormat("INSERT INTO t_oe_ied_param (ied_no,param_name,value_type,"
		"current_val,int_val,val_fmt,def_val,dsc) VALUES (%d,'%s',%d,'%s',%d,'%s','%s','%s')",
		ied->getIedNo(),
		"net_b_ip",
		TYPE_STRING,
		current_val.data(),
		int_val,
		"maxlen=16",
		"0.0.0.0",
		"装置B网IP地址");

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

	return b;
}

bool oe_ied_param::get_net_a_ip()
{
	if (mmsAName.isEmpty())
		return false;

	XmlObject * object = document.at(0)->findChildDeep("SubNetwork","name",mmsAName);
	if (object)
	{
		object = object->findChildDeep("ConnectedAP","iedName",iedName);
		if (object)
		{
			object = object->findChildDeep("P","type","IP");
			if (object)
			{
				current_val = object->text.toStdString();
				int_val = SSocket::IpStrToIpv4(current_val.data());
			}
			else
			{
				LOGWARN("未在装置[%s]中找到IP。",iedName.data());
				return false;
			}
		}
		else
		{
			LOGWARN("未在 ConnectedAP 中找到装置[%s]。",iedName.data());
			return false;
		}
	}
	else
	{
		LOGWARN("未在 SubNetwork 中找到 [%s] MMS A网",mmsAName.data());
		return false;
	}

	return true;
}

bool oe_ied_param::get_net_b_ip()
{
	if (mmsBName.isEmpty())
		return false;
	
	XmlObject * object = document.at(0)->findChildDeep("SubNetwork","name",mmsBName);
	if (object)
	{
		object = object->findChildDeep("ConnectedAP","iedName",iedName);
		if (object)
		{
			object = object->findChildDeep("P","type","IP");
			if (object)
			{
				current_val = object->text.toStdString();
				int_val = SSocket::IpStrToIpv4(current_val.data());
			}
			else
			{
				LOGWARN("未在装置[%s]中找到IP。",iedName.data());
				return false;
			}
		}
		else
		{
			LOGWARN("未在 ConnectedAP 中找到装置[%s]。",iedName.data());
			return false;
		}
	}
	else
	{
		LOGWARN("未在 SubNetwork 中找到 [%s] MMS B网",mmsBName.data());
		return false;
	}

	return true;
}
