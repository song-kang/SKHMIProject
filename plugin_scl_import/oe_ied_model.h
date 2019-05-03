#ifndef _OE_IED_MODEL_H_
#define _OE_IED_MODEL_H_

#include <QtGui>
#include "SApplication.h"
#include "sk_database.h"
#include "xmlobject.h"

class oe_ied_model
{
public:
	oe_ied_model(QList<XmlObject*> list,QString ied);
	~oe_ied_model(void);

public:

public:
	bool	execute(QString & error);

private:
	SString	model;
	SString	manufacturer;
	int		model_id;
	int		maf_no;

	QList<XmlObject*>	document;
	XmlObject			*object;
	QString				iedName;

private:
	bool	get_model(QString & error,bool * bInsert);
	bool	get_manufacturer(QString & error);
	bool	get_model_id(QString & error);
	bool	get_maf_no(QString & error);
};

#endif // _OE_IED_MODEL_H_