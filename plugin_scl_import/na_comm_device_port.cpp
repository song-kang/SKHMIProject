#include "na_comm_device_port.h"
#include "na_sub_network.h"
#include "oe_ied.h"
#include "view_plugin_scl_import.h"

na_comm_device_port::na_comm_device_port(QList<XmlObject*> list,na_sub_network *m_sub_network,oe_ied * m_ied)
{
	net_id = 0;
	dev_cls = 0;
	dev_id = 0;
	port_id = 0;
	port_name = "";
	port_type = 0;
	if_type = 0;
	ip_addr = "";
	port_state = 0;
	state_confirm = 1;
	port_flow = 0;

	document = list;
	ied = m_ied;
	sub_network = m_sub_network;
	sub_network_object = m_sub_network->getObject();
}

na_comm_device_port::~na_comm_device_port(void)
{
}

bool na_comm_device_port::execute(QString & error)
{
	bool b = true;

	//ports += sub_network_object->findChildrenDeep("ConnectedAP");
	//if (ports.count())
	//{
	//	foreach (port_object,ports)
	//	{
	port_object = sub_network_object->findChildDeep("ConnectedAP","iedName",ied->getIedMmsPath());
	if (port_object)
	{
		switch (sub_network->getNetType())
		{
		case 1:	//SV子网
			b = smv_db_replace(port_object,error);
			break;
		case 2:	//GOOSE子网
			b = goose_db_replace(port_object,error);
			break;
		case 3:	//MMS子网
			b = mms_db_replace(port_object,error);
			break;
		}

		//b = db_exits(port_object);
		//if (b)
		//	b = db_update(port_object,error);
		//else
		//	b = db_insert(port_object,error);
	}
			
			//if (!b)
			//	break;
	//	}
	//}

	return b;
}

bool na_comm_device_port::db_delete(int cls, int no)
{
	SString sql = SString::toFormat("DELETE FROM t_na_comm_device_port WHERE dev_cls=%d and dev_id=%d",cls,no);
	bool b = DB->Execute(sql);
	if (m_bMDB)
		b = MDB->Execute(sql);

	return b;
}

bool na_comm_device_port::db_exits(XmlObject * object)
{
	bool		b = true;
	SRecordset	rs;
	QString error;

	if (!get_net_id(object))
		return false;
	if (!get_dev_cls(object))
		return false;
	if (!get_dev_id(object,error))
		return false;

	SString sql = SString::toFormat("SELECT COUNT(*) FROM t_na_comm_device_port WHERE net_id=%d and dev_cls=%d and dev_id=%d",
		net_id,
		dev_cls,
		dev_id);

	int iRet = DB->Retrieve(sql,rs);
	if (iRet <= 0)
	{
		error = "SQL语句执行错误：" + sql;
		b = false;
	}
	else if (iRet > 0)
	{
		int count = rs.GetValue(0,0).toInt();
		if (count > 0)
			b = true;
		else
			b = false;
	}

	return b;
}

bool na_comm_device_port::db_update(XmlObject * object,QString & error)
{
	bool b = true;
	SString sql = "";

	if (!get_net_id(object))
		return false;
	if (!get_dev_cls(object))
		return false;
	if (!get_dev_id(object,error))
		return false;

	switch (sub_network->getNetType())
	{
	case 1:	//SV子网
	case 2:	//GOOSE子网
		if (!get_mac_addr(object))
			return false;
		sql = SString::toFormat("UPDATE t_na_comm_device_port SET mac_addr='%s' WHERE net_id=%d and dev_cls=%d and dev_id=%d",
			mac_addr.data(),
			net_id,
			dev_cls,
			dev_id);
		break;
		break;
	case 3:	//MMS子网
		if (!get_ip_addr(object))
			return false;
		sql = SString::toFormat("UPDATE t_na_comm_device_port SET ip_addr='%s' WHERE net_id=%d and dev_cls=%d and dev_id=%d",
			ip_addr.data(),
			net_id,
			dev_cls,
			dev_id);
		break;
	}
	
	if (!sql.isEmpty())
	{
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
	}

	return b;
}

bool na_comm_device_port::smv_db_replace(XmlObject * object,QString & error)
{
	bool b = true;
	XmlObject	*smvObject;
	QList<XmlObject*> smvs = object->findChildrenDeep("SMV");
	if (smvs.count())
	{
		foreach (smvObject,smvs)
		{
			b = db_replace(smvObject,error);
			break;	//只取第一个
			//if (!b)
			//	break;
		}
	}

	return b;
}

bool na_comm_device_port::goose_db_replace(XmlObject * object,QString & error)
{
	bool b = true;
	XmlObject	*gseObject;
	QList<XmlObject*> gses = object->findChildrenDeep("GSE");
	if (gses.count())
	{
		foreach (gseObject,gses)
		{
			b = db_replace(gseObject,error);
			break;	//只取第一个
			//if (!b)
			//	break;
		}
	}

	return b;
}

bool na_comm_device_port::mms_db_replace(XmlObject * object,QString & error)
{
	bool b = true;
	XmlObject	*mmsObject;
	QList<XmlObject*> mmss = object->findChildrenDeep("Address");
	if (mmss.count())
	{
		foreach (mmsObject,mmss)
		{
			b = db_replace(mmsObject,error);
			break;	//只取第一个
			//if (!b)
			//	break;
		}
	}
	
	return b;
}

