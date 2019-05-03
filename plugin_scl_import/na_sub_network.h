#ifndef _NA_SUB_NETWORK_H_
#define _NA_SUB_NETWORK_H_

#include <QtGui>
#include "SApplication.h"
#include "sk_database.h"
#include "xmlobject.h"

class view_plugin_scl_import;
class oe_ied;

class na_sub_network
{
public:
	na_sub_network(view_plugin_scl_import * scl,QList<XmlObject*> list,oe_ied * m_ied);
	~na_sub_network(void);

public:

public:
	bool		execute(QString & error);
	XmlObject*	getObject() { return subNetwork_object; }
	int			getNetId() { return net_id; }
	int			getNetType() { return type; }

private:
	int		net_id;
	int		sub_no;
	SString	name;
	int		type;
	SString	desc;
	int		bitrate;
	int		uiwnd_sn;

	view_plugin_scl_import			*sclImport;
	QList<XmlObject*>	document;
	oe_ied				*ied;
	XmlObject			*subNetwork_object;

	QList<XmlObject*>	subs;
	QMap<QString,int>	map_type;

private:
	bool	get_net_id(XmlObject * object,QString & error);
	bool	get_sub_no(XmlObject * object);
	bool	get_name(XmlObject * object);
	bool	get_type(XmlObject * object);
	bool	get_desc(XmlObject * object);
	bool	get_bitrate(XmlObject * object);

	bool	db_exits(XmlObject * object,QString & error);
	bool	db_insert(XmlObject * object,QString & error);
	bool	db_insert_uicfg_wnd(XmlObject * object,QString & error);
	bool	db_update_uiwnd_sn(XmlObject * object,QString & error);
};

#endif // _NA_SUB_NETWORK_H_