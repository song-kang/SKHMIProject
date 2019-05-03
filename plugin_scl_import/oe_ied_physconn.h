#ifndef _OE_IED_PHYSCONN_H_
#define _OE_IED_PHYSCONN_H_

#include <QtGui>
#include "SApplication.h"
#include "sk_database.h"
#include "xmlobject.h"

class view_plugin_scl_import;
class oe_ied;

class oe_ied_physconn
{
public:
	oe_ied_physconn(view_plugin_scl_import * scl,QList<XmlObject*> list,oe_ied *ied);
	~oe_ied_physconn(void);

	bool execute(QString & error,QString &warnText);

private:
	int		net_id;
	int		ied_no;
	SString ap_name;
	SString	physconn_type;
	SString	port;
	SString	plug;
	SString	type;
	SString cable;
	SString	rem_ied_name;
	SString	rem_ied_port;

	view_plugin_scl_import	*sclImport;
	QList<XmlObject*>		document;
	oe_ied					*m_ied;

	XmlObject *subnetwork_obj;

private:
	bool get_net_id(XmlObject * object,QString & error,QString &warnText);
	bool get_ied_no(XmlObject * object,QString & error,QString &warnText);
	bool get_ap_name(XmlObject * object,QString & error,QString &warnText);
	bool get_physconn_type(XmlObject * object,QString & error,QString &warnText);
	bool get_port(XmlObject * object,QString & error,QString &warnText);
	bool get_plug(XmlObject * object,QString & error,QString &warnText);
	bool get_type(XmlObject * object,QString & error,QString &warnText);
	bool get_cable(XmlObject * object,QString & error,QString &warnText);
	bool get_rem_ied_name(XmlObject * object,QString & error,QString &warnText);
	bool get_rem_ied_port(XmlObject * object,QString & error,QString &warnText);

	void initParam();
	bool insertPhysconn(XmlObject * object,QString & error,QString &warnText);
};

#endif //_OE_IED_PHYSCONN_H_