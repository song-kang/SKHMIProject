#include "cuserswidget.h"
#include "skgui.h"
#include "cfunpoint.h"

#define USER_GROUP		1
#define USER			2

#define COLUMN_CHECK	0
#define COLUMN_KEY		1
#define COLUMN_DESC		2

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
			item->setData(0,Qt::UserRole,user->GetSn());
		}
	}

	ui.tableWidgetAuth->setColumnWidth(COLUMN_CHECK,50);
	ui.tableWidgetAuth->setColumnWidth(COLUMN_KEY,180);
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
	connect(ui.treeWidgetUsers,SIGNAL(itemClicked(QTreeWidgetItem *,int)),this,SLOT(SlotTreeItemClicked(QTreeWidgetItem *,int)));
	connect(ui.tableWidgetAuth,SIGNAL(itemClicked(QTableWidgetItem *)),this,SLOT(SlotTableItemClicked(QTableWidgetItem *)));
}

void CUsersWidget::paintEvent(QPaintEvent *e)
{
	SKWidget::paintEvent(e);
}

void CUsersWidget::Start()
{

}

void CUsersWidget::SlotTreeItemClicked(QTreeWidgetItem *item, int column)
{
	ui.tableWidgetAuth->setRowCount(0);
	ui.tableWidgetAuth->clearContents();
	
	if (item->type() == USER_GROUP)
	{
		foreach (CUsers *users, SK_GUI->m_lstUsers)
		{
			if (users->GetCode() == item->data(0,Qt::UserRole).toString())
			{
				m_iType = USER_GROUP;
				m_pCurrentUsers = users;
				ShowUsersAuth(users);
				break;
			}
		}
	}
	else if (item->type() == USER)
	{
		foreach (CUsers *users, SK_GUI->m_lstUsers)
		{
			foreach (CUser *user, users->m_lstUser)
			{
				if (user->GetSn() == item->data(0,Qt::UserRole).toInt())
				{
					m_iType = USER;
					m_pCurrentUser = user;
					ShowUserAuth(user->m_pUsers, user);
					break;
				}
			}
		}
	}
}

void CUsersWidget::SlotTableItemClicked(QTableWidgetItem *item)
{
	if (item->column() > 0)
		return;

	Qt::CheckState state = item->checkState();
	if (state == Qt::Checked)
	{
		if (m_iType == USER_GROUP)
			SetUsersAuth(m_pCurrentUsers,true,item->data(Qt::UserRole).value<CFunPoint*>());
		else if (m_iType == USER)
			SetUserAuth(m_pCurrentUser,true,item->data(Qt::UserRole).value<CFunPoint*>());
	}
	else if (state == Qt::Unchecked)
	{
		if (m_iType == USER_GROUP)
			SetUsersAuth(m_pCurrentUsers,false,item->data(Qt::UserRole).value<CFunPoint*>());
		else if (m_iType == USER)
			SetUserAuth(m_pCurrentUser,false,item->data(Qt::UserRole).value<CFunPoint*>());
	}
}

void CUsersWidget::ShowUserAuth(CUsers *users, CUser *user)
{
	QList<CFunPoint*> lstFunPoint = SK_GUI->GetRunFunPoints();
	int row = 0;
	QTableWidgetItem * item;
	ui.tableWidgetAuth->setRowCount(lstFunPoint.count());
	foreach (CFunPoint *p, lstFunPoint)
	{
		foreach (stuAuth *auth, users->m_lstAuth)
		{
			if (auth->fun_key == p->GetKey() && auth->auth)
			{
				foreach (stuAuth *auth1, user->m_lstAuth)
				{
					if (auth1->fun_key == auth->fun_key)
					{
						item = new QTableWidgetItem("");
						item->setTextAlignment(Qt::AlignCenter);
						item->setCheckState(auth1->auth ? Qt::Checked : Qt::Unchecked);
						QVariant var;
						var.setValue(p);
						item->setData(Qt::UserRole,var);
						ui.tableWidgetAuth->setItem(row,COLUMN_CHECK,item);

						item = new QTableWidgetItem(auth1->fun_key);
						item->setTextAlignment(Qt::AlignCenter);
						item->setFlags(item->flags() & (~Qt::ItemIsEditable));	//设置item项不可编辑
						ui.tableWidgetAuth->setItem(row,COLUMN_KEY,item);

						item = new QTableWidgetItem(p->GetDesc());
						item->setTextAlignment(Qt::AlignCenter);
						item->setFlags(item->flags() & (~Qt::ItemIsEditable));	//设置item项不可编辑
						ui.tableWidgetAuth->setItem(row,COLUMN_DESC,item);

						row++;
					}
				}
			}
		}
	}

	ui.tableWidgetAuth->setRowCount(row);
}

void CUsersWidget::ShowUsersAuth(CUsers *users)
{
	QList<CFunPoint*> lstFunPoint = SK_GUI->GetRunFunPoints();
	int row = 0;
	QTableWidgetItem * item;
	ui.tableWidgetAuth->setRowCount(lstFunPoint.count());
	foreach (CFunPoint *p, lstFunPoint)
	{
		foreach (stuAuth *auth, users->m_lstAuth)
		{
			if (auth->fun_key == p->GetKey())
			{
				item = new QTableWidgetItem("");
				item->setTextAlignment(Qt::AlignCenter);
				item->setCheckState(auth->auth ? Qt::Checked : Qt::Unchecked);
				QVariant var;
				var.setValue(p);
				item->setData(Qt::UserRole,var);
				ui.tableWidgetAuth->setItem(row,COLUMN_CHECK,item);

				item = new QTableWidgetItem(auth->fun_key);
				item->setTextAlignment(Qt::AlignCenter);
				item->setFlags(item->flags() & (~Qt::ItemIsEditable));	//设置item项不可编辑
				ui.tableWidgetAuth->setItem(row,COLUMN_KEY,item);

				item = new QTableWidgetItem(p->GetDesc());
				item->setTextAlignment(Qt::AlignCenter);
				item->setFlags(item->flags() & (~Qt::ItemIsEditable));	//设置item项不可编辑
				ui.tableWidgetAuth->setItem(row,COLUMN_DESC,item);

				row++;
			}
		}
	}
}

void CUsersWidget::SetUserAuth(CUser *user, bool auth, CFunPoint *funPoint)
{
	user->SetAuth(funPoint,auth);
}

void CUsersWidget::SetUsersAuth(CUsers *users, bool auth, CFunPoint *funPoint)
{
	users->SetAuth(funPoint,auth);
}
