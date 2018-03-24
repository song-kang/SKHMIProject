#include "cmainwidget.h"
#include "skgui.h"

CMainWidget::CMainWidget(QWidget *parent)
	: SKWidget(parent)
{
	ui.setupUi(this);

	m_app = (SKBaseWidget *)parent;
	Init();
	InitUi();
	InitSlot();
}

CMainWidget::~CMainWidget()
{
	
}

void CMainWidget::Init()
{

}

void CMainWidget::InitUi()
{

}

void CMainWidget::InitSlot()
{
	connect(ui.btnTest, SIGNAL(clicked()), this, SLOT(SlotTest()));
}

void CMainWidget::paintEvent(QPaintEvent *e)
{
	SKWidget::paintEvent(e);
}

void CMainWidget::SlotTest()
{
	CBaseView *v = SK_GUI->NewView("plugin_demo2");
	ui.stackedWidget->addWidget(v);
}
