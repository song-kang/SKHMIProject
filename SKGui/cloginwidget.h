#ifndef CLOGINWIDGET_H
#define CLOGINWIDGET_H

#include "ui_cloginwidget.h"
#include "skwidget.h"
#include "skbasewidget.h"
#include "skhead.h"
#include "sk_database.h"
#include "sk_log.h"

class CLoginWidget : public SKWidget
{
	Q_OBJECT

public:
	CLoginWidget(QWidget *parent = 0);
	~CLoginWidget();

private:
	Ui::CLoginWidget ui;

	SKBaseWidget *m_pHmi;

protected:
	virtual void paintEvent(QPaintEvent *);
	virtual void keyPressEvent(QKeyEvent *);

private:
	void Init();
	void InitUi();
	void InitSlot();

signals:
	void SigClose();

private slots:
	void SlotClose();
	void SlotLogin();
	void SlotCancel();

private:
	SKBaseWidget *m_app;

};

#endif // CLOGINWIDGET_H
