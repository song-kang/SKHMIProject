#ifndef XMLOBJECT_H
#define XMLOBJECT_H

#include <QtGui>
#include "xmlobjectprivate.h"

class XmlObject
{
public:
	XmlObject();
	XmlObject(const XmlObject * other);
	~XmlObject();

	QString						name;
    QString						text;
    QList<XmlObject*>			children;
	QHash<QString,XmlObject*>	childrenHash;
    QMap<QString,QString>		attributes;
    XmlObject					*parent;
	XmlObject					*templates;
	int							line;

public:
	QString displayName() const;
	QString attrib(const QString & name) const;
	QList<XmlObject*> findChildren(const QString & name); 
	QList<XmlObject*> findChildrenDeep(const QString & name);
	QList<XmlObject*> findChildrenDeep(const QString & name,const QString & attr,const QString & value);
    XmlObject * findChild(const QString & name); 
    XmlObject * findChild(const QString & name,const QString & attr,const QString & value); 
	XmlObject * findChildDeep(const QString & name);
	XmlObject * findChildDeep(const QString & name,const QString & attr,const QString & value);
	XmlObject * findChildDeepWithAttribs(const QString & name,const QStringList & attr,const QStringList & value);
	XmlObject * findChildDeepWithReference(const QString & name,const QString & reference);

private:
	
};

#endif // XMLOBJECT_H
