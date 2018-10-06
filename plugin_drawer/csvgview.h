#ifndef CSVGVIEW_H
#define CSVGVIEW_H

#include "drawview.h"
#include "skbasewidget.h"
#include "skgui.h"

class CSvgView : public DrawView
{
	Q_OBJECT

public:
	CSvgView(QGraphicsScene *parent = NULL);
	~CSvgView();

public:
	virtual void OnClicked(QList<QGraphicsItem*> list);

public:
	SKBaseWidget *m_pCtrlWidget;

private slots:
	void SlotCtrlClose();
	
};

#endif // CSVGVIEW_H
