#include "seldb.h"
#include "sk_database.h"

#define TREE_ITEM_IED	1
#define TREE_ITEM_CPU	2
#define TREE_ITEM_GRP	3

SelDB::SelDB(QWidget *parent)
	: SKWidget(parent)
{
	ui.setupUi(this);

	Init();
	InitUi();
	InitSlot();
}

SelDB::~SelDB()
{

}

void SelDB::Init()
{
	ui.btnOk->setEnabled(false);
	ui.splitter->setStretchFactor(0,3);
	ui.splitter->setStretchFactor(1,7);

	ui.treeWidget->header()->setResizeMode(QHeaderView::ResizeToContents);
	ui.treeWidget->header()->setStretchLastSection(false);

	ui.tableWidget->setColumnWidth(0,100);
	ui.tableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);		//整行选择模式
	ui.tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);			//不可编辑
	ui.tableWidget->setSelectionMode(QAbstractItemView::SingleSelection);		//单选模式
	ui.tableWidget->setFocusPolicy(Qt::NoFocus);								//去除焦点，无虚框
	ui.tableWidget->horizontalHeader()->setStretchLastSection(true);			//设置充满表宽度
	ui.tableWidget->setStyleSheet("selection-background-color:lightblue;");		//设置选中背景色
	ui.tableWidget->verticalHeader()->setDefaultSectionSize(22);				//设置行高
	ui.tableWidget->horizontalHeader()->setHighlightSections(false);			//点击表时不对表头行光亮
	ui.tableWidget->setAlternatingRowColors(true);								//设置交替行色
	ui.tableWidget->verticalHeader()->setVisible(false);						//去除最前列
}

void SelDB::InitUi()
{
	setStyleSheet(tr("QWidget#%1{background:rgb(255,255,255,220);border-bottom-left-radius:6px;border-bottom-right-radius:6px;}").arg(objectName()));
}

void SelDB::InitSlot()
{
	connect(ui.btnOk, SIGNAL(clicked()), this, SLOT(SlotOk()));
	connect(ui.btnCancel, SIGNAL(clicked()), this, SLOT(SlotCancel()));
	connect(ui.treeWidget,SIGNAL(itemClicked(QTreeWidgetItem *,int)),this,SLOT(SlotTreeItemClicked(QTreeWidgetItem *,int)));
	connect(ui.tableWidget,SIGNAL(itemClicked(QTableWidgetItem *)),this,SLOT(SlotTableItemClicked(QTableWidgetItem *)));
}

void SelDB::paintEvent(QPaintEvent *e)
{
	SKWidget::paintEvent(e);
}

void SelDB::Start()
{
	SString sql;
	SRecordset rs;

	sql.sprintf("select ied_no,name from t_oe_ied");
	int cnt = DB->Retrieve(sql,rs);
	if (cnt > 0)
	{
		for (int i = 0; i < cnt; i++)
		{
			int ied_no = rs.GetValue(i,0).toInt();
			QString name = rs.GetValue(i,1).data();

			QTreeWidgetItem *item = NULL;
			item = new QTreeWidgetItem(ui.treeWidget,TREE_ITEM_IED);
			item->setText(0,name);
			item->setData(0,Qt::UserRole,ied_no);
			item->setIcon(0,QIcon(":/images/device"));
		}
	}
}

