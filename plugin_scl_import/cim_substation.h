#ifndef _CIM_SUBSTATION_H_
#define _CIM_SUBSTATION_H_

#include <QtGui>
#include "SApplication.h"
#include "sk_database.h"
#include "xmlobject.h"

class cim_substation
{
public:
	cim_substation(QList<XmlObject*> list);
	~cim_substation(void);

	bool execute(QString & error);

private:
	int		sub_no;
	int		cmp_no;
	SString	uri;
	SString	name;
	SString address;
	int		state;
	int		comstate;
	int		vlevel;
	int		join_time;
	SString ip_addr_a;
	SString ip_addr_b;
	int		svr_port;
	SString svg_file;

	QList<XmlObject*>	document;

private:
	bool	get_sub_no(bool &bInsert,QString & error);
	bool	get_cmp_no();
	bool	get_uri();
	bool	get_name(XmlObject * object);
	bool    get_address();
	bool	get_state();
	bool	get_comstate();
	bool	get_vlevel();
	bool	get_join_time();
	bool	get_ip_addr_a();
	bool	get_ip_addr_b();
	bool	get_svr_port();
	bool	get_svg_file();

	bool	db_insert(XmlObject * object,QString & error);

};

#endif // _CIM_SUBSTATION_H_