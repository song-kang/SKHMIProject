#include "ctoolwidget.h"
#include "chmiwidget.h"

CToolWidget::CToolWidget(QWidget *parent)
	: SKWidget(parent)
{
	ui.setupUi(this);

	m_pHmi = (CHMIWidget *)parent;

	Init();
	InitUi();
	InitSlot();
}

CToolWidget::~CToolWidget()
{
	foreach (stuToolButton *btn, m_lstToolButton)
		delete btn;
}

void CToolWidget::Init()
{
	QStringList qss;
	qss.append(QString("QWidget#%1{background:rgb(142,169,195,120);}").arg(objectName()));
	qss.append(QString("QScrollArea#scrollArea{background:transparent;}"));
	qss.append(QString("QWidget#scrollAreaWidgetContents{background:transparent;}"));
	this->setStyleSheet(qss.join(""));

	ui.btnStart->setFixedSize(36,36);
	ui.btnStart->setToolTip(tr("Start"));
	ui.btnStart->setStyleSheet(
		"QPushButton{border-image:url(:/images/nw_start_nor);}"
		"QPushButton::hover{border-image:url(:/images/nw_start_hov);}"
		"QPushButton::pressed{border-image:url(:/images/nw_start_cli);}");

	ui.btnLeft->setFixedSize(15,40);
	ui.btnLeft->setToolTip(tr("Move Left"));
	ui.btnLeft->setStyleSheet(
		"QPushButton{border-image:url(:/images/arr_left_nor);}"
		"QPushButton::hover{border-image:url(:/images/arr_left_hov);}"
		"QPushButton::pressed{border-image:url(:/images/arr_left_cli);}");

	ui.btnRight->setFixedSize(15,40);
	ui.btnRight->setToolTip(tr("Move Right"));
	ui.btnRight->setStyleSheet(
		"QPushButton{border-image:url(:/images/arr_right_nor);}"
		"QPushButton::hover{border-image:url(:/images/arr_right_hov);}"
		"QPushButton::pressed{border-image:url(:/images/arr_right_cli);}");

	ui.btnDesktop->setFixedSize(15,40);
	ui.btnDesktop->setToolTip(tr("Show Desktop"));
	ui.btnDesktop->setStyleSheet(
		"QPushButton{border-image:url(:/images/show_desktop_nor);}"
		"QPushButton::hover{border-image:url(:/images/show_desktop_hov);}"
		"QPushButton::pressed{border-image:url(:/images/show_desktop_cli);}");

	QFont m_font;
	m_font.setFamily("Microsoft YaHei");
	m_font.setPixelSize(13);
	m_font.setBold(true);
	ui.label_hms->setFont(m_font);
	ui.label_ymd->setFont(m_font);
	ui.label_hms->setStyleSheet("color:white");
	ui.label_ymd->setStyleSheet("color:white");

	m_weekMap.insert(1,"星期一");
	m_weekMap.insert(2,"星期二");
	m_weekMap.insert(3,"星期三");
	m_weekMap.insert(4,"星期四");
	m_weekMap.insert(5,"星期五");
	m_weekMap.insert(6,"星期六");
	m_weekMap.insert(7,"星期日");

	m_pDateTimer = new QTimer(this);
	m_pDateTimer->setInterval(2000);
	m_pDateTimer->start();

	ui.scrollArea->setFrameShape(QFrame::NoFrame);

	m_pFunPointMenu = new QMenu(this);
	m_pFunPointCloseAction = m_pFunPointMenu->addAction(QIcon(":/images/cancel"),tr("关闭窗口(&C)"));
	m_pFunPointLockAction  = m_pFunPointMenu->addAction(QIcon(":/images/pin-off"),tr("将此窗口锁定在任务栏(&L)"));
}

void CToolWidget::InitUi()
{

}

void CToolWidget::InitSlot()
{
	connect(ui.btnLeft, SIGNAL(clicked()), this, SLOT(SlotLeft()));
	connect(ui.btnRight, SIGNAL(clicked()), this, SLOT(SlotRight()));
	connect(ui.btnStart, SIGNAL(clicked()), this, SLOT(SlotStart()));
	connect(ui.btnDesktop, SIGNAL(clicked()), this, SLOT(SlotDesktop()));
	connect(m_pDateTimer, SIGNAL(timeout()), this, SLOT(SlotDateTime()));
	connect(m_pFunPointMenu, SIGNAL(triggered(QAction*)), this, SLOT(SlotTriggerMenu(QAction*)));
}

