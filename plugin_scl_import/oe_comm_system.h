#ifndef _OE_COMM_SYSTEM_H_
#define _OE_COMM_SYSTEM_H_

#include <QtGui>
#include "SApplication.h"
#include "ssp_database.h"
#include "xmlobject.h"

class view_na_zj;

class oe_comm_system
{
public:
	struct na_device_type_t 
	{
		int type_no;
		int dev_cls; //对应nam_score_cls_t.dev_cls
		int score_cls; //对应nam_score_cls_t.score_cls
		QString wildcard;
	};
public:
	oe_comm_system(view_na_zj *nam);
	~oe_comm_system(void);

	QList<XmlObject*> getDocument() { return document; }
	int getSubStationNo() { return substationNo; }
	view_na_zj *getPNam() { return pNam; }

	bool execute(QString & error);

private:
	int		comm_id;
	int		sub_no;
	int		dev_type;
	SString	com_name;
	SString	model;
	SString	manufacturer;
	int		mgr_ip;
	int		mgr_ip2;
	int		mgr_ip3;
	int		mgr_ip4;
	int		mgr_port;
	SString	version;
	int		comm_addr;
	float	weight;
	SString	com_desc;
	SString ips;

	view_na_zj *pNam;
	QList<XmlObject*> document;
	int	substationNo;

	QList<na_device_type_t*> lstNaDeviceType;

private:
	bool init_device_type(QString & error);

	bool get_comm_id(XmlObject *object,QString &error);
	bool get_sub_no(XmlObject *object,QString &error);
	bool get_dev_type(XmlObject *object,QString &error);
	bool get_com_name(XmlObject *object,QString &error);
	bool get_model(XmlObject *object,QString &error);
	bool get_manufacturer(XmlObject *object,QString &error);
	bool get_mgr_ip(XmlObject *object,QString &error);
	bool get_mgr_ip2(XmlObject *object,QString &error);
	bool get_mgr_ip3(XmlObject *object,QString &error);
	bool get_mgr_ip4(XmlObject *object,QString &error);
	bool get_mgr_port(XmlObject *object,QString &error);
	bool get_version(XmlObject *object,QString &error);
	bool get_comm_addr(XmlObject *object,QString &error);
	bool get_weight(XmlObject *object,QString &error);
	bool get_com_desc(XmlObject *object,QString &error);
	bool get_ips(XmlObject *object,QString &error);

	bool insertHost(XmlObject *object,QString &error);
	bool systemIsExist(bool &isExist,QString &error);
};

#endif // _OE_COMM_SYSTEM_H_