#include "chmiwidget.h"
#include "cuserswidget.h"
#include "cloginwidget.h"
#include "cfunpointedit.h"

#define MAX_LOGIN_OUT_TIME		300

CHMIWidget::CHMIWidget(QWidget *parent)
	: SKWidget(parent)
{
	m_app = (SKBaseWidget *)parent;
	SK_GUI->m_pHmiWidget = this;

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
	m_pFunPointEdit = NULL;

	m_pLoginOutTimer = new QTimer(this);
	m_pLoginOutTimer->setInterval(1000);
	m_pLoginOutTimer->start();
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
	connect(m_pNavigtion, SIGNAL(SigFPointEdit()), this, SLOT(SlotFunPointEdit()));
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
	connect(m_pLoginOutTimer, SIGNAL(timeout()), this, SLOT(SlotLoginTimeout()));
	connect(m_pToolWidget, SIGNAL(SigStart()), this, SLOT(SlotStart()));
	connect(m_app, SIGNAL(SigMax()), this, SLOT(SlotMax()));
	connect(m_app, SIGNAL(SigMin()), this, SLOT(SlotMin()));
	connect(m_app, SIGNAL(SigMove()), this, SLOT(SlotMove()));
	connect(m_app, SIGNAL(SigCtrlAlt()), this, SLOT(SlotCtrlAlt()));
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

void CHMIWidget::keyPressEvent(QKeyEvent *e)
{
	QWidget::keyPressEvent(e);
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
		m_pUsersWidget->SetWindowsFlagsDialog();
		m_pUsersWidget->SetWindowsModal();
		m_pUsersWidget->SetWindowTitle("用户权限");
#ifdef WIN32
		m_pUsersWidget->SetWindowIcon(QIcon(":/images/auth"));
#else
		m_pUsersWidget->SetWindowIcon(":/images/auth");
#endif
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
	m_pToolWidget->DeleteAllToolButton();
}

void CHMIWidget::SlotUserSwitch()
{
	m_pNavigtion->hide();
	m_pLoginOutTimer->stop();

	if (!m_pUserSwitchWidget)
	{
		CLoginWidget *w = new CLoginWidget;
		w->SetFirst(false);
		m_pUserSwitchWidget = new SKBaseWidget(NULL,w);
		m_pUserSwitchWidget->SetWindowsFlagsDialog();
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
		m_pToolWidget->DeleteAllToolButton();
		QMessageBox::information(NULL,tr("提示"),tr("用户切换成功"));
		m_pLoginOutTimer->start();
	}

	disconnect(m_pUserSwitchWidget, SIGNAL(SigClose()), this, SLOT(SlotUserSwitchClose()));
	delete m_pUserSwitchWidget;
	m_pUserSwitchWidget = NULL;
}

void CHMIWidget::SlotQuit()
{
	m_pNavigtion->hide();

	int ret = QMessageBox::question(NULL,tr("询问"),tr("确认退出系统？"),tr("退出"),tr("取消"));
	if (ret == 0)
		SigClose();
}

void CHMIWidget::SlotFunPointEdit()
{
	m_pNavigtion->hide();

	if (!m_pFunPointEdit)
	{
		CFunPointEdit *w = new CFunPointEdit(this);
		m_pFunPointEdit = new SKBaseWidget(NULL,w);
		m_pFunPointEdit->SetWindowsFlagsDialog();
		m_pFunPointEdit->SetWindowsModal();
		m_pFunPointEdit->SetWindowTitle("功能点管理");
#ifdef WIN32
		m_pFunPointEdit->SetWindowIcon(QIcon(":/images/config"));
#else
		m_pFunPointEdit->SetWindowIcon(":/images/config");
#endif
		m_pFunPointEdit->SetWindowFlags(0);
		m_pFunPointEdit->SetWindowSize(800,600);
		m_pFunPointEdit->SetIsDrag(true);
		connect(m_pFunPointEdit, SIGNAL(SigClose()), this, SLOT(SlotFunPointEditClose()));
	}

	((CFunPointEdit*)m_pFunPointEdit->GetCenterWidget())->Start();
	m_pFunPointEdit->Show();
}

void CHMIWidget::SlotFunPointEditClose()
{
	disconnect(m_pFunPointEdit, SIGNAL(SigClose()), this, SLOT(SlotFunPointEditClose()));
	delete m_pFunPointEdit;
	m_pFunPointEdit = NULL;

	foreach (CFunPoint *fp, SK_GUI->m_lstFunPoint)
		delete fp;
	SK_GUI->m_lstFunPoint.clear();
	SK_GUI->SetFunPoint(NULL);
	SK_GUI->CheckFunPoint();
	SK_GUI->SetUsersAuth(QString::null);
	SK_GUI->DeleteUserAuth();
	SK_GUI->CheckUserAuth(SK_GUI->m_lstFunPoint);
	foreach (CUsers *u, SK_GUI->m_lstUsers)
		delete u;
	SK_GUI->m_lstUsers.clear();
	SK_GUI->SetUsersAuth(QString::null);

	DeleteNavigtion();
	CreateNavigtion();
	m_pToolWidget->DeleteAllToolButton();
}

void CHMIWidget::SlotLoginTimeout()
{
	if (SK_GUI->m_iLoginOutTime++ > MAX_LOGIN_OUT_TIME)
	{
		SK_GUI->m_iLoginOutTime = 0; 
		SlotUserSwitch(); 
	}
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
			return 1; //新建插件成功
		}
		return 2; //未发现加载过的插件，且新建插件失败
	}

	return 0; //发现了加载过的插件
}

void CHMIWidget::GotoFunPoint(QString name, QString desc, QIcon icon)
{
	int ret = GotoWidget(name);
	if (ret == 1) //新建插件成功
	{
		m_pToolWidget->CreateToolButton(name,desc,icon);
		m_pToolWidget->SetToolButtonClicked(name);
	}
	else if (ret == 2) //未发现加载过的插件，且新建插件失败
	{
		QMessageBox::warning(NULL,tr("告警"),tr("界面【%1】加载失败！").arg(""));
	}

	m_pNavigtion->SetQuickFunPoint(name);
	m_pNavigtion->ClearFunPointLayout();
	m_pNavigtion->SetQuickFunPointList();

	m_pToolWidget->RefreshArrow();
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
