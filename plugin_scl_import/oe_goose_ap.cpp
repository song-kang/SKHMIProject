#include "oe_goose_ap.h"
#include "view_plugin_scl_import.h"
#include "oe_ied.h"
#include "oe_cpu.h"
#include "oe_group.h"
#include "oe_goose_ctrl_block.h"

oe_goose_ap::oe_goose_ap(view_plugin_scl_import * scl,QList<XmlObject*> list,oe_goose_ctrl_block *gcb)
{
	sclImport = scl;
	document = list;
	m_ied = gcb->getIed();
	m_cpu = gcb->getCpu();
	m_group = gcb->getGroup();
	m_gcb = gcb;

	initParam();
}

oe_goose_ap::~oe_goose_ap(void)
{
}

void oe_goose_ap::initParam()
{
	net_id = 0;
	ied_no = 0;
	cb_no = 0;
	sub_no = 0;
	cb_name = "";
	ld_inst = "";
	mac_address = "";
	appid = "";
	vlan_priority = 0;
	vlan_id = 0;
	min_time = 0;
	max_time = 0;
}

bool oe_goose_ap::execute(QString & error,QString &warnText)
{
	bool bFind = false;
	XmlObject *communication_obj = document.at(0)->findChild("Communication");
	if (!communication_obj)
	{
		error = "未发现Communication节点，请检查。";
		return false;
	}

	QList<XmlObject*> lstGse = communication_obj->findChildrenDeep("GSE");
	foreach (XmlObject *gse_obj, lstGse)
	{
		subnetwork_obj = gse_obj->parent->parent;
		connectedap_obj = gse_obj->parent;
		if (!subnetwork_obj)
		{
			error = QString("未发现GSE ldInst[%1] cbName[%2]的上层SubNetwork节点，请检查。")
				.arg(gse_obj->attrib("ldInst")).arg(gse_obj->attrib("cbName"));
			return false;
		}
		if (!connectedap_obj)
		{
			error = QString("未发现GSE ldInst[%1] cbName[%2]的上层ConnectedAP节点，请检查。")
				.arg(gse_obj->attrib("ldInst")).arg(gse_obj->attrib("cbName"));
			return false;
		}

		if (connectedap_obj->attrib("iedName") == m_ied->getIedMmsPath() &&
			gse_obj->attrib("ldInst") == m_cpu->getCpuMmsPath() &&
			gse_obj->attrib("cbName") == m_gcb->getCbName())
		{
			if (!insertGseAP(gse_obj,error,warnText))
				return false;
			bFind = true;
			break;
		}
	}

	if (!bFind)
	{
		warnText += QString("告警：GSEControl无法关联Goose访问点。GCB的iedName=%1,ldInst=%2,cbName=%3")
			.arg(m_ied->getIedMmsPath()).arg(m_cpu->getCpuMmsPath().data()).arg(m_gcb->getCbName().data());
		warnText += "\n";
	}

	return true;
}

