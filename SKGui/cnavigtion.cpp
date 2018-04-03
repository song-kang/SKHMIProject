#include "cnavigtion.h"
#include "chmiwidget.h"
#include "ctoolwidget.h"

CNavigtion::CNavigtion(QWidget *parent)
	: SKWidget(parent)
{
	ui.setupUi(this);

	Init();
	InitUi();
	InitSlot();
}

CNavigtion::~CNavigtion()
{
	
}

void CNavigtion::Init()
{
	setWindowFlags(Qt::FramelessWindowHint | Qt::Tool);
	setAttribute(Qt::WA_TranslucentBackground);

	m_bEveryFunction = false;
	ui.labelUser->setFixedSize(64,64);
	ui.labelUser->setPixmap(QPixmap(":/images/user"));
	ui.labelUser->setScaledContents(true);

	ui.btnUser->setFixedWidth(120);
	ui.btnUser->setText("宋康");
	
	ui.btnFunSwitch->setText(tr("  所有功能点"));
	ui.btnFunSwitch->setIcon(QIcon(":/images/arrow-right"));

	ui.lineEditFind->setPlaceholderText(tr("搜索功能点"));
	
	this->setMouseTracking(true);
	ui.btnUser->setMouseTracking(true);
	ui.btnConfig->setMouseTracking(true);
	ui.btnUserSwitch->setMouseTracking(true);
	ui.btnQuit->setMouseTracking(true);
	ui.btnHelp->setMouseTracking(true);

	ui.treeWidgetItems->setRootIsDecorated(false);
	ui.treeWidgetItems->hide();
}

void CNavigtion::InitUi()
{
	QPushButton *btn1 = new QPushButton("SCD对比");
	btn1->setIconSize(QSize(32,32));
	btn1->setIcon(QIcon(":/images/user"));
	btn1->setStyleSheet(
		"QPushButton{background: transparent;border-radius:2px;;text-align: left;height:40px;padding-left:1px;}"
		"QPushButton::hover{background-color:rgb(194,220,252);border:1px solid rgb(21,131,221)}");
	
	QPushButton *btn2 = new QPushButton("对比");
	btn2->setIconSize(QSize(32,32));
	btn2->setIcon(QIcon(":/images/config"));
	btn2->setStyleSheet(
		"QPushButton{background: transparent;border-radius:2px;;text-align: left;height:40px;padding-left:1px;}"
		"QPushButton::hover{background-color:rgb(194,220,252);border:1px solid rgb(21,131,221)}");

	QPushButton *btn3 = new QPushButton("对比对比");
	btn3->setIconSize(QSize(32,32));
	btn3->setIcon(QIcon(":/images/shutDown"));
	btn3->setStyleSheet(
		"QPushButton{background: transparent;border-radius:2px;;text-align: left;height:40px;padding-left:1px;}"
		"QPushButton::hover{background-color:rgb(194,220,252);border:1px solid rgb(21,131,221)}");

	QPushButton *btn4 = new QPushButton("SCD对比");
	btn4->setIconSize(QSize(32,32));
	btn4->setIcon(QIcon(":/images/user"));
	btn4->setStyleSheet(
		"QPushButton{background: transparent;border-radius:2px;;text-align: left;height:40px;padding-left:1px;}"
		"QPushButton::hover{background-color:rgb(194,220,252);border:1px solid rgb(21,131,221)}");

	QPushButton *btn5 = new QPushButton("对比");
	btn5->setIconSize(QSize(32,32));
	btn5->setIcon(QIcon(":/images/config"));
	btn5->setStyleSheet(
		"QPushButton{background: transparent;border-radius:2px;;text-align: left;height:40px;padding-left:1px;}"
		"QPushButton::hover{background-color:rgb(194,220,252);border:1px solid rgb(21,131,221)}");

	QPushButton *btn6 = new QPushButton("对比对比");
	btn6->setIconSize(QSize(32,32));
	btn6->setIcon(QIcon(":/images/shutDown"));
	btn6->setStyleSheet(
		"QPushButton{background: transparent;border-radius:2px;;text-align: left;height:40px;padding-left:1px;}"
		"QPushButton::hover{background-color:rgb(194,220,252);border:1px solid rgb(21,131,221)}");

	QPushButton *btn7 = new QPushButton("SCD对比");
	btn7->setIconSize(QSize(32,32));
	btn7->setIcon(QIcon(":/images/user"));
	btn7->setStyleSheet(
		"QPushButton{background: transparent;border-radius:2px;;text-align: left;height:40px;padding-left:1px;}"
		"QPushButton::hover{background-color:rgb(194,220,252);border:1px solid rgb(21,131,221)}");

	QPushButton *btn8 = new QPushButton("对比");
	btn8->setIconSize(QSize(32,32));
	btn8->setIcon(QIcon(":/images/config"));
	btn8->setStyleSheet(
		"QPushButton{background: transparent;border-radius:2px;;text-align: left;height:40px;padding-left:1px;}"
		"QPushButton::hover{background-color:rgb(194,220,252);border:1px solid rgb(21,131,221)}");

	QPushButton *btn9 = new QPushButton("对比对比");
	btn9->setIconSize(QSize(32,32));
	btn9->setIcon(QIcon(":/images/shutDown"));
	btn9->setStyleSheet(
		"QPushButton{background: transparent;border-radius:2px;;text-align: left;height:40px;padding-left:1px;}"
		"QPushButton::hover{background-color:rgb(194,220,252);border:1px solid rgb(21,131,221)}");

	ui.vLayoutFun->addWidget(btn1);
	ui.vLayoutFun->addWidget(btn2);
	ui.vLayoutFun->addWidget(btn3);
	ui.vLayoutFun->addWidget(btn4);
	ui.vLayoutFun->addWidget(btn5);
	ui.vLayoutFun->addWidget(btn6);
	ui.vLayoutFun->addWidget(btn7);
	ui.vLayoutFun->addWidget(btn8);
	ui.vLayoutFun->addWidget(btn9);
}

