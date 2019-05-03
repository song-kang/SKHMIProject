#ifndef SCLIEDSTABLEWIDGET_H
#define SCLIEDSTABLEWIDGET_H

#include <QTableWidget>
#include <QHeaderView>
#include "xmlobject.h"

#define TABLE_IEDS_NAME			0
#define TABLE_IEDS_DESC			1
#define TABLE_IEDS_TYPE			2
#define TABLE_IEDS_MANFACTURER	3

class SCLIedsTableWidget : public QTableWidget
{
	Q_OBJECT

public:
	SCLIedsTableWidget(QWidget *parent);
	~SCLIedsTableWidget();

public:
	void reloadIedsTableWidget();

public:
	void setDocument(QList<XmlObject*> doc) { document = doc; }
	void setCheckStateS1(Qt::CheckState check) { checkStateS1 = check; }
	void setCheckStateM1(Qt::CheckState check) { checkStateG1 = check; }
	void setCheckStateG1(Qt::CheckState check) { checkStateM1 = check; }

private:
	QList<XmlObject*>	document;
	XmlObject			*object;
	int					row;
	Qt::CheckState		checkStateS1;
	Qt::CheckState		checkStateG1;
	Qt::CheckState		checkStateM1;

private:
	void init();
	void show(QList<XmlObject*> list);
	bool isAappend(XmlObject * object);
	void appendTableWidget(XmlObject * object);

public slots:
	void slotHeaderClicked(int);
	
};

#endif // SCLIEDSTABLEWIDGET_H
