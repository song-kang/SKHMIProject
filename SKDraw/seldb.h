#ifndef SELDB_H
#define SELDB_H

#include "skhead.h"
#include "ui_seldb.h"
#include "skwidget.h"

class SelDB : public SKWidget
{
	Q_OBJECT

public:
	SelDB(QWidget *parent = 0);
	~SelDB();

	void Start();
	void SetPoint(QString point) { m_sPoint = point; }
	QString GetPoint() { return m_sPoint; }

private:
	Ui::SelDB ui;

	QString m_sPoint;

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
	void SlotTreeItemClicked(QTreeWidgetItem *item, int column);
	void SlotTableItemClicked(QTableWidgetItem *item);

};

#endif // SELDB_H
