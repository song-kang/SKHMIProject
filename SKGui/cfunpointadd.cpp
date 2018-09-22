#include "cfunpointadd.h"
#include "skgui.h"
#include "cpluginmgr.h"

#define COLUMN_USERS		0
#define COLUMN_USER			1
#define COLUMN_AUTH			2

CFunPointAdd::CFunPointAdd(QWidget *parent)
	: SKWidget(parent)
{
	ui.setupUi(this);

	Init();
	InitUi();
	InitSlot();
}

CFunPointAdd::~CFunPointAdd()
{

}

void CFunPointAdd::Init()
{
	m_iType = 0;
	m_pTreeItem = NULL;
	m_iImageLen = 0;
	m_bSaveSuccess = false;

	QRegExp regx("[0-9]+$");
	QValidator *validator = new QRegExpValidator(regx, ui.lineEdit_no);
	ui.lineEdit_no->setValidator(validator); 

	ui.comboBox_type->insertItem(NORMAL_PLUGIN,"一般插件功能点");
	ui.comboBox_type->insertSeparator(1);
	//ui.comboBox_type->insertItem(1,"动态菜单项");
	ui.comboBox_type->insertItem(DRAW_PLUGIN,"组态插件功能点");

	QList<CPlugin*> l = SK_GUI->m_pPluginMgr->GetPluginList();
	foreach (CPlugin *plugin, l)
	{
		QString funpoint = plugin->m_pPlugin_FunPointSupported();
		QStringList fpList = funpoint.split(";");
		foreach (QString s, fpList)
		{
			if (!s.isEmpty())
				ui.comboBox_plugin->addItem(s);
		}
	}

	ui.tableWidget_auth->setColumnWidth(COLUMN_USERS,150);
	ui.tableWidget_auth->setColumnWidth(COLUMN_USER,150);
	ui.tableWidget_auth->setColumnWidth(COLUMN_AUTH,80);
	ui.tableWidget_auth->setSelectionBehavior(QAbstractItemView::SelectRows);		//整行选择模式
	//ui.tableWidget_auth->setEditTriggers(QAbstractItemView::NoEditTriggers);		//不可编辑
	ui.tableWidget_auth->setSelectionMode(QAbstractItemView::SingleSelection);		//单选模式
	ui.tableWidget_auth->setFocusPolicy(Qt::NoFocus);								//去除焦点，无虚框
	//ui.tableWidget_auth->horizontalHeader()->setStretchLastSection(true);			//设置充满表宽度
	ui.tableWidget_auth->setStyleSheet("selection-background-color:lightblue;");	//设置选中背景色
	ui.tableWidget_auth->verticalHeader()->setDefaultSectionSize(22);				//设置行高
	ui.tableWidget_auth->horizontalHeader()->setHighlightSections(false);			//点击表时不对表头行光亮
	ui.tableWidget_auth->setAlternatingRowColors(true);								//设置交替行色
	ui.tableWidget_auth->verticalHeader()->setVisible(false);						//去除最前列
}

void CFunPointAdd::InitUi()
{
	ui.label_wnd->setVisible(false);
	ui.comboBox_wnd->setVisible(false);

	setStyleSheet(tr("QWidget#%1{background:rgb(255,255,255,220);border-bottom-left-radius:6px;border-bottom-right-radius:6px;}").arg(objectName()));
}

void CFunPointAdd::InitSlot()
{
	connect(ui.btnIconImport, SIGNAL(clicked()), this, SLOT(SlotIconImport()));
	connect(ui.btnSave, SIGNAL(clicked()), this, SLOT(SlotSave()));
	connect(ui.comboBox_type, SIGNAL(currentIndexChanged(int)), this, SLOT(SlotTypeCurrentIndexChanged(int)));
}

void CFunPointAdd::paintEvent(QPaintEvent *e)
{
	SKWidget::paintEvent(e);
}

