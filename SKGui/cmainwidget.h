#ifndef CMAINWIDGET_H
#define CMAINWIDGET_H

#include "ui_cmainwidget.h"
#include "skwidget.h"
#include "skbasewidget.h"
#include "skhead.h"
#include "sk_database.h"
#include "sk_log.h"

class CMainWidget : public SKWidget
{
	Q_OBJECT

public:
	CMainWidget(QWidget *parent = 0);
	~CMainWidget();

private:
	Ui::CMainWidget ui;

protected:
	virtual void paintEvent(QPaintEvent *);

private:
	void Init();
	void InitUi();
	void InitSlot();

private slots:
	void SlotTest();

private:
	SKBaseWidget *m_app;

};

#endif // CMAINWIDGET_H
