#ifndef DLGIED_H
#define DLGIED_H

#include <QDialog>
#include "ui_dlgied.h"

class DlgIed : public QDialog
{
	Q_OBJECT

public:
	DlgIed(QWidget *parent = 0);
	~DlgIed();

	void Start(quint32 no);
	void StartFix(quint32 no);

	quint32 GetNo() { return m_iNo; }
	QString GetName() { return m_sName; }
	QString GetModel() { return m_sModel; }
	QString GetVersion() { return m_sVersion; }

private:
	Ui::DlgIed ui;

	quint32 m_iNodeNo;
	quint32 m_iDevNo;
	quint32 m_iNo;
	QString m_sName;
	QString m_sModel;
	QString m_sVersion;

	bool m_isFix;

private:
	void Init();
	void InitUi();
	void InitSlot();

private slots:
	void SlotOk();

};

#endif // DLGIED_H
