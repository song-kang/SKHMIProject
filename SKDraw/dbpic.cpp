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
}

void DBPic::InitUi()
{
	setStyleSheet(tr("QWidget#%1{background:rgb(255,255,255,220);border-bottom-left-radius:6px;border-bottom-right-radius:6px;}").arg(objectName()));
}

void DBPic::InitSlot()
{
	connect(ui.btnOk, SIGNAL(clicked()), this, SLOT(SlotOk()));
	connect(ui.btnAdd, SIGNAL(clicked()), this, SLOT(SlotAdd()));
	connect(ui.btnDel, SIGNAL(clicked()), this, SLOT(SlotDel()));
	connect(ui.listWidget, SIGNAL(itemClicked(QListWidgetItem*)), this, SLOT(SlotListItemClicked(QListWidgetItem*)));
}

void DBPic::paintEvent(QPaintEvent *e)
{
	SKWidget::paintEvent(e);
}

void DBPic::Start()
{
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
			}
		}
	}
}

void DBPic::SlotOk()
{
	if (!m_pCurrentListWidgetItem)
		return;

	QPixmap pix = m_pCurrentListWidgetItem->data(Qt::UserRole).value<QPixmap>();
	m_app->SetPicture(pix);
	m_app->SetExitPicture(true);

	emit SigClose();
}

void DBPic::SlotAdd()
{
	QString fileName = QFileDialog::getOpenFileName(NULL, tr("选择图像文件"), QString::null, tr("图像文件(*.bmp *.jpg *.png)"));
	if (!fileName.isEmpty())
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
			if (!DB->UpdateLobFromMem("t_ssp_svglib_item","svg_file",sWhere,(unsigned char*)buffer.data(),len))
				QMessageBox::warning(NULL,tr("告警"),tr("添加失败"));
			else
				QMessageBox::information(NULL,tr("提示"),tr("添加成功"));
		}
	}
}

void DBPic::SlotDel()
{

}

void DBPic::SlotListItemClicked(QListWidgetItem *listItem)
{
	m_pCurrentListWidgetItem = listItem;
}
