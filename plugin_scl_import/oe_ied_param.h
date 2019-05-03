#ifndef _OE_IED_PARAM_H_
#define _OE_IED_PARAM_H_

#include <QtGui>
#include "SApplication.h"
#include "sk_database.h"
#include "xmlobject.h"

#define TYPE_BOOL			1
#define	TYPE_ENUM			2
#define TYPE_INTGER			3
#define	TYPE_FLOAT			4
#define TYPE_STRING			5

class oe_ied;

class oe_ied_param
{
public:
	oe_ied_param(QList<XmlObject*> list,oe_ied *ied);
	~oe_ied_param(void);

public:

public:
	void	setMmsAName(QString mms) { mmsAName = mms; }
	void	setMmsBName(QString mms) { mmsBName = mms; }

	bool	execute(QString & error);

private:
	SString	current_val;
	int		int_val;
	
	QList<XmlObject*>	document;
	oe_ied				*ied;
	XmlObject			*ied_object;
	QString				iedName;
	QString				mmsAName;
	QString				mmsBName;

private:
	bool	get_net_a_ip();
	bool	get_net_b_ip();
	bool	db_insert_net_a_ip(QString & error);
	bool	db_insert_net_b_ip(QString & error);
};

#endif // _OE_IED_PARAM_H_