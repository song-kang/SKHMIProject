#ifndef DLGMANUFACTURER_H
#define DLGMANUFACTURER_H

#include <QDialog>
#include "ui_dlgmanufacturer.h"
#include "SApplication.h"
#include "sk_database.h"

class DlgManufacturer : public QDialog
{
	Q_OBJECT

public:
	DlgManufacturer(QWidget *parent = 0);
	~DlgManufacturer();

	void setModel(QString m) { m_model = m; }
	void setWildCard(QString wc) { m_wildcard = wc; }
	int getMafNo() { return maf_no; }
	QString getError() { return m_error; }

	void start();

private:
	Ui::DlgManufacturer ui;

	int maf_no;
	int max_maf_no;
	QString m_error;
	QString m_model;
	QString m_wildcard;

private:
	void init();
	void insertDb();
	void updateDb();

public slots:
	void slotRadioMafSelect();
	void slotRadioMafNew();
	void slotOk();
	void slotClose();

};

#endif // DLGMANUFACTURER_H
