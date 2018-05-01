#include "cfunpointedit.h"
#include "skgui.h"
#include "chmiwidget.h"
#include "cpluginmgr.h"
#include "cusers.h"
#include "cfunpointadd.h"

#define TREE_ITEM_ROOT		0
#define TREE_ITEM_FOLDER	1
#define TREE_ITEM_APP		2

#define COLUMN_USERS		0
#define COLUMN_USER			1
#define COLUMN_AUTH			2

CFunPointEdit::CFunPointEdit(QWidget *parent)
	: SKWidget(parent)
{
	ui.setupUi(this);

	m_pHmi = (CHMIWidget *)parent;

	Init();
	InitUi();
	InitSlot();
}

CFunPointEdit::~CFunPointEdit()
{

}

void CFunPointEdit::Init()
{
	m_bIsSave = false;
	m_bIconChange = false;
	m_iImageLen = 0;

	ui.comboBox_type->insertItem(0,"插件功能点");
	ui.comboBox_type->insertSeparator(1);
	//ui.comboBox_type->insertItem(1,"动态菜单项");
	ui.comboBox_type->insertItem(2,"组态界面功能点");
	ui.comboBox_type->insertItem(3,"动态报表功能点");

	QRegExp regx("[0-9]+$");
	QValidator *validator = new QRegExpValidator(regx, ui.lineEdit_no);
	ui.lineEdit_no->setValidator(validator); 

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

	m_pMenuNull = new QMenu(this);
	m_pMenuNull->addAction(QIcon(":/images/folder_open"),tr("添加文件夹(&O)"));
	m_pMenuNull->addAction(QIcon(":/images/application"),tr("添加功能点(&N)"));
	m_pMenuFolder = new QMenu(this);
	m_pMenuFolder->addAction(QIcon(":/images/folder_open"),tr("添加文件夹(&O)"));
	m_pMenuFolder->addAction(QIcon(":/images/application"),tr("添加功能点(&N)"));
	m_pMenuFolder->addAction(QIcon(":/images/cancel"),tr("删除文件夹(&D)"));
	m_pMenuApp = new QMenu(this);
	m_pMenuApp->addAction(QIcon(":/images/cancel"),tr("删除功能点(&D)"));

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

	ui.treeWidget->installEventFilter(this);
	m_pFunPointAddWidget = NULL;
}

void CFunPointEdit::InitTreeWidget(QTreeWidgetItem *item, QString key)
{
	SString sql;
	SRecordset rs;
	sql.sprintf("select fun_key,p_fun_key,name,type from t_ssp_fun_point where p_fun_key='%s' group by idx,fun_key",key.toStdString().data());
	int cnt = DB->Retrieve(sql,rs);
	if (cnt > 0)
	{
		for (int i = 0; i < cnt; i++)
		{
			QTreeWidgetItem *it = NULL;
			QString fun_key = rs.GetValue(i,0).data();
			QString p_fun_key = rs.GetValue(i,1).data();
			QString name = rs.GetValue(i,2).data();
			int type = rs.GetValue(i,3).toInt();
			if (type == 1)
			{
				it = new QTreeWidgetItem(item,TREE_ITEM_FOLDER);
				it->setIcon(0,QIcon(":/images/folder_open"));
			}
			else
			{
				it = new QTreeWidgetItem(item,TREE_ITEM_APP);
				int imageLen = 0;
				unsigned char* imageBuffer = NULL;
				SString sWhere = SString::toFormat("fun_key='%s'",fun_key.toStdString().data());
				DB->ReadLobToMem("t_ssp_fun_point","img_normal",sWhere,imageBuffer,imageLen);
				if (imageBuffer && imageLen > 0)
				{
					QPixmap pix;
					pix.loadFromData(imageBuffer,imageLen);
					it->setIcon(0,QIcon(pix));
				}
				else
					it->setIcon(0,QIcon(":/images/application"));
				if (imageBuffer)
					delete imageBuffer;
			}
			it->setText(0,name);
			it->setToolTip(0,name);
			it->setData(0,Qt::UserRole,fun_key);
			InitTreeWidget(it,fun_key);
		}
	}
}

