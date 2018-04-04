#ifndef CUSERSWIDGET_H
#define CUSERSWIDGET_H

#include "ui_cuserswidget.h"
#include "skwidget.h"
#include "skbasewidget.h"
#include "skhead.h"
#include "sk_database.h"
#include "sk_log.h"

class CUsersWidget : public SKWidget
{
	Q_OBJECT

public:
	CUsersWidget(QWidget *parent = 0);
	~CUsersWidget();

	void Start();

private:
	Ui::CUsersWidget ui;

protected:
	virtual void paintEvent(QPaintEvent *);

signals:
	void SigClose();

private:
	void Init();
	void InitUi();
	void InitSlot();

private slots:
	void SlotItemClicked(QTreeWidgetItem *item, int column);

};

#endif // CUSERSWIDGET_H
