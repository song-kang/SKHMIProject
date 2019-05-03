#include "dlggrouptype.h"
#include "datasetdelegate.h"
#include "oe_group.h"
#include "view_plugin_scl_import.h"

DlgGroupType::DlgGroupType(QWidget *parent)
	: QDialog(parent)
{
	ui.setupUi(this);

	sclImport = (view_plugin_scl_import *)parent;

	init();
	initTable();

	connect(ui.pushButton_ok,SIGNAL(clicked()), this, SLOT(slotOk()));
	connect(ui.pushButton_cancel,SIGNAL(clicked()), this, SLOT(close()));
}

DlgGroupType::~DlgGroupType()
{

}

void DlgGroupType::init()
{
	row = 0;
	iedMap = sclImport->getIedGroupType();

	mapIntToStr.insert(GROUP_UNKNOWN,tr("δ֪"));
	mapIntToStr.insert(GROUP_SETTING,tr("��ֵ"));
	mapIntToStr.insert(GROUP_SETTING_AREA,tr("��ֵ��"));
	mapIntToStr.insert(GROUP_ANALOGUE,tr("������"));
	mapIntToStr.insert(GROUP_SWITCH,tr("������"));
	mapIntToStr.insert(GROUP_SOFT_STRAP,tr("��ѹ��"));
	mapIntToStr.insert(GROUP_EVENT,tr("����"));
	mapIntToStr.insert(GROUP_ALARM,tr("�澯"));
	mapIntToStr.insert(GROUP_FAULT,tr("����"));
	mapIntToStr.insert(GROUP_RELAY_REC,tr("¼��"));
	//mapIntToStr.insert(GROUP_CONTROL,tr("TeleControlling"));
	//mapIntToStr.insert(GROUP_ADJUSTING,tr("TeleAdjusting"));
	mapIntToStr.insert(GROUP_COMPLEX_ANALOGUE,tr("���Ӳ�����"));
	mapIntToStr.insert(GROUP_COMPLEX_SWITCH,tr("���ӿ�����"));
	mapIntToStr.insert(GROUP_GOOSE,tr("Goose"));
	mapIntToStr.insert(GROUP_SV,tr("SV"));
	mapIntToStr.insert(GROUP_LOG,tr("��־"));
	mapIntToStr.insert(GROUP_NO_IMPORT,tr("������"));

	mapStrToInt.insert(tr("δ֪"),GROUP_UNKNOWN);
	mapStrToInt.insert(tr("��ֵ"),GROUP_SETTING);
	mapStrToInt.insert(tr("��ֵ��"),GROUP_SETTING_AREA);
	mapStrToInt.insert(tr("������"),GROUP_ANALOGUE);
	mapStrToInt.insert(tr("������"),GROUP_SWITCH);
	mapStrToInt.insert(tr("��ѹ��"),GROUP_SOFT_STRAP);
	mapStrToInt.insert(tr("����"),GROUP_EVENT);
	mapStrToInt.insert(tr("�澯"),GROUP_ALARM);
	mapStrToInt.insert(tr("����"),GROUP_FAULT);
	mapStrToInt.insert(tr("¼��"),GROUP_RELAY_REC);
	//mapStrToInt.insert(tr("TeleControlling"),GROUP_CONTROL);
	//mapStrToInt.insert(tr("TeleAdjusting"),GROUP_ADJUSTING);
	mapStrToInt.insert(tr("���Ӳ�����"),GROUP_COMPLEX_ANALOGUE);
	mapStrToInt.insert(tr("���ӿ�����"),GROUP_COMPLEX_SWITCH);
	mapStrToInt.insert(tr("Goose"),GROUP_GOOSE);
	mapStrToInt.insert(tr("SV"),GROUP_SV);
	mapStrToInt.insert(tr("��־"),GROUP_LOG);
	mapStrToInt.insert(tr("������"),GROUP_NO_IMPORT);

	//mapStrToInt.insert(tr("Unknown"),GROUP_UNKNOWN);
	//mapStrToInt.insert(tr("Setting"),GROUP_SETTING);
	//mapStrToInt.insert(tr("SettingArea"),GROUP_SETTING_AREA);
	//mapStrToInt.insert(tr("Analogue"),GROUP_ANALOGUE);
	//mapStrToInt.insert(tr("Switch"),GROUP_SWITCH);
	//mapStrToInt.insert(tr("SoftStrap"),GROUP_SOFT_STRAP);
	//mapStrToInt.insert(tr("Event"),GROUP_EVENT);
	//mapStrToInt.insert(tr("Alarm"),GROUP_ALARM);
	//mapStrToInt.insert(tr("Fault"),GROUP_FAULT);
	//mapStrToInt.insert(tr("RelayRec"),GROUP_RELAY_REC);
	//mapStrToInt.insert(tr("TeleControlling"),GROUP_CONTROL);
	//mapStrToInt.insert(tr("TeleAdjusting"),GROUP_ADJUSTING);
	//mapStrToInt.insert(tr("ComplexAnalogue"),GROUP_COMPLEX_ANALOGUE);
	//mapStrToInt.insert(tr("ComplexSwitch"),GROUP_COMPLEX_SWITCH);
	//mapStrToInt.insert(tr("Goose"),GROUP_GOOSE);
	//mapStrToInt.insert(tr("SV"),GROUP_SV);
	//mapStrToInt.insert(tr("Log"),GROUP_LOG);
}

