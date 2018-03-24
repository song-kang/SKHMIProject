#include "cloginwidget.h"
#include "cmainwidget.h"
#include "skgui.h"

CLoginWidget::CLoginWidget(QWidget *parent)
	: SKWidget(parent)
{
	ui.setupUi(this);

	m_app = (SKBaseWidget *)parent;
	Init();
	InitUi();
	InitSlot();
}

CLoginWidget::~CLoginWidget()
{
	if (m_pHmi)
		delete m_pHmi;
}

void CLoginWidget::Init()
{
	m_pHmi = NULL;
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

void CLoginWidget::SlotClose()
{
	emit SigClose();
}

void CLoginWidget::SlotLogin()
{
	SString sql;
	SRecordset rs;
	sql.sprintf("select count(*) from t_ssp_user where usr_code='%s' and pwd='%s'",
		ui.comboBox_user->currentText().toStdString().data(),ui.lineEdit_password->text().toStdString().data());
	int cnt = DB->Retrieve(sql,rs);
	if (cnt > 0)
	{
		int number = rs.GetValue(0,0).toInt();
		if (number > 0)
		{
			SlotClose();
			m_pHmi = new SKBaseWidget(NULL,new CMainWidget);
			m_pHmi->SetWindowBackgroundImage(QPixmap(":/images/skin0"));
			m_pHmi->setMinimumSize(1000,650);
			m_pHmi->SetWindowTitle(SK_GUI->GetHmiName());
			m_pHmi->SetWindowIcon(QIcon(":/images/HmiLogo"));
			m_pHmi->SetWindowFlags(SKBASEWIDGET_MAXIMIZE | SKBASEWIDGET_MINIMIZE|SKBASEWIDGET_FULLSCREEN);
			m_pHmi->SetIsTopDrag(true);
			m_pHmi->ShowMaximized();
			//m_pHmi->ShowFullScreen();
			return;
		}
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
