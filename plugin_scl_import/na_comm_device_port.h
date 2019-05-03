#ifndef _NA_COMM_DEVICE_PORT_H_
#define _NA_COMM_DEVICE_PORT_H_

#include <QtGui>
#include "SApplication.h"
#include "sk_database.h"
#include "xmlobject.h"
#include "na_sub_network.h"

class oe_ied;

class na_comm_device_port
{
public:
	na_comm_device_port(QList<XmlObject*> list,na_sub_network *m_sub_network,oe_ied * m_ied);
	~na_comm_device_port(void);

public:
	bool	execute(QString & error);
	static bool	db_delete(int cls, int no);

private:
	int		net_id;
	int		dev_cls;
	int		dev_id;
	int		port_id;
	SString	port_name;
	int		port_type;
	int		if_type;
	SString	ip_addr;
	SString mac_addr;
	int		port_state;
	int		state_confirm;
	int		port_flow;

	QList<XmlObject*>	document;
	oe_ied				*ied;
	na_sub_network		*sub_network;
	XmlObject			*sub_network_object;
	XmlObject			*port_object;

	QList<XmlObject*>	ports;

private:
	bool		get_net_id(XmlObject * object);
	bool		get_dev_cls(XmlObject * object);
	bool		get_dev_id(XmlObject * object,QString & error);
	bool		get_port_id(XmlObject * object,QString & error);
	bool		get_port_name(XmlObject * object);
	bool		get_port_type(XmlObject * object);
	bool		get_if_type(XmlObject * object);
	bool		get_ip_addr(XmlObject * object);
	bool		get_mac_addr(XmlObject * object);
	bool		get_port_state(XmlObject * object);
	bool		get_state_confirm(XmlObject * object);
	bool		get_port_flow(XmlObject * object);

	bool		db_exits(XmlObject * object);
	bool		db_insert(XmlObject * object,QString & error);
	bool		db_update(XmlObject * object,QString & error);

	bool		db_replace(XmlObject * object,QString & error);
	bool		smv_db_replace(XmlObject * object,QString & error);
	bool		goose_db_replace(XmlObject * object,QString & error);
	bool		mms_db_replace(XmlObject * object,QString & error);
};

#endif // _NA_COMM_DEVICE_PORT_H_