void CFunPointEdit::InitUi()
{
	ui.splitter->setStretchFactor(0,4);
	ui.splitter->setStretchFactor(1,6);

	setStyleSheet(tr("QWidget#%1{background:rgb(255,255,255,220);border-bottom-left-radius:6px;border-bottom-right-radius:6px;}").arg(objectName()));
}

void CFunPointEdit::InitSlot()
{
	connect(ui.treeWidget,SIGNAL(itemClicked(QTreeWidgetItem *,int)),this,SLOT(SlotTreeItemClicked(QTreeWidgetItem *,int)));
	connect(ui.tableWidget_auth,SIGNAL(itemClicked(QTableWidgetItem *)),this,SLOT(SlotTableItemClicked(QTableWidgetItem *)));
	connect(ui.btnIconImport, SIGNAL(clicked()), this, SLOT(SlotIconImport()));
	connect(ui.btnIconExport, SIGNAL(clicked()), this, SLOT(SlotIconExport()));
	connect(ui.btnSave, SIGNAL(clicked()), this, SLOT(SlotSave()));
	connect(ui.lineEdit_no, SIGNAL(textChanged(const QString&)), this, SLOT(SlotNoTextChanged(const QString&)));
	connect(ui.lineEdit_name, SIGNAL(textChanged(const QString&)), this, SLOT(SlotNameTextChanged(const QString&)));
	connect(ui.comboBox_type, SIGNAL(currentIndexChanged(int)), this, SLOT(SlotTypeCurrentIndexChanged(int)));
	connect(ui.comboBox_plugin, SIGNAL(editTextChanged(const QString&)), this, SLOT(SlotPluginEditTextChanged(const QString&)));
	connect(m_pMenuNull,SIGNAL(triggered(QAction*)),this,SLOT(SlotTriggerMenu(QAction*)));
	connect(m_pMenuFolder,SIGNAL(triggered(QAction*)),this,SLOT(SlotTriggerMenu(QAction*)));
	connect(m_pMenuApp,SIGNAL(triggered(QAction*)),this,SLOT(SlotTriggerMenu(QAction*)));
}

void CFunPointEdit::Start()
{
	SString sql;
	SRecordset rs;
	sql.sprintf("select fun_key,p_fun_key,name,type from t_ssp_fun_point where p_fun_key='top' group by idx,fun_key");
	int cnt = DB->Retrieve(sql,rs);
	if (cnt > 0)
	{
		for (int i = 0; i < cnt; i++)
		{
			QTreeWidgetItem *root = NULL;
			QString fun_key = rs.GetValue(i,0).data();
			QString p_fun_key = rs.GetValue(i,1).data();
			QString name = rs.GetValue(i,2).data();
			int type = rs.GetValue(i,3).toInt();
			if (type == 1)
			{
				root = new QTreeWidgetItem(ui.treeWidget,TREE_ITEM_FOLDER);
				root->setIcon(0,QIcon(":/images/folder_open"));
			}
			else
			{
				root = new QTreeWidgetItem(ui.treeWidget,TREE_ITEM_APP);
				int imageLen = 0;
				unsigned char* imageBuffer = NULL;
				SString sWhere = SString::toFormat("fun_key='%s'",fun_key.toStdString().data());
				DB->ReadLobToMem("t_ssp_fun_point","img_normal",sWhere,imageBuffer,imageLen);
				if (imageBuffer && imageLen > 0)
				{
					QPixmap pix;
					pix.loadFromData(imageBuffer,imageLen);
					root->setIcon(0,QIcon(pix));
				}
				else
					root->setIcon(0,QIcon(":/images/application"));
				if (imageBuffer)
					delete imageBuffer;
			}
			root->setText(0,name);
			root->setToolTip(0,name);
			root->setData(0,Qt::UserRole,fun_key);
			InitTreeWidget(root,fun_key);
		}
	}

	ui.lineEdit_no->setEnabled(false);
	ui.lineEdit_name->setEnabled(false);
	ui.comboBox_type->setEnabled(false);
	ui.comboBox_plugin->setEnabled(false);
	ui.btnIconExport->setEnabled(false);
	ui.btnIconImport->setEnabled(false);
	ui.btnSave->setEnabled(false);
}