bool oe_goose_ap::insertGseAP(XmlObject * object,QString & error,QString &warnText)
{
	if (!get_net_id(object,error,warnText))
		return false;
	if (!get_ied_no(object,error,warnText))
		return false;
	if (!get_cb_no(object,error,warnText))
		return false;
	if (!get_sub_no(object,error,warnText))
		return false;
	if (!get_cb_name(object,error,warnText))
		return false;
	if (!get_ld_inst(object,error,warnText))
		return false;
	if (!get_mac_address(object,error,warnText))
		return false;
	if (!get_appid(object,error,warnText))
		return false;
	if (!get_vlan_priority(object,error,warnText))
		return false;
	if (!get_vlan_id(object,error,warnText))
		return false;
	if (!get_min_time(object,error,warnText))
		return false;
	if (!get_max_time(object,error,warnText))
		return false;

	SString sql;
	sql.sprintf("insert into t_oe_goose_ap (net_id,ied_no,cb_no,sub_no,cb_name,ld_inst,mac_address,appid,"
		"vlan_priority,vlan_id,min_time,max_time) "
		"values (%d,%d,%d,%d,'%s','%s','%s','%s',%d,%d,%d,%d)",
		net_id,ied_no,cb_no,sub_no,cb_name.data(),ld_inst.data(),mac_address.data(),appid.data(),
		vlan_priority,vlan_id,min_time,max_time);
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

bool oe_goose_ap::get_net_id(XmlObject * object,QString & error,QString &warnText)
{
	S_UNUSED(object);
	S_UNUSED(error);
	S_UNUSED(warnText);

	SString sql;
	SRecordset rs;

	sql.sprintf("select net_id from t_na_sub_network where sub_no=%d and net_name='%s'",
		m_ied->getSubNo(),subnetwork_obj->attrib("name").toLocal8Bit().data());
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

bool oe_goose_ap::get_ied_no(XmlObject * object,QString & error,QString &warnText)
{
	S_UNUSED(object);
	S_UNUSED(error);
	S_UNUSED(warnText);

	ied_no = m_ied->getIedNo();

	return true;
}

bool oe_goose_ap::get_cb_no(XmlObject * object,QString & error,QString &warnText)
{
	S_UNUSED(object);
	S_UNUSED(error);
	S_UNUSED(warnText);

	cb_no = m_gcb->getCbNo();

	return true;
}

bool oe_goose_ap::get_sub_no(XmlObject * object,QString & error,QString &warnText)
{
	S_UNUSED(object);
	S_UNUSED(error);
	S_UNUSED(warnText);

	sub_no = m_ied->getSubNo();

	return true;
}

bool oe_goose_ap::get_cb_name(XmlObject * object,QString & error,QString &warnText)
{
	S_UNUSED(object);
	S_UNUSED(error);
	S_UNUSED(warnText);

	cb_name = object->attrib("cbName").toLocal8Bit().data();

	return true;
}

bool oe_goose_ap::get_ld_inst(XmlObject * object,QString & error,QString &warnText)
{
	S_UNUSED(object);
	S_UNUSED(error);
	S_UNUSED(warnText);

	ld_inst = object->attrib("ldInst").toLocal8Bit().data();

	return true;
}

bool oe_goose_ap::get_mac_address(XmlObject * object,QString & error,QString &warnText)
{
	S_UNUSED(object);
	S_UNUSED(error);
	S_UNUSED(warnText);

	XmlObject *p_obj = object->findChildDeep("P","type","MAC-Address");
	if (p_obj)
	{
		mac_address = p_obj->text.toLocal8Bit().data();
	}
	else
	{
		warnText += QString("提示：SubNetwork name=[%1] type=[%2]的ConnectedAP iedName=[%3],apName=[%4]的GSE ldInst=[%5],cbName=[%6]"
			"内无MAC-Address定义，请检查。")
			.arg(subnetwork_obj->attrib("name")).arg(subnetwork_obj->attrib("type"))
			.arg(connectedap_obj->attrib("iedName")).arg(connectedap_obj->attrib("apName"))
			.arg(object->attrib("ldInst")).arg(object->attrib("cbName"));
		warnText += "\n";
	}

	return true;
}

bool oe_goose_ap::get_appid(XmlObject * object,QString & error,QString &warnText)
{
	S_UNUSED(object);
	S_UNUSED(error);
	S_UNUSED(warnText);

	XmlObject *p_obj = object->findChildDeep("P","type","APPID");
	if (p_obj)
	{
		SString zero;
		appid = p_obj->text.toUpper().toLocal8Bit().data();
		if (appid.length() < 4)
		{
			for (int i = 0; i < 4-appid.length(); i++)
				zero += "0";
			appid = zero+appid;
		}
	}
	else
	{
		warnText += QString("提示：SubNetwork name=[%1] type=[%2]的ConnectedAP iedName=[%3],apName=[%4]的GSE ldInst=[%5],cbName=[%6]"
			"内无APPID定义，请检查。")
			.arg(subnetwork_obj->attrib("name")).arg(subnetwork_obj->attrib("type"))
			.arg(connectedap_obj->attrib("iedName")).arg(connectedap_obj->attrib("apName"))
			.arg(object->attrib("ldInst")).arg(object->attrib("cbName"));
		warnText += "\n";
	}

	return true;
}

bool oe_goose_ap::get_vlan_priority(XmlObject * object,QString & error,QString &warnText)
{
	S_UNUSED(object);
	S_UNUSED(error);
	S_UNUSED(warnText);

	XmlObject *p_obj = object->findChildDeep("P","type","VLAN-PRIORITY");
	if (p_obj)
	{
		vlan_priority = p_obj->text.toInt();
	}
	else
	{
		warnText += QString("提示：SubNetwork name=[%1] type=[%2]的ConnectedAP iedName=[%3],apName=[%4]的GSE ldInst=[%5],cbName=[%6]"
			"内无VLAN-PRIORITY定义，请检查。")
			.arg(subnetwork_obj->attrib("name")).arg(subnetwork_obj->attrib("type"))
			.arg(connectedap_obj->attrib("iedName")).arg(connectedap_obj->attrib("apName"))
			.arg(object->attrib("ldInst")).arg(object->attrib("cbName"));
		warnText += "\n";
	}

	return true;
}

bool oe_goose_ap::get_vlan_id(XmlObject * object,QString & error,QString &warnText)
{
	S_UNUSED(object);
	S_UNUSED(error);
	S_UNUSED(warnText);

	XmlObject *p_obj = object->findChildDeep("P","type","VLAN-ID");
	if (p_obj)
	{
		vlan_id = p_obj->text.toInt();
	}
	else
	{
		warnText += QString("提示：SubNetwork name=[%1] type=[%2]的ConnectedAP iedName=[%3],apName=[%4]的GSE ldInst=[%5],cbName=[%6]"
			"内无VLAN-ID定义，请检查。")
			.arg(subnetwork_obj->attrib("name")).arg(subnetwork_obj->attrib("type"))
			.arg(connectedap_obj->attrib("iedName")).arg(connectedap_obj->attrib("apName"))
			.arg(object->attrib("ldInst")).arg(object->attrib("cbName"));
		warnText += "\n";
	}

	return true;
}

bool oe_goose_ap::get_min_time(XmlObject * object,QString & error,QString &warnText)
{
	S_UNUSED(object);
	S_UNUSED(error);
	S_UNUSED(warnText);

	XmlObject *minTime_obj = object->findChild("MinTime");
	if (minTime_obj)
	{
		min_time = minTime_obj->text.toInt();
	}
	else
	{
		warnText += QString("提示：SubNetwork name=[%1] type=[%2]的ConnectedAP iedName=[%3],apName=[%4]的GSE ldInst=[%5],cbName=[%6]"
			"内无MinTime定义，请检查。")
			.arg(subnetwork_obj->attrib("name")).arg(subnetwork_obj->attrib("type"))
			.arg(connectedap_obj->attrib("iedName")).arg(connectedap_obj->attrib("apName"))
			.arg(object->attrib("ldInst")).arg(object->attrib("cbName"));
		warnText += "\n";
	}

	return true;
}

bool oe_goose_ap::get_max_time(XmlObject * object,QString & error,QString &warnText)
{
	S_UNUSED(object);
	S_UNUSED(error);
	S_UNUSED(warnText);

	XmlObject *maxTime_obj = object->findChild("MaxTime");
	if (maxTime_obj)
	{
		max_time = maxTime_obj->text.toInt();
	}
	else
	{
		warnText += QString("提示：SubNetwork name=[%1] type=[%2]的ConnectedAP iedName=[%3],apName=[%4]的GSE ldInst=[%5],cbName=[%6]"
			"内无MaxTime定义，请检查。")
			.arg(subnetwork_obj->attrib("name")).arg(subnetwork_obj->attrib("type"))
			.arg(connectedap_obj->attrib("iedName")).arg(connectedap_obj->attrib("apName"))
			.arg(object->attrib("ldInst")).arg(object->attrib("cbName"));
		warnText += "\n";
	}

	return true;
}
