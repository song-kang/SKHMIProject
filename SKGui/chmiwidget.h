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
	int  GotoWidget(QString name);
	void GotoFunPoint(QString name, QString desc, QIcon icon);
	bool ShowWidgetByPluginName(QString name);
	void InsertWidget(CBaseView *view);
	void DeleteWidget(QString name);
	void ShowDesktop();
	void SetUser(QString user) { m_sUser = user; m_pNavigtion->SetUser(m_sUser);}
	QString GetUser() { return m_sUser; }

protected:
	virtual void paintEvent(QPaintEvent *);
	virtual void mousePressEvent(QMouseEvent *);
	virtual void keyPressEvent(QKeyEvent *e);

private:
	QVBoxLayout *m_vBoxLyout;
	QStackedWidget *m_pStackedWidget;
	QWidget *m_pEmptyWidget;
	CToolWidget *m_pToolWidget;
	CToolWidget *m_pFloatToolWidget;
	QString m_sUser;
	QTimer *m_pLoginOutTimer;

public:
	CNavigtion *m_pNavigtion;
	SKBaseWidget *m_pUsersWidget;
	SKBaseWidget *m_pUserSwitchWidget;
	SKBaseWidget *m_pFunPointEdit;

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
	void SlotUsersClose();
	void SlotUserSwitch();
	void SlotUserSwitchClose();
	void SlotQuit();
	void SlotFunPointEdit();
	void SlotFunPointEditClose();
	void SlotCtrlAlt() { SlotStart(); }
	void SlotLoginTimeout();

private:
	SKBaseWidget *m_app;
	
};

#endif // CHMIWIDGET_H
