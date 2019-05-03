#include "oe_smv_ctrl_block.h"
#include "view_plugin_scl_import.h"
#include "oe_ied.h"
#include "oe_cpu.h"
#include "oe_group.h"
#include "oe_smv_ap.h"

oe_smv_ctrl_block::oe_smv_ctrl_block(view_plugin_scl_import * scl,QList<XmlObject*> list,oe_group *group)
{
	sclImport = scl;
	document = list;
	m_ied = group->getCpu()->getIed();
	m_cpu = group->getCpu();
	m_group = group;
}

oe_smv_ctrl_block::~oe_smv_ctrl_block(void)
{
}

void oe_smv_ctrl_block::initParam()
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
	sv_ena = 0;
	smvid = "";
	smprate = 0;
	nof_asdu = 0;
	opt_reftime = 0;
	opt_sync = 0;
	opt_smprate = 0;
	opt_secu = 0;
	opt_dataref = 0;
}

bool oe_smv_ctrl_block::execute(QString & error,QString &warnText)
{
	XmlObject *cpu_obj = m_cpu->getObject();
	if (cpu_obj)
	{
		QList<XmlObject*> lstSmvCB = cpu_obj->findChildrenDeep("SampledValueControl");
		foreach (XmlObject *smv_obj, lstSmvCB)
		{
			initParam();
			if (!insertSmvCB(smv_obj,error,warnText))
				return false;

			if (group_no) //插入成功，并且组号非零时，处理SMV访问点
			{
				oe_smv_ap sa(sclImport,document,this);
				if (!sa.execute(error,warnText))
					return false;
			}
		}
	}

	return true;
}

bool oe_smv_ctrl_block::insertSmvCB(XmlObject * object,QString & error,QString &warnText)
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
	if (!get_sv_ena(object,error,warnText))
		return false;
	if (!get_smvid(object,error,warnText))
		return false;
	if (!get_smprate(object,error,warnText))
		return false;
	if (!get_nof_asdu(object,error,warnText))
		return false;
	if (!get_opt_reftime(object,error,warnText))
		return false;
	if (!get_opt_sync(object,error,warnText))
		return false;
	if (!get_opt_smprate(object,error,warnText))
		return false;
	if (!get_opt_secu(object,error,warnText))
		return false;
	if (!get_opt_dataref(object,error,warnText))
		return false;

	if (group_no == 0) //未发现组号，不加入数据库，但不中止
		return true;

	SString sql;
	sql.sprintf("insert into t_oe_smv_ctrl_block (ied_no,cb_no,cpu_no,group_no,cb_name,cb_desc,conf_rev,appid,datset,"
		"sv_ena,smvid,smprate,nof_asdu,opt_reftime,opt_sync,opt_smprate,opt_secu,opt_dataref) "
		"values (%d,%d,%d,%d,'%s','%s','%s','%s','%s',%d,'%s',%d,%d,%d,%d,%d,%d,%d)",
		ied_no,cb_no,cpu_no,group_no,cb_name.data(),cb_desc.data(),conf_rev.data(),appid.data(),datset.data(),
		sv_ena,smvid.data(),smprate,nof_asdu,opt_reftime,opt_sync,opt_smprate,opt_secu,opt_dataref);
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

bool oe_smv_ctrl_block::get_ied_no(XmlObject * object,QString & error,QString &warnText)
{
	S_UNUSED(object);
	S_UNUSED(error);
	S_UNUSED(warnText);

	ied_no = m_ied->getIedNo();

	return true;
}

bool oe_smv_ctrl_block::get_cb_no(XmlObject * object,QString & error,QString &warnText)
{
	S_UNUSED(object);
	S_UNUSED(error);
	S_UNUSED(warnText);

	cb_no = m_ied->getSmvCbNo();
	cb_no++;
	m_ied->setSmvCbNo(cb_no);

	return true;
}

bool oe_smv_ctrl_block::get_cpu_no(XmlObject * object,QString & error,QString &warnText)
{
	S_UNUSED(object);
	S_UNUSED(error);
	S_UNUSED(warnText);

	cpu_no = m_cpu->getCpuNo();

	return true;
}

bool oe_smv_ctrl_block::get_group_no(XmlObject * object,QString & error,QString &warnText)
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
		warnText += QString("告警：Ied[%1]-Cpu[%2]的SampledValueControl中datSet[%3]未找到对应组号，请检查。")
			.arg(m_cpu->getIedNo()).arg(m_cpu->getCpuNo()).arg(datset.data());
		warnText += "\n";
	}
	else if (cnt > 0)
	{
		group_no = rs.GetValue(0,0).toInt();
	}

	return true;
}

bool oe_smv_ctrl_block::get_cb_name(XmlObject * object,QString & error,QString &warnText)
{
	S_UNUSED(object);
	S_UNUSED(error);
	S_UNUSED(warnText);

	cb_name = object->attrib("name").toLocal8Bit().data();

	return true;
}

bool oe_smv_ctrl_block::get_cb_desc(XmlObject * object,QString & error,QString &warnText)
{
	S_UNUSED(object);
	S_UNUSED(error);
	S_UNUSED(warnText);

	cb_desc = object->attrib("desc").toLocal8Bit().data();

	return true;
}

bool oe_smv_ctrl_block::get_conf_rev(XmlObject * object,QString & error,QString &warnText)
{
	S_UNUSED(object);
	S_UNUSED(error);
	S_UNUSED(warnText);

	conf_rev = object->attrib("confRev").toLocal8Bit().data();

	return true;
}

