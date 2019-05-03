#include "xmlobject.h"

XmlObject::XmlObject()
{
	templates = 0;
}

XmlObject::XmlObject(const XmlObject * other)
{
	templates  = 0;
    name       = other->name;
    text       = other->text;
    attributes = other->attributes;

    foreach(XmlObject * child,other->children)
    {
        XmlObject * obj = new XmlObject(child);
        obj->parent = this;
        children.append(obj);
    }
}

XmlObject::~XmlObject()
{
	qDeleteAll(children);
}

//通过xml行中的name值找到treeWidget中需显示的字符串：如 <P type="IP">127.0.0.1</P> 中的P
	QString XmlObject::displayName() const
{
	QString key = XmlObjectPrivate::instance().key(name);

	if(key.isEmpty()) 
		return name;

	if(key == "text") 
		return text;

	QStringList list = key.split(":",QString::SkipEmptyParts);
	if(list.count() != 2)
	{
		QMap<QString,QString>::const_iterator iter = attributes.find(key);

		if(iter == attributes.end())
		{
			return name;
		}

		return iter.value();
	}

	QStringList paramters = list[1].split(",",QString::SkipEmptyParts);
	QString     value = list[0];

	Q_ASSERT_X( paramters.count() == value.count("%"),"Syntax error",key.toLatin1().data());

	for(int i = 0; i < paramters.count(); i++)
	{
		char buf[32] = {0};
#ifdef _WINDOWS
		sprintf_s(buf,"%%%d",i+1);
#else
		sprintf(buf,"%%%d",i+1);
#endif
		QString str = attrib(paramters[i]);
		if(str.isEmpty()) 
			str = QObject::tr("Undefined");

		value = value.replace(buf,str);
	}

	return value;
}

QString XmlObject::attrib(const QString & name) const
{
    QMap<QString,QString>::const_iterator iter = attributes.find(name);

    if(iter == attributes.end()) return QString::null;

    return iter.value();
}

QList<XmlObject*> XmlObject::findChildren(const QString & name)
{
	QList<XmlObject*> list;
	foreach(XmlObject * child,children)
	{
		if(child->name == name)
			list += child;
	}

	return list;
}

QList<XmlObject*> XmlObject::findChildrenDeep(const QString & name)
{
	QList<XmlObject*> list;
	foreach(XmlObject * child,children)
	{
		if(child->name == name)
			list += child;

		list += child->findChildrenDeep(name);
	}

	return list;
}

QList<XmlObject*> XmlObject::findChildrenDeep(const QString & name,const QString & attr,const QString & value)
{
	QList<XmlObject*> list;
	foreach(XmlObject * child,children)
	{
		if ((child->name == name) && (child->attrib(attr) == value))
			list += child;

		list += child->findChildrenDeep(name,attr,value);
	}

	return list;
}

XmlObject * XmlObject::findChild(const QString & name)
{
	foreach(XmlObject * child,children)
	{
		if(child->name == name)
			return child;
	}

	return 0;
}

XmlObject * XmlObject::findChild(const QString & name,const QString & attr,const QString & value)
{
	foreach(XmlObject * child,children)
	{
		if( (child->name == name) && (child->attrib(attr) == value) )
			return child;
	}

	return 0;
}

XmlObject * XmlObject::findChildDeep(const QString & name)
{
	XmlObject * object = 0;

	foreach(XmlObject * child,children)
	{
		if(child->name == name)
			return child;

		object = child->findChildDeep(name);
		if (object)
			return object;
	}

	return object;
}

XmlObject * XmlObject::findChildDeep(const QString & name,const QString & attr,const QString & value)
{
	XmlObject * object = 0;

    foreach(XmlObject * child,children)
    {
        if( (child->name == name) && (child->attrib(attr) == value) )
			return child;

		object = child->findChildDeep(name, attr, value);
		if (object)
			return object;
    }

    return object;
}

XmlObject * XmlObject::findChildDeepWithAttribs(const QString & name,const QStringList & attr,const QStringList & value)
{
	int			okNum = 0;
	XmlObject * object = 0;

	if (attr.count() != value.count())
		return object;

	foreach(XmlObject * child,children)
	{
		okNum = 0;
		for (int i = 0; i < attr.count(); i++)
		{
			if( (child->name == name) && (child->attrib(attr.at(i)) == value.at(i)) )
				okNum++;
		}
		if (okNum == attr.count())
			return child;

		object = child->findChildDeepWithAttribs(name, attr, value);
		if (object)
			return object;
	}

	return object;
}

XmlObject * XmlObject::findChildDeepWithReference(const QString & name,const QString & reference)
{
	//int			okNum = 0;
	XmlObject * object = 0;
	QString		m_reference = attrib("prefix")+attrib("lnClass")+attrib("inst");

	foreach(XmlObject * child,children)
	{
		m_reference = child->attrib("prefix")+child->attrib("lnClass")+child->attrib("inst");
		if (m_reference == reference)
			return child;

		object = child->findChildDeepWithReference(name, reference);
		if (object)
			return object;
	}

	return object;
}
