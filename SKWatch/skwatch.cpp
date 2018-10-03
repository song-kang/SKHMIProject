#include "skwatch.h"
#include "dlgadd.h"

#define COLUMN_STATUS		0
#define COLUMN_NAME			1
#define COLUMN_ARG			2
#define COLUMN_PATH			3
#define COLUMN_DELAY		4
#define COLUMN_SCCTIMES		5
#define COLUMN_SCCTIME		6
#define COLUMN_ERRTIMES		7
#define COLUMN_ERRTIME		8

SKWatch::SKWatch(QWidget *parent, Qt::WFlags flags)
	: QWidget(parent, flags)
{
	ui.setupUi(this);

	Init();
	InitUi();
	InitSlot();
}

SKWatch::~SKWatch()
{
	m_timer->stop();
	delete m_timer;
}

void SKWatch::Init()
{
	setWindowFlags(Qt::WindowCloseButtonHint);
	ui.btnRemove->setEnabled(false);
	ui.btnStart->setEnabled(false);
	ui.btnModify->setEnabled(false);
	m_pCurrentItem = NULL;

	m_pSystemTray = new CSysTray(this);
	m_pSystemTray->setToolTip(QString::fromLocal8Bit("软件狗"));
	m_pSystemTray->setIcon(QIcon(":/images/eye"));

	m_timer = new QTimer(this);
	m_timer->setInterval(3000);
}

void SKWatch::InitUi()
{
	ui.tableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);		//整行选择模式
	ui.tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);			//不可编辑
	ui.tableWidget->setSelectionMode(QAbstractItemView::SingleSelection);		//单选模式
	ui.tableWidget->setFocusPolicy(Qt::NoFocus);								//去除焦点，无虚框
	ui.tableWidget->setStyleSheet("selection-background-color:lightblue;");		//设置选中背景色
	ui.tableWidget->verticalHeader()->setDefaultSectionSize(24);				//设置行高
	ui.tableWidget->horizontalHeader()->setHighlightSections(false);			//点击表时不对表头行光亮
	ui.tableWidget->setAlternatingRowColors(true);								//设置交替行色
	ui.tableWidget->verticalHeader()->setVisible(false);						//去除最前列
}

void SKWatch::InitSlot()
{
	connect(ui.btnAdd, SIGNAL(clicked()), this, SLOT(SlotAdd()));
	connect(ui.btnRemove, SIGNAL(clicked()), this, SLOT(SlotRemove()));
	connect(ui.btnStart, SIGNAL(clicked()), this, SLOT(SlotStartStop()));
	connect(ui.btnModify, SIGNAL(clicked()), this, SLOT(SlotModify()));
	connect(m_timer, SIGNAL(timeout()), this, SLOT(SlotTimeout()));
	connect(ui.tableWidget, SIGNAL(itemClicked(QTableWidgetItem *)), this, SLOT(SlotTableItemClicked(QTableWidgetItem *)));
}

void SKWatch::Start(int argc, char *argv[])
{
	m_watch.m_sBinPath = Common::GetCurrentAppPath().toStdString().data();
	QString sConf = tr("%1/../conf/sys_watch.xml").arg(Common::GetCurrentAppPath());
	m_watch.Load(sConf.toStdString().data());
	
	//CConfigMgr::SetReloadSeconds(5);
	//CConfigMgr::StartReload();

	WriteConfigFile();
	QFileInfo info(argv[0]);
	QString name = info.fileName();
	m_watch.SetModuleName(name.toStdString().data());
	m_watch.Run(argc, argv);
	m_timer->start();
}

void SKWatch::closeEvent(QCloseEvent *event)
{
	if (!m_pSystemTray->isVisible())
	{
		m_pSystemTray->show();
		m_pSystemTray->showMessage("软件狗", "点击恢复", QSystemTrayIcon::Information, 1000);
		hide();
		event->ignore();
	}
}

void SKWatch::SlotAdd()
{
	DlgAdd dlg(this);
	dlg.SetType(TYPE_ADD);
	dlg.Start();
	if (dlg.exec() == QDialog::Accepted)
	{
		stuExtModule *module = new stuExtModule;
		module->iSn = m_watch.m_ExtModuleList.count();
		module->iStartErrTimes = module->iStartTimes = 0;
		module->tLastStartErr = module->tLastStart = -1;
		module->sModule = dlg.GetAppName().toStdString().data();
		module->sCmdLine = dlg.GetAppName().toStdString().data();
		module->sArg = dlg.GetAppArg().toStdString().data();
		module->sPath = dlg.GetAppPath().toStdString().data();
		module->first_delay_sec = dlg.GetDelayTime();
		module->is_first = false;
		module->is_run = false;
		module->is_watch = true;

		m_watch.m_Lock.lock();
		m_watch.m_ExtModuleList.append(module);
		m_watch.m_Lock.unlock();
		WriteConfigFile();
	}
}

