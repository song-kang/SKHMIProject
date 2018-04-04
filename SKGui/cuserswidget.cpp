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
		root->setText(0,tr("%1��%2��").arg(users->GetCode()).arg(users->GetName()));
		root->setToolTip(0,users->GetDesc());
		root->setIcon(0,QIcon(":/images/folder_open"));
		root->setData(0,Qt::UserRole,users->GetCode());
		foreach (CUser *user, users->m_lstUser)
		{
			item = new QTreeWidgetItem(root,USER);
			item->setText(0,tr("%1��%2��").arg(user->GetCode()).arg(user->GetName()));
			item->setToolTip(0,user->GetDesc());
			item->setIcon(0,QIcon(":/images/user"));
			item->setData(0,Qt::UserRole,user->GetCode());
		}
	}

	ui.tableWidgetAuth->setColumnWidth(0,50);
	ui.tableWidgetAuth->setColumnWidth(1,200);
	ui.tableWidgetAuth->setSelectionBehavior(QAbstractItemView::SelectRows);		//����ѡ��ģʽ
	//ui.subTableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);		//���ɱ༭
	ui.tableWidgetAuth->setSelectionMode(QAbstractItemView::SingleSelection);		//��ѡģʽ
	ui.tableWidgetAuth->setFocusPolicy(Qt::NoFocus);								//ȥ�����㣬�����
	ui.tableWidgetAuth->horizontalHeader()->setStretchLastSection(true);			//���ó�������
	ui.tableWidgetAuth->setStyleSheet("selection-background-color:lightblue;");		//����ѡ�б���ɫ
	ui.tableWidgetAuth->verticalHeader()->setDefaultSectionSize(22);				//�����и�
	ui.tableWidgetAuth->horizontalHeader()->setHighlightSections(false);			//�����ʱ���Ա�ͷ�й���
	ui.tableWidgetAuth->setAlternatingRowColors(true);								//���ý�����ɫ
	ui.tableWidgetAuth->verticalHeader()->setVisible(false);						//ȥ����ǰ��
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