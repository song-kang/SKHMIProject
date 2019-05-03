#include "oe_smv_ap.h"
#include "view_plugin_scl_import.h"
#include "oe_ied.h"
#include "oe_cpu.h"
#include "oe_group.h"
#include "oe_smv_ctrl_block.h"

oe_smv_ap::oe_smv_ap(view_plugin_scl_import * scl,QList<XmlObject*> list,oe_smv_ctrl_block *scb)
{
	sclImport = scl;
	document = list;
	m_ied = scb->getIed();
	m_cpu = scb->getCpu();
	m_group = scb->getGroup();
	m_scb = scb;

	initParam();
}

oe_smv_ap::~oe_smv_ap(void)
{
}

void oe_smv_ap::initParam()
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
}

bool oe_smv_ap::execute(QString & error,QString &warnText)
{
	bool bFind = false;
	XmlObject *communication_obj = document.at(0)->findChild("Communication");
	if (!communication_obj)
	{
		error = "δ����Communication�ڵ㣬���顣";
		return false;
	}

	QList<XmlObject*> lstSmv = communication_obj->findChildrenDeep("SMV");
	foreach (XmlObject *smv_obj, lstSmv)
	{
		subnetwork_obj = smv_obj->parent->parent;
		connectedap_obj = smv_obj->parent;
		if (!subnetwork_obj)
		{
			error = QString("δ����SMV ldInst[%1] cbName[%2]���ϲ�SubNetwork�ڵ㣬���顣")
				.arg(smv_obj->attrib("ldInst")).arg(smv_obj->attrib("cbName"));
			return false;
		}
		if (!connectedap_obj)
		{
			error = QString("δ����SMV ldInst[%1] cbName[%2]���ϲ�ConnectedAP�ڵ㣬���顣")
				.arg(smv_obj->attrib("ldInst")).arg(smv_obj->attrib("cbName"));
			return false;
		}

		if (connectedap_obj->attrib("iedName") == m_ied->getIedMmsPath() &&
			smv_obj->attrib("ldInst") == m_cpu->getCpuMmsPath() &&
			smv_obj->attrib("cbName") == m_scb->getCbName())
		{
			if (!insertSmvAP(smv_obj,error,warnText))
				return false;
			bFind = true;
			break;
		}
	}

	if (!bFind)
	{
		warnText += QString("�澯��GSEControl�޷�����Goose���ʵ㡣GCB��iedName=%1,ldInst=%2,cbName=%3")
			.arg(m_ied->getIedMmsPath()).arg(m_cpu->getCpuMmsPath().data()).arg(m_scb->getCbName().data());
		warnText += "\n";
	}

	return true;
}

bool oe_smv_ap::insertSmvAP(XmlObject * object,QString & error,QString &warnText)
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

	SString sql;
	sql.sprintf("insert into t_oe_smv_ap (net_id,ied_no,cb_no,sub_no,cb_name,ld_inst,mac_address,appid,vlan_priority,vlan_id) "
		"values (%d,%d,%d,%d,'%s','%s','%s','%s',%d,%d)",
		net_id,ied_no,cb_no,sub_no,cb_name.data(),ld_inst.data(),mac_address.data(),appid.data(),vlan_priority,vlan_id);
	if (!DB->Execute(sql))
	{
		error = "SQL���ִ�д���" + sql;
		return false;
	}

	if (m_bMDB && !MDB->Execute(sql))
	{
		error = "�ڴ��SQL���ִ�д���" + sql;
		return false;
	}

	return true;
}

bool oe_smv_ap::get_net_id(XmlObject * object,QString & error,QString &warnText)
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
		error = "SQL���ִ�д���" + sql;
		return false;
	}
	else if (cnt == 0)
	{
		error = QString("����SMV���ʵ�ʱ�޷������ݿ����ҵ�������������������վ��=%1��������=%2")
			.arg(m_ied->getSubNo()).arg(subnetwork_obj->attrib("name"));
		return false;
	}
	else if (cnt > 0)
	{
		net_id = rs.GetValue(0,0).toInt();
	}

	return true;
}

bool oe_smv_ap::get_ied_no(XmlObject * object,QString & error,QString &warnText)
{
	S_UNUSED(object);
	S_UNUSED(error);
	S_UNUSED(warnText);

	ied_no = m_ied->getIedNo();

	return true;
}

