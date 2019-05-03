#ifndef _OE_MMS_AP_H_
#define _OE_MMS_AP_H_

#include <QtGui>
#include "SApplication.h"
#include "sk_database.h"
#include "xmlobject.h"

class view_plugin_scl_import;
class oe_ied;

class oe_mms_ap
{
public:
	oe_mms_ap(view_plugin_scl_import * scl,QList<XmlObject*> list,oe_ied *ied);
	~oe_mms_ap(void);

	bool execute(QString & error,QString &warnText);

private:
	int		net_id;
	int		ied_no;
	int		sub_no;
	SString	ip_addr;
	SString	ip_subnet;
	SString ip_gateway;
	SString	osi_ap_title;
	SString	osi_ap_invoke;
	SString	osi_ae_q;
	SString	osi_ae_invoke;
	SString	osi_psel;
	SString	osi_ssel;
	SString	osi_tsel;

	view_plugin_scl_import			*sclImport;
	QList<XmlObject*>	document;
	oe_ied				*m_ied;

	XmlObject *subnetwork_obj;

private:
	bool get_net_id(XmlObject * object,QString & error,QString &warnText);
	bool get_ied_no(XmlObject * object,QString & error,QString &warnText);
	bool get_sub_no(XmlObject * object,QString & error,QString &warnText);
	bool get_ip_addr(XmlObject * object,QString & error,QString &warnText);
	bool get_ip_subnet(XmlObject * object,QString & error,QString &warnText);
	bool get_ip_gateway(XmlObject * object,QString & error,QString &warnText);
	bool get_osi_ap_title(XmlObject * object,QString & error,QString &warnText);
	bool get_osi_ap_invoke(XmlObject * object,QString & error,QString &warnText);
	bool get_osi_ae_q(XmlObject * object,QString & error,QString &warnText);
	bool get_osi_ae_invoke(XmlObject * object,QString & error,QString &warnText);
	bool get_osi_psel(XmlObject * object,QString & error,QString &warnText);
	bool get_osi_ssel(XmlObject * object,QString & error,QString &warnText);
	bool get_osi_tsel(XmlObject * object,QString & error,QString &warnText);

	void initParam();
	bool insertMmsAP(XmlObject * object,QString & error,QString &warnText);
};

#endif //_OE_MMS_AP_H_