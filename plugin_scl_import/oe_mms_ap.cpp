#include "oe_mms_ap.h"
#include "view_plugin_scl_import.h"
#include "oe_ied.h"
#include <QHostAddress>

oe_mms_ap::oe_mms_ap(view_plugin_scl_import * scl,QList<XmlObject*> list,oe_ied *ied)
{
	sclImport = scl;
	document = list;
	m_ied = ied;

	initParam();
}

oe_mms_ap::~oe_mms_ap(void)
{
}

void oe_mms_ap::initParam()
{
	net_id = 0;
	ied_no = 0;
	sub_no = 0;
	ip_addr = "";
	ip_subnet = "";
	ip_gateway = "";
	osi_ap_title = "";
	osi_ap_invoke = "";
	osi_ae_q = "";
	osi_ae_invoke = "";
	osi_psel = "";
	osi_ssel = "";
	osi_tsel = "";
}

bool oe_mms_ap::execute(QString & error,QString &warnText)
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
		//if (subnetwork_obj->attrib("type") != "8-MMS")
		//if (!subnetwork_obj->attrib("type").contains("MMS") && !subnetwork_obj->attrib("name").contains("MMS"))
		if (subnetwork_obj->attrib("name") != sclImport->mmsNetA_name && 
			subnetwork_obj->attrib("name") != sclImport->mmsNetB_name)
			continue;

		QList<XmlObject*> lstConnectedAp = subnetwork_obj->findChildrenDeep("ConnectedAP");
		foreach (XmlObject *connectedap_obj, lstConnectedAp)
		{
			if (connectedap_obj->attrib("iedName") == m_ied->getIedMmsPath())
			{
				if (!insertMmsAP(connectedap_obj,error,warnText))
					return false;
				bFind = true;
				break;
			}
		}
	}
	
	if (!bFind)
	{
		warnText += QString("告警：装置[%1]无法关联MMS访问点。").arg(m_ied->getIedMmsPath());
		warnText += "\n";
	}

	return true;
}

bool oe_mms_ap::insertMmsAP(XmlObject * object,QString & error,QString &warnText)
{
	if (!get_net_id(object,error,warnText))
		return false;
	if (!get_ied_no(object,error,warnText))
		return false;
	if (!get_sub_no(object,error,warnText))
		return false;
	if (!get_ip_addr(object,error,warnText))
		return false;
	if (!get_ip_subnet(object,error,warnText))
		return false;
	if (!get_ip_gateway(object,error,warnText))
		return false;
	if (!get_osi_ap_title(object,error,warnText))
		return false;
	if (!get_osi_ap_invoke(object,error,warnText))
		return false;
	if (!get_osi_ae_q(object,error,warnText))
		return false;
	if (!get_osi_ae_invoke(object,error,warnText))
		return false;
	if (!get_osi_psel(object,error,warnText))
		return false;
	if (!get_osi_ssel(object,error,warnText))
		return false;
	if (!get_osi_tsel(object,error,warnText))
		return false;

	SString sql;
	sql.sprintf("insert into t_oe_mms_ap (net_id,ied_no,sub_no,ip_addr,ip_subnet,ip_gateway,"
		"osi_ap_title,osi_ap_invoke,osi_ae_q,osi_ae_invoke,osi_psel,osi_ssel,osi_tsel) "
		"values (%d,%d,%d,'%s','%s','%s','%s','%s','%s','%s','%s','%s','%s')",
		net_id,ied_no,sub_no,ip_addr.data(),ip_subnet.data(),ip_gateway.data(),
		osi_ap_title.data(),osi_ap_invoke.data(),osi_ae_q.data(),osi_ae_invoke.data(),osi_psel.data(),osi_ssel.data(),osi_tsel.data());
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

bool oe_mms_ap::get_net_id(XmlObject * object,QString & error,QString &warnText)
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
		error = QString("插入MMS访问点时无法在数据库中找到子网，查找条件：厂站号=%1，子网名=%2")
			.arg(m_ied->getSubNo()).arg(subnetwork_obj->attrib("name"));
		return false;
	}
	else if (cnt > 0)
	{
		net_id = rs.GetValue(0,0).toInt();
	}

	return true;
}

bool oe_mms_ap::get_ied_no(XmlObject * object,QString & error,QString &warnText)
{
	S_UNUSED(object);
	S_UNUSED(error);
	S_UNUSED(warnText);

	ied_no = m_ied->getIedNo();

	return true;
}

