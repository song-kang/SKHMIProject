#ifndef _OE_ELEMENT_STATE_H_
#define _OE_ELEMENT_STATE_H_

#include <QtGui>
#include "SApplication.h"
#include "sk_database.h"
#include "xmlobject.h"

#define	STATE_DPS		1
#define	STATE_SPS		2
#define	STATE_ENUM		3
#define	STATE_MEASURE	4

class view_plugin_scl_import;
class oe_ied;
class oe_group;

class oe_element_state
{
public:
	oe_element_state(view_plugin_scl_import * scl,QList<XmlObject*> list,oe_group *m_group);
	~oe_element_state(void);

public:

public:
	bool	execute(QString & error);

private:
	int		ied_no;
	int		cpu_no;
	int		group_no;
	int		entry;
	SString	name;
	int		fun;
	int		inf;
	int		type;
	int		evt_cls;
	int		val_type;
	int     level;
	SString	on_desc;
	SString	off_desc;
	SString unknown_desc;
	int		inver;
	int		val;
	SString	measure_val;
	int		soc;
	int		usec;
	SString mms_path;
	SString da_name;
	SString detail;

	view_plugin_scl_import			*sclImport;
	QList<XmlObject*>	document;
	oe_ied				*ied;
	QString				iedName;
	oe_group			*group;
	XmlObject			*group_object;
	XmlObject			*element_object;

	QList<XmlObject*>	elements;
	SString				sql;
	XmlObject			*doi_Object;

	int		funBase;	//用于处理entry数大于255情况

private:
	bool		get_ied_no(XmlObject * object);
	bool		get_cpu_no(XmlObject * object);
	bool		get_group_no(XmlObject * object);
	bool		get_entry(XmlObject * object);
	bool		get_name(XmlObject * object);
	bool		get_fun(XmlObject * object);
	bool		get_inf(XmlObject * object);
	bool		get_type(XmlObject * object);
	bool		get_evt_cls(XmlObject * object);
	bool		get_val_type(XmlObject * object);
	bool		get_level(XmlObject * object);
	bool		get_on_desc(XmlObject * object);
	bool		get_off_desc(XmlObject * object);
	bool		get_unknown_desc(XmlObject * object);
	bool		get_inver(XmlObject * object);
	bool		get_val(XmlObject * object);
	bool		get_measure_val(XmlObject * object);
	bool		get_soc(XmlObject * object);
	bool		get_usec(XmlObject * object);
	bool		get_mms_path(XmlObject * object);
	bool		get_detail(XmlObject * object);

	bool		db_insert(XmlObject * object);
	XmlObject	*getDoiObjectByFcda(XmlObject * obj);
	void		init_fun();

	int GetSPS_DPS(XmlObject *object);
};

#endif // _OE_ELEMENT_STATE_H_