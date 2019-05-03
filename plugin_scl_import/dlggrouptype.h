#ifndef DLGGROUPTYPE_H
#define DLGGROUPTYPE_H

#include <QDialog>
#include <QHeaderView>
#include "ui_dlggrouptype.h"
#include "xmlobject.h"

#define TABLE_LDEVICE			0
#define TABLE_DATASET_NAME		1
#define TABLE_DATASET_GROUP		2

class view_plugin_scl_import;

class DlgGroupType : public QDialog
{
	Q_OBJECT

public:
	DlgGroupType(QWidget *parent = 0);
	~DlgGroupType();

	void setDocument(QList<XmlObject*> doc) { document = doc; }
	void setName(QString name) { iedName = name; }
	void setDesc(QString desc) { iedDesc = desc; }
	void setMap(QMap<QString,int> map) { typeMap = map; }
	void setSmartImport(bool b) { bSmartImport = b; }
	QMap<QString,int> getMap() { return typeMap; }
	QMap<XmlObject*,int> getIedMap() { return iedMap; }
	void start();

private:
	Ui::DlgGroupType ui;

	view_plugin_scl_import			*sclImport;
	QList<XmlObject*>	document;
	QString				iedName;
	QString				iedDesc;
	QMap<QString,int>	typeMap;
	QMap<XmlObject*,int> iedMap;
	int					row;
	bool				bSmartImport;
	QMap<int,QString>	mapIntToStr;
	QMap<QString,int>	mapStrToInt;

private:
	void init();
	void initTable();
	void appendTableWidget(XmlObject * object,int typeVal);
	bool replaceTypeMap(QString dataset,QString type);
	bool replaceTypeMap(QString ldevice,QString dataset,QString type);
	void closeEvent(QCloseEvent * e);

public slots:
	void slotOk();
};

#endif // DLGGROUPTYPE_H
