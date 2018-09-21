#include "dbpic.h"
#include "skdraw.h"
#include "sk_database.h"

DBPic::DBPic(QWidget *parent)
	: SKWidget(parent)
{
	ui.setupUi(this);

	m_app = (SKDraw *)parent;

	Init();
	InitUi();
	InitSlot();
}

DBPic::~DBPic()
{

}

void DBPic::Init()
{
	m_pCurrentListWidgetItem = NULL;

	ui.listWidget->setViewMode(QListView::IconMode);		//����QListWidget����ʾģʽ
	ui.listWidget->setIconSize(QSize(100,100));				//����QListWidget�е�Ԫ���ͼƬ��С
	ui.listWidget->setSpacing(10);							//����QListWidget�е�Ԫ��ļ��
	ui.listWidget->setResizeMode(QListWidget::Adjust);		//�����Զ���Ӧ���ֵ�����Adjust��Ӧ��Fixed����Ӧ����Ĭ�ϲ���Ӧ
	ui.listWidget->setMovement(QListWidget::Static);		//���ò����ƶ�
	ui.listWidget->setSelectionMode(QAbstractItemView:: ExtendedSelection);
}

void DBPic::InitUi()
{
	setStyleSheet(tr("QWidget#%1{background:rgb(255,255,255,220);border-bottom-left-radius:6px;border-bottom-right-radius:6px;}").arg(objectName()));
}

void DBPic::InitSlot()
{
	connect(ui.btnOk, SIGNAL(clicked()), this, SLOT(SlotOk()));
	connect(ui.btnImport, SIGNAL(clicked()), this, SLOT(SlotImport()));
	connect(ui.btnExport, SIGNAL(clicked()), this, SLOT(SlotExport()));
	connect(ui.btnDel, SIGNAL(clicked()), this, SLOT(SlotDel()));
	connect(ui.btnClose, SIGNAL(clicked()), this, SLOT(SlotClose()));
	connect(ui.listWidget, SIGNAL(itemClicked(QListWidgetItem*)), this, SLOT(SlotListItemClicked(QListWidgetItem*)));
}

void DBPic::paintEvent(QPaintEvent *e)
{
	SKWidget::paintEvent(e);
}

void DBPic::Start()
{
	m_pCurrentListWidgetItem = NULL;

	if (m_app->GetDBState() == false)
		return;

	SString sql;
	SRecordset rs;
	sql.sprintf("select svg_sn,svg_name from t_ssp_svglib_item where svgtype_sn=3");
	int cnt = DB->Retrieve(sql,rs);
	if (cnt > 0)
	{
		for (int i = 0; i < cnt; i++)
		{
			int sn = rs.GetValue(i,0).toInt();
			QString name = rs.GetValue(i,1).data();

			int len = 0;
			unsigned char* buffer = NULL;
			SString sWhere = SString::toFormat("svg_sn=%d",sn);
			if (DB->ReadLobToMem("t_ssp_svglib_item","svg_file",sWhere,buffer,len))
			{
				QPixmap pix;
				pix.loadFromData(buffer,len);
				QListWidgetItem *item = new QListWidgetItem(name,NULL,sn);
				item->setIcon(QIcon(pix));
				QVariant var;
				var.setValue(pix);
				item->setData(Qt::UserRole,var);
				ui.listWidget->addItem(item);
				delete buffer;
			}
		}
	}
}

void DBPic::SlotOk()
{
	QList<QListWidgetItem *> list = ui.listWidget->selectedItems();
	if (list.count() != 1)
	{
		QMessageBox::warning(NULL,tr("�澯"),tr("��ѡ��һ��ͼƬ"));
		return;
	}

	m_pCurrentListWidgetItem = list.at(0);
	QPixmap pix = m_pCurrentListWidgetItem->data(Qt::UserRole).value<QPixmap>();
	m_app->SetPicture(pix);
	m_app->SetPictureSn(m_pCurrentListWidgetItem->type());
	m_app->SetExitPicture(true);

	emit SigClose();
}