void SKWatch::SlotRemove()
{
	if (!m_pCurrentItem)
		return;

	QTableWidgetItem *item = ui.tableWidget->item(m_pCurrentItem->row(),COLUMN_STATUS);
	if (!item)
		return;

	int sn = item->data(Qt::UserRole).toInt();
	stuExtModule *module = GetExtModuleBySn(sn);
	if (module)
	{
		QString sQuestion = tr("是否确认删除应用【%1】？").arg(module->sCmdLine.data());
		int ret = QMessageBox::question(NULL,tr("询问"),sQuestion,tr("是"),tr("否"));
		if (ret == 0)
		{
			m_watch.m_Lock.lock();
			m_watch.m_ExtModuleList.remove(module);
			m_watch.m_Lock.unlock();
			WriteConfigFile();
		}
	}
}

void SKWatch::SlotStartStop()
{
	if (!m_pCurrentItem)
		return;

	QTableWidgetItem *item = ui.tableWidget->item(m_pCurrentItem->row(),COLUMN_STATUS);
	if (!item)
		return;

	int sn = item->data(Qt::UserRole).toInt();
	stuExtModule *module = GetExtModuleBySn(sn);
	if (module)
	{
		QString sQuestion;
		if (!module->is_watch)
			sQuestion = tr("目前【%1】为【未监视】状态，是否开启监视？").arg(module->sCmdLine.data());
		else
			sQuestion = tr("目前【%1】为【监视】状态，是否停止监视？").arg(module->sCmdLine.data());

		int ret = QMessageBox::question(NULL,tr("询问"),sQuestion,tr("是"),tr("否"));
		if (ret == 0)
			module->is_watch = !module->is_watch;
	}
}

void SKWatch::SlotModify()
{
	if (!m_pCurrentItem)
		return;

	QTableWidgetItem *item = ui.tableWidget->item(m_pCurrentItem->row(),COLUMN_STATUS);
	if (!item)
		return;

	int sn = item->data(Qt::UserRole).toInt();
	stuExtModule *module = GetExtModuleBySn(sn);
	DlgAdd dlg(this);
	dlg.SetType(TYPE_MODIFY);
	dlg.SetAppName(module->sCmdLine.data());
	dlg.SetAppArg(module->sArg.data());
	dlg.SetAppPath(module->sPath.data());
	dlg.SetDelayTime(module->first_delay_sec);
	dlg.Start();
	if (dlg.exec() == QDialog::Accepted)
	{
		m_watch.m_Lock.lock();
		module->sModule = dlg.GetAppName().toStdString().data();
		module->sCmdLine = dlg.GetAppName().toStdString().data();
		module->sArg = dlg.GetAppArg().toStdString().data();
		module->sPath = dlg.GetAppPath().toStdString().data();
		module->first_delay_sec = dlg.GetDelayTime();
		m_watch.m_Lock.unlock();
		WriteConfigFile();
	}
}

void SKWatch::SlotTableItemClicked(QTableWidgetItem *tableItem)
{
	m_pCurrentItem = tableItem;

	ui.btnRemove->setEnabled(true);
	ui.btnStart->setEnabled(true);
	ui.btnModify->setEnabled(true);
}

void SKWatch::SlotTimeout()
{
	Refresh();
}

