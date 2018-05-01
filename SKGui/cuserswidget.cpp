#include "cuserswidget.h"
#include "skgui.h"
#include "cfunpoint.h"
#include "chmiwidget.h"
#include "cauthwidget.h"

#define USER_GROUP		1
#define USER			2

#define COLUMN_CHECK	0
#define COLUMN_KEY		1
#define COLUMN_DESC		2

Q_DECLARE_METATYPE(CFunPoint*);

CUsersWidget::CUsersWidget(QWidget *parent)
	: SKWidget(parent)
{
	ui.setupUi(this);

	m_pHmi = (CHMIWidget *)parent;

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
	ui.treeWidgetUsers->installEventFilter(this);

	InitTreeWidget();

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

	m_pMenuNull = new QMenu(this);
	m_pMenuNull->addAction(QIcon(":/images/userGroup"),tr("添加用户组(&A)"));
	m_pMenuGrp = new QMenu(this);
	m_pMenuGrp->addAction(QIcon(":/images/user"),tr("添加用户(&A)"));
	m_pMenuGrp->addAction(QIcon(":/images/cancel"),tr("删除用户组(&D)"));
	m_pMenuUser = new QMenu(this);
	m_pMenuUser->addAction(QIcon(":/images/cancel"),tr("删除用户(&D)"));
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
	connect(m_pMenuNull,SIGNAL(triggered(QAction*)),this,SLOT(SlotTriggerMenu(QAction*)));
	connect(m_pMenuGrp,SIGNAL(triggered(QAction*)),this,SLOT(SlotTriggerMenu(QAction*)));
	connect(m_pMenuUser,SIGNAL(triggered(QAction*)),this,SLOT(SlotTriggerMenu(QAction*)));
}

void CUsersWidget::InitTreeWidget()
{
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
}

void CUsersWidget::paintEvent(QPaintEvent *e)
{
	SKWidget::paintEvent(e);
}

bool CUsersWidget::eventFilter(QObject *obj,QEvent *e)
{
	QTreeWidgetItem *item = NULL;
	if (obj && obj == ui.treeWidgetUsers)
	{
		if (e->type() == QEvent::ContextMenu)
		{
			QContextMenuEvent * m_e = (QContextMenuEvent*)e;
			QPoint point(m_e->pos().x(),m_e->pos().y() - ui.treeWidgetUsers->header()->height());

			item = (QTreeWidgetItem *)ui.treeWidgetUsers->itemAt(point);
			if (item)
			{
				if (item->type() == USER_GROUP)
				{
#ifndef WIN32
					foreach (CUsers *users, SK_GUI->m_lstUsers)
					{
						if (users->GetCode() == item->data(0,Qt::UserRole).toString())
						{
							m_iType = USER_GROUP;
							m_pCurrentUsers = users;
						}
					}
#endif
					m_pMenuGrp->popup(m_e->globalPos());
				}
				else if (item->type() == USER)
				{
#ifndef WIN32
					foreach (CUsers *users, SK_GUI->m_lstUsers)
					{
						foreach (CUser *user, users->m_lstUser)
						{
							if (user->GetSn() == item->data(0,Qt::UserRole).toInt())
							{
								m_iType = USER;
								m_pCurrentUser = user;
							}
						}
					}
#endif
					m_pMenuUser->popup(m_e->globalPos());
				}
			}
			else 
				m_pMenuNull->popup(m_e->globalPos());
		}
	}

	return QWidget::eventFilter(obj,e);
}

void CUsersWidget::Start()
{

}