bool oe_smv_ap::get_cb_no(XmlObject * object,QString & error,QString &warnText)
{
	S_UNUSED(object);
	S_UNUSED(error);
	S_UNUSED(warnText);

	cb_no = m_scb->getCbNo();

	return true;
}

bool oe_smv_ap::get_sub_no(XmlObject * object,QString & error,QString &warnText)
{
	S_UNUSED(object);
	S_UNUSED(error);
	S_UNUSED(warnText);

	sub_no = m_ied->getSubNo();

	return true;
}

bool oe_smv_ap::get_cb_name(XmlObject * object,QString & error,QString &warnText)
{
	S_UNUSED(object);
	S_UNUSED(error);
	S_UNUSED(warnText);

	cb_name = object->attrib("cbName").toLocal8Bit().data();

	return true;
}

bool oe_smv_ap::get_ld_inst(XmlObject * object,QString & error,QString &warnText)
{
	S_UNUSED(object);
	S_UNUSED(error);
	S_UNUSED(warnText);

	ld_inst = object->attrib("ldInst").toLocal8Bit().data();

	return true;
}

bool oe_smv_ap::get_mac_address(XmlObject * object,QString & error,QString &warnText)
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
		warnText += QString("��ʾ��SubNetwork name=[%1] type=[%2]��ConnectedAP iedName=[%3],apName=[%4]��SMV ldInst=[%5],cbName=[%6]"
			"����MAC-Address���壬���顣")
			.arg(subnetwork_obj->attrib("name")).arg(subnetwork_obj->attrib("type"))
			.arg(connectedap_obj->attrib("iedName")).arg(connectedap_obj->attrib("apName"))
			.arg(object->attrib("ldInst")).arg(object->attrib("cbName"));
		warnText += "\n";
	}

	return true;
}

bool oe_smv_ap::get_appid(XmlObject * object,QString & error,QString &warnText)
{
	S_UNUSED(object);
	S_UNUSED(error);
	S_UNUSED(warnText);

	XmlObject *p_obj = object->findChildDeep("P","type","APPID");
	if (p_obj)
	{
		appid = p_obj->text.toUpper().toLocal8Bit().data();
	}
	else
	{
		warnText += QString("��ʾ��SubNetwork name=[%1] type=[%2]��ConnectedAP iedName=[%3],apName=[%4]��SMV ldInst=[%5],cbName=[%6]"
			"����APPID���壬���顣")
			.arg(subnetwork_obj->attrib("name")).arg(subnetwork_obj->attrib("type"))
			.arg(connectedap_obj->attrib("iedName")).arg(connectedap_obj->attrib("apName"))
			.arg(object->attrib("ldInst")).arg(object->attrib("cbName"));
		warnText += "\n";
	}

	return true;
}

bool oe_smv_ap::get_vlan_priority(XmlObject * object,QString & error,QString &warnText)
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
		warnText += QString("��ʾ��SubNetwork name=[%1] type=[%2]��ConnectedAP iedName=[%3],apName=[%4]��SMV ldInst=[%5],cbName=[%6]"
			"����VLAN-PRIORITY���壬���顣")
			.arg(subnetwork_obj->attrib("name")).arg(subnetwork_obj->attrib("type"))
			.arg(connectedap_obj->attrib("iedName")).arg(connectedap_obj->attrib("apName"))
			.arg(object->attrib("ldInst")).arg(object->attrib("cbName"));
		warnText += "\n";
	}

	return true;
}

bool oe_smv_ap::get_vlan_id(XmlObject * object,QString & error,QString &warnText)
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
		warnText += QString("��ʾ��SubNetwork name=[%1] type=[%2]��ConnectedAP iedName=[%3],apName=[%4]��SMV ldInst=[%5],cbName=[%6]"
			"����VLAN-ID���壬���顣")
			.arg(subnetwork_obj->attrib("name")).arg(subnetwork_obj->attrib("type"))
			.arg(connectedap_obj->attrib("iedName")).arg(connectedap_obj->attrib("apName"))
			.arg(object->attrib("ldInst")).arg(object->attrib("cbName"));
		warnText += "\n";
	}

	return true;
}
