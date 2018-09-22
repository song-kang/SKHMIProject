#ifndef CFUNPOINTEDIT_H
#define CFUNPOINTEDIT_H

#include "ui_cfunpointedit.h"
#include "skwidget.h"
#include "skbasewidget.h"
#include "skhead.h"
#include "sk_database.h"
#include "sk_log.h"

class CHMIWidget;
class CFunPointEdit : public SKWidget
{
	Q_OBJECT

public:
	CFunPointEdit(QWidget *parent = 0);
	~CFunPointEdit();

	void Start();

private:
	Ui::CFunPointEdit ui;

	CHMIWidget *m_pHmi;
	QTreeWidgetItem *m_pCurrentTreeItem;
	QString m_sCurrentKey;
	int m_iImageLen;
	QByteArray m_ImageBuffer;
	bool m_bIsSave;
	bool m_bIconChange;

	QString m_sIdx;
	QString m_sName;
	int		m_iType;
	int		m_iRefSn;
	QString m_sPlugin;

	QMenu *m_pMenuNull;
	QMenu *m_pMenuFolder;
	QMenu *m_pMenuApp;

	SKBaseWidget *m_pFunPointAddWidget;

protected:
	virtual void paintEvent(QPaintEvent *);
	virtual bool eventFilter(QObject *obj,QEvent *e);

private:
	void Init();
	void InitUi();
	void InitSlot();
	void InitTreeWidget(QTreeWidgetItem *item, QString fun_key);
	bool CompareChange();
	bool CompareAuthChange();
	void DeleteTreeItem(QTreeWidgetItem *item);
	void DeleteTreeItemChild(QTreeWidgetItem *item);
	bool DeleteTreeItemContent(QTreeWidgetItem *item);

signals:
	void SigClose();

private slots:
	void SlotTreeItemClicked(QTreeWidgetItem *item, int column);
	void SlotTableItemClicked(QTableWidgetItem *item);
	void SlotIconImport();
	void SlotIconExport();
	void SlotSave();
	void SlotNoTextChanged(const QString &text);
	void SlotNameTextChanged(const QString &text);
	void SlotTypeCurrentIndexChanged(int index);
	void SlotPluginEditTextChanged(const QString &text);
	void SlotWndCurrentIndexChanged(int index);
	void SlotTriggerMenu(QAction *action);
	void SlotFunPointAddClose();
};

#endif // CFUNPOINTEDIT_H