void CFunPointEdit::paintEvent(QPaintEvent *e)
{
	SKWidget::paintEvent(e);
}

bool CFunPointEdit::eventFilter(QObject *obj,QEvent *e)
{
	QTreeWidgetItem *item = NULL;
	if (obj && obj == ui.treeWidget)
	{
		if (e->type() == QEvent::ContextMenu)
		{
			QContextMenuEvent * m_e = (QContextMenuEvent*)e;
			QPoint point(m_e->pos().x(),m_e->pos().y() - ui.treeWidget->header()->height());

			item = (QTreeWidgetItem *)ui.treeWidget->itemAt(point);
			if (item)
			{
				m_pCurrentTreeItem = item;
				if (item->type() == TREE_ITEM_FOLDER)
					m_pMenuFolder->popup(m_e->globalPos());
				else if (item->type() == TREE_ITEM_APP)
					m_pMenuApp->popup(m_e->globalPos());
			}
			else 
			{
				m_pCurrentTreeItem = NULL;
				m_pMenuNull->popup(m_e->globalPos());
			}
		}
	}

	return QWidget::eventFilter(obj,e);
}

void CFunPointEdit::SlotTreeItemClicked(QTreeWidgetItem *item, int column)
{
	if (m_bIsSave || m_bIconChange)
	{
		int ret = QMessageBox::question(NULL,tr("询问"),tr("有改动未保存，是否保存？"),tr("保存"),tr("放弃"));
		if (ret == 0)
			SlotSave();
	}
	m_bIsSave = false;
	m_bIconChange = false;

	m_pCurrentTreeItem = item;

	ui.lineEdit_no->setEnabled(true);
	ui.lineEdit_name->setEnabled(true);
	ui.comboBox_type->setEnabled(true);
	ui.comboBox_plugin->setEnabled(true);
	ui.btnIconExport->setEnabled(true);
	ui.btnIconImport->setEnabled(true);
	ui.btnSave->setEnabled(false);
	if (item->type() == TREE_ITEM_FOLDER)
	{
		ui.label_type->setVisible(false);
		ui.label_plugin->setVisible(false);
		ui.label_icon->setVisible(false);
		ui.label_auth->setVisible(false);
		ui.comboBox_type->setVisible(false);
		ui.comboBox_plugin->setVisible(false);
		ui.btnIconExport->setVisible(false);
		ui.btnIconImport->setVisible(false);
		ui.tableWidget_auth->setVisible(false);
	}
	else
	{
		ui.label_type->setVisible(true);
		ui.label_plugin->setVisible(true);
		ui.label_icon->setVisible(true);
		ui.label_auth->setVisible(true);
		ui.comboBox_type->setVisible(true);
		ui.comboBox_plugin->setVisible(true);
		ui.btnIconExport->setVisible(true);
		ui.btnIconImport->setVisible(true);
		ui.tableWidget_auth->setVisible(true);
	}

	m_sCurrentKey = item->data(column,Qt::UserRole).toString();

	SString sql;
	SRecordset rs;
	sql.sprintf("select fun_key,name,idx,type from t_ssp_fun_point where fun_key='%s'",m_sCurrentKey.toStdString().data());
	int cnt = DB->Retrieve(sql,rs);
	if (cnt == 1)
	{
		disconnect(ui.tableWidget_auth,SIGNAL(itemClicked(QTableWidgetItem *)),this,SLOT(SlotTableItemClicked(QTableWidgetItem *)));
		disconnect(ui.lineEdit_no, SIGNAL(textChanged(const QString&)), this, SLOT(SlotNoTextChanged(const QString&)));
		disconnect(ui.lineEdit_name, SIGNAL(textChanged(const QString&)), this, SLOT(SlotNameTextChanged(const QString&)));
		disconnect(ui.comboBox_type, SIGNAL(currentIndexChanged(int)), this, SLOT(SlotTypeCurrentIndexChanged(int)));
		disconnect(ui.comboBox_plugin, SIGNAL(editTextChanged(const QString&)), this, SLOT(SlotPluginEditTextChanged(const QString&)));

		QString fun_key = m_sPlugin = rs.GetValue(0,0).data();
		QString name = m_sName = rs.GetValue(0,1).data();
		int idx = rs.GetValue(0,2).toInt();
		m_sIdx = QString("%1").arg(idx);
		int type = m_iType = rs.GetValue(0,3).toInt();

		ui.lineEdit_no->setText(tr("%1").arg(idx));
		ui.lineEdit_name->setText(tr("%1").arg(name));
		ui.comboBox_type->setCurrentIndex(type);
		bool bFind = false;
		for (int i = 0; i < ui.comboBox_plugin->count(); i++)
		{
			QString text = ui.comboBox_plugin->itemText(i);
			text = text.split("=").at(0);
			if (text == fun_key)
			{
				ui.comboBox_plugin->setCurrentIndex(i);
				bFind = true;
				break;
			}
		}
		if (bFind == false)
			ui.comboBox_plugin->setEditText(fun_key);

		int imageLen = 0;
		unsigned char* imageBuffer = NULL;
		SString sWhere = SString::toFormat("fun_key='%s'",fun_key.toStdString().data());
		DB->ReadLobToMem("t_ssp_fun_point","img_normal",sWhere,imageBuffer,imageLen);
		if (imageBuffer && imageLen > 0)
		{
			QPixmap pix;
			pix.loadFromData(imageBuffer,imageLen);
			ui.btnIconImport->setIcon(QIcon(pix));
			ui.btnIconImport->setIconSize(QSize(32,32));
		}
		else
			ui.btnIconImport->setIcon(QIcon(":/images/application"));
		if (imageBuffer)
			delete imageBuffer;

		ui.tableWidget_auth->setRowCount(0);
		ui.tableWidget_auth->clearContents();
		SRecordset rs1;
		sql.sprintf("select a.grp_code,a.usr_code,b.auth from t_ssp_user a,t_ssp_user_auth b where b.fun_key='%s' and a.usr_sn=b.usr_sn",
			fun_key.toStdString().data());
		int cnt1 = DB->Retrieve(sql,rs);
		if (cnt1 > 0)
		{
			int row = 0;
			QTableWidgetItem * item;
			ui.tableWidget_auth->setRowCount(cnt1);
			for (int i = 0; i < cnt1; i++)
			{
				QString grp_code = rs.GetValue(i,0).data();
				QString usr_code = rs.GetValue(i,1).data();
				bool auth = (bool)rs.GetValue(i,2).toInt();

				item = new QTableWidgetItem(grp_code);
				item->setTextAlignment(Qt::AlignCenter);
				ui.tableWidget_auth->setItem(row,COLUMN_USERS,item);

				item = new QTableWidgetItem(usr_code);
				item->setTextAlignment(Qt::AlignCenter);
				item->setFlags(item->flags() & (~Qt::ItemIsEditable));	//设置item项不可编辑
				ui.tableWidget_auth->setItem(row,COLUMN_USER,item);

				item = new QTableWidgetItem("");
				item->setTextAlignment(Qt::AlignCenter);
				item->setCheckState(auth ? Qt::Checked : Qt::Unchecked);
				item->setFlags(item->flags() & (~Qt::ItemIsEditable));	//设置item项不可编辑
				ui.tableWidget_auth->setItem(row,COLUMN_AUTH,item);

				row++;
			}
		}

		connect(ui.tableWidget_auth,SIGNAL(itemClicked(QTableWidgetItem *)),this,SLOT(SlotTableItemClicked(QTableWidgetItem *)));
		connect(ui.lineEdit_no, SIGNAL(textChanged(const QString&)), this, SLOT(SlotNoTextChanged(const QString&)));
		connect(ui.lineEdit_name, SIGNAL(textChanged(const QString&)), this, SLOT(SlotNameTextChanged(const QString&)));
		connect(ui.comboBox_type, SIGNAL(currentIndexChanged(int)), this, SLOT(SlotTypeCurrentIndexChanged(int)));
		connect(ui.comboBox_plugin, SIGNAL(editTextChanged(const QString&)), this, SLOT(SlotPluginEditTextChanged(const QString&)));
	}
}

