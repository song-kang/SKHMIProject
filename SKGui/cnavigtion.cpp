#include "cnavigtion.h"
#include "chmiwidget.h"
#include "ctoolwidget.h"
#include "cloginwidget.h"

struct stuFunPoint
{
	quint32 times;
	QString key;
};

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

#ifdef WIN32
	setStyleSheet(tr("QWidget#CNavigtion{background:rgb(0,58,108,150);border-radius:4px;border:1px solid gray}"));
#else
	setStyleSheet(tr("QWidget#CNavigtion{background:rgb(0,58,108,150);border:1px solid gray}"));
#endif

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
	connect(ui.treeWidgetItems,SIGNAL(itemClicked(QTreeWidgetItem *,int)),this,SLOT(SlotTreeItemClicked(QTreeWidgetItem *,int)));
	connect(ui.lineEditFind,SIGNAL(textChanged(const QString&)),this,SLOT(SlotQueryTextChanged(const QString&)));
}

void CNavigtion::SetUser(QString user)
{
	m_sUser = user; 
	ui.btnUser->setText(m_sUser);

	foreach (CUsers *users, SK_GUI->m_lstUsers)
	{
		foreach (CUser *user, users->m_lstUser)
		{
			if (user->GetCode() == m_sUser)
			{
				SetQuickFunPointList();
				SetTreeFunPoint(SK_GUI->m_lstFunPoint,user,NULL);
				//ui.treeWidgetItems->expandAll();
			}
		}
	}

	SString sql;
	SRecordset rs;
	sql.sprintf("select grp_code from t_ssp_user where usr_code='%s'",user.toStdString().data());
	int cnt = DB->Retrieve(sql,rs);
	if (cnt > 0)
	{
		QString grp_code = rs.GetValue(0,0).data();
		if (grp_code == "admin")
		{
			ui.btnUsers->setVisible(true);
			ui.btnFunPoint->setVisible(true);
		}
		else
		{
			ui.btnUsers->setVisible(false);
			ui.btnFunPoint->setVisible(false);
		}
	}
	else
	{
		ui.btnUsers->setVisible(false);
		ui.btnFunPoint->setVisible(false);
	}
}

bool compareData(const stuFunPoint *data1, const stuFunPoint *data2)
{
	if (data1->times > data2->times)
		return true;

	return false;
}

void CNavigtion::SetQuickFunPointList()
{
	QList<stuFunPoint*> fpList;
	QString s = SK_GUI->GetSettingsValue(SG_FUNPOINT,SV_QUICKPOINT).toString();
	if (!s.isEmpty())
	{
		QStringList l = s.split("||");
		for (int i = 0; i < l.count(); i++)
		{
			stuFunPoint *fp = new stuFunPoint;
			fp->times = l.at(i).split("::").at(1).toUInt();
			fp->key = l.at(i).split("::").at(0);
			fpList.append(fp);
		}
	}
	qSort(fpList.begin(),fpList.end(),compareData);

	m_iQuickNum = 0;
	foreach (stuFunPoint *fp, fpList)
	{
		if (m_iQuickNum < MAX_QUICK_NUM)
		{
			QuickFunPointList(fp->key,SK_GUI->m_lstFunPoint);
			m_iQuickNum++;
		}
		delete fp;
	}
}

void CNavigtion::QuickFunPointList(QString key, QList<CFunPoint*> lstFunPoint)
{
	foreach (CFunPoint *p, lstFunPoint)
	{
		if (p->GetKey() == key)
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
			connect(btn, SIGNAL(clicked()), this, SLOT(SlotClickedFunPoint()));
			break;
		}

		QuickFunPointList(key, p->m_lstChilds);
	}
}

void CNavigtion::SetTreeFunPoint(QList<CFunPoint*> lstFunPoint, CUser *user, QTreeWidgetItem *itemParent)
{
	QTreeWidgetItem *item = itemParent;
	foreach (CFunPoint *p, lstFunPoint)
	{
		//if (p->m_lstChilds.count() == 0 || p->m_lstChilds.count() > 1)
		//{
			if (user->IsAuthTrue(p->GetKey()))
			{
				if (itemParent)
					item = new QTreeWidgetItem(itemParent,p->GetType());
				else
					item = new QTreeWidgetItem(ui.treeWidgetItems,p->GetType());

				item->setText(0,p->GetDesc());
				item->setToolTip(0,p->GetDesc());
				item->setData(0,Qt::UserRole,p->GetKey());

				if (p->m_lstChilds.count() == 0 && p->GetType() != 1)
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
				else/* if (p->m_lstChilds.count() > 1)*/
					item->setIcon(0,QIcon(":/images/folder_open"));
			}
		//}
		//else if (!itemParent && p->m_lstChilds.count() == 1)
		//	item = NULL;

		SetTreeFunPoint(p->m_lstChilds,user,item);
	}
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
	SigUserSwitch();
}

