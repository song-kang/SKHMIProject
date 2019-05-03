#include "oe_run_device.h"
#include "view_plugin_scl_import.h"
#include "oe_ied.h"

oe_run_device::oe_run_device(view_plugin_scl_import *scl,QList<XmlObject*> list,oe_ied *m_ied,int subNo)
{
	device_no = 0;
	name = "";
	p_device_no = 0;
	type = 0;
	comm_state = 0;
	sub_no = subNo;
	ied_no = m_ied->getIedNo();
	node_no = 0;
	unit_no = 0;
	run_state = 0;
	remote_ctrl = 0;
	clock_sync_s = 0;
	loop_ms = 0;
	need_wave = 0;

	sclImport = scl;
	document = list;
	ied = m_ied;
	ied_object = m_ied->getObject();
}

oe_run_device::~oe_run_device(void)
{
}

bool oe_run_device::execute(QString & error)
{
	bool b = false;

	if (!ied_object)
	{
		LOGFAULT("表t_oe_run_device插入操作时，装置对象未发现：%s.", ied->getIedName().toLatin1().data());
		error = QObject::tr("表t_oe_run_device插入操作时，装置对象未发现：%s.").arg(ied->getIedName().toLatin1().data());
		return false;
	}

	if (!get_device_no(error))
		return false;
	if (!get_name(error))
		return false;
	if (!get_p_device_no(error))
		return false;
	if (!get_type(error))
		return false;
	if (!get_comm_state(error))
		return false;
	if (!get_sub_no(error))
		return false;
	if (!get_ied_no(error))
		return false;
	if (!get_node_no(error))
		return false;
	if (!get_unit_no(error))
		return false;
	if (!get_run_state(error))
		return false;
	if (!get_remote_ctrl(error))
		return false;
	if (!get_clock_sync_s(error))
		return false;
	if (!get_loop_ms(error))
		return false;
	if (!get_need_wave(error))
		return false;

	SString sql = SString::toFormat("insert into t_oe_run_device (device_no,name,p_device_no,type,comm_state,sub_no,ied_no,node_no,"
		"unit_no,run_state,remote_ctrl,clock_sync_s,loop_ms,need_wave) values (%d,'%s',%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d)",
		device_no,
		name.data(),
		p_device_no,
		type,
		comm_state,
		sub_no,
		ied_no,
		node_no,
		unit_no,
		run_state,
		remote_ctrl,
		clock_sync_s,
		loop_ms,
		need_wave);

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

bool oe_run_device::get_device_no(QString & error)
{
	SRecordset	rs;

	SString sql = "select max(device_no) from t_oe_run_device";
	int iRet = DB->Retrieve(sql,rs);
	if (iRet < 0)
	{
		error = "SQL语句执行错误：" + sql;
		return false;
	}
	else if (iRet == 0)
	{
		device_no = 1;
	}
	else if (iRet > 0)
	{
		device_no = rs.GetValue(0,0).toInt();
		device_no++;
	}

	return true;
}

bool oe_run_device::get_name(QString & error)
{
	S_UNUSED(error);

	name = "";

	return true;
}

bool oe_run_device::get_p_device_no(QString & error)
{
	S_UNUSED(error);

	p_device_no = 0;

	return true;
}

bool oe_run_device::get_type(QString & error)
{
	S_UNUSED(error);

	type = 0;

	return true;
}

bool oe_run_device::get_comm_state(QString & error)
{
	S_UNUSED(error);

	comm_state = 0;

	return true;
}

bool oe_run_device::get_sub_no(QString & error)
{
	S_UNUSED(error);

	sub_no = sub_no;

	return true;
}

bool oe_run_device::get_ied_no(QString & error)
{
	S_UNUSED(error);

	ied_no = ied_no;

	return true;
}

bool oe_run_device::get_node_no(QString & error)
{
	SRecordset	rs;
	SString		sql;

	sql.sprintf("select t_oe_run_node.node_no from t_oe_run_node,t_oe_module_param where "
		"t_oe_run_node.node_no=t_oe_module_param.node_no and "
		"t_oe_module_param.param_name='substation_id' and "
		"t_oe_module_param.current_val='%d'",sub_no);
	int cnt = DB->Retrieve(sql,rs);
	if (cnt < 0)
	{
		error = "SQL语句执行错误：" + sql;
		return false;
	}
	else if (cnt == 0)
	{
		error = QString("未发现子站[%1]对应节点号。\n\n%2").arg(sub_no).arg(sql.data());
		return false;
	}
	else if (cnt > 0)
	{
		node_no = rs.GetValue(0,0).toInt();
	}

	return true;
}

bool oe_run_device::get_unit_no(QString & error)
{
	S_UNUSED(error);

	unit_no = 0;

	return true;
}

bool oe_run_device::get_run_state(QString & error)
{
	S_UNUSED(error);

	run_state = 0;

	return true;
}

bool oe_run_device::get_remote_ctrl(QString & error)
{
	S_UNUSED(error);

	remote_ctrl = 0;

	return true;
}

bool oe_run_device::get_clock_sync_s(QString & error)
{
	S_UNUSED(error);

	clock_sync_s = 0;

	return true;
}

bool oe_run_device::get_loop_ms(QString & error)
{
	S_UNUSED(error);

	loop_ms = 0;

	return true;
}

bool oe_run_device::get_need_wave(QString & error)
{
	S_UNUSED(error);

	need_wave = 0;

	return true;
}

