#ifndef _OE_GROUP_H_
#define _OE_GROUP_H_

#include <QtGui>
#include "SApplication.h"
#include "sk_database.h"
#include "xmlobject.h"

class view_plugin_scl_import;
class oe_cpu;

enum
{
	GROUP_UNKNOWN = 0,
	GROUP_SETTING,
	GROUP_SETTING_AREA,
	GROUP_ANALOGUE,
	GROUP_SWITCH,
	GROUP_SOFT_STRAP,
	GROUP_EVENT,
	GROUP_ALARM,
	GROUP_FAULT,
	GROUP_RELAY_REC,
	GROUP_CONTROL,
	GROUP_ADJUSTING,
	GROUP_COMPLEX_ANALOGUE,
	GROUP_COMPLEX_SWITCH,
	GROUP_GOOSE,
	GROUP_SV,
	GROUP_LOG,
	GROUP_ACCOUNT,
	GROUP_NO_IMPORT
};

class oe_group
{
public:
	oe_group(view_plugin_scl_import * scl,QList<XmlObject*> list,oe_cpu *m_cpu);
	~oe_group(void);

public:
	friend class view_plugin_scl_import;

public:
	bool		execute(QString & error,QString &warnText);

	XmlObject*  getCpuObject() { return cpu_object; }
	XmlObject*	getObject() { return group_object; }
	QString		getIedName() { return iedName; }
	int			getIedNo() { return ied_no; }
	int			getCpuNo() { return cpu_no; }
	int			getGroupNo() { return group_no; }
	SString		getGroupMmsPath() { return mms_path; }
	int			getGroupType() { return type; }
	oe_cpu*		getCpu() { return cpu; }

private:
	int		ied_no;
	int		cpu_no;
	int		group_no;
	SString	name;
	int		type;
	SString	mms_path;
	SString report_ln;
	SString	report_name;
	SString report_datset;
	int		report_intgPd;
	SString report_rptID;
	SString report_confRev;
	SString	report_buffered;
	int		report_bufTime;
	SString report_desc;
	SString	trgOps_qchg;
	SString	trgOps_period;
	SString	trgOps_dupd;
	SString	trgOps_dchg;
	SString trgOps_gi;
	SString	optFields_dataRef;
	SString	optFields_reasonCode;
	SString	optFields_configRef;
	SString	optFields_dataSet;
	SString	optFields_entryID;
	SString	optFields_timeStamp;
	SString	optFields_seqNum;
	int		rptEnabled_max;
	SString log_ena;
	SString log_reasonCode;
	SString log_name;
	SString entryID;

	view_plugin_scl_import			*sclImport;
	QList<XmlObject*>	document;
	QString				iedName;
	oe_cpu				*cpu;
	XmlObject			*cpu_object;
	XmlObject			*group_object;

	QList<XmlObject*>	groups;

private:
	bool	get_ied_no(XmlObject * object);
	bool	get_cpu_no(XmlObject * object);
	bool	get_group_no(XmlObject * object,QString & error);
	bool	get_name(XmlObject * object);
	bool	get_type(XmlObject * object);
	bool	get_mms_path(XmlObject * object);
	bool	get_report_control(XmlObject * object);
	bool    get_log_control(XmlObject * object);

	bool	db_insert(XmlObject * object,QString & error);
	bool	db_element(XmlObject * object,QString & error);

	bool	control_group(XmlObject * object,QString & error);
	bool	control_group_insert(QString & error);
	bool	control_element(SString &sql,int &entry,bool &isExistCtl,XmlObject * object,QString & error);
	bool	get_control_entry(int &entry,QString & error);
	bool	get_control_name(XmlObject * object,QString &name);
	bool	get_control_type(int &type);
	bool	get_control_mms_path(XmlObject * object,QString &mms_path,QString & error);
	bool	get_control_link(QString mms_path,QString & error,int &link_ied_no,int &link_cpu_no,int &link_group_no,int &link_entry,int &type);

	bool	account_group(XmlObject * object,QString & error);
	bool	account_LPHD(XmlObject * object,QString & error);
};

#endif // _OE_GROUP_H_