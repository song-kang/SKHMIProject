#include "oe_cpu.h"
#include "oe_ied.h"
#include "oe_group.h"
#include "view_plugin_scl_import.h"

oe_cpu::oe_cpu(view_plugin_scl_import * scl,QList<XmlObject*> list,oe_ied *m_ied)
{
	ied_no = m_ied->getIedNo();
	cpu_no = 0;
	name = "NULL";
	version = "NULL";
	crc = "NULL";
	version_time = "NULL";
	mms_path = "NULL";

	sclImport = scl;
	document = list;
	ied = m_ied;
	iedName = ied->getIedName();
	ied_object = ied->getObject();
	//object_s1 = ied_object->findChildDeep("AccessPoint","name","S1");
	//object_g1 = ied_object->findChildDeep("AccessPoint","name","G1");
	//object_m1 = ied_object->findChildDeep("AccessPoint","name","M1");
}

oe_cpu::~oe_cpu(void)
{
}

bool oe_cpu::execute(QString & error,QString &warnText)
{
	bool b = true;

	//if (checkStateS1 == Qt::Checked && object_s1)
	//{
	//	cpus += object_s1->findChildrenDeep("LDevice");
	//}
	//if (checkStateG1 == Qt::Checked && object_g1)
	//{
	//	cpus += object_g1->findChildrenDeep("LDevice");
	//}
	//if (checkStateM1 == Qt::Checked && object_m1)
	//{
	//	cpus += object_m1->findChildrenDeep("LDevice");
	//}
	//发现多了apName=A1，不知道会不会有其它apName，此处不再判断apName
	cpus = ied_object->findChildrenDeep("LDevice");

	if (cpus.count())
	{
		foreach (cpu_object,cpus)
		{
			b = db_insert(cpu_object,error);
			if (b)
			{
				oe_group group(sclImport,document,this);
				b = group.execute(error,warnText);
				if (!b)
					break;
			}
			else
				break;
		}
	}
	if (!b)
		return b;

	if (addExtendCpu(error))
	{
		if (addExtendGroup(error))
		{
			if (!addExtendElement(error))
			{
				b = false;
				LOGFAULT("导入扩展CPU点失败。");
			}
		}
		else
		{
			b = false;
			LOGFAULT("导入扩展CPU组失败。");
		}
	}
	else
	{
		b = false;
		LOGFAULT("导入扩展CPU失败。");
	}

	return b;
}

