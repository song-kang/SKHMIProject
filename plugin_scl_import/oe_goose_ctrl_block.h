#ifndef _OE_GOOSE_CTRL_BLOCK_H_
#define _OE_GOOSE_CTRL_BLOCK_H_

#include <QtGui>
#include "SApplication.h"
#include "sk_database.h"
#include "xmlobject.h"

class view_plugin_scl_import;
class oe_ied;
class oe_cpu;
class oe_group;

class oe_goose_ctrl_block
{
public:
	oe_goose_ctrl_block(view_plugin_scl_import * scl,QList<XmlObject*> list,oe_group *group);
	~oe_goose_ctrl_block(void);

	bool execute(QString & error,QString &warnText);
	oe_ied *getIed() { return m_ied; }
	oe_cpu *getCpu() { return m_cpu; }
	oe_group *getGroup() { return m_group; }

	int getCbNo() { return cb_no; }
	SString getCbName() { return cb_name; }

private:
	int		ied_no;
	int		cb_no;
	int		cpu_no;
	int		group_no;
	SString	cb_name;
	SString	cb_desc;
	SString conf_rev;
	SString	appid;
	SString datset;	

	view_plugin_scl_import			*sclImport;
	QList<XmlObject*>	document;
	oe_ied				*m_ied;
	oe_cpu				*m_cpu;
	oe_group			*m_group;

private:
	bool get_ied_no(XmlObject * object,QString & error,QString &warnText);
	bool get_cb_no(XmlObject * object,QString & error,QString &warnText);
	bool get_cpu_no(XmlObject * object,QString & error,QString &warnText);
	bool get_group_no(XmlObject * object,QString & error,QString &warnText);
	bool get_cb_name(XmlObject * object,QString & error,QString &warnText);
	bool get_cb_desc(XmlObject * object,QString & error,QString &warnText);
	bool get_conf_rev(XmlObject * object,QString & error,QString &warnText);
	bool get_appid(XmlObject * object,QString & error,QString &warnText);
	bool get_datset(XmlObject * object,QString & error,QString &warnText);

	void initParam();
	bool insertGseCB(XmlObject * object,QString & error,QString &warnText);
};

#endif //_OE_GOOSE_CTRL_BLOCK_H_