bool na_comm_device_port::db_replace(XmlObject * object,QString & error)
{
	bool b = true;
	SString sql;

	if (!get_net_id(object))
		return false;
	if (!get_dev_cls(object))
		return false;
	if (!get_dev_id(object,error))
		return false;
	if (!get_port_id(object,error))
		return false;
	if (!get_port_name(object))
		return false;
	if (!get_port_type(object))
		return false;
	if (!get_if_type(object))
		return false;
	if (!get_ip_addr(object))
		return false;
	if (!get_mac_addr(object))
		return false;
	if (!get_port_state(object))
		return false;
	if (!get_state_confirm(object))
		return false;
	if (!get_port_flow(object))
		return false;

	sql = SString::toFormat("REPLACE INTO t_na_comm_device_port (net_id,dev_cls,dev_id,port_id,port_name,port_type,\
							if_type,ip_addr,mac_addr,port_state,state_confirm,port_flow) VALUES (%d,%d,%d,%d,'%s',%d,\
							%d,'%s','%s',%d,%d,%d)",
							net_id,dev_cls,dev_id,port_id,port_name.data(),port_type,if_type,ip_addr.data(),mac_addr.data(),
							port_state,state_confirm,port_flow);

	b = DB->Execute(sql);
	if (!b)
		error = "SQL语句执行错误：" + sql;

	return b;
}

bool na_comm_device_port::db_insert(XmlObject * object,QString & error)
{
	bool b = true;
	SString sql;

	if (!get_net_id(object))
		return false;
	if (!get_dev_cls(object))
		return false;
	if (!get_dev_id(object,error))
		return false;
	if (!get_port_id(object,error))
		return false;
	if (!get_port_name(object))
		return false;
	if (!get_port_type(object))
		return false;
	if (!get_if_type(object))
		return false;
	if (!get_ip_addr(object))
		return false;
	if (!get_mac_addr(object))
		return false;
	if (!get_port_state(object))
		return false;
	if (!get_state_confirm(object))
		return false;
	if (!get_port_flow(object))
		return false;

	sql = SString::toFormat("INSERT INTO t_na_comm_device_port (net_id,dev_cls,dev_id,port_id,port_name,port_type,\
							if_type,ip_addr,mac_addr,port_state,state_confirm,port_flow) VALUES (%d,%d,%d,%d,'%s',%d,\
							%d,'%s','%s',%d,%d,%d)",
							net_id,dev_cls,dev_id,port_id,port_name.data(),port_type,if_type,ip_addr.data(),mac_addr.data(),
							port_state,state_confirm,port_flow);

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

bool na_comm_device_port::get_net_id(XmlObject * object)
{
	S_UNUSED(object);

	bool b = true;

	net_id = sub_network->getNetId();

	return b;
}

bool na_comm_device_port::get_dev_cls(XmlObject * object)
{
	S_UNUSED(object);

	bool b = true;

	dev_cls = 1;	//1表示IED设备端口

	return b;
}

bool na_comm_device_port::get_dev_id(XmlObject * object,QString & error)
{
	S_UNUSED(object);
	S_UNUSED(error);

	bool		b = true;

	dev_id = ied->getIedNo();

	return b;
}

bool na_comm_device_port::get_port_id(XmlObject * object,QString & error)
{
	S_UNUSED(object);

	bool		b = true;
	SRecordset	rs;

	SString sql = SString::toFormat("SELECT MAX(port_id) FROM t_na_comm_device_port where dev_cls=1 and dev_id=%d",dev_id);
	int iRet = DB->Retrieve(sql,rs);
	if (iRet < 0)
	{
		error = "SQL语句执行错误：" + sql;
		b = false;
	}
	else if (iRet == 0)
	{
		port_id = 1;
		b = true;
	}
	else if (iRet > 0)
	{
		port_id = rs.GetValue(0,0).toInt();
		port_id++;
		b = true;
	}

	return b;
}

bool na_comm_device_port::get_port_name(XmlObject * object)
{
	S_UNUSED(object);

	bool b = true;

	QString n = QString("电口%1").arg(port_id);
	port_name = n.toStdString();

	return b;
}

bool na_comm_device_port::get_port_type(XmlObject * object)
{
	S_UNUSED(object);

	bool b = true;

	port_type = 1;	//1表示通信端口

	return b;
}

bool na_comm_device_port::get_if_type(XmlObject * object)
{
	S_UNUSED(object);

	bool b = true;

	if_type = 1;	//1表示电以太网

	return b;
}

bool na_comm_device_port::get_ip_addr(XmlObject * object)
{
	bool b = true;

	XmlObject * ip = object->findChildDeep("P","type","IP");
	if (ip)
	{
		ip_addr = ip->text.toStdString();
	}

	return b;
}

bool na_comm_device_port::get_mac_addr(XmlObject * object)
{
	bool b = true;

	XmlObject * mac = object->findChildDeep("P","type","MAC-Address");
	if (mac)
	{
		QString s = mac->text.replace("-",":");
		mac_addr = s.toStdString();
	}

	return b;
}

bool na_comm_device_port::get_port_state(XmlObject * object)
{
	S_UNUSED(object);

	bool b = true;

	port_state = 0;	//0表示未连接

	return b;
}

bool na_comm_device_port::get_state_confirm(XmlObject * object)
{
	S_UNUSED(object);

	bool b = true;

	state_confirm = 1;	//1表示已确认

	return b;
}

bool na_comm_device_port::get_port_flow(XmlObject * object)
{
	S_UNUSED(object);

	bool b = true;

	port_flow = 0;

	return b;
}

