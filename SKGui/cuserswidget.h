#ifndef CUSERSWIDGET_H
#define CUSERSWIDGET_H

#include "ui_cuserswidget.h"
#include "skwidget.h"
#include "skbasewidget.h"
#include "skhead.h"
#include "sk_database.h"
#include "sk_log.h"
#include "cusers.h"
#include "cfunpoint.h"

class CHMIWidget;
class CUsersWidget : public SKWidget
{
	Q_OBJECT

public:
	CUsersWidget(QWidget *parent = 0);
	~CUsersWidget();

	void Start();

public:
	QList<CUser*> m_lstUser;

private:
	Ui::CUsersWidget ui;

	CHMIWidget *m_pHmi;
	int m_iType;
	CUsers *m_pCurrentUsers;
	CUser *m_pCurrentUser;
	QMenu *m_pMenuNull;
	QMenu *m_pMenuGrp;
	QMenu *m_pMenuUser;
	SKBaseWidget *m_pAuthWidget;

protected:
	virtual void paintEvent(QPaintEvent *);
	virtual bool eventFilter(QObject *obj,QEvent *e);

signals:
	void SigClose();

private:
	void Init();
	void InitUi();
	void InitSlot();
	void InitTreeWidget();
	void ShowUserAuth(CUsers *users, CUser *user);
	void ShowUsersAuth(CUsers *users);
	void SetUserAuth(CUser *user, bool auth, CFunPoint *funPoint);
	void SetUsersAuth(CUsers *users, bool auth, CFunPoint *funPoint);
	void DelUserAuth(CUser *user);
	void DelUsersAuth(CUsers *users);

private slots:
	void SlotTreeItemClicked(QTreeWidgetItem *item, int column);
	void SlotTableItemClicked(QTableWidgetItem *item);
	void SlotTrigerMenu(QAction *action);
	void SlotAuthWidgetClose();

};

#endif // CUSERSWIDGET_H