void CNavigtion::InitSlot()
{
	connect(ui.btnFunSwitch, SIGNAL(clicked()), this, SLOT(SlotFunSwitch()));
	connect(ui.btnUserSwitch, SIGNAL(clicked()), this, SLOT(SlotUserSwitch()));
	connect(ui.btnQuit, SIGNAL(clicked()), this, SLOT(SlotQuit()));
}

void CNavigtion::paintEvent(QPaintEvent *e)
{
	SKWidget::paintEvent(e);
}

void CNavigtion::mouseMoveEvent(QMouseEvent *e)
{
	QString name;
	QWidget *wgt = Common::GetWidget(e->globalX(), e->globalY());
	if (wgt)
		name = wgt->objectName();

	if (name == "btnUser")
		ui.labelUser->setPixmap(QPixmap(":/images/user"));
	else if (name == "btnConfig")
		ui.labelUser->setPixmap(QPixmap(":/images/config"));
	else if (name == "btnHelp")
		ui.labelUser->setPixmap(QPixmap(":/images/help"));
	else if (name == "btnUserSwitch")
		ui.labelUser->setPixmap(QPixmap(":/images/userSwitch"));
	else if (name == "btnQuit")
		ui.labelUser->setPixmap(QPixmap(":/images/shutDown"));
	else
		ui.labelUser->setPixmap(QPixmap(":/images/user"));
}

void CNavigtion::SlotFunSwitch()
{
	if (m_bEveryFunction)
	{
		ui.btnFunSwitch->setText(tr("  所有功能点"));
		ui.btnFunSwitch->setIcon(QIcon(":/images/arrow-right"));
		ui.widgetItem->show();
		ui.treeWidgetItems->hide();
	}
	else
	{
		ui.btnFunSwitch->setText(tr("  快捷功能点"));
		ui.btnFunSwitch->setIcon(QIcon(":/images/arrow-left"));
		ui.widgetItem->hide();
		ui.treeWidgetItems->show();
	}

	m_bEveryFunction = !m_bEveryFunction;
}

void CNavigtion::SlotUserSwitch()
{
	bool ret = m_pHmi->GotoWidget("plugin_demo1");
	if (!ret)
	{
		m_pTool->CreateToolButton("plugin_demo1","插件测试用例1");
		m_pTool->SetToolButtonClicked("plugin_demo1");
	}
}

void CNavigtion::SlotQuit()
{
	bool ret = m_pHmi->GotoWidget("plugin_demo2");
	if (!ret)
	{
		m_pTool->CreateToolButton("plugin_demo2","插件测试用例2");
		m_pTool->SetToolButtonClicked("plugin_demo2");
	}
}
