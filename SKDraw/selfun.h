#ifndef SELFUN_H
#define SELFUN_H

#include "skhead.h"
#include "ui_selfun.h"
#include "skwidget.h"

class SelFun : public SKWidget
{
	Q_OBJECT

public:
	SelFun(QWidget *parent = 0);
	~SelFun();

	void Start();

	QString GetFunPoint() { return m_sFunPoint; }

private:
	Ui::selfun ui;

	QString m_sFunPoint;

protected:
	virtual void paintEvent(QPaintEvent *);

private:
	void Init();
	void InitUi();
	void InitSlot();

signals:
	void SigClose();

private slots:
	void SlotOk();
	void SlotCancel();
	void SlotTableItemClicked(QTableWidgetItem *item);

};

#endif // SELFUN_H
