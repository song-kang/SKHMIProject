#include "inputsthread.h"
#include "view_plugin_scl_import.h"

InputsThread::InputsThread(QObject *parent)
	: QThread(parent)
{
	sclImport = (view_plugin_scl_import *)parent;
}

InputsThread::~InputsThread()
{

}

void InputsThread::run()
{
	document = sclImport->document;
	sub_no = sclImport->getSubstationNo();
	error = QString::null;
	warnText = QString::null;

	SString sql;
	SRecordset rs;
	sql.sprintf("select t_oe_element_input_extref.ied_no,t_oe_element_input_extref.int_no,"
		"t_oe_element_input_extref.ied_name,t_oe_element_input_extref.do_name,t_oe_element_input_extref.ln_inst,"
		"t_oe_element_input_extref.ln_class,t_oe_element_input_extref.da_name,t_oe_element_input_extref.ld_inst,prefix "
		"from t_oe_element_input_extref,t_cim_substation,t_oe_ied "
		"where t_cim_substation.sub_no=%d and t_cim_substation.sub_no=t_oe_ied.sub_no and "
		"t_oe_element_input_extref.ied_no=t_oe_ied.ied_no",sub_no);
	int cnt = DB->Retrieve(sql,rs);
	if (cnt < 0)
	{
		error = "SQL语句执行错误：" + sql;
		return;
	}
	else if (cnt == 0)
	{
		warnText += QString("告警：数据库表t_oe_element_input_extref内容为空，将无虚端子连接，请检查。");
		warnText += "\n";
	}
	else if (cnt > 0)
	{
		int step = 0;
		for (int i = 0; i < cnt; i++)
		{
			int ied_no = rs.GetValue(i,0).toInt();
			int int_no = rs.GetValue(i,1).toInt();
			QString ied_name = rs.GetValue(i,2).data();
			QString do_name = rs.GetValue(i,3).data();
			do_name = do_name.replace(".","$");
			QString ln_inst = rs.GetValue(i,4).data();
			QString ln_class = rs.GetValue(i,5).data();
			QString da_name = rs.GetValue(i,6).data();
			da_name = da_name.replace(".","$");
			QString ld_inst = rs.GetValue(i,7).data();
			QString prefix = rs.GetValue(i,8).data();

			QString mms;
			if (da_name.isEmpty())
				mms = ied_name+ld_inst+"/"+prefix+ln_class+ln_inst+"$%%$"+do_name+"%";
			else 
				mms = ied_name+ld_inst+"/"+prefix+ln_class+ln_inst+"$%%$"+do_name+"$"+da_name;

			emit sclImport->sigProgressValue(++step);
			if (!findElementByState(ied_no,int_no,ied_name,mms,error,warnText))
				break;
		}
	}

	sclImport->parseError = error;
	sclImport->parseWarn += warnText;
}

bool InputsThread::findElementByState(int extrefIedNo,int extrefIntNo,QString iedName,QString mms,
	QString & error,QString &warnText)
{
	int ied_no = _findIedNo(iedName,error);
	if (ied_no == 0)
	{
		warnText += QString("告警：订阅条目[%1,%2]未找到发布装置[%3]，将无虚端子连接，请检查。")
			.arg(extrefIedNo).arg(extrefIntNo).arg(iedName);
		warnText += "\n";
		return true;
	}
	else if (ied_no == -1)
		return false;

	SString sql;
	SRecordset rs;
	sql.sprintf("select ied_no,cpu_no,group_no,entry from t_oe_element_state where ied_no=%d and mms_path like '%s'",
		ied_no,mms.toLocal8Bit().data());
	int cnt = DB->Retrieve(sql,rs);
	if (cnt < 0)
	{
		error = "SQL语句执行错误：" + sql;
		return false;
	}
	else if (cnt == 0)
	{
		if (!findElementByGeneral(extrefIedNo,extrefIntNo,ied_no,mms,error,warnText))
			return false;
		return true;
	}
	else if (cnt > 0)
	{
		int iedNo = rs.GetValue(0,0).toInt();
		int cpuNo = rs.GetValue(0,1).toInt();
		int grpNo = rs.GetValue(0,2).toInt();
		int entry = rs.GetValue(0,3).toInt();
		if (!updateElementInputExtref(extrefIedNo,extrefIntNo,iedNo,cpuNo,grpNo,entry,error,warnText))
			return false;
	}

	return true;
}

