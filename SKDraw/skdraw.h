#ifndef SKDRAW_H
#define SKDRAW_H

#include "ui_skdraw.h"
#include "skhead.h"
#include "skbasewidget.h"
#include "drawtool.h"
#include "drawscene.h"
#include "drawview.h"

class SKDraw : public QMainWindow
{
	Q_OBJECT

public:
	SKDraw(QWidget *parent = 0, Qt::WFlags flags = 0);
	~SKDraw();

	void InitSlot();
	void SetApp(SKBaseWidget *app) { m_app = app; }

private:
	Ui::SKDrawClass ui;

	DrawView *m_pView;
	DrawScene *m_pScene;

private:
	SKBaseWidget *m_app;

private:
	void Init();
	void InitUi();
	DrawView* CreateView();

private slots:
	void SlotNew();
	void SlotClose();
	void SlotAddShape();
	void SlotDataChanged();
	void SlotPositionChanged(int x, int y);
	void SlotKeyUp();
	void SlotKeyDown();
	void SlotKeyLeft();
	void SlotKeyRight();
	void SlotKeyEqual();
	void SlotKeyMinus();

};

#endif // SKDRAW_H