void CUsersWidget::SlotTreeItemClicked(QTreeWidgetItem *item, int column)
{
	Q_UNUSED(column);

	ui.tableWidgetAuth->setRowCount(0);
	ui.tableWidgetAuth->clearContents();
	ui.tableWidgetAuth->scrollToTop();
	
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

void CUsersWidget::SlotTriggerMenu(QAction *action)
{
	if (action->text() == "添加用户组(&A)")  
	{
		CAuthWidget *wgt = new CAuthWidget(this);
		m_pAuthWidget = new SKBaseWidget(NULL,wgt);
		wgt->SetType(USER_GROUP);
		wgt->SetApp(m_pAuthWidget);
		((CAuthWidget*)m_pAuthWidget->GetCenterWidget())->Start();
		m_pAuthWidget->SetWindowsFlagsTool();
		m_pAuthWidget->SetWindowsModal();
		m_pAuthWidget->SetWindowTitle("添加用户组");
#ifdef WIN32
		m_pAuthWidget->SetWindowIcon(QIcon(":/images/userGroup"));
#else
		m_pAuthWidget->SetWindowIcon(":/images/userGroup");
#endif
		m_pAuthWidget->SetWindowFlags(0);
		m_pAuthWidget->SetWindowSize(500,600);
		m_pAuthWidget->SetIsDrag(true);
		m_pAuthWidget->SetContentsMargins(1,0,1,1);
		connect(m_pAuthWidget, SIGNAL(SigClose()), this, SLOT(SlotAuthWidgetClose()));
		m_pAuthWidget->Show();
#ifndef WIN32
		m_pHmi->m_pUsersWidget->hide();
#endif
	}
	else if (action->text() == "删除用户组(&D)")
	{
		if (m_pCurrentUsers->GetCode() == "admin")
		{
			QMessageBox::warning(NULL,"告警","管理员组不允许删除");
			return;
		}

		foreach (CUser *user, m_pCurrentUsers->m_lstUser)
		{
			if (user->GetCode() == m_pHmi->GetUser())
			{
				QMessageBox::warning(NULL,"告警","此用户组中有当前运行用户，无法删除");
				return;
			}
		}

		int ret = QMessageBox::question(NULL,tr("询问"),tr("确认删除【%1】用户组及此用户组下所有用户？").arg(m_pCurrentUsers->GetName()),tr("确认"),tr("取消"));
		if (ret == 0)
		{
			foreach (CUser *user, m_pCurrentUsers->m_lstUser)
				DelUserAuth(user);
			DelUsersAuth(m_pCurrentUsers);

			ui.treeWidgetUsers->clear();
			ui.tableWidgetAuth->clearContents();
			ui.tableWidgetAuth->setRowCount(0);
			InitTreeWidget();
		}
	}
	else if (action->text() == "添加用户(&A)")
	{
		CAuthWidget *wgt = new CAuthWidget(this);
		m_pAuthWidget = new SKBaseWidget(NULL,wgt);
		wgt->SetType(USER);
		wgt->SetUsers(m_pCurrentUsers);
		wgt->SetApp(m_pAuthWidget);
		((CAuthWidget*)m_pAuthWidget->GetCenterWidget())->Start();
		m_pAuthWidget->SetWindowsFlagsTool();
		m_pAuthWidget->SetWindowsModal();
		m_pAuthWidget->SetWindowTitle("添加用户");
#ifdef WIN32
		m_pAuthWidget->SetWindowIcon(QIcon(":/images/user"));
#else
		m_pAuthWidget->SetWindowIcon(":/images/user");
#endif
		m_pAuthWidget->SetWindowFlags(0);
		m_pAuthWidget->SetWindowSize(500,600);
		m_pAuthWidget->SetIsDrag(true);
		m_pAuthWidget->SetContentsMargins(1,0,1,1);
		connect(m_pAuthWidget, SIGNAL(SigClose()), this, SLOT(SlotAuthWidgetClose()));
		m_pAuthWidget->Show();
#ifndef WIN32
		m_pHmi->m_pUsersWidget->hide();
#endif
	}
	else if (action->text() == "删除用户(&D)")
	{
		if (m_pCurrentUser->GetCode() == "admin")
		{
			QMessageBox::warning(NULL,"告警","管理员【admin】不允许删除");
			return;
		}

		if (m_pCurrentUser->GetCode() == m_pHmi->GetUser())
		{
			QMessageBox::warning(NULL,"告警","此用户为运行用户，无法删除");
			return;
		}

		int ret = QMessageBox::question(NULL,tr("询问"),tr("确认删除【%1】用户？").arg(m_pCurrentUser->GetName()),tr("确认"),tr("取消"));
		if (ret == 0)
		{
			DelUserAuth(m_pCurrentUser);

			ui.treeWidgetUsers->clear();
			ui.tableWidgetAuth->clearContents();
			ui.tableWidgetAuth->setRowCount(0);
			InitTreeWidget();
		}
	}
}

void CUsersWidget::SlotAuthWidgetClose()
{
	disconnect(m_pAuthWidget, SIGNAL(SigClose()), this, SLOT(SlotAuthWidgetClose()));
	delete m_pAuthWidget;
	m_pAuthWidget = NULL;

	ui.treeWidgetUsers->clear();
	ui.tableWidgetAuth->clearContents();
	ui.tableWidgetAuth->setRowCount(0);
	InitTreeWidget();
#ifndef WIN32
	m_pHmi->m_pUsersWidget->show();
#endif
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

void CUsersWidget::DelUserAuth(CUser *user)
{
	SString sql;
	sql.sprintf("delete from t_ssp_user where usr_sn=%d",user->GetSn());
	DB->ExecuteSQL(sql);
	sql.sprintf("delete from t_ssp_user_auth where usr_sn=%d",user->GetSn());
	DB->ExecuteSQL(sql);

	m_pCurrentUsers->m_lstUser.removeOne(user);
	if (m_lstUser.contains(user))
	{
		m_lstUser.removeOne(user);
		delete user;
	}
}

void CUsersWidget::DelUsersAuth(CUsers *users)
{
	SString sql;
	sql.sprintf("delete from t_ssp_user_group where grp_code='%s'",users->GetCode().toStdString().data());
	DB->ExecuteSQL(sql);
	sql.sprintf("delete from t_ssp_usergroup_auth where grp_code='%s'",users->GetCode().toStdString().data());
	DB->ExecuteSQL(sql);

	SK_GUI->m_lstUsers.removeOne(users);
	delete users;
}