bool oe_mms_ap::get_sub_no(XmlObject * object,QString & error,QString &warnText)
{
	S_UNUSED(object);
	S_UNUSED(error);
	S_UNUSED(warnText);

	sub_no = m_ied->getSubNo();

	return true;
}

bool oe_mms_ap::get_ip_addr(XmlObject * object,QString & error,QString &warnText)
{
	S_UNUSED(object);
	S_UNUSED(error);
	S_UNUSED(warnText);

	XmlObject *p_obj = object->findChildDeep("P","type","IP");
	if (p_obj)
	{
		QHostAddress addr;
		addr.setAddress(p_obj->text); //处理ip地址为172.016.123.01此类情况
		QString ip = addr.toString(); //统一成172.16.123.1

		ip_addr = ip.toLocal8Bit().data();
	}
	else
	{
		warnText += QString("提示：SubNetwork name=[%1] type=[%2]的ConnectedAP iedName=[%3],apName=[%4]内无IP定义，请检查。")
			.arg(subnetwork_obj->attrib("name")).arg(subnetwork_obj->attrib("type")).arg(object->attrib("iedName")).arg(object->attrib("apName"));
		warnText += "\n";
	}

	return true;
}

bool oe_mms_ap::get_ip_subnet(XmlObject * object,QString & error,QString &warnText)
{
	S_UNUSED(object);
	S_UNUSED(error);
	S_UNUSED(warnText);

	XmlObject *p_obj = object->findChildDeep("P","type","IP-SUBNET");
	if (p_obj)
	{
		ip_subnet = p_obj->text.toLocal8Bit().data();
	}
	else
	{
		warnText += QString("提示：SubNetwork name=[%1] type=[%2]的ConnectedAP iedName=[%3],apName=[%4]内无IP-SUBNET定义，请检查。")
			.arg(subnetwork_obj->attrib("name")).arg(subnetwork_obj->attrib("type")).arg(object->attrib("iedName")).arg(object->attrib("apName"));
		warnText += "\n";
	}

	return true;
}

bool oe_mms_ap::get_ip_gateway(XmlObject * object,QString & error,QString &warnText)
{
	S_UNUSED(object);
	S_UNUSED(error);
	S_UNUSED(warnText);

	XmlObject *p_obj = object->findChildDeep("P","type","IP-GATEWAY");
	if (p_obj)
	{
		ip_gateway = p_obj->text.toLocal8Bit().data();
	}
	else
	{
		warnText += QString("提示：SubNetwork name=[%1] type=[%2]的ConnectedAP iedName=[%3],apName=[%4]内无IP-GATEWAY定义，请检查。")
			.arg(subnetwork_obj->attrib("name")).arg(subnetwork_obj->attrib("type")).arg(object->attrib("iedName")).arg(object->attrib("apName"));
		warnText += "\n";
	}

	return true;
}

bool oe_mms_ap::get_osi_ap_title(XmlObject * object,QString & error,QString &warnText)
{
	S_UNUSED(object);
	S_UNUSED(error);
	S_UNUSED(warnText);

	XmlObject *p_obj = object->findChildDeep("P","type","OSI-AP-Title");
	if (p_obj)
	{
		osi_ap_title = p_obj->text.toLocal8Bit().data();
	}
	else
	{
		warnText += QString("提示：SubNetwork name=[%1] type=[%2]的ConnectedAP iedName=[%3],apName=[%4]内无OSI-AP-Title定义，请检查。")
			.arg(subnetwork_obj->attrib("name")).arg(subnetwork_obj->attrib("type")).arg(object->attrib("iedName")).arg(object->attrib("apName"));
		warnText += "\n";
	}

	return true;
}

bool oe_mms_ap::get_osi_ap_invoke(XmlObject * object,QString & error,QString &warnText)
{
	S_UNUSED(object);
	S_UNUSED(error);
	S_UNUSED(warnText);

	XmlObject *p_obj = object->findChildDeep("P","type","OSI-AP-Invoke");
	if (p_obj)
	{
		osi_ap_invoke = p_obj->text.toLocal8Bit().data();
	}
	else
	{
		warnText += QString("提示：SubNetwork name=[%1] type=[%2]的ConnectedAP iedName=[%3],apName=[%4]内无OSI-AP-Invoke定义，请检查。")
			.arg(subnetwork_obj->attrib("name")).arg(subnetwork_obj->attrib("type")).arg(object->attrib("iedName")).arg(object->attrib("apName"));
		warnText += "\n";
	}

	return true;
}

