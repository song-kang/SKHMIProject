#ifndef CNAVIGTION_H
#define CNAVIGTION_H

#include "ui_cnavigtion.h"
#include "skwidget.h"
#include "skbasewidget.h"
#include "skhead.h"
#include "sk_database.h"
#include "sk_log.h"
#include "cfunpoint.h"
#include "cusers.h"

class CHMIWidget;
class CToolWidget;
class CNavigtion : public SKWidget
{
	Q_OBJECT

public:
	CNavigtion(QWidget *parent = 0);
	~CNavigtion();

	void SetHmi(CHMIWidget *hmi) { m_pHmi = hmi; }
	void SetTool(CToolWidget *tool) { m_pTool = tool; }
	void SetEveryFunction(bool b) { m_bEveryFunction = b; }

	void SetUser(QString user);
	void SetQuickFunPointList();
	void SetTreeFunPoint(QList<CFunPoint*> lstFunPoint, CUser *user, QTreeWidgetItem *itemParent);

private:
	Ui::CNavigtion ui;

	bool m_bEveryFunction;
	qint8 m_iQuickNum;
	QString m_sUser;

protected:
	virtual void paintEvent(QPaintEvent *);
	virtual void mouseMoveEvent(QMouseEvent *);

private:
	void Init();
	void InitUi();
	void InitSlot();
	void SetQuickFunPoint(QString key);
	void QuickFunPointList(QString key, QList<CFunPoint*> lstFunPoint);
	void FindFunPoint(QTreeWidgetItem *item, const QString &text);

signals:
	void SigUsers();
	void SigUserSwitch();
	void SigQuit();

public slots:
	void SlotFunSwitch();
	void SlotUserSwitch();
	void SlotQuit();
	void SlotFunPoint();
	void SlotUsers();
	void SlotClickedFunPoint();
	void SlotTreeItemClicked(QTreeWidgetItem *item,int column);
	void SlotQueryTextChanged(const QString &text);

private:
	CHMIWidget *m_pHmi;
	CToolWidget *m_pTool;

};

#endif // CNAVIGTION_H