void CFunPointEdit::SlotIconImport()
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

		m_bIconChange = true;
		ui.btnSave->setEnabled(true);
	}
}

void CFunPointEdit::SlotIconExport()
{
	int imageLen = 0;
	unsigned char* imageBuffer = NULL;
	SString sWhere = SString::toFormat("fun_key='%s'",m_sCurrentKey.toStdString().data());
	DB->ReadLobToMem("t_ssp_fun_point","img_normal",sWhere,imageBuffer,imageLen);
	if (imageBuffer && imageLen > 0)
	{
		QString filter = tr("Image files(*.png *.jpg)");
		QString fileName = QFileDialog::getSaveFileName(this,tr("保存图像"),QString::null,filter);
		if (!fileName.isEmpty())
		{
			QFile file(fileName);
			if (!file.open(QIODevice::WriteOnly))
			{
				QMessageBox::warning(NULL,tr("告警"),tr("文件写模式打开失败"));
				return;
			}
			qint64 len = file.write((char*)imageBuffer,imageLen);
			file.close();
		}
	}
	else
		QMessageBox::warning(NULL,tr("告警"),tr("无图标，无法导出"));
}

void CFunPointEdit::SlotSave()
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

	QString tmp_key = ui.comboBox_plugin->currentText().trimmed();
	if (tmp_key.contains("="))
		tmp_key = tmp_key.split("=").at(0);

	char buf[1024] = {'\0'};
	strcpy(buf,ui.lineEdit_name->text().trimmed().toStdString().data());
	int size = strlen(buf);
	if (size > 32) //数据库字段长度32
	{
		QMessageBox::warning(NULL,tr("告警"),tr("条目名称过长"));
		return;
	}
	memset(buf,0,1024);
	strcpy(buf,tmp_key.toStdString().data());
	size = strlen(buf);
	if (size > 32) //数据库字段长度32
	{
		QMessageBox::warning(NULL,tr("告警"),tr("插件名称过长"));
		return;
	}

	SString sql;
	//SRecordset rs;
	//sql.sprintf("select count(*) from t_ssp_fun_point where fun_key='%s'",tmp_key.toStdString().data());
	//int cnt = DB->Retrieve(sql,rs);
	//if (rs.GetValue(0,0).toInt() > 0)
	//{
	//	QMessageBox::warning(NULL,tr("告警"),tr("插件名称重复"));
	//	goto fin;
	//}

	sql.sprintf("update t_ssp_fun_point set fun_key='%s',name='%s',idx=%d,type=%d where fun_key='%s'",
		tmp_key.toStdString().data(),
		ui.lineEdit_name->text().toStdString().data(),
		ui.lineEdit_no->text().trimmed().toInt(),
		ui.comboBox_type->currentIndex(),
		m_sCurrentKey.toStdString().data());
	if (!DB->Execute(sql))
	{
		QMessageBox::warning(NULL,tr("告警"),tr("保存失败"));
		goto fin;
	}

	if (!m_pCurrentTreeItem)
		return;
	m_pCurrentTreeItem->setText(0,ui.lineEdit_name->text());
	m_pCurrentTreeItem->setToolTip(0,ui.lineEdit_name->text());
	m_pCurrentTreeItem->setData(0,Qt::UserRole,tmp_key);
	SApi::UsSleep(500000);
	if (m_iImageLen > 0)
	{
		SString sWhere = SString::toFormat("fun_key='%s'",tmp_key.toStdString().data());
		if (!DB->UpdateLobFromMem("t_ssp_fun_point","img_normal",sWhere,(unsigned char*)m_ImageBuffer.data(),m_iImageLen))
		{
			QMessageBox::warning(NULL,tr("告警"),tr("图标更换失败"));
			goto fin;
		}
		else
		{
			QPixmap pix;
			pix.loadFromData((unsigned char*)m_ImageBuffer.data(),m_iImageLen);
			m_pCurrentTreeItem->setIcon(0,QIcon(pix));
		}
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

		sql.sprintf("update t_ssp_user_auth set auth=%d where usr_sn in (select usr_sn from t_ssp_user where usr_code='%s') and fun_key='%s'",
			state,ui.tableWidget_auth->item(i,COLUMN_USER)->text().trimmed().toStdString().data(),tmp_key.toStdString().data());
		if (!DB->Execute(sql))
		{
			QMessageBox::warning(NULL,tr("告警"),tr("用户权限更新失败"));
			goto fin;
		}

		if (state == 1)
		{
			sql.sprintf("update t_ssp_usergroup_auth set auth=%d where grp_code='%s' and fun_key='%s'",
				state,ui.tableWidget_auth->item(i,COLUMN_USERS)->text().trimmed().toStdString().data(),tmp_key.toStdString().data());
			if (!DB->Execute(sql))
			{
				QMessageBox::warning(NULL,tr("告警"),tr("用户组权限更新失败"));
				goto fin;
			}
		}
	}

	QMessageBox::information(NULL,tr("提示"),tr("保存成功"));

