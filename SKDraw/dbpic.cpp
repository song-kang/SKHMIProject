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

	ui.listWidget->setViewMode(QListView::IconMode);		//设置QListWidget的显示模式
	ui.listWidget->setIconSize(QSize(100,100));				//设置QListWidget中单元项的图片大小
	ui.listWidget->setSpacing(10);							//设置QListWidget中单元项的间距
	ui.listWidget->setResizeMode(QListWidget::Adjust);		//设置自动适应布局调整（Adjust适应，Fixed不适应），默认不适应
	ui.listWidget->setMovement(QListWidget::Static);		//设置不能移动
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
		QMessageBox::warning(NULL,tr("告警"),tr("请选择一张图片"));
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
		QMessageBox::warning(NULL,tr("告警"),tr("需连接数据库"));
		return;
	}

	QStringList listFileName = QFileDialog::getOpenFileNames(NULL, tr("选择图像文件"), QString::null, tr("图像文件(*.bmp *.jpg *.png)"));
	foreach (QString fileName, listFileName)
	{
		QFile file(fileName);
		if (!file.open(QIODevice::ReadOnly))
		{
			QMessageBox::warning(NULL,tr("告警"),tr("文件读模式打开失败"));
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
		QMessageBox::warning(NULL,tr("告警"),tr("请选择需导出的图片"));
		return;
	}

	if (m_app->GetDBState() == false)
	{
		QMessageBox::warning(NULL,tr("告警"),tr("需连接数据库"));
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

	QMessageBox::information(NULL,tr("提示"),tr("图片导出完毕"));
}

void DBPic::SlotDel()
{
	QList<QListWidgetItem *> list = ui.listWidget->selectedItems();
	if (list.count() == 0)
	{
		QMessageBox::warning(NULL,tr("告警"),tr("请选择需删除的图片"));
		return;
	}

	if (m_app->GetDBState() == false)
	{
		QMessageBox::warning(NULL,tr("告警"),tr("需连接数据库"));
		return;
	}

	int ret = QMessageBox::question(this, "询问", "确认删除图片？",tr("删除"),tr("放弃"));
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