void DlgGroupType::initTable()
{
	ui.tableWidget_type->horizontalHeader()->setStyleSheet("QHeaderView::section{background:yellow;}"); 
	ui.tableWidget_type->verticalHeader()->setStyleSheet("QHeaderView::section{background:yellow;}");
	ui.tableWidget_type->setStyleSheet("QTableCornerButton::section{background:yellow;}");

	ui.tableWidget_type->setSelectionBehavior(QAbstractItemView::SelectRows);		//����ѡ��ģʽ
	//ui.tableWidget_type->setEditTriggers(QAbstractItemView::NoEditTriggers);			//���ɱ༭
	ui.tableWidget_type->setSelectionMode(QAbstractItemView::SingleSelection);		//��ѡģʽ
	ui.tableWidget_type->setFocusPolicy(Qt::NoFocus);								//ȥ�����㣬�����
	//ui.tableWidget_type->horizontalHeader()->setStretchLastSection(true);			//���ó�������
	ui.tableWidget_type->setStyleSheet("selection-background-color:lightblue;");		//����ѡ�б���ɫ
	ui.tableWidget_type->verticalHeader()->setDefaultSectionSize(20);				//�����и�
	ui.tableWidget_type->horizontalHeader()->setHighlightSections(false);			//�����ʱ���Ա�ͷ�й���
	ui.tableWidget_type->setAlternatingRowColors(true);								//���ý�����ɫ
	ui.tableWidget_type->setItemDelegateForColumn(TABLE_DATASET_GROUP,new DataSetDelegate(this));

	ui.tableWidget_type->setColumnWidth(TABLE_LDEVICE,200);
	ui.tableWidget_type->setColumnWidth(TABLE_DATASET_NAME,200);
	ui.tableWidget_type->setColumnWidth(TABLE_DATASET_GROUP,150);
}

void DlgGroupType::slotOk()
{
	for (int i = 0; i < ui.tableWidget_type->rowCount(); i++)
	{
		QString ld = ui.tableWidget_type->item(i,TABLE_LDEVICE)->text();
		QString ds = ui.tableWidget_type->item(i,TABLE_DATASET_NAME)->text();
		QString tp = ui.tableWidget_type->item(i,TABLE_DATASET_GROUP)->text();
		if (tp.isEmpty() || tp == tr("δ֪"))
		{
			QMessageBox::warning(this,tr("Message"),tr("Please select the type of DataSet, Can not be unknown."));
			return;
		}
		else
		{
			if (!bSmartImport)	//�����ܵ���
			{
				if(!replaceTypeMap(ld.split("-").at(0).trimmed(),ds.split("-").at(0).trimmed(),tp))
				{
					QMessageBox::warning(this,tr("Message"),tr("Cannot replace type of DataSet."));
					return;
				}
			}
			else	//���ܵ���
			{
				if(!replaceTypeMap(ds.split("-").at(0).trimmed(),tp))
				{
					QMessageBox::warning(this,tr("Message"),tr("Cannot replace type of DataSet."));
					return;
				}
			}
		}
	}

	accept();
}

