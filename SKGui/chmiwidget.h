#ifndef CHMIWIDGET_H
#define CHMIWIDGET_H

#include "skwidget.h"
#include "skbasewidget.h"
#include "skhead.h"
#include "sk_database.h"
#include "sk_log.h"
#include "ctoolwidget.h"
#include "cnavigtion.h"

class CHMIWidget : public SKWidget
{
	Q_OBJECT

public:
	CHMIWidget(QWidget *parent = 0);
	~CHMIWidget();

	void SetApp(SKBaseWidget *app) { m_app = app; }
	bool GotoWidget(QString name);
	bool ShowWidgetByPluginName(QString name);
	void InsertWidget(CBaseView *view);
	void ShowDesktop();
	void SetUser(QString user) { m_sUser = user; m_pNavigtion->SetUser(m_sUser);}
	QString GetUser() { return m_sUser; }

protected:
	virtual void paintEvent(QPaintEvent *);
	virtual void mousePressEvent(QMouseEvent *);

private:
	QVBoxLayout *m_vBoxLyout;
	QStackedWidget *m_pStackedWidget;
	QWidget *m_pEmptyWidget;
	CToolWidget *m_pToolWidget;
	CToolWidget *m_pFloatToolWidget;
	QString m_sUser;

public:
	SKBaseWidget *m_pUsersWidget;
	CNavigtion *m_pNavigtion;

private:
	void Init();
	void InitUi();
	void CreateNavigtion();
	void DeleteNavigtion();

public:
	void InitSlot();

signals:
	void SigClose();

private slots:
	void SlotStart();
	void SlotMax();
	void SlotMin();
	void SlotMove();
	void SlotUsers();

	void SlotUsersWidgetClose();

private:
	SKBaseWidget *m_app;
	
};

#endif // CHMIWIDGET_H
