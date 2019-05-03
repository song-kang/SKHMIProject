#ifndef _OE_SMV_AP_H_
#define _OE_SMV_AP_H_

#include <QtGui>
#include "SApplication.h"
#include "sk_database.h"
#include "xmlobject.h"

class view_plugin_scl_import;
class oe_ied;
class oe_cpu;
class oe_group;
class oe_smv_ctrl_block;

class oe_smv_ap
{
public:
	oe_smv_ap(view_plugin_scl_import * scl,QList<XmlObject*> list,oe_smv_ctrl_block *scb);
	~oe_smv_ap(void);

	bool execute(QString & error,QString &warnText);

private:
	int		net_id;
	int		ied_no;
	int		cb_no;
	int		sub_no;
	SString	cb_name;
	SString	ld_inst;
	SString mac_address;
	SString	appid;
	int		vlan_priority;
	int		vlan_id;

	view_plugin_scl_import			*sclImport;
	QList<XmlObject*>	document;
	oe_ied				*m_ied;
	oe_cpu				*m_cpu;
	oe_group			*m_group;
	oe_smv_ctrl_block	*m_scb;

	XmlObject *subnetwork_obj;
	XmlObject *connectedap_obj;

private:
	bool get_net_id(XmlObject * object,QString & error,QString &warnText);
	bool get_ied_no(XmlObject * object,QString & error,QString &warnText);
	bool get_cb_no(XmlObject * object,QString & error,QString &warnText);
	bool get_sub_no(XmlObject * object,QString & error,QString &warnText);
	bool get_cb_name(XmlObject * object,QString & error,QString &warnText);
	bool get_ld_inst(XmlObject * object,QString & error,QString &warnText);
	bool get_mac_address(XmlObject * object,QString & error,QString &warnText);
	bool get_appid(XmlObject * object,QString & error,QString &warnText);
	bool get_vlan_priority(XmlObject * object,QString & error,QString &warnText);
	bool get_vlan_id(XmlObject * object,QString & error,QString &warnText);

	void initParam();
	bool insertSmvAP(XmlObject * object,QString & error,QString &warnText);
};

#endif //_OE_SMV_AP_H_