#include "cloginwidget.h"
#include "chmiwidget.h"
#include "skgui.h"

CLoginWidget::CLoginWidget(QWidget *parent)
	: SKWidget(parent)
{
	ui.setupUi(this);

	m_app = (SKBaseWidget *)parent;

	Init();
	InitUi();
	InitSlot();

	ui.comboBox_user->setEditText("admin");
	ui.lineEdit_password->setText("123");
}

CLoginWidget::~CLoginWidget()
{
	if (m_pHmi)
		delete m_pHmi;
}

void CLoginWidget::Init()
{
	m_pHmi = NULL;
	m_bFirst = true;
	m_bLoginOk = false;
}

void CLoginWidget::InitUi()
{
	ui.label_tip->setVisible(false);
	ui.btnClose->setCursor(QCursor(Qt::PointingHandCursor));
	ui.btnCancel->setCursor(QCursor(Qt::PointingHandCursor));
	ui.btnLogin->setCursor(QCursor(Qt::PointingHandCursor));
	ui.lineEdit_password->setEchoMode(QLineEdit::Password);

	setStyleSheet(tr("QWidget#%1{background:rgb(240,240,240,0);}").arg(objectName()));
}

void CLoginWidget::InitSlot()
{
	connect(ui.btnClose,SIGNAL(clicked()),this,SLOT(SlotClose()));
	connect(ui.btnLogin,SIGNAL(clicked()),this,SLOT(SlotLogin()));
	connect(ui.btnCancel,SIGNAL(clicked()),this,SLOT(SlotCancel()));
}

void CLoginWidget::paintEvent(QPaintEvent *e)
{
	SKWidget::paintEvent(e);
}

void CLoginWidget::keyPressEvent(QKeyEvent *e)
{
	if (e->key() == Qt::Key_Return)
	{
		SlotLogin();
	}
}

void CLoginWidget::SlotClose()
{
	emit SigClose();
}

void CLoginWidget::SlotLogin()
{
	SString sql;
	SRecordset rs;
	
	sql.sprintf("select usr_sn from t_ssp_user where binary usr_code='%s' and binary pwd='%s'",
		ui.comboBox_user->currentText().toStdString().data(),ui.lineEdit_password->text().toStdString().data());
	int cnt = DB->Retrieve(sql,rs);
	if (cnt > 0)
	{
		int sn = rs.GetValue(0,0).toInt();
		SK_GUI->InitSettings(tr("USER%1").arg(sn));
		SK_GUI->SetSettingsValue(SG_SETTING,SV_NAME,ui.comboBox_user->currentText().trimmed());
		SK_GUI->SetSettingsValue(SG_SETTING,SV_LOGINTIME,QDateTime::currentDateTime().toTime_t());
		if (m_bFirst)
		{
			CHMIWidget *wgt = new CHMIWidget();
			m_pHmi = new SKBaseWidget(NULL,wgt);
			wgt->SetApp(m_pHmi);
			wgt->SetUser(ui.comboBox_user->currentText());
			wgt->InitSlot();
			m_pHmi->SetWindowBackgroundImage(QPixmap(tr(":/skins/skin%1").arg(SK_GUI->GetSkinNo())));
			m_pHmi->SetWindowSize(1000,650);
			m_pHmi->setMinimumSize(1000,650);
			m_pHmi->SetWindowTitle(SK_GUI->GetHmiName());
#ifdef WIN32
			m_pHmi->SetWindowIcon(QIcon(":/images/HmiLogo"));
#else
			m_pHmi->SetWindowIcon(":/images/HmiLogo");
#endif
			m_pHmi->SetWindowFlags(SKBASEWIDGET_MAXIMIZE | SKBASEWIDGET_MINIMIZE|SKBASEWIDGET_FULLSCREEN);
			m_pHmi->SetIsTopDrag(true);
			m_pHmi->Show();
			//m_pHmi->ShowMaximized();
			//m_pHmi->ShowFullScreen();
		}
		else
		{
			m_sUser = ui.comboBox_user->currentText().trimmed();
			m_bLoginOk = true;
		}

		SlotClose();
		return;
	}

	ui.comboBox_user->setFocus();
	ui.label_tip->setVisible(true);
}

void CLoginWidget::SlotCancel()
{
	ui.comboBox_user->setEditText(QString::null);
	ui.comboBox_user->setFocus();
	ui.lineEdit_password->clear();
	ui.label_tip->setVisible(false);
}