void CToolWidget::paintEvent(QPaintEvent *e)
{
	SKWidget::paintEvent(e);
}

void CToolWidget::mousePressEvent(QMouseEvent *e)
{
	QString name;
	m_pHmi->m_pNavigtion->hide();

	if (e->button() == Qt::RightButton)
	{
		QWidget *wgt = Common::GetWidget(e->globalX(), e->globalY());
		if (wgt)
			name = wgt->objectName();

		if (name.left(7) == "plugin_")
		{
			m_sCurrentFunName = name;
			for (int pos = 0; pos < m_lstToolButton.count(); pos++)
			{
				stuToolButton *btn = m_lstToolButton.at(pos);
				if (btn->m_pToolButton->objectName() == m_sCurrentFunName)
				{
					if (btn->m_bLock)
					{
						m_pFunPointLockAction->setText(tr("将此窗口从任务栏中解锁(&L)"));
						m_pFunPointLockAction->setIcon(QIcon(":/images/pin-on"));
					}
					else
					{
						m_pFunPointLockAction->setText(tr("将此窗口锁定在任务栏(&L)"));
						m_pFunPointLockAction->setIcon(QIcon(":/images/pin-off"));
					}
					m_pFunPointCloseAction->setEnabled(!btn->m_bLock);
					break;
				}
			}
			
			m_pFunPointMenu->popup(e->globalPos());
		}
	}
	else if (e->button() == Qt::LeftButton)
	{
		QWidget *wgt = Common::GetWidget(e->globalX(), e->globalY());
		if (wgt)
			name = wgt->objectName();

		if (name == "label_hms" || name == "label_ymd")
		{
			int a = 0;
		}
	}
}

void CToolWidget::SlotStart()
{
	SigStart();
}

void CToolWidget::SlotDesktop()
{
	m_pHmi->ShowDesktop();

	foreach (stuToolButton *btn, m_lstToolButton)
		SetToolButtonUnclicked(btn->m_pToolButton);

	m_pHmi->m_pNavigtion->hide();
}

void CToolWidget::SlotLeft()
{
	QScrollBar *scrollBar = ui.scrollArea->horizontalScrollBar();
	scrollBar->setValue(scrollBar->value() - 70);

	m_pHmi->m_pNavigtion->hide();
}

void CToolWidget::SlotRight()
{
	QScrollBar *scrollBar = ui.scrollArea->horizontalScrollBar();
	scrollBar->setValue(scrollBar->value() + 70);

	m_pHmi->m_pNavigtion->hide();
}

void CToolWidget::SlotToolButtonClick()
{
	QToolButton *btn = (QToolButton *)sender();
	if (btn)
		m_pHmi->ShowWidgetByPluginName(btn->objectName());

	m_pHmi->m_pNavigtion->hide();
}

void CToolWidget::SlotDateTime()
{
	QString text;
	QDateTime dt = QDateTime::currentDateTime();
	ui.label_hms->setText(text.sprintf("%02d:%02d",dt.time().hour(),dt.time().minute()));
	text.sprintf("%d/%d/%d",dt.date().year(),dt.date().month(),dt.date().day());
	ui.label_ymd->setText(text + " " + m_weekMap.value(dt.date().dayOfWeek()));
}

void CToolWidget::SlotTriggerMenu(QAction *action)
{
	if (action->text() == "关闭窗口(&C)")  
	{
		int pos = 0;
		for (pos = 0; pos < m_lstToolButton.count(); pos++)
		{
			stuToolButton *btn = m_lstToolButton.at(pos);
			if (btn->m_pToolButton->objectName() == m_sCurrentFunName)
			{
				if (btn->m_bLock)
					return;
				m_lstToolButton.removeAt(pos);
				delete btn;
				break;
			}
		}

		m_pHmi->DeleteWidget(m_sCurrentFunName);
		DeleteToolButton(m_sCurrentFunName);

		if (m_lstToolButton.count() > 0)
		{
			stuToolButton *btn = NULL;
			if (pos - 1 < 0) //删除的是第一个窗口
				btn = m_lstToolButton.at(0);
			else
				btn = m_lstToolButton.at(pos-1);
			SetToolButtonClicked(btn->m_pToolButton);
		}		
	}
	else if (action->text() == "将此窗口锁定在任务栏(&L)" || action->text() == "将此窗口从任务栏中解锁(&L)")
	{
		for (int pos = 0; pos < m_lstToolButton.count(); pos++)
		{
			stuToolButton *btn = m_lstToolButton.at(pos);
			if (btn->m_pToolButton->objectName() == m_sCurrentFunName)
			{
				btn->m_bLock = !btn->m_bLock;
				break;
			}
		}
	}
}