fin:
	foreach (CUsers *u, SK_GUI->m_lstUsers)
		delete u;
	SK_GUI->m_lstUsers.clear();
	SK_GUI->SetUsersAuth(QString::null);

	m_bIsSave = false;
	m_bIconChange = false;
	SlotTreeItemClicked(m_pCurrentTreeItem,0);
}

void CFunPointEdit::SlotTableItemClicked(QTableWidgetItem *item)
{
	if (item->column() != COLUMN_AUTH)
		return;

	CompareChange();
}

void CFunPointEdit::SlotNoTextChanged(const QString &text)
{
	Q_UNUSED(text);

	CompareChange();
}

void CFunPointEdit::SlotNameTextChanged(const QString &text)
{
	Q_UNUSED(text);

	CompareChange();
}

void CFunPointEdit::SlotTypeCurrentIndexChanged(int index)
{
	Q_UNUSED(index);

	CompareChange();
}

void CFunPointEdit::SlotPluginEditTextChanged(const QString &text)
{
	Q_UNUSED(text);

	CompareChange();
}

void CFunPointEdit::SlotTriggerMenu(QAction *action)
{
	if (action->text() == "添加文件夹(&O)")  
	{
		if (!m_pFunPointAddWidget)
		{
			CFunPointAdd *w = new CFunPointAdd(this);
			w->SetType(TYPE_FOLDER);
			w->SetTreeItem(m_pCurrentTreeItem);
			m_pFunPointAddWidget = new SKBaseWidget(NULL,w);
			m_pFunPointAddWidget->SetWindowsFlagsTool();
			m_pFunPointAddWidget->SetWindowsModal();
			m_pFunPointAddWidget->SetWindowTitle("添加文件夹");
#ifdef WIN32
			m_pFunPointAddWidget->SetWindowIcon(QIcon(":/images/folder_open"));
#else
			m_pFunPointAddWidget->SetWindowIcon(":/images/folder_open");
#endif
			m_pFunPointAddWidget->SetWindowFlags(0);
			m_pFunPointAddWidget->SetWindowSize(500,550);
			m_pFunPointAddWidget->SetIsDrag(true);
			connect(m_pFunPointAddWidget, SIGNAL(SigClose()), this, SLOT(SlotFunPointAddClose()));
		}
		((CFunPointAdd*)m_pFunPointAddWidget->GetCenterWidget())->Start();
		m_pFunPointAddWidget->Show();
#ifndef WIN32
		m_pHmi->m_pFunPointEdit->hide();
#endif
	}
	else if (action->text() == "添加功能点(&N)")  
	{
		if (!m_pFunPointAddWidget)
		{
			CFunPointAdd *w = new CFunPointAdd(this);
			w->SetType(TYPE_APP);
			w->SetTreeItem(m_pCurrentTreeItem);
			m_pFunPointAddWidget = new SKBaseWidget(NULL,w);
			m_pFunPointAddWidget->SetWindowsFlagsTool();
			m_pFunPointAddWidget->SetWindowsModal();
			m_pFunPointAddWidget->SetWindowTitle("添加功能点");
#ifdef WIN32
			m_pFunPointAddWidget->SetWindowIcon(QIcon(":/images/application"));
#else
			m_pFunPointAddWidget->SetWindowIcon(":/images/application");
#endif
			m_pFunPointAddWidget->SetWindowFlags(0);
			m_pFunPointAddWidget->SetWindowSize(500,550);
			m_pFunPointAddWidget->SetIsDrag(true);
			connect(m_pFunPointAddWidget, SIGNAL(SigClose()), this, SLOT(SlotFunPointAddClose()));
		}
		((CFunPointAdd*)m_pFunPointAddWidget->GetCenterWidget())->Start();
		m_pFunPointAddWidget->Show();
#ifndef WIN32
		m_pHmi->m_pFunPointEdit->hide();
#endif
	}
	else if (action->text() == "删除文件夹(&D)")
	{
		DeleteTreeItemChild(m_pCurrentTreeItem);
		DeleteTreeItem(m_pCurrentTreeItem);
	}
	else if (action->text() == "删除功能点(&D)")  
	{
		DeleteTreeItemChild(m_pCurrentTreeItem);
		DeleteTreeItem(m_pCurrentTreeItem);
	}
}