void SelDB::SlotTreeItemClicked(QTreeWidgetItem *item, int column)
{
	ui.btnOk->setEnabled(false);
	ui.tableWidget->setRowCount(0);
	ui.tableWidget->clearContents();
	if (item->childCount() > 0)
		return;

	SString sql;
	SRecordset rs,rs1;
	if (item->type() == TREE_ITEM_IED)
	{
		int ied_no = item->data(0, Qt::UserRole).toInt();
		sql.sprintf("select cpu_no,name from t_oe_cpu where ied_no=%d",ied_no);
		int cnt = DB->Retrieve(sql,rs);
		if (cnt > 0)
		{
			for (int i = 0; i < cnt; i++)
			{
				int cpu_no = rs.GetValue(i,0).toInt();
				QString name = rs.GetValue(i,1).data();

				QTreeWidgetItem *it = new QTreeWidgetItem(item,TREE_ITEM_CPU);
				it->setText(0,name);
				it->setData(0,Qt::UserRole,cpu_no);
				it->setIcon(0,QIcon(":/images/cpu"));
			}

			item->setExpanded(true);
		}
	}
	else if (item->type() == TREE_ITEM_CPU)
	{
		int cpu_no = item->data(0, Qt::UserRole).toInt();
		sql.sprintf("select group_no,name from t_oe_group where ied_no=%d and cpu_no=%d",item->parent()->data(0,Qt::UserRole).toInt(),cpu_no);
		int cnt = DB->Retrieve(sql,rs);
		if (cnt > 0)
		{
			for (int i = 0; i < cnt; i++)
			{
				int grp_no = rs.GetValue(i,0).toInt();
				QString name = rs.GetValue(i,1).data();

				QTreeWidgetItem *it = new QTreeWidgetItem(item,TREE_ITEM_GRP);
				it->setText(0,name);
				it->setData(0,Qt::UserRole,grp_no);
				it->setIcon(0,QIcon(":/images/reorder"));
			}

			item->setExpanded(true);
		}
	}
	else if (item->type() == TREE_ITEM_GRP)
	{
		int grp_no = item->data(0, Qt::UserRole).toInt();
		sql.sprintf("select entry,name from t_oe_element_state where ied_no=%d and cpu_no=%d and group_no=%d",
			item->parent()->parent()->data(0,Qt::UserRole).toInt(),item->parent()->data(0,Qt::UserRole).toInt(),grp_no);
		int cnt = DB->Retrieve(sql,rs);
		if (cnt > 0)
		{
			int row = 0;
			ui.tableWidget->setRowCount(cnt);
			for (int i = 0; i < cnt; i++)
			{
				int entry = rs.GetValue(i,0).toInt();
				QString name = rs.GetValue(i,1).data();

				QTableWidgetItem *it = new QTableWidgetItem(tr("%1").arg(entry));
				it->setTextAlignment(Qt::AlignCenter);
				it->setData(Qt::UserRole, entry);
				ui.tableWidget->setItem(row, 0, it);

				it = new QTableWidgetItem(tr("%1").arg(name));
				it->setTextAlignment(Qt::AlignCenter);
				it->setData(Qt::UserRole, "t_oe_element_state");
				ui.tableWidget->setItem(row++, 1, it);
			}
		}
		else if (cnt == 0)
		{
			sql.sprintf("select entry,name from t_oe_element_general where ied_no=%d and cpu_no=%d and group_no=%d",
				item->parent()->parent()->data(0,Qt::UserRole).toInt(),item->parent()->data(0,Qt::UserRole).toInt(),grp_no);
			int cnt1 = DB->Retrieve(sql,rs1);
			if (cnt1 > 0)
			{
				int row = 0;
				ui.tableWidget->setRowCount(cnt1);
				for (int i = 0; i < cnt1; i++)
				{
					int entry = rs1.GetValue(i,0).toInt();
					QString name = rs1.GetValue(i,1).data();

					QTableWidgetItem *it = new QTableWidgetItem(tr("%1").arg(entry));
					it->setTextAlignment(Qt::AlignCenter);
					it->setData(Qt::UserRole, entry);
					ui.tableWidget->setItem(row, 0, it);

					it = new QTableWidgetItem(tr("%1").arg(name));
					it->setTextAlignment(Qt::AlignCenter);
					it->setData(Qt::UserRole, "t_oe_element_general");
					ui.tableWidget->setItem(row++, 1, it);
				}
			}
		}
	}
}

void SelDB::SlotTableItemClicked(QTableWidgetItem *item)
{
	ui.btnOk->setEnabled(true);
}

void SelDB::SlotOk()
{
	QTableWidgetItem *tableItem = ui.tableWidget->currentItem();
	if (!tableItem)
		return;

	QTreeWidgetItem *treeItem = ui.treeWidget->currentItem();
	if (!treeItem)
		return;

	m_sPoint = tr("%1::%2,%3,%4,%5::%6")
		.arg(ui.tableWidget->item(ui.tableWidget->currentRow(),1)->data(Qt::UserRole).toString())
		.arg(ui.treeWidget->currentItem()->parent()->parent()->data(0,Qt::UserRole).toInt())
		.arg(ui.treeWidget->currentItem()->parent()->data(0,Qt::UserRole).toInt())
		.arg(ui.treeWidget->currentItem()->data(0,Qt::UserRole).toInt())
		.arg(ui.tableWidget->item(ui.tableWidget->currentRow(),0)->data(Qt::UserRole).toInt())
		.arg(ui.tableWidget->item(ui.tableWidget->currentRow(),1)->text());

	emit SigClose();
}

void SelDB::SlotCancel()
{
	emit SigClose();
}
