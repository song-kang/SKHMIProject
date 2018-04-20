#include "chmiwidget.h"
#include "cuserswidget.h"
#include "cloginwidget.h"

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
	m_pUserSwitchWidget = NULL;
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
	connect(m_pNavigtion, SIGNAL(SigUserSwitch()), this, SLOT(SlotUserSwitch()));
	connect(m_pNavigtion, SIGNAL(SigQuit()), this, SLOT(SlotQuit()));
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
		m_pUsersWidget->SetWindowTitle(" �û�Ȩ��");
		m_pUsersWidget->SetWindowIcon(QIcon(""));
		m_pUsersWidget->SetWindowFlags(0);
		m_pUsersWidget->SetWindowSize(700,500);
		m_pUsersWidget->SetIsDrag(true);
		connect(m_pUsersWidget, SIGNAL(SigClose()), this, SLOT(SlotUsersClose()));
	}
	
	((CUsersWidget*)m_pUsersWidget->GetCenterWidget())->Start();
	m_pUsersWidget->Show();
}

void CHMIWidget::SlotUsersClose()
{
	disconnect(m_pUsersWidget, SIGNAL(SigClose()), this, SLOT(SlotUsersClose()));
	delete m_pUsersWidget;
	m_pUsersWidget = NULL;

	DeleteNavigtion();
	CreateNavigtion();
}

void CHMIWidget::SlotUserSwitch()
{
	m_pNavigtion->hide();

	if (!m_pUserSwitchWidget)
	{
		CLoginWidget *w = new CLoginWidget;
		w->SetFirst(false);
		m_pUserSwitchWidget = new SKBaseWidget(NULL,w);
		m_pUserSwitchWidget->SetWindowsFlagsTool();
		m_pUserSwitchWidget->SetWindowsModal();
		m_pUserSwitchWidget->SetWindowFixSize(420,290);
		m_pUserSwitchWidget->SetWindowBackgroundImage(QPixmap(":/images/login"));
		m_pUserSwitchWidget->HideTopFrame();
		connect(m_pUserSwitchWidget, SIGNAL(SigClose()), this, SLOT(SlotUserSwitchClose()));
		m_pUserSwitchWidget->Show();
	}
}

void CHMIWidget::SlotUserSwitchClose()
{
	if (((CLoginWidget*)m_pUserSwitchWidget->GetCenterWidget())->GetLoginOk())
	{
		m_sUser = ((CLoginWidget*)m_pUserSwitchWidget->GetCenterWidget())->GetUser();
		DeleteNavigtion();
		CreateNavigtion();
	}

	disconnect(m_pUserSwitchWidget, SIGNAL(SigClose()), this, SLOT(SlotUserSwitchClose()));
	delete m_pUserSwitchWidget;
	m_pUserSwitchWidget = NULL;
}

void CHMIWidget::SlotQuit()
{
	m_pNavigtion->hide();

	int ret = QMessageBox::question(NULL,tr("ѯ��"),tr("ȷ���˳�ϵͳ��"),tr("�˳�"),tr("ȡ��"));
	if (ret == 0)
		SigClose();
}

int CHMIWidget::GotoWidget(QString name)
{
	m_pNavigtion->hide();
	bool ret = ShowWidgetByPluginName(name);
	if (!ret)
	{
		CBaseView *view = SK_GUI->NewView(name);
		if (view)
		{
			view->SetPluginName(name);
			InsertWidget(view);
			return 1; //�½�����ɹ�
		}
		return 2; //δ���ּ��ع��Ĳ�������½����ʧ��
	}

	return 0; //�����˼��ع��Ĳ��
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

void CHMIWidget::DeleteWidget(QString name)
{
	for (int i = 1; i < m_pStackedWidget->count(); i++)
	{
		CBaseView *view = (CBaseView*)m_pStackedWidget->widget(i);
		if (view->GetPluginName() == name)
		{
			m_pStackedWidget->removeWidget(view);
			delete view;
		}
	}
}

void CHMIWidget::ShowDesktop()
{
	m_pStackedWidget->setCurrentWidget(m_pEmptyWidget);
}