void DlgGroupType::closeEvent(QCloseEvent * e)
{
	int ret;

	ret = QMessageBox::question(this,tr("Message"),tr("You choose to cancel.\n\nDevice [%1] will not be imported, can you?")
		.arg(ui.label_ied_name->text()),tr("Ok"),tr("Cancel"));
	if (ret != 0)
	{
		e->ignore();
		return;
	}

	QDialog::closeEvent(e);
}

void DlgGroupType::start()
{
	row = 0;

	ui.label_ied_name->setText(tr("%1 - %2").arg(iedName).arg(iedDesc));

	if (!bSmartImport)	//�����ܵ���
	{
		QMap<XmlObject*,int>::const_iterator obj_iter = iedMap.begin();
		for (;obj_iter != iedMap.end(); obj_iter++)
		{
			XmlObject * obj = obj_iter.key();
			appendTableWidget(obj,obj_iter.value());
			continue;
		}
	}
	else	//���ܵ���
	{
		XmlObject * object = document.at(0)->findChildDeep("IED","name",iedName);
		if (object)
		{
			ui.tableWidget_type->setRowCount(0);
			ui.tableWidget_type->clearContents();

			QMap<QString,int>::const_iterator iter = typeMap.begin();
			for (;iter != typeMap.end(); iter++)
			{
				XmlObject * obj = object->findChildDeep("DataSet","name",iter.key());
				if (obj)
				{
					appendTableWidget(obj,iter.value());
					continue;
				}
			}
		}
	}
}

void DlgGroupType::appendTableWidget(XmlObject * object,int typeVal)
{
	ui.tableWidget_type->setRowCount(++row);
	QString text = object->attrib("name") + " - " + object->attrib("desc");

	QTableWidgetItem * item = new QTableWidgetItem(text);
	item->setTextAlignment(Qt::AlignCenter);
	item->setFlags(item->flags() & (~Qt::ItemIsEditable));	//����item��ɱ༭
	ui.tableWidget_type->setItem(row-1,TABLE_DATASET_NAME,item);	

	QString strType = mapIntToStr.value(typeVal);
	item = new QTableWidgetItem(strType);
	item->setTextAlignment(Qt::AlignCenter);
	ui.tableWidget_type->setItem(row-1,TABLE_DATASET_GROUP,item);

	while(object)
	{
		if (object->name == "LDevice")
		{
			text = object->attrib("inst") + " - " + object->attrib("desc");
			item = new QTableWidgetItem(text);
			item->setTextAlignment(Qt::AlignCenter);
			item->setFlags(item->flags() & (~Qt::ItemIsEditable));	//����item��ɱ༭
			ui.tableWidget_type->setItem(row-1,TABLE_LDEVICE,item);	
			break;
		}

		object = object->parent;
	}
}

bool DlgGroupType::replaceTypeMap(QString dataset,QString type)
{
	QMap<QString,int>::const_iterator iter = typeMap.find(dataset);
	while (iter == typeMap.end()) 
		return false;

	int t = mapStrToInt.value(type);
	typeMap[dataset] = t;

	return true;
}

bool DlgGroupType::replaceTypeMap(QString ldevice,QString dataset,QString type)
{
	XmlObject * object = document.at(0)->findChildDeep("IED","name",iedName);
	if (object)
	{
		object = object->findChildDeep("LDevice","inst",ldevice);
		if (object)
		{
			object = object->findChildDeep("DataSet","name",dataset);
			if (object)
			{
				QMap<XmlObject*,int>::const_iterator obj_iter = iedMap.find(object);
				if (obj_iter != iedMap.end())
				{
					int t = mapStrToInt.value(type);
					iedMap[object] = t;
				}
				else
					return false;
			}
			else
				return false;
		}
		else
			return false;
	}
	else
		return false;

	return true;
}
