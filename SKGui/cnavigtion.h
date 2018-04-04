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
	void SetUser(QString user) { m_sUser = user; ui.btnUser->setText(m_sUser);}

	void SetQuickFunPoint(QList<CFunPoint*> lstFunPoint);
	void SetTreeFunPoint(QList<CFunPoint*> lstFunPoint, QList<CUsers*> lstUsers, QTreeWidgetItem *itemParent);

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

signals:
	void SigUsers();

public slots:
	void SlotFunSwitch();
	void SlotUserSwitch();
	void SlotQuit();
	void SlotFunPoint();
	void SlotUsers();

private:
	CHMIWidget *m_pHmi;
	CToolWidget *m_pTool;

};

#endif // CNAVIGTION_H
