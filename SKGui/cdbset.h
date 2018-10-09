#ifndef CDBSET_H
#define CDBSET_H

#include "cbaseview.h"
#include "sk_database.h"
#include "SMdb.h"
#include "skhead.h"
#include "skwidget.h"
#include "ui_cdbset.h"

class CDBSet : public SKWidget
{
	Q_OBJECT

public:
	CDBSet(QWidget *parent = 0);
	~CDBSet();

	void Start();

protected:
	virtual void paintEvent(QPaintEvent *);

private:
	Ui::CDBSet ui;

	QString m_cfgPathName;
	SXmlConfig m_cfgXml;
	QMap<QString,int> m_mapDBType;

private:
	void Init();
	void InitUi();
	void InitSlot();
	void WriteConfigFile(QString cfgName,bool isCreate);

signals:
	void SigClose();

private slots:
	void SlotOk();
	void SlotDBTest();
	void SlotMDBTest();
	void SlotCurrentIndexChanged(QString text);

};

#endif // CDBSET_H