void CToolWidget::CreateToolButton(QString name, QString desc, QIcon icon)
{
	QToolButton *btn = new QToolButton();

	btn->setObjectName(name);
	btn->setToolTip(desc);
	//btn->setFixedSize(QSize(60, 40));
	btn->setFixedHeight(40);
	btn->setIconSize(QSize(32, 32));
	btn->setIcon(icon);
	//btn->setToolButtonStyle(Qt::ToolButtonIconOnly);
	btn->setText(desc);
	btn->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
	SetToolButtonClicked(btn);

	stuToolButton *stuTB = new stuToolButton;
	stuTB->m_pToolButton = btn;
	m_lstToolButton.append(stuTB);
	InsertToolButton(btn);

	connect(btn, SIGNAL(clicked(bool)), this, SLOT(SlotToolButtonClick()));
}

void CToolWidget::DeleteToolButton(QString name)
{
	QLayoutItem *item = NULL;
	int count = ui.toolHLayout->count();
	for (int i = 0; i < count; i++)
	{
		item = ui.toolHLayout->itemAt(i);
		if (item && item->widget() && item->widget()->objectName() == name)
		{
			ui.toolHLayout->takeAt(i);
			delete item->widget();
			delete item;
			break;
		}
	}
}

void CToolWidget::DeleteAllToolButton()
{
	for (int pos = 0; pos < m_lstToolButton.count(); pos++)
	{
		stuToolButton *btn = m_lstToolButton.at(pos);
		QString funPointKey = btn->m_pToolButton->objectName();
		delete btn;

		m_pHmi->DeleteWidget(funPointKey);
		DeleteToolButton(funPointKey);
	}

	m_lstToolButton.clear();
}

void CToolWidget::InsertToolButton(QToolButton *btn)
{
	if (ui.toolHLayout->count() == 0)
	{
		ui.toolHLayout->addWidget(btn);
		ui.toolHLayout->addStretch();
	}
	else if (ui.toolHLayout->count() == 1)
	{
		ui.toolHLayout->insertWidget(0,btn);
	}
	else if (ui.toolHLayout->count() > 1)
	{
		ui.toolHLayout->insertWidget(ui.toolHLayout->count()-1,btn);
	}
}

void CToolWidget::SetToolButtonClicked(QString name)
{
	foreach (stuToolButton *btn, m_lstToolButton)
	{
		if (btn->m_pToolButton->objectName() == name)
			SetToolButtonClicked(btn->m_pToolButton);
		else
			SetToolButtonUnclicked(btn->m_pToolButton);
	}
}

void CToolWidget::SetToolButtonClicked(QToolButton *btn)
{
	QStringList list;
	list.append(QString("QToolButton{background:rgb(246,245,236,200);}"));
	list.append(QString("QToolButton{border:none solid gray;border-radius:1px;padding:2px;}"));
	list.append(QString("QToolButton:hover{background:%1;}").arg("#d3d7d4"));
	list.append(QString("QToolButton:pressed{background:%1;}").arg("#f6f5ec"));
	btn->setStyleSheet(list.join(""));
}

void CToolWidget::SetToolButtonUnclicked(QToolButton *btn)
{
	QStringList list;
	list.append(QString("QToolButton{background:rgb(255,255,255,50);}"));
	list.append(QString("QToolButton{border:none solid gray;border-radius:1px;padding:2px;}"));
	list.append(QString("QToolButton:hover{background:%1;}").arg("#d3d7d4"));
	list.append(QString("QToolButton:pressed{background:%1;}").arg("#f6f5ec"));
	btn->setStyleSheet(list.join(""));
}
