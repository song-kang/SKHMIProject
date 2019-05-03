#include "oe_ied_physconn.h"
#include "view_plugin_scl_import.h"
#include "oe_ied.h"
#include "oe_cpu.h"
#include "oe_group.h"
#include "oe_goose_ctrl_block.h"

oe_ied_physconn::oe_ied_physconn(view_plugin_scl_import * scl,QList<XmlObject*> list,oe_ied *ied)
{
	sclImport = scl;
	document = list;
	m_ied = ied;

	initParam();
}

oe_ied_physconn::~oe_ied_physconn(void)
{
}

void oe_ied_physconn::initParam()
{
	net_id = 0;
	ied_no = 0;
	ap_name = "";
	physconn_type = "";
	port = "";
	plug = "";
	type = "";
	cable = "";
	rem_ied_name = "";
	rem_ied_port = "";
}

bool oe_ied_physconn::execute(QString & error,QString &warnText)
{
	bool bFind = false;
	XmlObject *communication_obj = document.at(0)->findChild("Communication");
	if (!communication_obj)
	{
		error = "未发现Communication节点，请检查。";
		return false;
	}

	QList<XmlObject*> lstSubnetwork = communication_obj->findChildren("SubNetwork");
	foreach (subnetwork_obj, lstSubnetwork)
	{
		QList<XmlObject*> lstConnectedAp = subnetwork_obj->findChildrenDeep("ConnectedAP");
		foreach (XmlObject *connectedap_obj, lstConnectedAp)
		{
			if (connectedap_obj->attrib("iedName") == m_ied->getIedMmsPath())
			{
				QList<XmlObject*> lstPhysConn = connectedap_obj->findChildrenDeep("PhysConn");
				foreach (XmlObject *physconn_obj, lstPhysConn)
				{
					if (!insertPhysconn(physconn_obj,error,warnText))
						return false;
				}
				bFind = true;
			}
		}
	}

	if (!bFind)
	{
		warnText += QString("告警：装置[%1]无法找到物理连接信息。").arg(m_ied->getIedMmsPath());
		warnText += "\n";
	}

	return true;
}

bool oe_ied_physconn::insertPhysconn(XmlObject * object,QString & error,QString &warnText)
{
	if (!get_net_id(object,error,warnText))
		return false;
	if (!get_ied_no(object,error,warnText))
		return false;
	if (!get_ap_name(object,error,warnText))
		return false;
	if (!get_physconn_type(object,error,warnText))
		return false;
	if (!get_port(object,error,warnText))
		return false;
	if (!get_plug(object,error,warnText))
		return false;
	if (!get_type(object,error,warnText))
		return false;
	if (!get_cable(object,error,warnText))
		return false;
	if (!get_rem_ied_name(object,error,warnText))
		return false;
	if (!get_rem_ied_port(object,error,warnText))
		return false;

	SString sql;
	sql.sprintf("insert into t_oe_ied_physconn (net_id,ied_id,physconn_type,ap_name,port,plug,type,cable,rem_ied_name,rem_ied_port) "
		"values (%d,%d,'%s','%s','%s','%s','%s','%s','%s','%s')",
		net_id,ied_no,ap_name.data(),physconn_type.data(),port.data(),plug.data(),type.data(),cable.data(),rem_ied_name.data(),rem_ied_port.data());
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

bool oe_ied_physconn::get_net_id(XmlObject * object,QString & error,QString &warnText)
{
	S_UNUSED(object);
	S_UNUSED(error);
	S_UNUSED(warnText);

	SString sql;
	SRecordset rs;

	sql.sprintf("select net_id from t_na_sub_network where sub_no=%d and net_name='%s'",
		m_ied->getSubNo(),subnetwork_obj->attrib("name").toStdString().data());
	int cnt = DB->Retrieve(sql,rs);
	if (cnt < 0)
	{
		error = "SQL语句执行错误：" + sql;
		return false;
	}
	else if (cnt == 0)
	{
		error = QString("插入Goose访问点时无法在数据库中找到子网，查找条件：厂站号=%1，子网名=%2")
			.arg(m_ied->getSubNo()).arg(subnetwork_obj->attrib("name"));
		return false;
	}
	else if (cnt > 0)
	{
		net_id = rs.GetValue(0,0).toInt();
	}

	return true;
}

bool oe_ied_physconn::get_ied_no(XmlObject * object,QString & error,QString &warnText)
{
	S_UNUSED(object);
	S_UNUSED(error);
	S_UNUSED(warnText);

	ied_no = m_ied->getIedNo();

	return true;
}

bool oe_ied_physconn::get_ap_name(XmlObject * object,QString & error,QString &warnText)
{
	S_UNUSED(object);
	S_UNUSED(error);
	S_UNUSED(warnText);

	if (object->parent)
		ap_name = object->parent->attrib("apName").toStdString().data();

	return true;
}

bool oe_ied_physconn::get_physconn_type(XmlObject * object,QString & error,QString &warnText)
{
	S_UNUSED(object);
	S_UNUSED(error);
	S_UNUSED(warnText);

	physconn_type = object->attrib("type").toStdString().data();

	return true;
}

bool oe_ied_physconn::get_port(XmlObject * object,QString & error,QString &warnText)
{
	S_UNUSED(object);
	S_UNUSED(error);
	S_UNUSED(warnText);

	XmlObject *obj = object->findChildDeep("P","type","Port");
	if (obj)
		port = obj->text.toStdString().data();

	return true;
}

bool oe_ied_physconn::get_plug(XmlObject * object,QString & error,QString &warnText)
{
	S_UNUSED(object);
	S_UNUSED(error);
	S_UNUSED(warnText);

	XmlObject *obj = object->findChildDeep("P","type","Plug");
	if (obj)
		plug = obj->text.toStdString().data();

	return true;
}

bool oe_ied_physconn::get_type(XmlObject * object,QString & error,QString &warnText)
{
	S_UNUSED(object);
	S_UNUSED(error);
	S_UNUSED(warnText);

	XmlObject *obj = object->findChildDeep("P","type","Type");
	if (obj)
		type = obj->text.toStdString().data();

	return true;
}

bool oe_ied_physconn::get_cable(XmlObject * object,QString & error,QString &warnText)
{
	S_UNUSED(object);
	S_UNUSED(error);
	S_UNUSED(warnText);

	XmlObject *obj = object->findChildDeep("P","type","Cable");
	if (obj)
		cable = obj->text.toStdString().data();

	return true;
}

bool oe_ied_physconn::get_rem_ied_name(XmlObject * object,QString & error,QString &warnText)
{
	S_UNUSED(object);
	S_UNUSED(error);
	S_UNUSED(warnText);

	XmlObject *obj = object->findChildDeep("Private","type","RemDev-iedName");
	if (obj)
		rem_ied_name = obj->text.toStdString().data();

	return true;
}

bool oe_ied_physconn::get_rem_ied_port(XmlObject * object,QString & error,QString &warnText)
{
	S_UNUSED(object);
	S_UNUSED(error);
	S_UNUSED(warnText);

	XmlObject *obj = object->findChildDeep("Private","type","RemDev-Port");
	if (obj)
		rem_ied_port = obj->text.toStdString().data();

	return true;
}