void CFunPointEdit::SlotFunPointAddClose()
{
	if (((CFunPointAdd*)m_pFunPointAddWidget->GetCenterWidget())->GetSaveSuccess())
	{
		QTreeWidgetItem *item = NULL;
		int type = ((CFunPointAdd*)m_pFunPointAddWidget->GetCenterWidget())->GetType();
		if (type == TYPE_FOLDER)
		{
			if (!m_pCurrentTreeItem)
				item = new QTreeWidgetItem(ui.treeWidget,TREE_ITEM_FOLDER);
			else
				item = new QTreeWidgetItem(m_pCurrentTreeItem,TREE_ITEM_FOLDER);
			item->setIcon(0,QIcon(":/images/folder_open"));
		}
		else if (type == TYPE_APP)
		{
			if (!m_pCurrentTreeItem)
				item = new QTreeWidgetItem(ui.treeWidget,TREE_ITEM_APP);
			else
				item = new QTreeWidgetItem(m_pCurrentTreeItem,TREE_ITEM_APP);
			int imageLen = 0;
			unsigned char* imageBuffer = NULL;
			SString sWhere = SString::toFormat("fun_key='%s'",
				((CFunPointAdd*)m_pFunPointAddWidget->GetCenterWidget())->GetFunPointKey().toStdString().data());
			DB->ReadLobToMem("t_ssp_fun_point","img_normal",sWhere,imageBuffer,imageLen);
			if (imageBuffer && imageLen > 0)
			{
				QPixmap pix;
				pix.loadFromData(imageBuffer,imageLen);
				item->setIcon(0,QIcon(pix));
			}
			else
				item->setIcon(0,QIcon(":/images/application"));
			if (imageBuffer)
				delete imageBuffer;
		}
		item->setText(0,((CFunPointAdd*)m_pFunPointAddWidget->GetCenterWidget())->GetFunPointName());
		item->setToolTip(0,((CFunPointAdd*)m_pFunPointAddWidget->GetCenterWidget())->GetFunPointName());
		item->setData(0,Qt::UserRole,((CFunPointAdd*)m_pFunPointAddWidget->GetCenterWidget())->GetFunPointKey());
		foreach (CUsers *u, SK_GUI->m_lstUsers)
			delete u;
		SK_GUI->m_lstUsers.clear();
		SK_GUI->SetUsersAuth(QString::null);
		SlotTreeItemClicked(item,0);
		ui.treeWidget->expandItem(item);
	}

	disconnect(m_pFunPointAddWidget, SIGNAL(SigClose()), this, SLOT(SlotUsersClose()));
	delete m_pFunPointAddWidget;
	m_pFunPointAddWidget = NULL;
#ifndef WIN32
	m_pHmi->m_pFunPointEdit->show();
#endif
}

