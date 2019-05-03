#ifndef _OE_RUN_DEVICE_H_
#define _OE_RUN_DEVICE_H_

#include <QtGui>
#include "SApplication.h"
#include "sk_database.h"
#include "xmlobject.h"

class view_plugin_scl_import;
class oe_ied;

class oe_run_device
{
public:
	oe_run_device(view_plugin_scl_import *scl,QList<XmlObject*> list,oe_ied *m_ied,int subNo);
	~oe_run_device(void);

public:
	bool execute(QString & error);

private:
	int		device_no;
	SString	name;
	int		p_device_no;
	int		type;
	int		comm_state;
	int		sub_no;
	int		ied_no;
	int		node_no;
	int		unit_no;
	int		run_state;
	int		remote_ctrl;
	int		clock_sync_s;
	int		loop_ms;
	int		need_wave;

	view_plugin_scl_import			*sclImport;
	QList<XmlObject*>	document;
	XmlObject			*ied_object;
	oe_ied				*ied;

private:
	bool	get_device_no(QString & error);
	bool	get_name(QString & error);
	bool	get_p_device_no(QString & error);
	bool	get_type(QString & error);
	bool	get_comm_state(QString & error);
	bool	get_sub_no(QString & error);
	bool	get_ied_no(QString & error);
	bool	get_node_no(QString & error);
	bool	get_unit_no(QString & error);
	bool	get_run_state(QString & error);
	bool	get_remote_ctrl(QString & error);
	bool	get_clock_sync_s(QString & error);
	bool	get_loop_ms(QString & error);
	bool	get_need_wave(QString & error);
};

#endif // _OE_RUN_DEVICE_H_