void CFunPointAdd::Start()
{
	SString sql;
	SRecordset rs;

	if (!m_pTreeItem)
		sql.sprintf("select max(idx) from t_ssp_fun_point where p_fun_key='top'");
	else
		sql.sprintf("select max(idx) from t_ssp_fun_point where p_fun_key='%s'",m_sKey.toStdString().data());
	int cnt = DB->Retrieve(sql,rs);
	if (cnt > 0)
	{
		int idx = rs.GetValue(0,0).toInt();
		ui.lineEdit_no->setText(tr("%1").arg(++idx));
	}

	if (m_iType == TYPE_FOLDER)
	{
		ui.comboBox_plugin->setEditText(tr("folder.%1").arg(QDateTime::currentDateTime().toTime_t()));
		ui.label_type->setVisible(false);
		ui.label_plugin->setVisible(false);
		ui.label_icon->setVisible(false);
		ui.label_auth->setVisible(false);
		ui.label_wnd->setVisible(false);
		ui.comboBox_type->setVisible(false);
		ui.comboBox_plugin->setVisible(false);
		ui.comboBox_wnd->setVisible(false);
		ui.btnIconImport->setVisible(false);
		ui.tableWidget_auth->setVisible(false);
	}

	sql.sprintf("select grp_code,usr_code,usr_sn from t_ssp_user group by grp_code asc,usr_code asc");
	cnt = DB->Retrieve(sql,rs);
	if (cnt > 0)
	{
		int row = 0;
		QTableWidgetItem * item;
		ui.tableWidget_auth->setRowCount(cnt);
		for (int i = 0; i < cnt; i++)
		{
			QString grp_code = rs.GetValue(i,0).data();
			QString usr_code = rs.GetValue(i,1).data();
			int usr_sn = rs.GetValue(i,2).toInt();

			item = new QTableWidgetItem(grp_code);
			item->setTextAlignment(Qt::AlignCenter);
			ui.tableWidget_auth->setItem(row,COLUMN_USERS,item);

			item = new QTableWidgetItem(usr_code,usr_sn);
			item->setTextAlignment(Qt::AlignCenter);
			item->setFlags(item->flags() & (~Qt::ItemIsEditable));	//设置item项不可编辑
			ui.tableWidget_auth->setItem(row,COLUMN_USER,item);

			item = new QTableWidgetItem("");
			item->setTextAlignment(Qt::AlignCenter);
			item->setCheckState(Qt::Unchecked);
			item->setFlags(item->flags() & (~Qt::ItemIsEditable));	//设置item项不可编辑
			ui.tableWidget_auth->setItem(row,COLUMN_AUTH,item);

			row++;
		}
	}

	sql.sprintf("select wnd_sn,wnd_name from t_ssp_uicfg_wnd");
	cnt = DB->Retrieve(sql,rs);
	if (cnt > 0)
	{
		ui.comboBox_wnd->insertItem(0, "空", 0);
		for (int i = 0; i < cnt; i++)
			ui.comboBox_wnd->insertItem(i+1, rs.GetValue(i,1).data(), rs.GetValue(i,0).toInt());
	}
}

void CFunPointAdd::SlotTypeCurrentIndexChanged(int index)
{
	Q_UNUSED(index);

	if (index == NORMAL_PLUGIN)
	{
		ui.label_wnd->setVisible(false);
		ui.comboBox_wnd->setVisible(false);
	}
	else if (index == DRAW_PLUGIN)
	{
		ui.label_wnd->setVisible(true);
		ui.comboBox_wnd->setVisible(true);
	}
}

void CFunPointAdd::SlotIconImport()
{
	QString filter = tr("Image files(*.png *.jpg)");
	QString fileName = QFileDialog::getOpenFileName(this,tr("打开图像"),QString::null,filter);
	if (!fileName.isEmpty())
	{
		QFile file(fileName);
		if (!file.open(QIODevice::ReadOnly))
		{
			QMessageBox::warning(NULL,tr("告警"),tr("文件读模式打开失败"));
			return;
		}

		m_ImageBuffer = file.read(file.bytesAvailable());
		m_iImageLen = file.size();
		file.close();

		QPixmap pix;
		pix.loadFromData((uchar*)m_ImageBuffer.data(),m_iImageLen);
		ui.btnIconImport->setIcon(QIcon(pix));
	}
}

