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

	SigText(tr("加载功能点配置......"));
	//SApi::UsSleep(500000);
	SK_GUI->SetFunPoint(NULL);
	CheckFunPoint();

	SigText(tr("加载用户组、用户及权限配置......"));
	//SApi::UsSleep(500000);
	SK_GUI->SetUsersAuth(QString::null);
	DeleteUserAuth();
	CheckUserAuth(SK_GUI->m_lstFunPoint);
	foreach (CUsers *u, SK_GUI->m_lstUsers)
		delete u;
	SK_GUI->m_lstUsers.clear();
	SK_GUI->SetUsersAuth(QString::null);

	SigText(tr("加载代理配置，启动代理......"));
	//SApi::UsSleep(500000);
	SK_GUI->BeginAgent();

	SigText(tr("加载动态插件库......"));
	//SApi::UsSleep(500000);
	SK_GUI->m_pPluginMgr->Init();

	SigText(tr("加载完毕."));
	//SApi::UsSleep(500000);
}

void LoadThread::CheckFunPoint()
{
	SString sql;
	SRecordset rs;

	sql.sprintf("select fun_key from t_ssp_fun_point order by fun_key asc");
	int cnt = DB->Retrieve(sql,rs);
	if (cnt > 0)
	{
		for (int i = 0; i < cnt; i++)
		{
			QString key = rs.GetValue(i,0).data();
			if (!IsExistKey(key))
			{
				sql.sprintf("delete from t_ssp_fun_point where fun_key='%s'",key.toStdString().data());
				DB->Execute(sql);
			}
		}
	}
}

void LoadThread::CheckUserAuth(QList<CFunPoint*> lstFunPoint)
{
	SString sql;
	foreach (CFunPoint *p, lstFunPoint)
	{
		foreach (CUsers *users, SK_GUI->m_lstUsers)
		{
			if (!users->IsExistKey(p->GetKey()))
			{
				sql.sprintf("insert into t_ssp_usergroup_auth values ('%s','%s',1)",
					users->GetCode().toStdString().data(),p->GetKey().toStdString().data());
				DB->Execute(sql);
			}

			foreach (CUser *user, users->m_lstUser)
			{
				if (!user->IsExistKey(p->GetKey()))
				{
					sql.sprintf("insert into t_ssp_user_auth values (%d,'%s',1)",user->GetSn(),p->GetKey().toStdString().data());
					DB->Execute(sql);
				}
			}
		}

		CheckUserAuth(p->m_lstChilds);
	}
}

void LoadThread::DeleteUserAuth()
{
	SString sql;
	foreach (CUsers *users, SK_GUI->m_lstUsers)
	{
		foreach (stuAuth *auth, users->m_lstAuth)
		{
			if (!IsExistKey(auth->fun_key))
			{
				sql.sprintf("delete from t_ssp_usergroup_auth where grp_code='%s' and fun_key='%s'",
					users->GetCode().toStdString().data(),auth->fun_key.toStdString().data());
				DB->Execute(sql);
			}
		}

		foreach (CUser *user, users->m_lstUser)
		{
			foreach (stuAuth *auth, user->m_lstAuth)
			{
				if (!IsExistKey(auth->fun_key))
				{
					sql.sprintf("delete from t_ssp_user_auth where usr_sn=%d and fun_key='%s'",
						user->GetSn(),auth->fun_key.toStdString().data());
					DB->Execute(sql);
				}
			}
		}
	}
}

bool LoadThread::IsExistKey(QString key)
{
	bool bFind = false;

	foreach (CFunPoint *p, SK_GUI->m_lstFunPoint)
	{
		if (p->IsExist(key))
			bFind = true;
	}

	return bFind;
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
