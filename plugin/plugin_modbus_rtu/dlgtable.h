#ifndef DLGTABLE_H
#define DLGTABLE_H

#include <QDialog>
#include "ui_dlgtable.h"

enum eOper {
	eOperAdd,
	eOperCommon,
	eOperSequence,
};

class DlgTable : public QDialog
{
	Q_OBJECT

public:
	DlgTable(QWidget *parent = 0);
	~DlgTable();

	void Start(eOper oper);
	QString GetVal() { return m_sVal; }

private:
	Ui::DlgTable ui;

	eOper m_eOper;
	QString m_sVal;

private:
	void Init();
	void InitUi();
	void InitSlot();

private slots:
	void SlotOk();

};

#endif // DLGTABLE_H