bool CFunPointEdit::CompareChange()
{
	if (m_bIconChange)
		return true;

	QString tmp = ui.comboBox_plugin->currentText().trimmed();
	if (tmp.contains("="))
		tmp = tmp.split("=").at(0);
	QString idx = ui.lineEdit_no->text().trimmed();
	QString name = ui.lineEdit_name->text().trimmed();
	int type = ui.comboBox_type->currentIndex();

	if (m_sIdx != idx || 
		m_sName != name ||
		m_iType != type ||
		m_sPlugin != tmp ||
		CompareAuthChange())
	{
		m_bIsSave = true;
		ui.btnSave->setEnabled(true);
		return true;
	}
	else
	{
		m_bIsSave = false;
		ui.btnSave->setEnabled(false);
		return false;
	}

	return false;
}

bool CFunPointEdit::CompareAuthChange()
{
	for (int i = 0; i < ui.tableWidget_auth->rowCount(); i++)
	{
		QTableWidgetItem *item = ui.tableWidget_auth->item(i,COLUMN_AUTH);
		bool state = false;
		if (item->checkState() == Qt::Checked)
			state = true;
		else if (item->checkState() == Qt::Unchecked)
			state = false;

		foreach (CUsers *users, SK_GUI->m_lstUsers)
		{
			if (users->GetCode() == ui.tableWidget_auth->item(item->row(),COLUMN_USERS)->text().trimmed())
			{
				foreach (CUser *user, users->m_lstUser)
				{
					if (user->GetCode() == ui.tableWidget_auth->item(item->row(),COLUMN_USER)->text().trimmed())
					{
						foreach (stuAuth *auth, user->m_lstAuth)
						{
							if (auth->fun_key == m_sPlugin)
							{
								if (auth->auth != state)
									return true;
							}
						}
					}
				}
			}
		}
	}

	return false;
}

