#ifndef DBPIC_H
#define DBPIC_H

#include "skhead.h"
#include "ui_dbpic.h"
#include "skwidget.h"

class SKDraw;
class DBPic : public SKWidget
{
	Q_OBJECT

public:
	DBPic(QWidget *parent = 0);
	~DBPic();

	void Start();

private:
	Ui::DBPic ui;

	QListWidgetItem *m_pCurrentListWidgetItem;

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
	void SlotAdd();
	void SlotDel();
	void SlotListItemClicked(QListWidgetItem *listItem);

private:
	SKDraw *m_app;

};

#endif // DBPIC_H
