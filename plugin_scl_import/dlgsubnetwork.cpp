#include "dlgsubnetwork.h"
#include "subnetworkdelegate.h"

DlgSubNetwork::DlgSubNetwork(QWidget *parent)
	: QDialog(parent)
{
	ui.setupUi(this);

	initTable();

	connect(ui.pushButton_ok,SIGNAL(clicked()), this, SLOT(slotOk()));
	connect(ui.pushButton_cancel,SIGNAL(clicked()), this, SLOT(close()));
}

DlgSubNetwork::~DlgSubNetwork()
{

}

void DlgSubNetwork::initTable()
{
	row = 0;
	mmsAName = QString::null;
	mmsBName = QString::null;

	ui.tableWidget->horizontalHeader()->setStyleSheet("QHeaderView::section{background:yellow;}"); 
	ui.tableWidget->verticalHeader()->setStyleSheet("QHeaderView::section{background:yellow;}");
	ui.tableWidget->setStyleSheet("QTableCornerButton::section{background:yellow;}");

	ui.tableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);		//整行选择模式
	//ui.tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);			//不可编辑
	ui.tableWidget->setSelectionMode(QAbstractItemView::SingleSelection);		//单选模式
	ui.tableWidget->setFocusPolicy(Qt::NoFocus);								//去除焦点，无虚框
	ui.tableWidget->horizontalHeader()->setStretchLastSection(true);			//设置充满表宽度
	ui.tableWidget->setStyleSheet("selection-background-color:lightblue;");		//设置选中背景色
	ui.tableWidget->verticalHeader()->setDefaultSectionSize(20);				//设置行高
	ui.tableWidget->horizontalHeader()->setHighlightSections(false);			//点击表时不对表头行光亮
	ui.tableWidget->setAlternatingRowColors(true);								//设置交替行色
	ui.tableWidget->setItemDelegateForColumn(TABLE_SUBNETWORK_NETNAME,new SubNetworkDelegate(this));

	ui.tableWidget->setColumnWidth(TABLE_SUBNETWORK_NAME,100);
	ui.tableWidget->setColumnWidth(TABLE_SUBNETWORK_DESC,100);
	ui.tableWidget->setColumnWidth(TABLE_SUBNETWORK_TYPE,100);
}

void DlgSubNetwork::start()
{
	QString text;
	QTableWidgetItem * item;

	ui.tableWidget->clearContents();
	foreach (XmlObject * object,subNetworks)
	{
		ui.tableWidget->setRowCount(++row);

		text = object->attrib("name");
		item = new QTableWidgetItem(text);
		item->setTextAlignment(Qt::AlignCenter);
		item->setFlags(item->flags() & (~Qt::ItemIsEditable));	//设置item项不可编辑
		ui.tableWidget->setItem(row-1,TABLE_SUBNETWORK_NAME,item);	

		text = object->attrib("desc");
		item = new QTableWidgetItem(text);
		item->setTextAlignment(Qt::AlignCenter);
		item->setFlags(item->flags() & (~Qt::ItemIsEditable));	//设置item项不可编辑
		ui.tableWidget->setItem(row-1,TABLE_SUBNETWORK_DESC,item);

		text = object->attrib("type");
		item = new QTableWidgetItem(text);
		item->setTextAlignment(Qt::AlignCenter);
		item->setFlags(item->flags() & (~Qt::ItemIsEditable));	//设置item项不可编辑
		ui.tableWidget->setItem(row-1,TABLE_SUBNETWORK_TYPE,item);

		item = new QTableWidgetItem(QString::null);
		item->setTextAlignment(Qt::AlignCenter);
		ui.tableWidget->setItem(row-1,TABLE_SUBNETWORK_NETNAME,item);
	}
}

void DlgSubNetwork::slotOk()
{
	int cntMmsA = 0;
	int cntMmsB = 0;

	for (int i = 0; i < ui.tableWidget->rowCount(); i++)
	{
		QTableWidgetItem * item = ui.tableWidget->item(i,TABLE_SUBNETWORK_NETNAME);
		if (item->text() == tr("MMS A NET"))
		{
			mmsAName = ui.tableWidget->item(i,TABLE_SUBNETWORK_NAME)->text();
			cntMmsA++;
		}
		else if (item->text() == tr("MMS B NET"))
		{
			mmsBName = ui.tableWidget->item(i,TABLE_SUBNETWORK_NAME)->text();
			cntMmsB++;
		}
	}

	if (cntMmsA == 0)
	{
		QMessageBox::warning(this,tr("告警"),tr("请至少选择一个MMS子网，单网下选择MMS A网。"));
		return;
	}

	if (cntMmsA > 1 || cntMmsB > 1)
	{
		QMessageBox::warning(this,tr("告警"),tr("只能选择一个MMS A网和一个MMS B网。"));
		return;
	}

	accept();
}

void DlgSubNetwork::closeEvent(QCloseEvent * e)
{
	int ret;

	ret = QMessageBox::question(this,tr("Message"),
		tr("You choose to cancel.\n\nWill not import the IP address of the device, can you?"),tr("Ok"),tr("Cancel"));
	if (ret != 0)
	{
		e->ignore();
		return;
	}

	QDialog::closeEvent(e);
}
