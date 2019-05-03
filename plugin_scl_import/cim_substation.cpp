#include "cim_substation.h"
#include "view_plugin_scl_import.h"

cim_substation::cim_substation(QList<XmlObject*> list)
{
	sub_no = 0;
	cmp_no = 0;
	uri = "";
	name = "";
	state = 0;
	comstate = 0;
	vlevel = 0;
	join_time = 0;
	ip_addr_a = "";
	ip_addr_b = "";
	svr_port = 0;
	svg_file = "";

	document = list;
}

cim_substation::~cim_substation(void)
{
}

bool cim_substation::execute(QString & error)
{
	bool b = false;
	bool bInsert = false;

	if (!get_sub_no(bInsert,error))
		return false;
	if (!get_cmp_no())
		return false;
	if (!get_uri())
		return false;
	if (!get_name(document.at(0)))
		return false;
	if (!get_state())
		return false;
	if (!get_comstate())
		return false;
	if (!get_vlevel())
		return false;
	if (!get_join_time())
		return false;
	if (!get_ip_addr_a())
		return false;
	if (!get_ip_addr_b())
		return false;
	if (!get_svr_port())
		return false;
	if (!get_svg_file())
		return false;

	SString sql;
	if (bInsert)
	{
		sql.sprintf("INSERT INTO t_cim_substation (sub_no,cmp_no,uri,name,address,state,comstate,"
			"vlevel,join_time,ip_addr_a,ip_addr_b,svr_port,svg_file_dq) "
			"VALUES (%d,%d,'%s','%s','%s',%d,%d,%d,%d,'%s','%s',%d,'%s')",
			sub_no,cmp_no,uri.data(),name.data(),address.data(),state,comstate,vlevel,join_time,
			ip_addr_a.data(),ip_addr_b.data(),svr_port,NULL);
	}
	else
	{
		sql.sprintf("update t_cim_substation set name='%s' where sub_no=%d",name.data(),sub_no);
	}

	b = DB->Execute(sql);
	if (!b)
	{
		error = "SQLÓï¾äÖ´ĞĞ´íÎó£º" + sql;
		return b;
	}

	if (m_bMDB)
	{
		b = MDB->Execute(sql);
		if (!b)
			error = "ÄÚ´æ¿âSQLÓï¾äÖ´ĞĞ´íÎó£º" + sql;
	}
	
	return b;
}

bool cim_substation::get_sub_no(bool &bInsert,QString & error)
{
	SString		sql;
	SRecordset	rs;

	sub_no = 1;
	sql.sprintf("select count(*) from t_cim_substation where sub_no=%d ",sub_no);
	int iRet = DB->Retrieve(sql,rs);
	if (iRet < 0)
	{
		error = "SQLÓï¾äÖ´ĞĞ´íÎó£º" + sql;
		return false;
	}
	else if (iRet > 0)
	{
		int count = rs.GetValue(0,0).toInt();
		if (!count)
			bInsert = true;
	}

	return true;
}

bool cim_substation::get_cmp_no()
{
	cmp_no = 1;

	return true;
}

bool cim_substation::get_uri()
{
	uri = "";

	return true;
}

bool cim_substation::get_name(XmlObject * object)
{
	XmlObject *obj = object->findChildDeep("Substation");

	if (!obj)
		return false;

	name = obj->attrib("desc").toStdString();

	return true;
}

bool cim_substation::get_address()
{
	address = "";

	return true;
}

bool cim_substation::get_state()
{
	state = 0;

	return true;
}

bool cim_substation::get_comstate()
{
	comstate = 0;

	return true;
}

bool cim_substation::get_vlevel()
{
	vlevel = 220;

	return true;
}

bool cim_substation::get_join_time()
{
	join_time = 0;

	return true;
}

bool cim_substation::get_ip_addr_a()
{
	ip_addr_a = "";

	return true;
}

bool cim_substation::get_ip_addr_b()
{
	ip_addr_b = "";

	return true;
}

bool cim_substation::get_svr_port()
{
	svr_port = 0;

	return true;
}

bool cim_substation::get_svg_file()
{
	svg_file = "";

	return true;
}