bool oe_mms_ap::get_osi_ae_q(XmlObject * object,QString & error,QString &warnText)
{
	S_UNUSED(object);
	S_UNUSED(error);
	S_UNUSED(warnText);

	XmlObject *p_obj = object->findChildDeep("P","type","OSI-AE-Qualifier");
	if (p_obj)
	{
		osi_ae_q = p_obj->text.toLocal8Bit().data();
	}
	else
	{
		warnText += QString("提示：SubNetwork name=[%1] type=[%2]的ConnectedAP iedName=[%3],apName=[%4]内无OSI-AE-Qualifier定义，请检查。")
			.arg(subnetwork_obj->attrib("name")).arg(subnetwork_obj->attrib("type")).arg(object->attrib("iedName")).arg(object->attrib("apName"));
		warnText += "\n";
	}

	return true;
}

bool oe_mms_ap::get_osi_ae_invoke(XmlObject * object,QString & error,QString &warnText)
{
	S_UNUSED(object);
	S_UNUSED(error);
	S_UNUSED(warnText);

	XmlObject *p_obj = object->findChildDeep("P","type","OSI-AE-Invoke");
	if (p_obj)
	{
		osi_ae_invoke = p_obj->text.toLocal8Bit().data();
	}
	else
	{
		warnText += QString("提示：SubNetwork name=[%1] type=[%2]的ConnectedAP iedName=[%3],apName=[%4]内无OSI-AE-Invoke定义，请检查。")
			.arg(subnetwork_obj->attrib("name")).arg(subnetwork_obj->attrib("type")).arg(object->attrib("iedName")).arg(object->attrib("apName"));
		warnText += "\n";
	}

	return true;
}

bool oe_mms_ap::get_osi_psel(XmlObject * object,QString & error,QString &warnText)
{
	S_UNUSED(object);
	S_UNUSED(error);
	S_UNUSED(warnText);

	XmlObject *p_obj = object->findChildDeep("P","type","OSI-PSEL");
	if (p_obj)
	{
		osi_psel = p_obj->text.toLocal8Bit().data();
	}
	else
	{
		warnText += QString("提示：SubNetwork name=[%1] type=[%2]的ConnectedAP iedName=[%3],apName=[%4]内无OSI-PSEL定义，请检查。")
			.arg(subnetwork_obj->attrib("name")).arg(subnetwork_obj->attrib("type")).arg(object->attrib("iedName")).arg(object->attrib("apName"));
		warnText += "\n";
	}

	return true;
}

bool oe_mms_ap::get_osi_ssel(XmlObject * object,QString & error,QString &warnText)
{
	S_UNUSED(object);
	S_UNUSED(error);
	S_UNUSED(warnText);

	XmlObject *p_obj = object->findChildDeep("P","type","OSI-SSEL");
	if (p_obj)
	{
		osi_ssel = p_obj->text.toLocal8Bit().data();
	}
	else
	{
		warnText += QString("提示：SubNetwork name=[%1] type=[%2]的ConnectedAP iedName=[%3],apName=[%4]内无OSI-SSEL定义，请检查。")
			.arg(subnetwork_obj->attrib("name")).arg(subnetwork_obj->attrib("type")).arg(object->attrib("iedName")).arg(object->attrib("apName"));
		warnText += "\n";
	}

	return true;
}

bool oe_mms_ap::get_osi_tsel(XmlObject * object,QString & error,QString &warnText)
{
	S_UNUSED(object);
	S_UNUSED(error);
	S_UNUSED(warnText);

	XmlObject *p_obj = object->findChildDeep("P","type","OSI-TSEL");
	if (p_obj)
	{
		osi_tsel = p_obj->text.toLocal8Bit().data();
	}
	else
	{
		warnText += QString("提示：SubNetwork name=[%1] type=[%2]的ConnectedAP iedName=[%3],apName=[%4]内无OSI-TSEL定义，请检查。")
			.arg(subnetwork_obj->attrib("name")).arg(subnetwork_obj->attrib("type")).arg(object->attrib("iedName")).arg(object->attrib("apName"));
		warnText += "\n";
	}

	return true;
}