bool oe_smv_ctrl_block::get_appid(XmlObject * object,QString & error,QString &warnText)
{
	S_UNUSED(object);
	S_UNUSED(error);
	S_UNUSED(warnText);

	appid = object->attrib("appID").toLocal8Bit().data();

	return true;
}

bool oe_smv_ctrl_block::get_datset(XmlObject * object,QString & error,QString &warnText)
{
	S_UNUSED(object);
	S_UNUSED(error);
	S_UNUSED(warnText);

	datset = object->attrib("datSet").toLocal8Bit().data();

	return true;
}

bool oe_smv_ctrl_block::get_sv_ena(XmlObject * object,QString & error,QString &warnText)
{
	S_UNUSED(object);
	S_UNUSED(error);
	S_UNUSED(warnText);

	sv_ena = object->attrib("svEna") == "true" ? 1 : 0;

	return true;
}

bool oe_smv_ctrl_block::get_smvid(XmlObject * object,QString & error,QString &warnText)
{
	S_UNUSED(object);
	S_UNUSED(error);
	S_UNUSED(warnText);

	smvid = object->attrib("smvID").toLocal8Bit().data();

	return true;
}

bool oe_smv_ctrl_block::get_smprate(XmlObject * object,QString & error,QString &warnText)
{
	S_UNUSED(object);
	S_UNUSED(error);
	S_UNUSED(warnText);

	smprate = object->attrib("smpRate").toInt();

	return true;
}

bool oe_smv_ctrl_block::get_nof_asdu(XmlObject * object,QString & error,QString &warnText)
{
	S_UNUSED(object);
	S_UNUSED(error);
	S_UNUSED(warnText);

	nof_asdu = object->attrib("nofASDU").toInt();

	return true;
}

bool oe_smv_ctrl_block::get_opt_reftime(XmlObject * object,QString & error,QString &warnText)
{
	S_UNUSED(object);
	S_UNUSED(error);
	S_UNUSED(warnText);

	XmlObject *smv_opt_obj = object->findChild("SmvOpts");
	if (smv_opt_obj)
	{
		opt_reftime = smv_opt_obj->attrib("refreshTime") == "true" ? 1 : 0;
	}
	else
	{
		//error = QString("获取Ied:%1 Cpu:%2 Group:%3的SampledValueControl[%4]的SmvOpts失败。")
		//	.arg(m_cpu->getIedNo()).arg(m_cpu->getCpuNo()).arg(m_group->getGroupNo()).arg(cb_name.data());
		//return false;
		opt_reftime = 0;
	}

	return true;
}

bool oe_smv_ctrl_block::get_opt_sync(XmlObject * object,QString & error,QString &warnText)
{
	S_UNUSED(object);
	S_UNUSED(error);
	S_UNUSED(warnText);

	XmlObject *smv_opt_obj = object->findChild("SmvOpts");
	if (smv_opt_obj)
	{
		opt_sync = smv_opt_obj->attrib("sampleSynchronized") == "true" ? 1 : 0;
	}
	else
	{
		//error = QString("获取Ied:%1 Cpu:%2 Group:%3的SampledValueControl[%4]的SmvOpts失败。")
		//	.arg(m_cpu->getIedNo()).arg(m_cpu->getCpuNo()).arg(m_group->getGroupNo()).arg(cb_name.data());
		//return false;
		opt_sync = 0;
	}

	return true;
}

bool oe_smv_ctrl_block::get_opt_smprate(XmlObject * object,QString & error,QString &warnText)
{
	S_UNUSED(object);
	S_UNUSED(error);
	S_UNUSED(warnText);

	XmlObject *smv_opt_obj = object->findChild("SmvOpts");
	if (smv_opt_obj)
	{
		opt_smprate = smv_opt_obj->attrib("samleRate") == "true" ? 1 : 0;
	}
	else
	{
		//error = QString("获取Ied:%1 Cpu:%2 Group:%3的SampledValueControl[%4]的SmvOpts失败。")
		//	.arg(m_cpu->getIedNo()).arg(m_cpu->getCpuNo()).arg(m_group->getGroupNo()).arg(cb_name.data());
		//return false;
		opt_smprate = 0;
	}

	return true;
}

bool oe_smv_ctrl_block::get_opt_secu(XmlObject * object,QString & error,QString &warnText)
{
	S_UNUSED(object);
	S_UNUSED(error);
	S_UNUSED(warnText);

	XmlObject *smv_opt_obj = object->findChild("SmvOpts");
	if (smv_opt_obj)
	{
		opt_secu = smv_opt_obj->attrib("security") == "true" ? 1 : 0;
	}
	else
	{
		//error = QString("获取Ied:%1 Cpu:%2 Group:%3的SampledValueControl[%4]的SmvOpts失败。")
		//	.arg(m_cpu->getIedNo()).arg(m_cpu->getCpuNo()).arg(m_group->getGroupNo()).arg(cb_name.data());
		//return false;
		opt_secu = 0;
	}

	return true;
}

bool oe_smv_ctrl_block::get_opt_dataref(XmlObject * object,QString & error,QString &warnText)
{
	S_UNUSED(object);
	S_UNUSED(error);
	S_UNUSED(warnText);

	XmlObject *smv_opt_obj = object->findChild("SmvOpts");
	if (smv_opt_obj)
	{
		opt_dataref = smv_opt_obj->attrib("dataRef") == "true" ? 1 : 0;
	}
	else
	{
		//error = QString("获取Ied:%1 Cpu:%2 Group:%3的SampledValueControl[%4]的SmvOpts失败。")
		//	.arg(m_cpu->getIedNo()).arg(m_cpu->getCpuNo()).arg(m_group->getGroupNo()).arg(cb_name.data());
		//return false;
		opt_dataref = 0;
	}

	return true;
}