void CFunPointAdd::SlotSave()
{
	if (ui.lineEdit_no->text().trimmed().isEmpty())
	{
		QMessageBox::warning(NULL,tr("告警"),tr("排序编号不可为空"));
		return;
	}
	if (ui.lineEdit_name->text().trimmed().isEmpty())
	{
		QMessageBox::warning(NULL,tr("告警"),tr("条目名称不可为空"));
		return;
	}
	if (ui.comboBox_plugin->currentText().trimmed().isEmpty())
	{
		QMessageBox::warning(NULL,tr("告警"),tr("插件名称不可为空"));
		return;
	}

	m_sFunPointKey = ui.comboBox_plugin->currentText().trimmed();
	if (m_sFunPointKey.contains("="))
		m_sFunPointKey = m_sFunPointKey.split("=").at(0).trimmed();

	char buf[1024] = {'\0'};
	strcpy(buf,ui.lineEdit_name->text().trimmed().toStdString().data());
	int size = strlen(buf);
	if (size > 32) //数据库字段长度32
	{
		QMessageBox::warning(NULL,tr("告警"),tr("条目名称过长"));
		return;
	}
	memset(buf,0,1024);
	strcpy(buf,m_sFunPointKey.toStdString().data());
	size = strlen(buf);
	if (size > 32) //数据库字段长度32
	{
		QMessageBox::warning(NULL,tr("告警"),tr("插件名称过长"));
		return;
	}

	SString sql;
	SRecordset rs;
	sql.sprintf("select count(*) from t_ssp_fun_point where fun_key='%s'",m_sFunPointKey.toStdString().data());
	int cnt = DB->Retrieve(sql,rs);
	if (m_sFunPointKey != "plugin_drawer" && 
		rs.GetValue(0,0).toInt() > 0)
	{
		QMessageBox::warning(NULL,tr("告警"),tr("插件名称重复"));
		return;
	}

	m_sFunPointName = ui.lineEdit_name->text();
	if (m_sFunPointKey == "plugin_drawer")
		m_sFunPointKey = QString("%1.%2").arg(m_sFunPointKey).arg(SDateTime::getNowSoc());

	sql.sprintf("insert into t_ssp_fun_point (fun_key,p_fun_key,name,idx,type,ref_sn) values ('%s','%s','%s',%d,%d,%d)",
		m_sFunPointKey.toStdString().data(),
		m_pTreeItem ? m_sKey.toStdString().data() : "top",
		m_sFunPointName.toStdString().data(),
		ui.lineEdit_no->text().toInt(),
		m_iType == TYPE_FOLDER ? 1 : ui.comboBox_type->currentIndex(),
		m_iType == TYPE_APP ? ui.comboBox_wnd->itemData(ui.comboBox_wnd->currentIndex()).toInt() : -1);
	if (!DB->Execute(sql))
	{
		QMessageBox::warning(NULL,tr("告警"),tr("保存失败"));
		return;
	}

	SApi::UsSleep(500000);
	if (m_iImageLen > 0)
	{
		SString sWhere = SString::toFormat("fun_key='%s'",m_sFunPointKey.toStdString().data());
		if (!DB->UpdateLobFromMem("t_ssp_fun_point","img_normal",sWhere,(unsigned char*)m_ImageBuffer.data(),m_iImageLen))
			QMessageBox::warning(NULL,tr("告警"),tr("图标更换失败"));
		m_iImageLen = 0;
	}

	for (int i = 0; i < ui.tableWidget_auth->rowCount(); i++)
	{
		QTableWidgetItem *item = ui.tableWidget_auth->item(i,COLUMN_AUTH);
		int state = 0;
		if (item->checkState() == Qt::Checked)
			state = 1;
		else if (item->checkState() == Qt::Unchecked)
			state = 0;

		sql.sprintf("insert into t_ssp_user_auth (usr_sn,fun_key,auth) values (%d,'%s',%d)",
			ui.tableWidget_auth->item(i,COLUMN_USER)->type(),
			m_sFunPointKey.toStdString().data(),
			m_iType == TYPE_APP ? state : 1);
		if (!DB->Execute(sql))
			QMessageBox::warning(NULL,tr("告警"),tr("用户权限更新失败"));

		sql.sprintf("insert into t_ssp_usergroup_auth (grp_code,fun_key,auth) values ('%s','%s',%d)",
			ui.tableWidget_auth->item(i,COLUMN_USERS)->text().trimmed().toStdString().data(),
			m_sFunPointKey.toStdString().data(),
			m_iType == TYPE_APP ? state : 1);
		if (!DB->Execute(sql))
			QMessageBox::warning(NULL,tr("告警"),tr("用户组权限更新失败"));
	}

	m_bSaveSuccess = true;
	emit SigClose();
}
