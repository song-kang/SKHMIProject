#include "cdbset.h"

CDBSet::CDBSet(QWidget *parent)
	: SKWidget(parent)
{
	ui.setupUi(this);

	Init();
	InitUi();
	InitSlot();
}

CDBSet::~CDBSet()
{

}

void CDBSet::Init()
{
	m_cfgPathName = tr("%1/../conf/sys_database.xml").arg(Common::GetCurrentAppPath());
	ui.lineEdit_db_port->setValidator(new QIntValidator(0, 65535, this));
	ui.lineEdit_mdb_port->setValidator(new QIntValidator(0, 65535, this));
}

void CDBSet::InitUi()
{
	setStyleSheet(tr("QWidget#%1{background:rgb(255,255,255,220);border-bottom-left-radius:3px;border-bottom-right-radius:3px;}").arg(objectName()));
}

void CDBSet::InitSlot()
{
	connect(ui.btnOk, SIGNAL(clicked()), this, SLOT(SlotOk()));
	connect(ui.btn_db_test, SIGNAL(clicked()), this, SLOT(SlotDBTest()));
	connect(ui.btn_mdb_test, SIGNAL(clicked()), this, SLOT(SlotMDBTest()));
	connect(ui.comboBox_db_type, SIGNAL(currentIndexChanged(QString)), this, SLOT(SlotCurrentIndexChanged(QString)));
}

void CDBSet::paintEvent(QPaintEvent *e)
{
	SKWidget::paintEvent(e);
}

void CDBSet::Start()
{
	if(!m_cfgXml.ReadConfig(m_cfgPathName.toStdString().data()))
	{
		WriteConfigFile(m_cfgPathName,true);
		Start();
		return;
	}

	SBaseConfig *master = m_cfgXml.GetChildNode("master");
	SString type = master->GetAttribute("type");
	ui.comboBox_db_type->setCurrentIndex(m_mapDBType.value(type.data()));
	ui.lineEdit_db_ip->setText(master->GetAttribute("hostaddr").data());
	ui.lineEdit_db_port->setText(master->GetAttribute("port").data());
	ui.lineEdit_db_user->setText(master->GetAttribute("user").data());
	ui.lineEdit_db_pwd->setText(SK_DATABASE->DecryptPwd(master->GetAttribute("password")).data());
	ui.lineEdit_db_name->setText(master->GetAttribute("dbname").data());

	SBaseConfig *mdb = m_cfgXml.GetChildNode("mdb");
	ui.lineEdit_mdb_ip->setText(mdb->GetAttribute("hostaddr").data());
	ui.lineEdit_mdb_port->setText(mdb->GetAttribute("port").data());
	ui.lineEdit_mdb_user->setText(mdb->GetAttribute("user").data());
	ui.lineEdit_mdb_pwd->setText(SK_DATABASE->DecryptPwd(mdb->GetAttribute("password")).data());
	ui.lineEdit_mdb_name->setText(mdb->GetAttribute("dbname").data());
}

void CDBSet::WriteConfigFile(QString cfgName, bool isCreate)
{
	m_cfgXml.clear();
	m_cfgXml.SetNodeName("database");

	if (isCreate)
	{
		m_cfgXml.AddChildNode("master",SString::toFormat("type=%s;hostaddr=%s;port=%s;user=%s;password=%s;dbname=%s",
			"mysql","127.0.0.1","3306","","",""));
		m_cfgXml.AddChildNode("mdb",SString::toFormat("hostaddr=%s;port=%s;user=%s;password=%s;dbname=%s",
			"127.0.0.1","1400","mdb","70FD7FA0142F524D6BE4E6D1FDAB02BD","ukdb"));
	}
	else
	{
		m_cfgXml.AddChildNode("master",SString::toFormat("type=%s;hostaddr=%s;port=%s;user=%s;password=%s;dbname=%s",
			ui.comboBox_db_type->currentText().toLower().trimmed().toStdString().data(),
			ui.lineEdit_db_ip->text().trimmed().toStdString().data(),
			ui.lineEdit_db_port->text().trimmed().toStdString().data(),
			ui.lineEdit_db_user->text().trimmed().toStdString().data(),
			SK_DATABASE->EncryptPwd(ui.lineEdit_db_pwd->text().trimmed().toStdString().data()).data(),
			ui.lineEdit_db_name->text().trimmed().toStdString().data()));
		m_cfgXml.AddChildNode("mdb",SString::toFormat("hostaddr=%s;port=%s;user=%s;password=%s;dbname=%s",
			ui.lineEdit_mdb_ip->text().trimmed().toStdString().data(),
			ui.lineEdit_mdb_port->text().trimmed().toStdString().data(),
			ui.lineEdit_mdb_user->text().trimmed().toStdString().data(),
			SK_DATABASE->EncryptPwd(ui.lineEdit_mdb_pwd->text().trimmed().toStdString().data()).data(),
			ui.lineEdit_mdb_name->text().trimmed().toStdString().data()));
	}

	m_cfgXml.SaveConfig(cfgName.toStdString().data());
}

void CDBSet::SlotOk()
{
	WriteConfigFile(m_cfgPathName, false);
	emit SigClose();
}

void CDBSet::SlotDBTest()
{
	WriteConfigFile(m_cfgPathName, false);
	if (SK_DATABASE->Load(QCoreApplication::applicationDirPath().toStdString()+"/../conf/sys_database.xml"))
	{
		SDatabase* pDb = NULL;
		if ((pDb = DB->GetDatabasePool()->GetDatabase(true)) != NULL)
		{
			QMessageBox::information(this, "提示", "数据库连接【测试成功】");
			return;
		}
	}

	QMessageBox::critical(this, "错误", "数据库连接【测试失败】");
}

void CDBSet::SlotMDBTest()
{
	WriteConfigFile(m_cfgPathName, false);
	if (SK_DATABASE->Load(QCoreApplication::applicationDirPath().toStdString()+"/../conf/sys_database.xml"))
	{
		SDatabase* pDb = NULL;
		if ((pDb = MDB->GetDatabasePool()->GetDatabase(true)) != NULL)
		{
			QMessageBox::information(this, "提示", "内存库连接【测试成功】");
			return;
		}
	}

	QMessageBox::critical(this, "错误", "内存库连接【测试失败】");
}

void CDBSet::SlotCurrentIndexChanged(QString text)
{
	if (text.toLower() == "mysql")
		ui.lineEdit_db_port->setText("3306");
	else if (text.toLower() == "oracle")
		ui.lineEdit_db_port->setText("1521");
	else if (text.toLower() == "postgresql")
		ui.lineEdit_db_port->setText("5432");
}
