#ifndef CNAVIGTION_H
#define CNAVIGTION_H

#include "ui_cnavigtion.h"
#include "skwidget.h"
#include "skbasewidget.h"
#include "skhead.h"
#include "sk_database.h"
#include "sk_log.h"

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

private:
	Ui::CNavigtion ui;

	bool m_bEveryFunction;

protected:
	virtual void paintEvent(QPaintEvent *);
	virtual void mouseMoveEvent(QMouseEvent *);

private:
	void Init();
	void InitUi();
	void InitSlot();

private slots:
	void SlotFunSwitch();
	void SlotUserSwitch();
	void SlotQuit();

private:
	CHMIWidget *m_pHmi;
	CToolWidget *m_pTool;

};

#endif // CNAVIGTION_H
