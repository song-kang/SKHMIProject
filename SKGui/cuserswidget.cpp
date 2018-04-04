#include "cuserswidget.h"
#include "skgui.h"
#include "cusers.h"

#define USER_GROUP		1
#define USER			2

CUsersWidget::CUsersWidget(QWidget *parent)
	: SKWidget(parent)
{
	ui.setupUi(this);

	Init();
	InitUi();
	InitSlot();
}

CUsersWidget::~CUsersWidget()
{

}

void CUsersWidget::Init()
{
	ui.treeWidgetUsers->setRootIsDecorated(false);
	ui.treeWidgetUsers->header()->setResizeMode(QHeaderView::ResizeToContents);
	ui.treeWidgetUsers->header()->setStretchLastSection(false);

	QTreeWidgetItem *root,*item = NULL;
	foreach (CUsers *users, SK_GUI->m_lstUsers)
	{
		root = new QTreeWidgetItem(ui.treeWidgetUsers,USER_GROUP);
		root->setText(0,tr("%1【%2】").arg(users->GetCode()).arg(users->GetName()));
		root->setToolTip(0,users->GetDesc());
		root->setIcon(0,QIcon(":/images/folder_open"));
		root->setData(0,Qt::UserRole,users->GetCode());
		foreach (CUser *user, users->m_lstUser)
		{
			item = new QTreeWidgetItem(root,USER);
			item->setText(0,tr("%1【%2】").arg(user->GetCode()).arg(user->GetName()));
			item->setToolTip(0,user->GetDesc());
			item->setIcon(0,QIcon(":/images/user"));
			item->setData(0,Qt::UserRole,user->GetCode());
		}
	}

	ui.tableWidgetAuth->setColumnWidth(0,50);
	ui.tableWidgetAuth->setColumnWidth(1,200);
	ui.tableWidgetAuth->setSelectionBehavior(QAbstractItemView::SelectRows);		//整行选择模式
	//ui.subTableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);		//不可编辑
	ui.tableWidgetAuth->setSelectionMode(QAbstractItemView::SingleSelection);		//单选模式
	ui.tableWidgetAuth->setFocusPolicy(Qt::NoFocus);								//去除焦点，无虚框
	ui.tableWidgetAuth->horizontalHeader()->setStretchLastSection(true);			//设置充满表宽度
	ui.tableWidgetAuth->setStyleSheet("selection-background-color:lightblue;");		//设置选中背景色
	ui.tableWidgetAuth->verticalHeader()->setDefaultSectionSize(22);				//设置行高
	ui.tableWidgetAuth->horizontalHeader()->setHighlightSections(false);			//点击表时不对表头行光亮
	ui.tableWidgetAuth->setAlternatingRowColors(true);								//设置交替行色
	ui.tableWidgetAuth->verticalHeader()->setVisible(false);						//去除最前列
}

void CUsersWidget::InitUi()
{
	ui.splitter->setStretchFactor(0,3);
	ui.splitter->setStretchFactor(1,7);
}

void CUsersWidget::InitSlot()
{
	connect(ui.treeWidgetUsers,SIGNAL(itemClicked(QTreeWidgetItem *,int)),this,SLOT(SlotItemClicked(QTreeWidgetItem *,int)));
}

void CUsersWidget::paintEvent(QPaintEvent *e)
{
	SKWidget::paintEvent(e);
}

void CUsersWidget::Start()
{

}

void CUsersWidget::SlotItemClicked(QTreeWidgetItem *item, int column)
{
	if (item->type() == USER_GROUP)
	{
		foreach (CUsers *users, SK_GUI->m_lstUsers)
		{
			if (users->GetCode() == item->data(0,Qt::UserRole).toString())
			{
				break;
			}
		}
	}
	else if (item->type() == USER)
	{

	}
}