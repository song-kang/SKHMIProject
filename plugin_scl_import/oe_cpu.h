#ifndef _OE_CPU_H_
#define _OE_CPU_H_

#include <QtGui>
#include "SApplication.h"
#include "sk_database.h"
#include "xmlobject.h"

class view_plugin_scl_import;
class oe_ied;

class oe_cpu
{
public:
	oe_cpu(view_plugin_scl_import * scl,QList<XmlObject*> list,oe_ied *ied);
	~oe_cpu(void);

public:
	oe_ied	*ied;

public:
	bool		execute(QString & error,QString &warnText);
	void		setCheckStateS1(Qt::CheckState check) { checkStateS1 = check; }
	void		setCheckStateM1(Qt::CheckState check) { checkStateG1 = check; }
	void		setCheckStateG1(Qt::CheckState check) { checkStateM1 = check; }
	XmlObject*	getIedObject() { return ied_object; }
	XmlObject*	getObject() { return cpu_object; }
	oe_ied*		getIed() { return ied; }
	QString		getIedName() { return iedName; }
	int			getIedNo() { return ied_no; }
	int			getCpuNo() { return cpu_no; }
	SString		getCpuMmsPath() { return mms_path; }

private:
	int		ied_no;
	int		cpu_no;
	SString	name;
	SString	version;
	SString	crc;
	SString	version_time;
	SString	mms_path;

	view_plugin_scl_import			*sclImport;
	QList<XmlObject*>	document;
	QString				iedName;
	XmlObject			*ied_object;
	XmlObject			*cpu_object;
	//XmlObject			*object_s1;
	//XmlObject			*object_g1;
	//XmlObject			*object_m1;

	Qt::CheckState		checkStateS1;
	Qt::CheckState		checkStateG1;
	Qt::CheckState		checkStateM1;

	QList<XmlObject*>	cpus;

private:
	bool	get_ied_no(XmlObject * object);
	bool	get_cpu_no(XmlObject * object,QString & error);
	bool	get_name(XmlObject * object);
	bool	get_version(XmlObject * object);
	bool	get_crc(XmlObject * object);
	bool	get_version_time(XmlObject * object);
	bool	get_mms_path(XmlObject * object);

	bool	db_insert(XmlObject * object,QString & error);
	bool	addExtendCpu(QString & error);
	bool	addExtendGroup(QString & error);
	bool	addExtendElement(QString & error);
};

#endif // _OE_CPU_H_