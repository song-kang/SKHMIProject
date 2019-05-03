#ifndef DLGACKSUBSTATION_H
#define DLGACKSUBSTATION_H

#include <QDialog>
#include "ui_dlgacksubstation.h"
#include "SApplication.h"
#include "sk_database.h"

class DlgAckSubstation : public QDialog
{
	Q_OBJECT

public:
	DlgAckSubstation(QWidget *parent = 0);
	~DlgAckSubstation();

	void setInvalidNew(bool s) { s == true ? ui.newRadioButton->setEnabled(false) : ui.newRadioButton->setEnabled(true); }
	int getSubstationNo() { return sub_no; };

private:
	Ui::DlgAckSubstation ui;

	int sub_no;

private:
	void init();
	bool initComboBox();
	bool SetNodeParam();

public slots:
	void slotOk();
	void slotNewRadioButtonClicked();
	void slotSelectRadioButtonClicked();
};

#endif // DLGACKSUBSTATION_H
