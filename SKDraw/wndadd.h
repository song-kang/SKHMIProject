#ifndef WNDADD_H
#define WNDADD_H

#include "skhead.h"
#include "ui_wndadd.h"
#include "skwidget.h"
#include "skbasewidget.h"
#include "db.h"

class SKDraw;
class WndAdd : public SKWidget
{
	Q_OBJECT

public:
	WndAdd(QWidget *parent = 0);
	~WndAdd();

	void Start();

	void SetType(int type) { m_iType = type; }
	int GetType() { return m_iType; }

protected:
	virtual void paintEvent(QPaintEvent *);

private:
	Ui::WndAdd ui;

	int m_iType;
	QMap<int,QString> m_mapIcon;

private:
	void Init();
	void InitUi();
	void InitSlot();

signals:
	void SigClose();

private slots:
	void SlotOk();
	void SlotCancel();

private:
	SKDraw *m_app;

};

#endif // WNDADD_H