bool InputsThread::findElementByGeneral(int extrefIedNo,int extrefIntNo,int iedNo,QString mms,
	QString & error,QString &warnText)
{
	SString sql;
	SRecordset rs;
	sql.sprintf("select ied_no,cpu_no,group_no,entry from t_oe_element_general where ied_no=%d and mms_path like '%s'",
		iedNo,mms.toLocal8Bit().data());
	int cnt = DB->Retrieve(sql,rs);
	if (cnt < 0)
	{
		error = "SQL语句执行错误：" + sql;
		return false;
	}
	else if (cnt == 0)
	{
		warnText += QString("告警：订阅条目[%1,%2]未找到发布点[%3]，将无虚端子连接，请检查。")
			.arg(extrefIedNo).arg(extrefIntNo).arg(mms);
		warnText += "\n";
		return true;
	}
	else if (cnt > 0)
	{
		int iedNo = rs.GetValue(0,0).toInt();
		int cpuNo = rs.GetValue(0,1).toInt();
		int grpNo = rs.GetValue(0,2).toInt();
		int entry = rs.GetValue(0,3).toInt();
		if (!updateElementInputExtref(extrefIedNo,extrefIntNo,iedNo,cpuNo,grpNo,entry,error,warnText))
			return false;
	}

	return true;
}

bool InputsThread::updateElementInputExtref(int extrefIedNo,int extrefIntNo,
	int iedNo,int cpuNo,int grpNo,int entry,QString & error,QString &warnText)
{
	int type = _get_type(extrefIedNo,extrefIntNo,iedNo,cpuNo,grpNo,error,warnText);
	if (type == -1)
		return false;

	SString sql;
	sql.sprintf("update t_oe_element_input_extref set type=%d,link_ied_no=%d,link_cpu_no=%d,link_group_no=%d,link_entry=%d "
		"where ied_no=%d and int_no=%d",type,iedNo,cpuNo,grpNo,entry,extrefIedNo,extrefIntNo);
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

int InputsThread::_findIedNo(QString iedName,QString & error)
{
	SString sql;
	SRecordset rs;
	int no = 0;

	sql.sprintf("select t_oe_ied.ied_no from t_oe_ied,t_cim_substation "
		"where t_cim_substation.sub_no=%d and t_oe_ied.sub_no=t_cim_substation.sub_no and t_oe_ied.mms_path='%s'",
		sub_no,iedName.toLocal8Bit().data());
	int cnt = DB->Retrieve(sql,rs);
	if (cnt < 0)
	{
		error = "SQL语句执行错误：" + sql;
		return -1;
	}
	else if (cnt == 0)
	{
		return 0;
	}
	else if (cnt > 0)
	{
		no = rs.GetValue(0,0).toInt();
	}

	return no;
}

int InputsThread::_get_type(int extrefIedNo,int extrefIntNo,int iedNo,int cpuNo,int grpNo,QString & error,QString &warnText)
{
	SString sql;
	SRecordset rs;
	int type = 0;

	sql.sprintf("select 't_oe_goose_ctrl_block' as mtable from t_oe_goose_ctrl_block where ied_no=%d and cpu_no=%d and group_no=%d "
		"union select 't_oe_smv_ctrl_block' as mtable from t_oe_smv_ctrl_block where ied_no=%d and cpu_no=%d and group_no=%d",
		iedNo,cpuNo,grpNo,iedNo,cpuNo,grpNo);
	int cnt = DB->Retrieve(sql,rs);
	if (cnt < 0)
	{
		error = "SQL语句执行错误：" + sql;
		return -1;
	}
	else if (cnt == 0)
	{
		warnText += QString("告警：订阅条目[%1,%2]通过[%3,%4,%5]未找到发布类型，请检查。")
			.arg(extrefIedNo).arg(extrefIntNo).arg(iedNo).arg(cpuNo).arg(grpNo);
		warnText += "\n";
		type = 0;
	}
	else if (cnt > 1)
	{
		warnText += QString("告警：订阅条目[%1,%2]通过[%3,%4,%5]找到多种发布类型，请检查。")
			.arg(extrefIedNo).arg(extrefIntNo).arg(iedNo).arg(cpuNo).arg(grpNo);
		warnText += "\n";
		type = 0;
	}
	else if (cnt == 1)
	{
		SString mtable = rs.GetValue(0,0);
		if (mtable == "t_oe_goose_ctrl_block")
			type = 1;
		else if (mtable == "t_oe_smv_ctrl_block")
			type = 2;
	}

	return type;
}