bool oe_cpu::db_insert(XmlObject * object,QString & error)
{
	bool	b = false;

	if (!get_ied_no(object))
		return false;
	if (!get_cpu_no(object,error))
		return false;
	if (!get_name(object))
		return false;
	if (!get_version(object))
		return false;
	if (!get_crc(object))
		return false;
	if (!get_version_time(object))
		return false;
	if (!get_mms_path(object))
		return false;

	SString sql = SString::toFormat("INSERT INTO t_oe_cpu (ied_no,cpu_no,name,version,crc,version_time,mms_path) "
		"VALUES (%d,%d,'%s','%s','%s','%s','%s')",
		ied_no,
		cpu_no,
		name.data(),
		version.data(),
		crc.data(),
		version_time.data(),
		mms_path.data());

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

bool oe_cpu::get_ied_no(XmlObject * object)
{
	S_UNUSED(object);
	bool		b = true;

	ied_no = ied->getIedNo();

	return b;
}

bool oe_cpu::get_cpu_no(XmlObject * object,QString & error)
{
	S_UNUSED(object);
	bool		b = true;
	SRecordset	rs;

	SString sql = SString::toFormat("SELECT MAX(cpu_no) FROM t_oe_cpu where ied_no=%d",ied_no);
	int iRet = DB->Retrieve(sql,rs);
	if (iRet < 0)
	{
		error = "SQL语句执行错误：" + sql;
		b = false;
	}
	else if (iRet == 0)
	{
		cpu_no = 1;
		b = true;
	}
	else if (iRet > 0)
	{
		cpu_no = rs.GetValue(0,0).toInt();
		cpu_no++;
		b = true;
	}

	return b;
}

bool oe_cpu::get_name(XmlObject * object)
{
	bool b = true;

	name = object->attrib("desc").toStdString();

	return b;
}

bool oe_cpu::get_version(XmlObject * object)
{
	S_UNUSED(object);
	bool		b = true;

	version = "NULL";

	return b;
}

bool oe_cpu::get_crc(XmlObject * object)
{
	S_UNUSED(object);
	bool		b = true;

	crc = "NULL";

	return b;
}

bool oe_cpu::get_version_time(XmlObject * object)
{
	S_UNUSED(object);
	bool		b = true;

	version_time = "NULL";

	return b;
}

bool oe_cpu::get_mms_path(XmlObject * object)
{
	bool b = true;

	mms_path = object->attrib("inst").toStdString();

	return b;
}

bool oe_cpu::addExtendCpu(QString & error)
{
	bool b = false;
	SRecordset	rs;
	SString sql;
	sql.sprintf("select cpu_no from t_oe_extend_cpu group by cpu_no");
	int iRet = DB->Retrieve(sql,rs);
	if (iRet <= 0)
		return false;
	else if (iRet > 0)
	{
		switch (DB_MASTER_TYPE)
		{
		case DB_MYSQL:
			sql.sprintf("insert into t_oe_cpu (ied_no,cpu_no,name,version,crc,version_time,mms_path) values ");
			break;

		case DB_ORACLE:
			sql.sprintf("insert all ");
			break;

		default:
			return false;
		}

		for (int i = 0; i < iRet; i++)
		{
			switch (DB_MASTER_TYPE)
			{
			case DB_MYSQL:
				sql += SString::toFormat("(%d,%d,'%s','%s','%s','%s','%s'),",ied_no,rs.GetValue(i,0).toInt(),"扩展CPU","1.0.0","","","");
				break;

			case DB_ORACLE:
				sql += SString::toFormat("into t_oe_cpu (ied_no,cpu_no,name,version,crc,version_time,mms_path) "
				"values (%d,%d,'%s','%s','%s','%s','%s') ",ied_no,rs.GetValue(i,0).toInt(),"扩展CPU","1.0.0","","","");
				break;
			}
		}

		switch (DB_MASTER_TYPE)
		{
		case DB_MYSQL:
			sql = sql.left(sql.size()-1);	//去除最后的逗号
			break;

		case DB_ORACLE:
			sql += SString::toFormat("SELECT 1 FROM DUAL");
			break;
		}

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
			{
				error = "内存库SQL语句执行错误：" + sql;
				return b;
			}
		}
	}

	return b;
}

bool oe_cpu::addExtendGroup(QString & error)
{
	bool b = false;
	SRecordset	rs;
	SString sql;
	sql.sprintf("select cpu_no,group_no,group_name,group_type from t_oe_extend_cpu group by cpu_no,group_no,group_name,group_type");
	int iRet = DB->Retrieve(sql,rs);
	if (iRet <= 0)
		return false;
	else if (iRet > 0)
	{
		switch (DB_MASTER_TYPE)
		{
		case DB_MYSQL:
			sql.sprintf("insert into t_oe_group (ied_no,cpu_no,group_no,name,type) values ");
			break;

		case DB_ORACLE:
			sql.sprintf("insert all ");
			break;

		default:
			return false;
		}

		for (int i = 0; i < iRet; i++)
		{
			switch (DB_MASTER_TYPE)
			{
			case DB_MYSQL:
				sql += SString::toFormat("(%d,%d,%d,'%s',%d),",
				ied_no,rs.GetValue(i,0).toInt(),rs.GetValue(i,1).toInt(),rs.GetValue(i,2).data(),rs.GetValue(i,3).toInt());
				break;

			case DB_ORACLE:
				sql += SString::toFormat("into t_oe_group (ied_no,cpu_no,group_no,name,type) values (%d,%d,%d,'%s',%d) ",
				ied_no,rs.GetValue(i,0).toInt(),rs.GetValue(i,1).toInt(),rs.GetValue(i,2).data(),rs.GetValue(i,3).toInt());
				break;
			}
		}

		switch (DB_MASTER_TYPE)
		{
		case DB_MYSQL:
			sql = sql.left(sql.size()-1);	//去除最后的逗号
			break;

		case DB_ORACLE:
			sql += SString::toFormat("SELECT 1 FROM DUAL");
			break;
		}

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
			{
				error = "内存库SQL语句执行错误：" + sql;
				return b;
			}
		}
	}

	return b;
}

