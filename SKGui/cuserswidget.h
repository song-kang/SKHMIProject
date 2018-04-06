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

Q_DECLARE_METATYPE(CFunPoint*);

class CUsersWidget : public SKWidget
{
	Q_OBJECT

public:
	CUsersWidget(QWidget *parent = 0);
	~CUsersWidget();

	void Start();

private:
	Ui::CUsersWidget ui;

	int m_iType;
	CUsers *m_pCurrentUsers;
	CUser *m_pCurrentUser;

protected:
	virtual void paintEvent(QPaintEvent *);

signals:
	void SigClose();

private:
	void Init();
	void InitUi();
	void InitSlot();
	void ShowUserAuth(CUsers *users, CUser *user);
	void ShowUsersAuth(CUsers *users);
	void SetUserAuth(CUser *user, bool auth, CFunPoint *funPoint);
	void SetUsersAuth(CUsers *users, bool auth, CFunPoint *funPoint);

private slots:
	void SlotTreeItemClicked(QTreeWidgetItem *item, int column);
	void SlotTableItemClicked(QTableWidgetItem *item);

};

#endif // CUSERSWIDGET_H
