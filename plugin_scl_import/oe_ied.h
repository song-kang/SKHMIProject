#ifndef _OE_IED_H_
#define _OE_IED_H_

#include <QtGui>
#include "SApplication.h"
#include "sk_database.h"
#include "xmlobject.h"

class view_plugin_scl_import;

class oe_ied
{
public:
	oe_ied(view_plugin_scl_import *scl,QList<XmlObject*> list,QString ied,int no,int subNo);
	~oe_ied(void);

public:

public:
	bool		execute(QString & error);
	XmlObject*	getObject() { return object; }
	QString		getIedName() { return iedName; }
	int			getIedNo() { return ied_no; }
	QString		getIedMmsPath() { return QString("%1").arg(mms_path.data()); }
	int			getIedType() { return type; }
	int			getSubNo() { return sub_no; }
	int			getGooseCbNo() { return goose_cb_no; }
	void		setGooseCbNo(int no) { goose_cb_no = no; }
	int			getSmvCbNo() { return smv_cb_no; }
	void		setSmvCbNo(int no) { smv_cb_no = no; }

private:
	int		ied_no;
	int		sub_no;
	int		device_no;
	SString	name;
	int		type;
	int		pri_code;
	SString	uri;
	SString	model;
	SString	version;
	SString	crc;
	SString	version_time;
	SString	mms_path;

	view_plugin_scl_import			*sclImport;
	QList<XmlObject*>	document;
	XmlObject			*object;
	QString				iedName;

	int goose_cb_no;
	int	smv_cb_no;

private:
	bool	get_ied_no(QString & error);
	bool	get_sub_no();
	bool	get_device_no();
	bool	get_name();
	bool	get_type();
	bool	get_pri_code();
	bool	get_uri();
	bool	get_model();
	bool	get_version();
	bool	get_crc();
	bool	get_version_time();
	bool	get_mms_path();

	void	SetElementStateUnknown();

};

#endif // _OE_IED_H_