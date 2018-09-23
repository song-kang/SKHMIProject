#include "selfun.h"
#include "sk_database.h"

SelFun::SelFun(QWidget *parent)
	: SKWidget(parent)
{
	ui.setupUi(this);

	Init();
	InitUi();
	InitSlot();
}

SelFun::~SelFun()
{

}

void SelFun::Init()
{
	ui.tableWidget->setColumnWidth(0,300);
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

void SelFun::InitUi()
{
	setStyleSheet(tr("QWidget#%1{background:rgb(255,255,255,220);border-bottom-left-radius:6px;border-bottom-right-radius:6px;}").arg(objectName()));
}

void SelFun::InitSlot()
{
	connect(ui.btnOk, SIGNAL(clicked()), this, SLOT(SlotOk()));
	connect(ui.btnCancel, SIGNAL(clicked()), this, SLOT(SlotCancel()));
	connect(ui.tableWidget,SIGNAL(itemClicked(QTableWidgetItem *)),this,SLOT(SlotTableItemClicked(QTableWidgetItem *)));
}

void SelFun::paintEvent(QPaintEvent *e)
{
	SKWidget::paintEvent(e);
}

void SelFun::Start()
{
	SString sql;
	SRecordset rs;
	sql.sprintf("select fun_key,name from t_ssp_fun_point where type!=1");
	int cnt = DB->Retrieve(sql,rs);
	if (cnt > 0)
	{
		int row = 0;
		ui.tableWidget->setRowCount(cnt);
		for (int i = 0; i < cnt; i++)
		{
			QString fun_key = rs.GetValue(i,0).data();
			QString name = rs.GetValue(i,1).data();

			QTableWidgetItem *it = new QTableWidgetItem(tr("%1").arg(fun_key));
			it->setTextAlignment(Qt::AlignCenter);
			ui.tableWidget->setItem(row, 0, it);

			it = new QTableWidgetItem(tr("%1").arg(name));
			it->setTextAlignment(Qt::AlignLeft | Qt::AlignVCenter);
			ui.tableWidget->setItem(row++, 1, it);
		}
	}
}

void SelFun::SlotTableItemClicked(QTableWidgetItem *item)
{

}

void SelFun::SlotOk()
{
	QTableWidgetItem *tableItem = ui.tableWidget->currentItem();
	if (!tableItem)
	{
		QMessageBox::warning(this, "告警", "请选择一个功能场景");
		return;
	}

	m_sFunPoint = tr("%1::%2")
		.arg(ui.tableWidget->item(ui.tableWidget->currentRow(),0)->text())
		.arg(ui.tableWidget->item(ui.tableWidget->currentRow(),1)->text());

	emit SigClose();
}

void SelFun::SlotCancel()
{
	emit SigClose();
}
