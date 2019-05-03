#ifndef _OE_IED_MODEL_VERSION_H_
#define _OE_IED_MODEL_VERSION_H_

#include <QtGui>
#include "SApplication.h"
#include "sk_database.h"
#include "xmlobject.h"

class oe_ied_model_version
{
public:
	oe_ied_model_version(QList<XmlObject*> list,QString ied);
	~oe_ied_model_version(void);

public:

public:
	bool	execute(QString & error);

private:
	SString	model;
	SString	version;
	SString	crc;
	SString	scene;
	SString	defect;

	QList<XmlObject*>	document;
	XmlObject			*object;
	QString				iedName;

private:
	bool	get_model(QString & error,bool * bInsert);
	bool	get_version();
	bool	get_crc();
	bool	get_scene();
	bool	get_defect();
};

#endif // _OE_IED_MODEL_VERSION_H_