void CFunPointEdit::DeleteTreeItem(QTreeWidgetItem *item)
{
	delete item;

	if (ui.treeWidget->currentItem())
		SlotTreeItemClicked(ui.treeWidget->currentItem(),0);
	else
	{
		ui.lineEdit_no->setEnabled(false);
		ui.lineEdit_name->setEnabled(false);
		ui.comboBox_type->setEnabled(false);
		ui.comboBox_plugin->setEnabled(false);
		ui.btnIconExport->setEnabled(false);
		ui.btnIconImport->setEnabled(false);
		ui.btnSave->setEnabled(false);
	}
}

void CFunPointEdit::DeleteTreeItemChild(QTreeWidgetItem *item)
{
	for (int i = 0; i < item->childCount(); i++)
	{
		QTreeWidgetItem *it = item->child(i);
		if (it)
			DeleteTreeItemChild(it);
	}

	DeleteTreeItemContent(item);
}

bool CFunPointEdit::DeleteTreeItemContent(QTreeWidgetItem *item)
{
	SString sql;
	sql.sprintf("delete from t_ssp_fun_point where fun_key='%s'",
		item->data(0,Qt::UserRole).toString().toStdString().data());
	if (!DB->Execute(sql))
	{
		QMessageBox::warning(NULL,tr("告警"),tr("删除功能点失败"));
		return false;
	}

	sql.sprintf("delete from t_ssp_user_auth where fun_key='%s'",
		item->data(0,Qt::UserRole).toString().toStdString().data());
	if (!DB->Execute(sql))
	{
		QMessageBox::warning(NULL,tr("告警"),tr("删除用户权限失败"));
		return false;
	}

	sql.sprintf("delete from t_ssp_usergroup_auth where fun_key='%s'",
		item->data(0,Qt::UserRole).toString().toStdString().data());
	if (!DB->Execute(sql))
	{
		QMessageBox::warning(NULL,tr("告警"),tr("删除用户组权限失败"));
		return false;
	}

	return true;
}