bool oe_cpu::addExtendElement(QString & error)
{
	bool b = false;
	SRecordset	rs;
	SString sql1;
	SString sql2;
	SString sql11 = "";
	SString sql21 = "";
	sql1.sprintf("select cpu_no,group_no,group_type,entry,entry_name,"
		"valtype,dime,maxval,minval,stepval,precision_n,precision_m,itemtype,factor,offset,threshold,smooth,"
		"fun,inf,type,evt_cls,val_type,severity_level,on_dsc,off_dsc,unknown_desc,inver "
		"from t_oe_extend_cpu");
	int iRet = DB->Retrieve(sql1,rs);
	if (iRet <= 0)
		return false;
	else if (iRet > 0)
	{
		switch (DB_MASTER_TYPE)
		{
		case DB_MYSQL:
			sql1.sprintf("insert into t_oe_element_general "
			"(ied_no,cpu_no,group_no,entry,name,valtype,dime,maxval,minval,stepval,precision_n,precision_m,"
			"itemtype,factor,offset,threshold,smooth) values ");
		sql2.sprintf("insert into t_oe_element_state "
			"(ied_no,cpu_no,group_no,entry,fun,inf,name,type,evt_cls,val_type,severity_level,on_dsc,off_dsc,unknown_desc,inver) values ");
			break;

		case DB_ORACLE:
			sql1.sprintf("insert all ");
			sql2.sprintf("insert all ");
			break;
		}

		for (int i = 0; i < iRet; i++)
		{
			int group_type = rs.GetValue(i,2).toInt();
			if (group_type == GROUP_SWITCH || 
				group_type == GROUP_SOFT_STRAP ||
				group_type == GROUP_EVENT || 
				group_type == GROUP_ALARM ||
				group_type == GROUP_GOOSE)
			{
				switch (DB_MASTER_TYPE)
				{
				case DB_MYSQL:
					sql21 += SString::toFormat("(%d,%d,%d,%d,%d,%d,'%s',%d,%d,%d,%d,'%s','%s','%s',%d),",
						ied_no,
						rs.GetValue(i,0).toInt(),
						rs.GetValue(i,1).toInt(),
						rs.GetValue(i,3).toInt(),
						rs.GetValue(i,17).toInt(),
						rs.GetValue(i,18).toInt(),
						rs.GetValue(i,4).data(),
						rs.GetValue(i,19).toInt(),
						rs.GetValue(i,20).toInt(),
						rs.GetValue(i,21).toInt(),
						rs.GetValue(i,22).toInt(),
						rs.GetValue(i,23).data(),
						rs.GetValue(i,24).data(),
						rs.GetValue(i,25).data(),
						rs.GetValue(i,26).toInt());
					break;

				case DB_ORACLE:
					sql21 += SString::toFormat("into t_oe_element_state "
						"(ied_no,cpu_no,group_no,entry,fun,inf,name,type,evt_cls,val_type,severity_level,on_dsc,off_dsc,unknown_desc,inver) values "
						"(%d,%d,%d,%d,%d,%d,'%s',%d,%d,%d,%d,'%s','%s','%s',%d) ",
						ied_no,
						rs.GetValue(i,0).toInt(),
						rs.GetValue(i,1).toInt(),
						rs.GetValue(i,3).toInt(),
						rs.GetValue(i,17).toInt(),
						rs.GetValue(i,18).toInt(),
						rs.GetValue(i,4).data(),
						rs.GetValue(i,19).toInt(),
						rs.GetValue(i,20).toInt(),
						rs.GetValue(i,21).toInt(),
						rs.GetValue(i,22).toInt(),
						rs.GetValue(i,23).data(),
						rs.GetValue(i,24).data(),
						rs.GetValue(i,25).data(),
						rs.GetValue(i,26).toInt());
					break;
				}
			}
			else
			{
				switch (DB_MASTER_TYPE)
				{
				case DB_MYSQL:
				sql11 += SString::toFormat("(%d,%d,%d,%d,'%s',%d,'%s',%f,%f,%f,%d,%d,%d,%f,%f,%f,%f),",
					ied_no,
					rs.GetValue(i,0).toInt(),
					rs.GetValue(i,1).toInt(),
					rs.GetValue(i,3).toInt(),
					rs.GetValue(i,4).data(),
					rs.GetValue(i,5).toInt(),
					rs.GetValue(i,6).data(),
					rs.GetValue(i,7).toFloat(),
					rs.GetValue(i,8).toFloat(),
					rs.GetValue(i,9).toFloat(),
					rs.GetValue(i,10).toInt(),
					rs.GetValue(i,11).toInt(),
					rs.GetValue(i,12).toInt(),
					rs.GetValue(i,13).toFloat(),
					rs.GetValue(i,14).toFloat(),
					rs.GetValue(i,15).toFloat(),
					rs.GetValue(i,16).toFloat());
				break;

				case DB_ORACLE:
					sql11 += SString::toFormat("into t_oe_element_general "
						"(ied_no,cpu_no,group_no,entry,name,valtype,dime,maxval,minval,stepval,precision_n,precision_m,"
						"itemtype,factor,offset,threshold,smooth) values (%d,%d,%d,%d,'%s',%d,'%s',%f,%f,%f,%d,%d,%d,%f,%f,%f,%f) ",
						ied_no,
						rs.GetValue(i,0).toInt(),
						rs.GetValue(i,1).toInt(),
						rs.GetValue(i,3).toInt(),
						rs.GetValue(i,4).data(),
						rs.GetValue(i,5).toInt(),
						rs.GetValue(i,6).data(),
						rs.GetValue(i,7).toFloat(),
						rs.GetValue(i,8).toFloat(),
						rs.GetValue(i,9).toFloat(),
						rs.GetValue(i,10).toInt(),
						rs.GetValue(i,11).toInt(),
						rs.GetValue(i,12).toInt(),
						rs.GetValue(i,13).toFloat(),
						rs.GetValue(i,14).toFloat(),
						rs.GetValue(i,15).toFloat(),
						rs.GetValue(i,16).toFloat());
					break;
				}
			}
		}

		if (!sql11.isEmpty())
		{
			sql1 += sql11;
			switch (DB_MASTER_TYPE)
			{
			case DB_MYSQL:
				sql1 = sql1.left(sql1.size()-1);	//去除最后的逗号
				break;

			case DB_ORACLE:
				sql1 += SString::toFormat("SELECT 1 FROM DUAL");
				break;
			}

			b = DB->Execute(sql1);
			if (b)
			{
				if (m_bMDB)
				{
					b = MDB->Execute(sql1);
					if (!b)
					{
						error = "内存库SQL语句执行错误：" + sql1;
						return b;
					}
				}
			}
			else
			{
				error = "SQL语句执行错误：" + sql1;
				return b;
			}
		}

		if (!sql21.isEmpty())
		{
			sql2 += sql21;
			switch (DB_MASTER_TYPE)
			{
			case DB_MYSQL:
				sql2 = sql2.left(sql2.size()-1);	//去除最后的逗号
				break;

			case DB_ORACLE:
				sql2 += SString::toFormat("SELECT 1 FROM DUAL");
				break;
			}

			b = DB->Execute(sql2);
			if (b)
			{
				if (m_bMDB)
				{
					b = MDB->Execute(sql2);
					if (!b)
					{
						error = "内存库SQL语句执行错误：" + sql2;
						return b;
					}
				}
			}
			else
			{
				error = "SQL语句执行错误：" + sql2;
				return b;
			}
		}
	}

	return b;
}
