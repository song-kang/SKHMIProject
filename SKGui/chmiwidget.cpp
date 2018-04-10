#include "chmiwidget.h"
#include "cuserswidget.h"

CHMIWidget::CHMIWidget(QWidget *parent)
	: SKWidget(parent)
{
	m_app = (SKBaseWidget *)parent;

	Init();
	InitUi();
}

CHMIWidget::~CHMIWidget()
{
	DeleteNavigtion();

	if (m_pUsersWidget)
		delete m_pUsersWidget;
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

	CreateNavigtion();
	m_pUsersWidget = NULL;
}

void CHMIWidget::CreateNavigtion()
{
	m_pNavigtion = new CNavigtion();
	m_pNavigtion->SetHmi(this);
	m_pNavigtion->SetTool(m_pToolWidget);
	m_pNavigtion->setFixedSize(450,550);
	m_pNavigtion->SetUser(m_sUser);
	m_pNavigtion->hide();
	connect(m_pNavigtion, SIGNAL(SigUsers()), this, SLOT(SlotUsers()));
}

void CHMIWidget::DeleteNavigtion()
{
	disconnect(m_pNavigtion, SIGNAL(SigUsers()), this, SLOT(SlotUsers()));
	delete m_pNavigtion;
	m_pNavigtion = NULL;
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
	{
		m_pNavigtion->SetEveryFunction(true);
		m_pNavigtion->SlotFunSwitch();
		m_pNavigtion->show();
	}
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

void CHMIWidget::SlotUsers()
{
	m_pNavigtion->hide();

	if (!m_pUsersWidget)
	{
		m_pUsersWidget = new SKBaseWidget(NULL,new CUsersWidget(this));
		m_pUsersWidget->SetWindowsFlagsTool();
		m_pUsersWidget->SetWindowsModal();
		m_pUsersWidget->SetWindowTitle(" 用户权限");
		m_pUsersWidget->SetWindowIcon(QIcon(""));
		m_pUsersWidget->SetWindowFlags(0);
		m_pUsersWidget->SetWindowSize(700,500);
		m_pUsersWidget->SetIsDrag(true);
		connect(m_pUsersWidget, SIGNAL(SigClose()), this, SLOT(SlotUsersWidgetClose()));
	}
	
	((CUsersWidget*)m_pUsersWidget->GetCenterWidget())->Start();
	m_pUsersWidget->Show();
}

void CHMIWidget::SlotUsersWidgetClose()
{
	disconnect(m_pUsersWidget, SIGNAL(SigClose()), this, SLOT(SlotUsersWidgetClose()));
	delete m_pUsersWidget;
	m_pUsersWidget = NULL;

	DeleteNavigtion();
	CreateNavigtion();
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
