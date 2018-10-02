#ifndef DLGADD_H
#define DLGADD_H

#include "skhead.h"
#include "ui_dlgadd.h"

#define TYPE_ADD		1
#define TYPE_MODIFY		2

class DlgAdd : public QDialog
{
	Q_OBJECT

public:
	DlgAdd(QWidget *parent = 0);
	~DlgAdd();

	void Start();

	void SetType(int type) { m_type = type; }
	void SetAppName(QString name) { ui.lineEditName->setText(name); }
	void SetAppPath(QString path) { ui.lineEditPath->setText(path); }
	void SetAppArg(QString arg) { ui.lineEditArg->setText(arg); }
	void SetDelayTime(int time) { ui.spinBoxDelay->setValue(time); }
	QString GetAppName() { return m_sAppName; }
	QString GetAppArg() { return m_sAppArg; }
	QString GetAppPath() { return m_sAppPath; }
	int GetDelayTime() { return m_iDelayTime; }

private:
	Ui::DlgAdd ui;

	int m_type;
	QString m_sAppName;
	QString m_sAppArg;
	QString m_sAppPath;
	int m_iDelayTime;

private:
	void Init();
	void InitUi();
	void InitSlot();

private slots:
	void SlotApp();
	void SlotOk();

};

#endif // DLGADD_H
