#include "na_sub_network.h"
#include "na_comm_device_port.h"
#include "oe_ied.h"
#include "view_plugin_scl_import.h"

na_sub_network::na_sub_network(view_plugin_scl_import * scl,QList<XmlObject*> list,oe_ied * m_ied)
{
	net_id = 0;
	sub_no = scl->getSubstationNo();
	name = "";
	type = 0;
	desc = "";
	bitrate = 100;

	map_type.insert("8-MMS",3);
	map_type.insert("SMV",1);
	map_type.insert("IECGOOSE",2);

	sclImport = scl;
	document = list;
	ied = m_ied;
}

na_sub_network::~na_sub_network(void)
{
}

bool na_sub_network::execute(QString & error)
{
	bool b = true;

	if (DB_MASTER_TYPE == DB_MYSQL)
	{
		if (ied)
			na_comm_device_port::db_delete(1,ied->getIedNo());	//1ÎªIEDÉè±¸¶Ë¿Ú
	}
	
	subs += document.at(0)->findChildrenDeep("SubNetwork");
	if (subs.count())
	{
		foreach (subNetwork_object,subs)
		{
			b = db_exits(subNetwork_object,error);
			if (!b)
			{
				//b = db_insert_uicfg_wnd(subNetwork_object,error);
				//if (b)
					b = db_insert(subNetwork_object,error);
					if (!b)
						break;
			}

			if (DB_MASTER_TYPE == DB_MYSQL)
			{
				if (b && ied != NULL)
				{
					//if (type == 3)
					//{
					na_comm_device_port device_port(document,this,ied);
					b = device_port.execute(error);
					//	if (!b)
					//		break;
					//}
				}
				//else
				//	break;
			}
		}
	}

	return b;
}

bool na_sub_network::db_exits(XmlObject * object,QString & error)
{
	S_UNUSED(error);

	bool		b = true;
	SRecordset	rs;
	
	if (!get_name(object))
		return false;
	if (!get_sub_no(object))
		return false;
	if (!get_type(object))
		return false;

	SString sql = SString::toFormat("SELECT net_id,uiwnd_sn FROM t_na_sub_network "
		"WHERE net_name='%s' and net_type=%d and sub_no=%d",name.data(),type,sub_no);
	int iRet = DB->Retrieve(sql,rs);
	if (iRet <= 0)
	{
		//error = "SQLÓï¾äÖ´ÐÐ´íÎó£º" + sql;
		b = false;
	}
	else if (iRet > 0)
	{
		net_id = rs.GetValue(0,0).toInt();
		b = true;

		//uiwnd_sn = rs.GetValue(0,1).toInt();
		//if (!uiwnd_sn)
		//{
		//	b = db_insert_uicfg_wnd(object,error);
		//	if (b)
		//		b = db_update_uiwnd_sn(object,error);
		//}
	}

	return b;
}

bool na_sub_network::db_insert_uicfg_wnd(XmlObject * object,QString & error)
{
	bool		b = true;
	SString		sql;
	SRecordset	rs;

	sql = "SELECT MAX(wnd_sn) FROM t_ssp_uicfg_wnd";
	int iRet = DB->Retrieve(sql,rs);
	if (iRet < 0)
	{
		error = "SQLÓï¾äÖ´ÐÐ´íÎó£º" + sql;
		return false;
	}
	else if (iRet == 0)
		uiwnd_sn = 1;
	else if (iRet > 0)
	{
		uiwnd_sn = rs.GetValue(0,0).toInt();
		uiwnd_sn++;
	}

	int time = (int)SDateTime::getNowSoc();

	if (!get_name(object))
		return false;
	name = name + "ÍøÂçÍØÆËÍ¼";

	sql.sprintf("INSERT INTO t_ssp_uicfg_wnd \
				(wnd_sn,wnd_name,create_author,create_time,modify_author,modify_time,wnd_type,svg_file,dataset,ds_cond,refresh_sec) \
				VALUES (%d,'%s',null,%d,null,%d,%d,null,null,null,%d)",
				uiwnd_sn,name.data(),time,time,1,5);
	b = DB->Execute(sql);
	if (!b)
	{
		error = "SQLÓï¾äÖ´ÐÐ´íÎó£º" + sql;
		return b;
	}

	if (m_bMDB)
	{
		b = MDB->Execute(sql);
		if (!b)
			error = "ÄÚ´æ¿âSQLÓï¾äÖ´ÐÐ´íÎó£º" + sql;
	}

	return b;
}