void SKWatch::Refresh()
{
	m_pCurrentItem = NULL;
	ui.btnRemove->setEnabled(false);
	ui.btnStart->setEnabled(false);
	ui.btnModify->setEnabled(false);

	m_watch.m_Lock.lock();

	ui.tableWidget->clearContents();
	ui.tableWidget->setRowCount(0);

	int row = m_watch.m_ExtModuleList.count();
	ui.tableWidget->setRowCount(row);
	for(int i = 0; i < row; i++)
	{
		stuExtModule *pExtModule = m_watch.m_ExtModuleList[i];

		QTableWidgetItem *item;
		QString sDesc;
		QColor color;
		if (pExtModule->is_watch == false)
		{
			sDesc = " 未监视 ";
			color = QColor(Qt::gray);
		}
		else if (pExtModule->iStartTimes > 1)
		{
			sDesc = " 重启过 ";
			color = QColor(Qt::yellow);
		}
		else if (pExtModule->is_run)
		{
			sDesc = " 正常 ";
			color = QColor(Qt::green);
		}
		else
		{
			sDesc = " 异常 ";
			color = QColor(Qt::red);
		}
		item = new QTableWidgetItem(sDesc);
		item->setTextAlignment(Qt::AlignCenter);
		item->setBackgroundColor(color);
		item->setData(Qt::UserRole,pExtModule->iSn);
		ui.tableWidget->setItem(i,COLUMN_STATUS,item);

		item = new QTableWidgetItem(tr(" %1 ").arg(pExtModule->sCmdLine.data()));
		item->setTextAlignment(Qt::AlignCenter);
		item->setBackgroundColor(color);
		ui.tableWidget->setItem(i,COLUMN_NAME,item);

		item = new QTableWidgetItem(tr(" %1 ").arg(pExtModule->sArg.data()));
		item->setTextAlignment(Qt::AlignCenter);
		item->setBackgroundColor(color);
		ui.tableWidget->setItem(i,COLUMN_ARG,item);

		item = new QTableWidgetItem(tr(" %1 ").arg(pExtModule->sPath.data()));
		item->setTextAlignment(Qt::AlignLeft|Qt::AlignVCenter);
		item->setBackgroundColor(color);
		ui.tableWidget->setItem(i,COLUMN_PATH,item);

		item = new QTableWidgetItem(tr("%1").arg(pExtModule->first_delay_sec));
		item->setTextAlignment(Qt::AlignCenter);
		item->setBackgroundColor(color);
		ui.tableWidget->setItem(i,COLUMN_DELAY,item);

		item = new QTableWidgetItem(tr("%1").arg(pExtModule->iStartTimes));
		item->setTextAlignment(Qt::AlignCenter);
		item->setBackgroundColor(color);
		ui.tableWidget->setItem(i,COLUMN_SCCTIMES,item);

		QDateTime dt = QDateTime::fromTime_t(pExtModule->tLastStart);
		item = new QTableWidgetItem(pExtModule->iStartTimes==0?"无":dt.toString("yyyy-MM-dd hh:mm:ss"));
		item->setTextAlignment(Qt::AlignCenter);
		item->setBackgroundColor(color);
		ui.tableWidget->setItem(i,COLUMN_SCCTIME,item);

		item = new QTableWidgetItem(tr("%1").arg(pExtModule->iStartErrTimes));
		item->setTextAlignment(Qt::AlignCenter);
		item->setBackgroundColor(color);
		ui.tableWidget->setItem(i,COLUMN_ERRTIMES,item);

		dt = QDateTime::fromTime_t(pExtModule->tLastStartErr);
		item = new QTableWidgetItem(pExtModule->tLastStartErr<=0?"无":dt.toString("yyyy-MM-dd hh:mm:ss"));
		item->setTextAlignment(Qt::AlignCenter);
		item->setBackgroundColor(color);
		ui.tableWidget->setItem(i,COLUMN_ERRTIME,item);
	}

	ui.tableWidget->resizeColumnsToContents();
	m_watch.m_Lock.unlock();
}

void SKWatch::WriteConfigFile()
{
	m_cfgXml.clear();
	m_cfgXml.SetNodeName("watch-config");
	SBaseConfig *hard_dog = m_cfgXml.AddChildNode("hard_dog","open=false;");
	SBaseConfig *watch_list = m_cfgXml.AddChildNode("watch-list");

	m_watch.m_Lock.lock();
	for(int i = 0; i < m_watch.m_ExtModuleList.count(); i++)
	{
		stuExtModule *pExtModule = m_watch.m_ExtModuleList[i];
		SBaseConfig *process = watch_list->AddChildNode("process",
			SString::toFormat("module=%s;cmdline=%s;path=%s;first_delay_sec=%d",
			pExtModule->sModule.data(),
			pExtModule->sCmdLine.data(),
			pExtModule->sPath.data(),
			pExtModule->first_delay_sec));
		process->SetAttribute("arg",SString::toFormat("%s",pExtModule->sArg.data()));
	}

	QString sConf = tr("%1/../conf/sys_watch.xml").arg(Common::GetCurrentAppPath());
	m_cfgXml.SaveConfig(sConf.toStdString().data());

	m_watch.m_Lock.unlock();
}

stuExtModule* SKWatch::GetExtModuleBySn(int sn)
{
	m_watch.m_Lock.lock();
	for(int i = 0; i < m_watch.m_ExtModuleList.count(); i++)
	{
		stuExtModule *pExtModule = m_watch.m_ExtModuleList[i];
		if (pExtModule->iSn == sn)
		{
			m_watch.m_Lock.unlock();
			return pExtModule;
		}
	}

	m_watch.m_Lock.unlock();
	return NULL;
}
