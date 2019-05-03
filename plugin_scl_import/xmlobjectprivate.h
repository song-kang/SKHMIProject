#ifndef XMLOBJECTPRIVATE_H
#define XMLOBJECTPRIVATE_H

#include <QtGui>

class XmlObjectPrivate
{
public:
	XmlObjectPrivate();
	~XmlObjectPrivate();

	void	init();
	static	XmlObjectPrivate & instance();
	QString key(const QString & name) const;

private:
	static XmlObjectPrivate	*sInstance;
    QMap<QString,QString>	maps;
	
};

#endif // XMLOBJECTPRIVATE_H
