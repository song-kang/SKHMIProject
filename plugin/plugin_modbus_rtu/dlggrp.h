#ifndef DLGGRP_H
#define DLGGRP_H

#include <QDialog>
#include "ui_dlggrp.h"

#define GROUP_TYPE_DGT	4
#define GROUP_TYPE_ANA	3
#define GROUP_TYPE_CTL	10
#define GROUP_TYPE_SET	1

class DlgGrp : public QDialog
{
	Q_OBJECT

public:
	DlgGrp(QWidget *parent = 0);
	~DlgGrp();

	void Start(quint32 iedNo);
	void StartFix(quint32 iedNo, quint32 groupNo);

	quint32 GetNo() { return m_iNo; }
	quint32 GetType() { return m_iType; }
	QString GetName() { return m_sName; }

private:
	Ui::DlgGrp ui;

	quint32 m_iIedNo;
	quint32 m_iNo;
	quint32 m_iType;
	QString m_sName;

	bool m_isFix;
	QMap<int, int> m_mapType;

private:
	void Init();
	void InitUi();
	void InitSlot();

private slots:
	void SlotOk();

};

#endif // DLGGRP_H
