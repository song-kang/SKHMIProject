#include "xmlobjectprivate.h"

XmlObjectPrivate * XmlObjectPrivate::sInstance(0);
XmlObjectPrivate & XmlObjectPrivate::instance()
{
    if(!sInstance)
    {
        sInstance = new XmlObjectPrivate();
        Q_CHECK_PTR(sInstance);
    }

    return *sInstance;
}

XmlObjectPrivate::XmlObjectPrivate()
{
	init();
}

XmlObjectPrivate::~XmlObjectPrivate()
{
	maps.clear();
}

void XmlObjectPrivate::init()
{
    maps.insert("P","type");
	maps.insert("SubNetwork","name");
	maps.insert("IED","%1 - %2:name,desc");
	maps.insert("AccessPoint","AccessPoint - %1:name");
    maps.insert("LDevice","%1 - %2:inst,desc");
    maps.insert("FCDA","%1$%2%3$%4$%5:ldInst,prefix,lnClass,doName,daName");
    maps.insert("LNodeType","%1 - %2:id,desc");
    maps.insert("DOType","%1 - %2:id,desc");
    maps.insert("DO","%1 - %2:name,type");
    maps.insert("SDO","%1 - %2:name,type");
    maps.insert("DAType","id");
    maps.insert("DA","%1(%2) - %3:name,fc,bType");
    maps.insert("BDA","%1 - %2:name,bType");
    maps.insert("EnumType","id");
    maps.insert("EnumVal","text");
    maps.insert("LN0","lnClass");
    maps.insert("LN","%1 - %2:lnClass,desc");
    maps.insert("DOI","%1 - %2:name,desc");
    maps.insert("DAI","name");
    maps.insert("SDI","name");
    maps.insert("DataSet","DataSet - %1:desc");
    maps.insert("ReportControl","ReportControl - %1:desc");
}

QString XmlObjectPrivate::key(const QString & name) const
{
    QMap<QString,QString>::const_iterator iter = maps.find(name);

    if(iter == maps.end()) 
		return QString::null;

    return iter.value();
}
