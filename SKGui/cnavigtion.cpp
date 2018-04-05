#include "cnavigtion.h"
#include "chmiwidget.h"
#include "ctoolwidget.h"

#define MAX_QUICK_NUM		10

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
	ui.btnUser->setText("未登录");
	
	ui.btnFunSwitch->setText(tr("  所有功能点"));
	ui.btnFunSwitch->setIcon(QIcon(":/images/arrow-right"));

	ui.lineEditFind->setPlaceholderText(tr("搜索功能点"));
	
	this->setMouseTracking(true);
	ui.btnUser->setMouseTracking(true);
	ui.btnUsers->setMouseTracking(true);
	ui.btnUserSwitch->setMouseTracking(true);
	ui.btnFunPoint->setMouseTracking(true);
	ui.btnQuit->setMouseTracking(true);
	ui.btnHelp->setMouseTracking(true);
	ui.btnPlugin->setMouseTracking(true);
	ui.btnLog->setMouseTracking(true);
	ui.btnDB->setMouseTracking(true);
	ui.btnSysConfig->setMouseTracking(true);

	ui.treeWidgetItems->setRootIsDecorated(false);
	ui.treeWidgetItems->setStyleSheet("QTreeView::branch:has-children:!has-siblings:closed,\
								 QTreeView::branch:closed:has-children:has-siblings{border-image: none; image: none;}\
								 QTreeView::branch:open:has-children:!has-siblings,\
								 QTreeView::branch:open:has-children:has-siblings{border-image: none; image: none;}");
	ui.treeWidgetItems->hide();

	m_iQuickNum = 0;
	SetQuickFunPoint(SK_GUI->m_lstFunPoint);
	SetTreeFunPoint(SK_GUI->m_lstFunPoint,SK_GUI->m_lstUsers,NULL);
}

void CNavigtion::SetQuickFunPoint(QList<CFunPoint*> lstFunPoint)
{
	foreach (CFunPoint *p, lstFunPoint)
	{
		if (p->m_lstChilds.count() == 0 && m_iQuickNum < MAX_QUICK_NUM)
		{
			QPushButton *btn = new QPushButton(p->GetDesc());
			btn->setObjectName(p->GetKey());
			btn->setIconSize(QSize(32,32));
			if (p->m_pImageBuffer && p->m_iImageLen > 0)
			{
				QPixmap pix;
				pix.loadFromData(p->m_pImageBuffer,p->m_iImageLen);
				btn->setIcon(QIcon(pix));
			}
			else
				btn->setIcon(QIcon(":/images/application"));

			btn->setStyleSheet("QPushButton{background: transparent;border-radius:2px;;text-align: left;height:40px;padding-left:1px;}"
				"QPushButton::hover{background-color:rgb(194,220,252);border:1px solid rgb(21,131,221)}");

			ui.vLayoutFun->addWidget(btn);
			m_iQuickNum++;
		}

		SetQuickFunPoint(p->m_lstChilds);
	}
}

void CNavigtion::SetTreeFunPoint(QList<CFunPoint*> lstFunPoint, QList<CUsers*> lstUsers, QTreeWidgetItem *itemParent)
{
	QTreeWidgetItem *item = itemParent;
	foreach (CFunPoint *p, lstFunPoint)
	{
		if (p->m_lstChilds.count() == 0 || p->m_lstChilds.count() > 1)
		{
			if (itemParent)
				item = new QTreeWidgetItem(itemParent);
			else
				item = new QTreeWidgetItem(ui.treeWidgetItems);

			item->setText(0,p->GetDesc());
			item->setToolTip(0,p->GetDesc());
			item->setData(0,Qt::UserRole,p->GetKey());

			if (p->m_lstChilds.count() == 0)
			{
				if (p->m_pImageBuffer && p->m_iImageLen > 0)
				{
					QPixmap pix;
					pix.loadFromData(p->m_pImageBuffer,p->m_iImageLen);
					item->setIcon(0,QIcon(pix));
				}
				else
					item->setIcon(0,QIcon(":/images/application"));
			}
			else if (p->m_lstChilds.count() > 1)
				item->setIcon(0,QIcon(":/images/folder_open"));
	}
		else if (!itemParent && p->m_lstChilds.count() == 1)
			item = NULL;

		SetTreeFunPoint(p->m_lstChilds,lstUsers,item);
	}
}

void CNavigtion::InitUi()
{
	
}

void CNavigtion::InitSlot()
{
	connect(ui.btnFunSwitch, SIGNAL(clicked()), this, SLOT(SlotFunSwitch()));
	connect(ui.btnUserSwitch, SIGNAL(clicked()), this, SLOT(SlotUserSwitch()));
	connect(ui.btnQuit, SIGNAL(clicked()), this, SLOT(SlotQuit()));
	connect(ui.btnFunPoint, SIGNAL(clicked()), this, SLOT(SlotFunPoint()));
	connect(ui.btnUsers, SIGNAL(clicked()), this, SLOT(SlotUsers()));
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
	else if (name == "btnUsers")
		ui.labelUser->setPixmap(QPixmap(":/images/auth"));
	else if (name == "btnFunPoint")
		ui.labelUser->setPixmap(QPixmap(":/images/config"));
	else if (name == "btnHelp")
		ui.labelUser->setPixmap(QPixmap(":/images/help"));
	else if (name == "btnUserSwitch")
		ui.labelUser->setPixmap(QPixmap(":/images/userSwitch"));
	else if (name == "btnQuit")
		ui.labelUser->setPixmap(QPixmap(":/images/shutDown"));
	else if (name == "btnLog")
		ui.labelUser->setPixmap(QPixmap(":/images/logConfig"));
	else if (name == "btnDB")
		ui.labelUser->setPixmap(QPixmap(":/images/dbConfig"));
	else if (name == "btnSysConfig")
		ui.labelUser->setPixmap(QPixmap(":/images/sysConfig"));
	else if (name == "btnPlugin")
		ui.labelUser->setPixmap(QPixmap(":/images/plugin"));
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

void CNavigtion::SlotFunPoint()
{

}

void CNavigtion::SlotUsers()
{
	SigUsers();
}
