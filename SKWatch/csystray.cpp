#include "csystray.h"
#include "skwatch.h"

CSysTray::CSysTray(QWidget *parent)
	: QSystemTrayIcon(parent)
{
	m_pApp = (SKWatch *)parent;

	Init();
	InitUi();
	InitSlot();
}

CSysTray::~CSysTray()
{
	delete m_pTrayIconMenu;
}

void CSysTray::Init()
{
	m_pShowAction = new QAction(this);
	m_pShowAction->setText("��ʾ");
	m_pShowAction->setIcon(QIcon(":/images/monitor"));
	m_pQuitAction = new QAction(this);
	m_pQuitAction->setText("�˳�");
	m_pQuitAction->setIcon(QIcon(":/images/quit"));

	m_pTrayIconMenu = new QMenu();
	m_pTrayIconMenu->addAction(m_pShowAction);
	m_pTrayIconMenu->addAction(m_pQuitAction);
	setContextMenu(m_pTrayIconMenu);
}

void CSysTray::InitUi()
{

}

void CSysTray::InitSlot()
{
	connect(m_pShowAction, SIGNAL(triggered()), this, SLOT(SlotShow()));
	connect(m_pQuitAction, SIGNAL(triggered()), this, SLOT(SlotQuit()));
	connect(this, SIGNAL(activated(QSystemTrayIcon::ActivationReason)), 
		this, SLOT(SlotActivated(QSystemTrayIcon::ActivationReason)));
}

void CSysTray::SlotShow()
{
	m_pApp->showNormal();
	m_pApp->raise();
	m_pApp->activateWindow();
	hide();
}

void CSysTray::SlotQuit()
{
	int ret = QMessageBox::question(NULL,tr("ѯ��"),tr("ȷ���˳���"),tr("��"),tr("��"));
	if (ret == 0)
		QApplication::quit();
}

void CSysTray::SlotActivated(QSystemTrayIcon::ActivationReason reason)
{
	switch (reason)
	{
	case QSystemTrayIcon::Trigger:
	case QSystemTrayIcon::DoubleClick: 
		{                
			if (m_pApp->isHidden()) 
			{
				SlotShow();
			}                
			else
			{
				m_pApp->hide();
				show();
				showMessage("�����", "����ָ�", QSystemTrayIcon::Information, 1000);
			}
		}
		break;
	case QSystemTrayIcon::Context:
		break;
	}
}
