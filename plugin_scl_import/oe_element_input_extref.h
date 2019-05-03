#ifndef _OE_ELEMENT_INPUT_EXTREF_H_
#define _OE_ELEMENT_INPUT_EXTREF_H_

#include <QtGui>
#include "SApplication.h"
#include "sk_database.h"
#include "xmlobject.h"

class view_plugin_scl_import;
class oe_ied;

class oe_element_input_extref
{
public:
	oe_element_input_extref(view_plugin_scl_import * scl,QList<XmlObject*> list,oe_ied *ied);
	~oe_element_input_extref(void);

	bool execute(QString & error,QString &warnText);

private:
	int		ied_no;
	int		int_no;
	SString	int_addr;
	SString	int_desc;
	SString ied_name;
	SString	do_name;
	SString	ln_inst;
	SString	ln_class;
	SString	da_name;
	SString	ld_inst;
	SString prefix;
	int		type;
	int		link_ied_no;
	int		link_cpu_no;
	int		link_group_no;
	int		link_entry;
	SString	strap;
	SString	link_strap;
	SString port;
	SString link_port;

	view_plugin_scl_import			*sclImport;
	QList<XmlObject*>	document;
	oe_ied				*m_ied;

private:
	bool get_ied_no(XmlObject * object,QString & error,QString &warnText);
	bool get_int_no(XmlObject * object,QString & error,QString &warnText);
	bool get_int_addr(XmlObject * object,QString & error,QString &warnText);
	bool get_int_desc(XmlObject * object,QString & error,QString &warnText);
	bool get_ied_name(XmlObject * object,QString & error,QString &warnText);
	bool get_do_name(XmlObject * object,QString & error,QString &warnText);
	bool get_ln_inst(XmlObject * object,QString & error,QString &warnText);
	bool get_ln_class(XmlObject * object,QString & error,QString &warnText);
	bool get_da_name(XmlObject * object,QString & error,QString &warnText);
	bool get_ld_inst(XmlObject * object,QString & error,QString &warnText);
	bool get_prefix(XmlObject * object,QString & error,QString &warnText);
	bool get_type(XmlObject * object,QString & error,QString &warnText);
	bool get_link_ied_no(XmlObject * object,QString & error,QString &warnText);
	bool get_link_cpu_no(XmlObject * object,QString & error,QString &warnText);
	bool get_link_group_no(XmlObject * object,QString & error,QString &warnText);
	bool get_link_entry(XmlObject * object,QString & error,QString &warnText);
	bool get_strap(XmlObject * object,QString & error,QString &warnText);
	bool get_link_strap(XmlObject * object,QString & error,QString &warnText);
	bool get_port(XmlObject * object,QString & error,QString &warnText);
	bool get_link_port(XmlObject * object,QString & error,QString &warnText);

	void initParam();
	bool insertExtRef(XmlObject * object,SString &sql,QString & error,QString &warnText);
};

#endif //_OE_ELEMENT_INPUT_EXTREF_H_