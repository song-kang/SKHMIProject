#ifndef DLGNODE_H
#define DLGNODE_H

#include <QDialog>
#include "ui_dlgnode.h"

#define NODE_NAME	"fis_modbus_rtu"

class DlgNode : public QDialog
{
	Q_OBJECT

public:
	DlgNode(QWidget *parent = 0);
	~DlgNode();

	void Start();
	void StartFix(quint32 no);

	quint32 GetNo() { return m_iNo; }
	QString GetName() { return m_sName; }
	QString GetDesc() { return m_sDesc; }
	QString GetParam() { return m_sParam; }

private:
	Ui::DlgNode ui;

	quint32 m_iNo;
	QString m_sName;
	QString m_sDesc;
	QString m_sParam;

	bool m_isFix;

private:
	void Init();
	void InitUi();
	void InitSlot();

private slots:
	void SlotOk();
};

#endif // DLGNODE_H