void DBPic::SlotImport()
{
	if (m_app->GetDBState() == false)
	{
		QMessageBox::warning(NULL,tr("�澯"),tr("���������ݿ�"));
		return;
	}

	QStringList listFileName = QFileDialog::getOpenFileNames(NULL, tr("ѡ��ͼ���ļ�"), QString::null, tr("ͼ���ļ�(*.bmp *.jpg *.png)"));
	foreach (QString fileName, listFileName)
	{
		QFile file(fileName);
		if (!file.open(QIODevice::ReadOnly))
		{
			QMessageBox::warning(NULL,tr("�澯"),tr("�ļ���ģʽ��ʧ��"));
			return;
		}
		int len = file.size();
		QByteArray buffer = file.read(file.bytesAvailable());;
		file.close();

		int sn = DB->SelectIntoI(SString::toFormat("select max(svg_sn) from t_ssp_svglib_item")) + 1;
		SString sql = SString::toFormat("insert into t_ssp_svglib_item (svg_sn,svgtype_sn,svg_name) values (%d,%d,'%s')",
			sn,3,QFileInfo(fileName).fileName().toStdString().data());
		if (DB->Execute(sql))
		{
			SString sWhere = SString::toFormat("svg_sn=%d",sn);
			bool ret = DB->UpdateLobFromMem("t_ssp_svglib_item","svg_file",sWhere,(unsigned char*)buffer.data(),len);
		}
	}

	ui.listWidget->clear();
	Start();
}

void DBPic::SlotExport()
{
	QList<QListWidgetItem *> list = ui.listWidget->selectedItems();
	if (list.count() == 0)
	{
		QMessageBox::warning(NULL,tr("�澯"),tr("��ѡ���赼����ͼƬ"));
		return;
	}

	if (m_app->GetDBState() == false)
	{
		QMessageBox::warning(NULL,tr("�澯"),tr("���������ݿ�"));
		return;
	}

	QString dir = QFileDialog::getExistingDirectory();
	foreach (QListWidgetItem *item, list)
	{
		SString sql;
		SRecordset rs;
		sql.sprintf("select svg_name from t_ssp_svglib_item where svg_sn=%d",item->type());
		int cnt = DB->Retrieve(sql,rs);
		if (cnt > 0)
		{
			QString name = rs.GetValue(0,0).data();

			int len = 0;
			unsigned char* buffer = NULL;
			SString sWhere = SString::toFormat("svg_sn=%d",item->type());
			if (DB->ReadLobToMem("t_ssp_svglib_item","svg_file",sWhere,buffer,len))
			{
				QString path = dir + "\\" + name;
				QFile file(path);
				if (!file.open(QFile::WriteOnly))
				{
					delete buffer;
					continue;
				}
				file.write((char*)buffer, len);
				delete buffer;
			}
		}
	}

	QMessageBox::information(NULL,tr("��ʾ"),tr("ͼƬ�������"));
}

void DBPic::SlotDel()
{
	QList<QListWidgetItem *> list = ui.listWidget->selectedItems();
	if (list.count() == 0)
	{
		QMessageBox::warning(NULL,tr("�澯"),tr("��ѡ����ɾ����ͼƬ"));
		return;
	}

	if (m_app->GetDBState() == false)
	{
		QMessageBox::warning(NULL,tr("�澯"),tr("���������ݿ�"));
		return;
	}

	int ret = QMessageBox::question(this, "ѯ��", "ȷ��ɾ��ͼƬ��",tr("ɾ��"),tr("����"));
	if (ret != 0)
		return;

	foreach (QListWidgetItem *item, list)
	{
		SString sql = SString::toFormat("delete from t_ssp_svglib_item where svg_sn=%d",item->type());
		bool ret = DB->Execute(sql);
	}

	ui.listWidget->clear();
	Start();
}

void DBPic::SlotClose()
{
	emit SigClose();
}

void DBPic::SlotListItemClicked(QListWidgetItem *listItem)
{
	m_pCurrentListWidgetItem = listItem;
}
