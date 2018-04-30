#ifndef CFUNPOINTADD_H
#define CFUNPOINTADD_H

#include "ui_cfunpointadd.h"
#include "skwidget.h"
#include "skbasewidget.h"
#include "skhead.h"
#include "sk_database.h"
#include "sk_log.h"

#define TYPE_FOLDER		1
#define TYPE_APP		2

class CFunPointAdd : public SKWidget
{
	Q_OBJECT

public:
	CFunPointAdd(QWidget *parent = 0);
	~CFunPointAdd();

	void Start();
	void SetType(int type) { m_iType = type; }
	void SetTreeItem(QTreeWidgetItem *item) { m_pTreeItem = item; 
		if (m_pTreeItem) m_sKey = m_pTreeItem->data(0,Qt::UserRole).toString(); }
	void SetKey(QString key) { m_sKey = key; }
	int GetType() { return m_iType; }
	QString GetFunPointKey() { return m_sFunPointKey; }
	QString GetFunPointName() { return m_sFunPointName; }
	bool GetSaveSuccess() { return m_bSaveSuccess; }

private:
	Ui::CFunPointAdd ui;

	int m_iType;
	QString m_sKey;
	QTreeWidgetItem *m_pTreeItem;

	int m_iImageLen;
	QByteArray m_ImageBuffer;

	QString m_sFunPointKey;
	QString m_sFunPointName;
	bool m_bSaveSuccess;

protected:
	virtual void paintEvent(QPaintEvent *);

private:
	void Init();
	void InitUi();
	void InitSlot();

signals:
	void SigClose();

private slots:
	void SlotIconImport();
	void SlotSave();

};

#endif // CFUNPOINTADD_H
