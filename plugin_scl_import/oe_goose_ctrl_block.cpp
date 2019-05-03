#include "oe_goose_ctrl_block.h"
#include "view_plugin_scl_import.h"
#include "oe_ied.h"
#include "oe_cpu.h"
#include "oe_group.h"
#include "oe_goose_ap.h"

oe_goose_ctrl_block::oe_goose_ctrl_block(view_plugin_scl_import * scl,QList<XmlObject*> list,oe_group *group)
{
	sclImport = scl;
	document = list;
	m_ied = group->getCpu()->getIed();
	m_cpu = group->getCpu();
	m_group = group;
}

oe_goose_ctrl_block::~oe_goose_ctrl_block(void)
{
}

void oe_goose_ctrl_block::initParam()
{
	ied_no = 0;
	cb_no = 0;
	cpu_no = 0;
	group_no = 0;
	cb_name = "";
	cb_desc = "";
	conf_rev = "";
	appid = "";
	datset = "";
}

bool oe_goose_ctrl_block::execute(QString & error,QString &warnText)
{
	XmlObject *cpu_obj = m_cpu->getObject();
	if (cpu_obj)
	{
		QList<XmlObject*> lstGseCB = cpu_obj->findChildrenDeep("GSEControl");
		foreach (XmlObject *gcb_obj, lstGseCB)
		{
			initParam();
			if (!insertGseCB(gcb_obj,error,warnText))
				return false;

			if (group_no) //插入成功，并且组号非零时，处理Goose访问点
			{
				oe_goose_ap ga(sclImport,document,this);
				if (!ga.execute(error,warnText))
					return false;
			}
		}
	}

	return true;
}

bool oe_goose_ctrl_block::insertGseCB(XmlObject * object,QString & error,QString &warnText)
{
	if (!get_ied_no(object,error,warnText))
		return false;
	if (!get_cb_no(object,error,warnText))
		return false;
	if (!get_cpu_no(object,error,warnText))
		return false;
	if (!get_datset(object,error,warnText))
		return false;
	if (!get_group_no(object,error,warnText))
		return false;
	if (!get_cb_name(object,error,warnText))
		return false;
	if (!get_cb_desc(object,error,warnText))
		return false;
	if (!get_conf_rev(object,error,warnText))
		return false;
	if (!get_appid(object,error,warnText))
		return false;

	if (group_no == 0) //未发现组号，不加入数据库，但不中止
		return true;

	SString sql;
	sql.sprintf("insert into t_oe_goose_ctrl_block (ied_no,cb_no,cpu_no,group_no,cb_name,cb_desc,conf_rev,appid,datset) "
		"values (%d,%d,%d,%d,'%s','%s','%s','%s','%s')",
		ied_no,cb_no,cpu_no,group_no,cb_name.data(),cb_desc.data(),conf_rev.data(),appid.data(),datset.data());
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

bool oe_goose_ctrl_block::get_ied_no(XmlObject * object,QString & error,QString &warnText)
{
	S_UNUSED(object);
	S_UNUSED(error);
	S_UNUSED(warnText);

	ied_no = m_ied->getIedNo();

	return true;
}

bool oe_goose_ctrl_block::get_cb_no(XmlObject * object,QString & error,QString &warnText)
{
	S_UNUSED(object);
	S_UNUSED(error);
	S_UNUSED(warnText);

	cb_no = m_ied->getGooseCbNo();
	cb_no++;
	m_ied->setGooseCbNo(cb_no);

	return true;
}

bool oe_goose_ctrl_block::get_cpu_no(XmlObject * object,QString & error,QString &warnText)
{
	S_UNUSED(object);
	S_UNUSED(error);
	S_UNUSED(warnText);

	cpu_no = m_cpu->getCpuNo();

	return true;
}

bool oe_goose_ctrl_block::get_group_no(XmlObject * object,QString & error,QString &warnText)
{
	S_UNUSED(object);
	S_UNUSED(error);
	S_UNUSED(warnText);

	SString sql;
	SRecordset rs;

	SString mms_path = m_cpu->getCpuMmsPath()+"$"+ datset;
	sql.sprintf("select group_no from t_oe_group where ied_no=%d and cpu_no=%d and mms_path='%s'",
		m_ied->getIedNo(),m_cpu->getCpuNo(),mms_path.data());
	int cnt = DB->Retrieve(sql,rs);
	if (cnt < 0)
	{
		error = "SQL语句执行错误：" + sql;
		return false;
	}
	else if (cnt == 0)
	{
		warnText += QString("告警：Ied[%1]-Cpu[%2]的GSEControl中datSet[%3]未找到对应组号，请检查。")
			.arg(m_cpu->getIedNo()).arg(m_cpu->getCpuNo()).arg(datset.data());
		warnText += "\n";
	}
	else if (cnt > 0)
	{
		group_no = rs.GetValue(0,0).toInt();
	}

	return true;
}

bool oe_goose_ctrl_block::get_cb_name(XmlObject * object,QString & error,QString &warnText)
{
	S_UNUSED(object);
	S_UNUSED(error);
	S_UNUSED(warnText);

	cb_name = object->attrib("name").toLocal8Bit().data();

	return true;
}

bool oe_goose_ctrl_block::get_cb_desc(XmlObject * object,QString & error,QString &warnText)
{
	S_UNUSED(object);
	S_UNUSED(error);
	S_UNUSED(warnText);

	cb_desc = object->attrib("desc").toLocal8Bit().data();

	return true;
}

bool oe_goose_ctrl_block::get_conf_rev(XmlObject * object,QString & error,QString &warnText)
{
	S_UNUSED(object);
	S_UNUSED(error);
	S_UNUSED(warnText);

	conf_rev = object->attrib("confRev").toLocal8Bit().data();

	return true;
}

bool oe_goose_ctrl_block::get_appid(XmlObject * object,QString & error,QString &warnText)
{
	S_UNUSED(object);
	S_UNUSED(error);
	S_UNUSED(warnText);

	appid = object->attrib("appID").toLocal8Bit().data();

	return true;
}

bool oe_goose_ctrl_block::get_datset(XmlObject * object,QString & error,QString &warnText)
{
	S_UNUSED(object);
	S_UNUSED(error);
	S_UNUSED(warnText);

	datset = object->attrib("datSet").toLocal8Bit().data();

	return true;
}
