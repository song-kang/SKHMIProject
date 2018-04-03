#include "cinitwidget.h"
#include "cloginwidget.h"
#include "skgui.h"
#include "cpluginmgr.h"

///////////////////////////// LoadThread //////////////////////////////////
LoadThread::LoadThread(QObject *parent)
	: QThread(parent)
{
	m_pApp = (CInitWidget *)parent;
}

LoadThread::~LoadThread()
{

}

void LoadThread::run()
{
	SApi::UsSleep(200000); //大于1/8秒，避免等待图形不出现情况

	SigText(tr("加载日志配置......"));
	//SApi::UsSleep(500000);
	if (!SK_LOG->Load(QCoreApplication::applicationDirPath().toStdString()+"\\..\\conf\\sys_log.xml"))
	{
		m_sError = tr("日志配置加载出现异常！");
		return;
	}

	SigText(tr("加载数据库配置......"));
	//SApi::UsSleep(500000);
	if (!SK_DATABASE->Load(QCoreApplication::applicationDirPath().toStdString()+"\\..\\conf\\sys_database.xml"))
	{
		m_sError = tr("数据库配置加载出现异常！");
		return;
	}

	SDatabase* pDb;
	SigText(tr("测试连接数据库......"));
	//SApi::UsSleep(500000);
	if ((pDb = DB->GetDatabasePool()->GetDatabase(true)) == NULL)
	{
		m_sError = tr("数据库连接测试失败！");
		return;
	}

	SigText(tr("加载代理配置，启动代理......"));
	//SApi::UsSleep(500000);
	SK_GUI->BeginAgent();

	SigText(tr("加载动态插件库......"));
	//SApi::UsSleep(500000);
	SK_GUI->m_pPluginMgr->Init();

	SigText(tr("加载完毕."));
	//SApi::UsSleep(500000);
}

///////////////////////////// CInitWidget //////////////////////////////////
CInitWidget::CInitWidget(QWidget *parent)
	: SKWidget(parent)
{
	ui.setupUi(this);

	m_app = (SKBaseWidget *)parent;
	Init();
	InitUi();
	InitSlot();
	m_pAnimation->start();
	m_pLoadThread->start();
}

CInitWidget::~CInitWidget()
{
	delete m_pAnimation;
	if (m_pLogin)
		delete m_pLogin;
}

void CInitWidget::Init()
{
	m_pLogin = NULL;

	m_pAnimation = new QPropertyAnimation(this, QByteArray());
	m_pAnimation->setDuration(1000);
	m_pAnimation->setEasingCurve(QEasingCurve::Linear);
	m_pAnimation->setStartValue(0);
	m_pAnimation->setEndValue(8);

	m_pLoadThread = new LoadThread(this);
}

void CInitWidget::InitUi()
{
	QFont font;
	font.setFamily("Microsoft YaHei");
	font.setPixelSize(13);
	font.setBold(false);
	ui.label_msg->setFont(font);

	ui.btnClose->setVisible(false);
	ui.btnClose->setCursor(QCursor(Qt::PointingHandCursor));

	setStyleSheet(tr("QWidget#%1{background:rgb(240,240,240,0);}").arg(objectName()));
}

void CInitWidget::InitSlot()
{
	connect(m_pAnimation,SIGNAL(valueChanged(QVariant)),SLOT(SlotValueChanged(QVariant)));
	connect(m_pAnimation,SIGNAL(finished()),SLOT(SlotAnimationFinished()));
	connect(ui.btnClose,SIGNAL(clicked()),this,SLOT(SlotClose()));
	connect(m_pLoadThread,SIGNAL(finished()),this,SLOT(SlotLoadThreadFinished()));
	connect(m_pLoadThread,SIGNAL(SigText(QString)),this,SLOT(SlotLoadThreadText(QString)));
}

void CInitWidget::paintEvent(QPaintEvent *e)
{
	SKWidget::paintEvent(e);
}

void CInitWidget::SlotValueChanged(const QVariant &value)
{
	int m_currentValue = value.toInt();
	QString s = QString(":/images/loading%1").arg(m_currentValue);
	QPixmap pixmap(s);
	ui.label_load->setPixmap(pixmap);
}

void CInitWidget::SlotAnimationFinished()
{
	m_pAnimation->start();
}

void CInitWidget::SlotClose()
{
	emit SigClose();
}

void CInitWidget::SlotLoadThreadFinished()
{
	if (m_pLoadThread->GetError().isEmpty())
	{
		SlotClose();
		m_pLogin = new SKBaseWidget(NULL,new CLoginWidget);
		m_pLogin->SetWindowFixSize(420,290);
		m_pLogin->SetWindowBackgroundImage(QPixmap(":/images/login"));
		m_pLogin->HideTopFrame();
		m_pLogin->Show();
	}
	else
	{
		SlotLoadThreadText(m_pLoadThread->GetError());
		ui.btnClose->setVisible(true);
		m_pAnimation->stop();
	}
}

void CInitWidget::SlotLoadThreadText(QString text)
{
	ui.label_msg->setText(text);
}
