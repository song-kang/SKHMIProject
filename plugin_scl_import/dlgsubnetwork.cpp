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

	ui.tableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);		//����ѡ��ģʽ
	//ui.tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);			//���ɱ༭
	ui.tableWidget->setSelectionMode(QAbstractItemView::SingleSelection);		//��ѡģʽ
	ui.tableWidget->setFocusPolicy(Qt::NoFocus);								//ȥ�����㣬�����
	ui.tableWidget->horizontalHeader()->setStretchLastSection(true);			//���ó�������
	ui.tableWidget->setStyleSheet("selection-background-color:lightblue;");		//����ѡ�б���ɫ
	ui.tableWidget->verticalHeader()->setDefaultSectionSize(20);				//�����и�
	ui.tableWidget->horizontalHeader()->setHighlightSections(false);			//�����ʱ���Ա�ͷ�й���
	ui.tableWidget->setAlternatingRowColors(true);								//���ý�����ɫ
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
		item->setFlags(item->flags() & (~Qt::ItemIsEditable));	//����item��ɱ༭
		ui.tableWidget->setItem(row-1,TABLE_SUBNETWORK_NAME,item);	

		text = object->attrib("desc");
		item = new QTableWidgetItem(text);
		item->setTextAlignment(Qt::AlignCenter);
		item->setFlags(item->flags() & (~Qt::ItemIsEditable));	//����item��ɱ༭
		ui.tableWidget->setItem(row-1,TABLE_SUBNETWORK_DESC,item);

		text = object->attrib("type");
		item = new QTableWidgetItem(text);
		item->setTextAlignment(Qt::AlignCenter);
		item->setFlags(item->flags() & (~Qt::ItemIsEditable));	//����item��ɱ༭
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
		QMessageBox::warning(this,tr("�澯"),tr("������ѡ��һ��MMS������������ѡ��MMS A����"));
		return;
	}

	if (cntMmsA > 1 || cntMmsB > 1)
	{
		QMessageBox::warning(this,tr("�澯"),tr("ֻ��ѡ��һ��MMS A����һ��MMS B����"));
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
