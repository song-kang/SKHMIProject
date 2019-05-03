#ifndef _OE_ELEMENT_GENERAL_H_
#define _OE_ELEMENT_GENERAL_H_

#include <QtGui>
#include "SApplication.h"
#include "sk_database.h"
#include "xmlobject.h"

#define VAL_TYPE_STRING			1
#define VAL_TYPE_INTEGER		3
#define VAL_TYPE_FLOAT			7

class view_plugin_scl_import;
class oe_ied;
class oe_group;

class oe_element_general
{
public:
	oe_element_general(view_plugin_scl_import * scl,QList<XmlObject*> list,oe_group *m_group);
	~oe_element_general(void);

public:

public:
	bool	execute(QString & error);

private:
	int		ied_no;
	int		cpu_no;
	int		group_no;
	int		entry;
	SString	name;
	int		val_type;
	SString	dime;
	float	maxval;
	float	minval;
	float	stepval;
	int		precision_n;
	int		precision_m;
	int		itemtype;
	float	factor;
	float	offset;
	float	threshold;
	float	smooth;
	SString	current_val;
	SString	reference_val;
	SString mms_path;
	SString da_name;

	view_plugin_scl_import			*sclImport;
	QList<XmlObject*>	document;
	oe_ied				*ied;
	QString				iedName;
	oe_group			*group;
	XmlObject			*group_object;
	XmlObject			*element_object;

	QList<XmlObject*>	elements;
	SString				sql;
	SString				sqlMdb;
	XmlObject			*doi_Object;
	XmlObject			*dataTypeTemplates;
	bool isHaveAng;

private:
	bool		get_ied_no(XmlObject * object);
	bool		get_cpu_no(XmlObject * object);
	bool		get_group_no(XmlObject * object);
	bool		get_entry(XmlObject * object);
	bool		get_name(XmlObject * object);
	bool		get_val_type(XmlObject * object);
	bool		get_dime(XmlObject * object);
	bool		get_maxval(XmlObject * object);
	bool		get_minval(XmlObject * object);
	bool		get_stepval(XmlObject * object);
	bool		get_precision_n(XmlObject * object);
	bool		get_precision_m(XmlObject * object);
	bool		get_itemtype(XmlObject * object);
	bool		get_factor(XmlObject * object);
	bool		get_offset(XmlObject * object);
	bool		get_threshold(XmlObject * object);
	bool		get_smooth(XmlObject * object);
	bool		get_current_val(XmlObject * object);
	bool		get_reference_val(XmlObject * object);
	bool		get_mms_path(XmlObject * object);

	bool		db_insert(XmlObject * object);
	XmlObject	*getDoiObjectByFcda(XmlObject * obj);
	XmlObject	*getDoTypeObject(XmlObject * lntype_object,QString doName);
	QString		getDAbTypeObject(XmlObject * dotype_object,QString daName,QString &bdaName);
	QString		GetDaName(XmlObject * fcda,QString &err);
	QString		GetDaNameByFC(XmlObject * fcda,QString fc, QString &err);
};

#endif // _OE_ELEMENT_GENERAL_H_