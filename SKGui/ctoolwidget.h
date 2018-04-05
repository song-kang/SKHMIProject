#ifndef CTOOLWIDGET_H
#define CTOOLWIDGET_H

#include "ui_ctoolwidget.h"
#include "skwidget.h"
#include "skbasewidget.h"
#include "skhead.h"
#include "sk_database.h"
#include "sk_log.h"
#include "skgui.h"

class CHMIWidget;
class CToolWidget : public SKWidget
{
	Q_OBJECT

public:
	CToolWidget(QWidget *parent = 0);
	~CToolWidget();

	void InsertToolButton(QToolButton *btn);
	void SetToolButtonClicked(QString name);
	void SetToolButtonClicked(QToolButton *btn);
	void SetToolButtonUnclicked(QToolButton *btn);
	QPoint GetStartPos() { return ui.btnStart->pos();  }
	void CreateToolButton(QString name, QString desc);

public:
	QList<QToolButton*> m_lstToolButton;

private:
	Ui::CToolWidget ui;

	QTimer *m_pDateTimer;
	QMap<int,QString> m_weekMap;

protected:
	virtual void paintEvent(QPaintEvent *);
	virtual void mousePressEvent(QMouseEvent *);

private:
	void Init();
	void InitUi();
	void InitSlot();

signals:
	void SigStart();

private slots:
	void SlotLeft();
	void SlotRight();
	void SlotStart();
	void SlotDesktop();
	void SlotToolButtonClick();
	void SlotDateTime();

private:
	CHMIWidget *m_pHmi;

};

#endif // CTOOLWIDGET_H
