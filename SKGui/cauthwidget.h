#ifndef CAUTHWIDGET_H
#define CAUTHWIDGET_H

#include "ui_cauthwidget.h"
#include "skwidget.h"
#include "skbasewidget.h"
#include "skhead.h"
#include "sk_database.h"
#include "sk_log.h"
#include "cusers.h"

class CUsersWidget;
class CAuthWidget : public SKWidget
{
	Q_OBJECT

public:
	CAuthWidget(QWidget *parent = 0);
	~CAuthWidget();

	void Start();
	void SetApp(SKBaseWidget *app) { m_app = app; }
	void SetType(int type) { m_iType = type; }
	void SetUsers(CUsers *users) { m_pUsers = users; }
	void SetUser(CUser *user) { m_pUser = user; }

private:
	Ui::CAuthWidget ui;

	int m_iType;
	CUsers *m_pUsers;
	CUser *m_pUser;

protected:
	virtual void paintEvent(QPaintEvent *);

private:
	void Init();
	void InitUi();
	void InitSlot();
	void ShowUsersAuth();
	void ShowUserAuth(CUsers *users);
	void AffirmGroup();
	void AffirmUser();
	void SetListFunPointAuth(QString code,QList<CFunPoint*> lstFunPoint);
	void SetListFunPointAuth(int sn,QList<CFunPoint*> lstFunPoint);
	void SetFunPointAuth(QString code, CFunPoint* funPoint);
	void SetFunPointAuth(int sn, CFunPoint* funPoint);

private slots:
	void SlotOk();
	void SlotCancel();

private:
	SKBaseWidget *m_app;
	CUsersWidget *m_pUsersWidget;

};

#endif // CAUTHWIDGET_H
