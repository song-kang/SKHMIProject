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
	SApi::UsSleep(200000); //����1/8�룬����ȴ�ͼ�β��������

	SigText(tr("������־����......"));
#ifdef WIN32
	if (!SK_LOG->Load(QCoreApplication::applicationDirPath().toStdString()+"\\..\\conf\\sys_log.xml"))
#else
	if (!SK_LOG->Load(QCoreApplication::applicationDirPath().toStdString()+"/../conf/sys_log.xml"))
#endif
	{
		m_sError = tr("��־���ü��س����쳣��");
		return;
	}

	SigText(tr("�������ݿ�����......"));
#ifdef WIN32
	if (!SK_DATABASE->Load(QCoreApplication::applicationDirPath().toStdString()+"\\..\\conf\\sys_database.xml"))
#else
	if (!SK_DATABASE->Load(QCoreApplication::applicationDirPath().toStdString()+"/../conf/sys_database.xml"))
#endif
	{
		m_sError = tr("���ݿ����ü��س����쳣��");
		return;
	}

	SDatabase* pDb = NULL;
	SigText(tr("�����������ݿ�......"));
	if ((pDb = DB->GetDatabasePool()->GetDatabase(true)) == NULL)
	{
		m_sError = tr("���ݿ����Ӳ���ʧ�ܣ�");
		return;
	}

	pDb = NULL;
	SigText(tr("���������ڴ����ݿ�......"));
	if ((pDb = MDB->GetDatabasePool()->GetDatabase(true)) == NULL)
	{
		m_sError = tr("�ڴ����ݿ����Ӳ���ʧ�ܣ�");
		return;
	}

	SigText(tr("����ͼ��......"));
	QString dir = Common::GetCurrentAppPath() + "../picture/";
	if (!Common::FolderExists(dir) && !Common::CreateFolder(dir))
	{
		m_sError = tr("ͼƬ�ļ���(picture)�����ڻ��޷�������");
		return;
	}
	DownLoadPicture();

	SigText(tr("���ش������ã���������......"));
	SK_GUI->BeginAgent();

	SigText(tr("���ض�̬�����......"));
	SK_GUI->m_pPluginMgr->Init();

	SigText(tr("�������."));
}

void LoadThread::DownLoadPicture()
{
	SString sql;
	SRecordset rs;
	sql.sprintf("select svg_sn from t_ssp_svglib_item");
	int cnt = DB->Retrieve(sql,rs);
	if (cnt > 0)
	{
		for (int i = 0; i < cnt; i++)
		{
			int sn = rs.GetValue(i,0).toInt();

			int len = 0;
			unsigned char* buffer = NULL;
			SString sWhere = SString::toFormat("svg_sn=%d",sn);
			if (DB->ReadLobToMem("t_ssp_svglib_item","svg_file",sWhere,buffer,len))
			{
				QString path = Common::GetCurrentAppPath() + tr("../picture/%1").arg(sn);
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
		SK_GUI->SetFunPoint(NULL);
		SK_GUI->CheckFunPoint();
		SK_GUI->SetUsersAuth(QString::null);
		SK_GUI->DeleteUserAuth();
		SK_GUI->CheckUserAuth(SK_GUI->m_lstFunPoint);
		foreach (CUsers *u, SK_GUI->m_lstUsers)
			delete u;
		SK_GUI->m_lstUsers.clear();
		SK_GUI->SetUsersAuth(QString::null);

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
