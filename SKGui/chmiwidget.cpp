#include "chmiwidget.h"

CHMIWidget::CHMIWidget(QWidget *parent)
	: SKWidget(parent)
{
	m_app = (SKBaseWidget *)parent;

	Init();
	InitUi();
	//InitSlot();
}

CHMIWidget::~CHMIWidget()
{
	delete m_pNavigtion;
}

void CHMIWidget::Init()
{
	setObjectName("CHMIWidget");
	m_vBoxLyout = new QVBoxLayout;

	m_pStackedWidget = new QStackedWidget;
	m_pEmptyWidget = new QWidget();
	m_pStackedWidget->addWidget(m_pEmptyWidget);

	m_pToolWidget = new CToolWidget(this);
	m_pToolWidget->setFixedHeight(40);

	m_pFloatToolWidget = new CToolWidget(this);
	m_pFloatToolWidget->setFixedHeight(40);
	m_pFloatToolWidget->hide();

	//m_pNavigtion = new SKBaseWidget(NULL,new CNavigtion(this));
	//m_pNavigtion->SetWindowsFlagsTool();
	//m_pNavigtion->SetWindowFixSize(400,500);
	//m_pNavigtion->SetIsDrag(false);
	//m_pNavigtion->HideTopFrame();
	m_pNavigtion = new CNavigtion();
	m_pNavigtion->SetHmi(this);
	m_pNavigtion->SetTool(m_pToolWidget);
	m_pNavigtion->setFixedSize(450,550);
	m_pNavigtion->hide();
}

void CHMIWidget::InitUi()
{
	setContentsMargins(0,0,0,0);
	m_vBoxLyout->addWidget(m_pStackedWidget);
	m_vBoxLyout->addWidget(m_pToolWidget);
	m_vBoxLyout->setSpacing(0);
	m_vBoxLyout->setContentsMargins(0,0,0,0);
	setLayout(m_vBoxLyout);
}

void CHMIWidget::InitSlot()
{
	connect(m_pToolWidget, SIGNAL(SigStart()), this, SLOT(SlotStart()));
	connect(m_app, SIGNAL(SigMax()), this, SLOT(SlotMax()));
	connect(m_app, SIGNAL(SigMin()), this, SLOT(SlotMin()));
	connect(m_app, SIGNAL(SigMove()), this, SLOT(SlotMove()));
}

void CHMIWidget::paintEvent(QPaintEvent *e)
{
	SKWidget::paintEvent(e);
}

void CHMIWidget::mousePressEvent(QMouseEvent *e)
{
	m_pNavigtion->hide();
	if (e->button() == Qt::LeftButton)
	{
		
	}
}

void CHMIWidget::SlotStart()
{
	QPoint p = m_pToolWidget->GetStartPos();
	p = mapToGlobal(p);
	p.setX(p.x() - 5);
	p.setY(p.y() + (m_pStackedWidget->height() - m_pNavigtion->height() - 5));
	m_pNavigtion->move(p);
	if (m_pNavigtion->isHidden())
		m_pNavigtion->show();
	else
		m_pNavigtion->hide();
}

void CHMIWidget::SlotMax()
{
	m_pNavigtion->hide();
}

void CHMIWidget::SlotMin()
{
	m_pNavigtion->hide();
}

void CHMIWidget::SlotMove()
{
	m_pNavigtion->hide();
}

bool CHMIWidget::GotoWidget(QString name)
{
	m_pNavigtion->hide();
	bool ret = ShowWidgetByPluginName(name);
	if (!ret)
	{
		CBaseView *view = SK_GUI->NewView(name);
		view->SetPluginName(name);
		InsertWidget(view);
	}

	return ret;
}

bool CHMIWidget::ShowWidgetByPluginName(QString name)
{
	for (int i = 1; i < m_pStackedWidget->count(); i++)
	{
		CBaseView *view = (CBaseView*)m_pStackedWidget->widget(i);
		if (view->GetPluginName() == name)
		{
			m_pStackedWidget->setCurrentWidget(view);
			m_pToolWidget->SetToolButtonClicked(name);
			return true;
		}
	}

	return false;
}

void CHMIWidget::InsertWidget(CBaseView *view)
{
	m_pStackedWidget->addWidget(view);
	m_pStackedWidget->setCurrentWidget(view);
}

void CHMIWidget::ShowDesktop()
{
	m_pStackedWidget->setCurrentWidget(m_pEmptyWidget);
}