void CNavigtion::SlotQuit()
{
	SigQuit();
}

void CNavigtion::SlotFunPoint()
{
	SigFPointEdit();
}

void CNavigtion::SlotUsers()
{
	SigUsers();
}

void CNavigtion::SlotClickedFunPoint()
{
	QPushButton *btn = (QPushButton*)sender();
	if (!btn)
		return;

	QString name = btn->objectName();
	QString desc = btn->text().trimmed();
	QIcon icon = btn->icon();
	int ret = m_pHmi->GotoWidget(name);
	if (ret == 1) //新建插件成功
	{
		m_pTool->CreateToolButton(name,desc,icon);
		m_pTool->SetToolButtonClicked(name);
	}
	else if (ret == 2) //未发现加载过的插件，且新建插件失败
	{
		QMessageBox::warning(NULL,tr("告警"),tr("界面【%1】加载失败！").arg(desc));
	}

	SetQuickFunPoint(name);
	Common::ClearLayout(ui.vLayoutFun);
	SetQuickFunPointList();
}

void CNavigtion::SlotTreeItemClicked(QTreeWidgetItem *item,int column)
{
	if (item->childCount() || item->type() == 1)
		return;

	QString name = item->data(column,Qt::UserRole).toString();
	int ret = m_pHmi->GotoWidget(name);
	if (ret == 1) //新建插件成功
	{
		m_pTool->CreateToolButton(name,item->text(column),item->icon(column));
		m_pTool->SetToolButtonClicked(name);
	}
	else if (ret == 2) //未发现加载过的插件，且新建插件失败
	{
		QMessageBox::warning(NULL,tr("告警"),tr("界面【%1】加载失败！").arg(item->text(column)));
	}

	SetQuickFunPoint(name);
	Common::ClearLayout(ui.vLayoutFun);
	SetQuickFunPointList();
}

void CNavigtion::SlotQueryTextChanged(const QString &text)
{
	if (!text.isEmpty())
	{
		ui.btnFunSwitch->setText(tr("  快捷功能点"));
		ui.btnFunSwitch->setIcon(QIcon(":/images/arrow-left"));
		ui.widgetItem->hide();
		ui.treeWidgetItems->show();
	}

	for (int i = 0; i < ui.treeWidgetItems->topLevelItemCount(); i++)
	{
		QTreeWidgetItem *item = ui.treeWidgetItems->topLevelItem(i);
		FindFunPoint(item,text);
	}
}

void CNavigtion::FindFunPoint(QTreeWidgetItem *item, const QString &text)
{
	if (!item)
	{
		item->setHidden(true);
		return;
	}

	if (item->text(0).contains(text,Qt::CaseInsensitive))
	{
		item->setHidden(false);
		QTreeWidgetItem *parent = item->parent();
		while (parent)
		{
			parent->setHidden(false);
			if (!text.isEmpty())
				ui.treeWidgetItems->expandItem(parent);
			parent = parent->parent();
		}
	}
	else
		item->setHidden(true);

	for (int i = 0; i < item->childCount(); i++)
		FindFunPoint(item->child(i),text);
}

void CNavigtion::SetQuickFunPoint(QString key)
{
	QString result;
	QString s = SK_GUI->GetSettingsValue(SG_FUNPOINT,SV_QUICKPOINT).toString();
	if (s.isEmpty())
	{
		result = key + "::1||";
	}
	else
	{
		bool bFind = false;
		QStringList l = s.split("||");
		for (int i = 0; i < l.count(); i++) 
		{
			if (l.at(i).split("::").at(0) == key)
			{
				result += tr("%1::%2||").arg(key).arg(l.at(i).split("::").at(1).toInt()+1);
				bFind = true;
			}
			else
				result += l.at(i) + "||";
		}
		if (!bFind)
			result += key + "::1||";
	}

	if (result.right(2) == "||")
		result = result.left(result.size()-2);

	SK_GUI->SetSettingsValue(SG_FUNPOINT,SV_QUICKPOINT,result);
}