bool na_sub_network::db_update_uiwnd_sn(XmlObject * object,QString & error)
{
	S_UNUSED(object);

	bool	b = false;
	SString sql;

	sql.sprintf("UPDATE t_na_sub_network SET uiwnd_sn=%d WHERE net_id=%d",uiwnd_sn,net_id);
	b = DB->Execute(sql);
	if (!b)
	{
		error = "SQLÓï¾äÖ´ÐÐ´íÎó£º" + sql;
		return b;
	}

	if (m_bMDB)
	{
		b = MDB->Execute(sql);
		if (!b)
			error = "ÄÚ´æ¿âSQLÓï¾äÖ´ÐÐ´íÎó£º" + sql;
	}

	return b;
}

bool na_sub_network::db_insert(XmlObject * object,QString & error)
{
	bool	b = false;
	SString sql;

	if (!get_net_id(object,error))
		return false;
	if (!get_sub_no(object))
		return false;
	if (!get_name(object))
		return false;
	if (!get_type(object))
		return false;
	if (!get_desc(object))
		return false;
	if (!get_bitrate(object))
		return false;

	sql.sprintf("INSERT INTO t_na_sub_network (net_id,sub_no,net_name,net_type,net_desc,net_bitrate,uiwnd_sn) "
		"VALUES (%d,%d,'%s',%d,'%s',%d,%d)",
		net_id,sub_no,name.data(),type,desc.data(),bitrate,uiwnd_sn);

	b = DB->Execute(sql);
	if (!b)
	{
		error = "SQLÓï¾äÖ´ÐÐ´íÎó£º" + sql;
		return b;
	}

	if (m_bMDB)
	{
		b = MDB->Execute(sql);
		if (!b)
			error = "ÄÚ´æ¿âSQLÓï¾äÖ´ÐÐ´íÎó£º" + sql;
	}

	return b;
}

bool na_sub_network::get_net_id(XmlObject * object,QString & error)
{
	S_UNUSED(object);

	bool		b = true;
	SRecordset	rs;

	SString sql = SString::toFormat("SELECT MAX(net_id) FROM t_na_sub_network");
	int iRet = DB->Retrieve(sql,rs);
	if (iRet < 0)
	{
		error = "SQLÓï¾äÖ´ÐÐ´íÎó£º" + sql;
		b = false;
	}
	else if (iRet == 0)
	{
		net_id = 1;
		b = true;
	}
	else if (iRet > 0)
	{
		net_id = rs.GetValue(0,0).toInt();
		net_id++;
		b = true;
	}

	return b;
}

bool na_sub_network::get_sub_no(XmlObject * object)
{
	S_UNUSED(object);

	sub_no = sub_no;

	return true;
}

bool na_sub_network::get_name(XmlObject * object)
{
	bool b = true;

	name = object->attrib("name").toStdString();

	return b;
}

bool na_sub_network::get_type(XmlObject * object)
{
	bool b = true;

	type = map_type[object->attrib("type")];

	return b;
}

bool na_sub_network::get_desc(XmlObject * object)
{
	bool b = true;

	desc = object->attrib("desc").toStdString();

	return b;
}

bool na_sub_network::get_bitrate(XmlObject * object)
{
	bool b = true;

	if (type == 3)
	{
		XmlObject *obj = object->findChildDeep("BitRate");
		if (obj)
			bitrate = obj->text.toInt();
	}
	else
		bitrate = 0;

	return b;
}

