#include "skdraw.h"

SKDraw::SKDraw(QWidget *parent, Qt::WFlags flags)
	: QMainWindow(parent, flags)
{
	ui.setupUi(this);

	Init();
	InitUi();
}

SKDraw::~SKDraw()
{

}

void SKDraw::Init()
{
	m_pView = NULL;
	m_pScene = NULL;

	ui.actionClose->setEnabled(false);
}

void SKDraw::InitUi()
{
	setStyleSheet(tr("QWidget#%1{background:rgb(240,240,240,255);}").arg(objectName()));
}

void SKDraw::InitSlot()
{
	connect(ui.actionNew,SIGNAL(triggered()),this,SLOT(SlotNew()));
	connect(ui.actionClose,SIGNAL(triggered()),this,SLOT(SlotClose()));
	connect(ui.actionSelect,SIGNAL(triggered()),this,SLOT(SlotAddShape()));
	connect(ui.actionLine,SIGNAL(triggered()),this,SLOT(SlotAddShape()));
	connect(m_app, SIGNAL(SigKeyUp()), this, SLOT(SlotKeyUp()));
	connect(m_app, SIGNAL(SigKeyDown()), this, SLOT(SlotKeyDown()));
	connect(m_app, SIGNAL(SigKeyLeft()), this, SLOT(SlotKeyLeft()));
	connect(m_app, SIGNAL(SigKeyRight()), this, SLOT(SlotKeyRight()));
	connect(m_app, SIGNAL(SigKeyEqual()), this, SLOT(SlotKeyEqual()));
	connect(m_app, SIGNAL(SigKeyMinus()), this, SLOT(SlotKeyMinus()));
	connect(m_app, SIGNAL(SigKeyShift()), this, SLOT(SlotKeyShift()));
	connect(m_app, SIGNAL(SigReleaseKeyShift()), this, SLOT(SlotReleaseKeyShift()));
	connect(m_app, SIGNAL(SigKeyEscape()), this, SLOT(SlotKeyEscape()));
	connect(QApplication::clipboard(),SIGNAL(dataChanged()),this,SLOT(SlotDataChanged()));

}

void SKDraw::SlotNew()
{
	DrawView *view = CreateView();
	ui.gridLayoutCentral->addWidget(view);
	view->show();

	ui.actionNew->setEnabled(false);
	ui.actionOpen->setEnabled(false);
	ui.actionClose->setEnabled(true);
}

void SKDraw::SlotClose()
{
	delete m_pView;
	delete m_pScene;
	m_pView = NULL;
	m_pScene = NULL;

	ui.actionNew->setEnabled(true);
	ui.actionOpen->setEnabled(true);
	ui.actionClose->setEnabled(false);
}

void SKDraw::SlotAddShape()
{
	if (sender() == ui.actionSelect)
		DrawTool::c_drawShape = eDrawSelection;
	else if (sender() == ui.actionRotate)
		DrawTool::c_drawShape = eDrawRotation;
	else if (sender() == ui.actionLine)
		DrawTool::c_drawShape = eDrawLine;
	else if (sender() == ui.actionPolyline)
		DrawTool::c_drawShape = eDrawPolyline;
	else if (sender() == ui.actionRectangle)
		DrawTool::c_drawShape = eDrawRectangle;
	else if (sender() == ui.actionRoundRect)
		DrawTool::c_drawShape = eDrawRoundrect;
	else if (sender() == ui.actionCiricle)
		DrawTool::c_drawShape = eDrawCiricle;
	else if (sender() == ui.actionEllipse)
		DrawTool::c_drawShape = eDrawEllipse ;
	else if (sender() == ui.actionPolygon)
		DrawTool::c_drawShape = eDrawPolygon;
}

void SKDraw::SlotDataChanged()
{
	ui.actionPaste->setEnabled(true);
}

DrawView* SKDraw::CreateView()
{
	m_pScene = new DrawScene(this);
	connect(m_pScene, SIGNAL(selectionChanged()),											 this, SLOT(SlotItemSelected()));
	connect(m_pScene, SIGNAL(itemAdded(QGraphicsItem*)),									 this, SLOT(SlotItemAdded(QGraphicsItem*)));
	connect(m_pScene, SIGNAL(itemMoved(QGraphicsItem*,QPointF)),							 this, SLOT(SlotItemMoved(QGraphicsItem*,QPointF)));
	connect(m_pScene, SIGNAL(itemRotate(QGraphicsItem*,qreal)),								 this, SLOT(SlotItemRotate(QGraphicsItem*,qreal)));
	connect(m_pScene, SIGNAL(itemResize(QGraphicsItem*,int,const QPointF&)),				 this, SLOT(SlotItemResize(QGraphicsItem*,int,QPointF)));
	connect(m_pScene, SIGNAL(itemControl(QGraphicsItem*,int,const QPointF&,const QPointF&)), this, SLOT(SlotItemControl(QGraphicsItem*,int,QPointF,QPointF)));

	m_pView = new DrawView(m_pScene);
	m_pScene->SetView(m_pView);
	connect(m_pView, SIGNAL(SigPositionChanged(int,int)), this, SLOT(SlotPositionChanged(int,int)));

	return m_pView;
}

void SKDraw::SlotPositionChanged(int x, int y)
{
	statusBar()->showMessage(tr("×ø±ê£¨%1,%2£©").arg(x).arg(y));
}

void SKDraw::SlotKeyUp()
{
	if (m_pView)
		m_pView->Translate(QPointF(0, -10));
}

void SKDraw::SlotKeyDown()
{
	if (m_pView)
		m_pView->Translate(QPointF(0, 10));
}

void SKDraw::SlotKeyLeft()
{
	if (m_pView)
		m_pView->Translate(QPointF(10, 0));
}

void SKDraw::SlotKeyRight()
{
	if (m_pView)
		m_pView->Translate(QPointF(-10, 0));
}

void SKDraw::SlotKeyEqual()
{
	if (m_pView)
		m_pView->ZoomIn();
}

void SKDraw::SlotKeyMinus()
{
	if (m_pView)
		m_pView->ZoomOut();
}

void SKDraw::SlotKeyShift()
{
	if (m_pScene)
		m_pScene->SetPressShift(true);
}

void SKDraw::SlotReleaseKeyShift()
{
	if (m_pScene)
		m_pScene->SetPressShift(false);
}

void SKDraw::SlotKeyEscape()
{
	DrawTool *tool = DrawTool::findTool(DrawTool::c_drawShape);
	if (tool && ((DrawPolygonTool*)tool)->item)
	{
		m_pScene->removeItem(((DrawPolygonTool*)tool)->item);
		delete ((DrawPolygonTool*)tool)->item;
		((DrawPolygonTool*)tool)->item = NULL;
	}

	DrawTool::c_drawShape = eDrawSelection;
}
