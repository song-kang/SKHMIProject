#include "ctopwidget.h"
#include "skbasewidget.h"

CTopWidget::CTopWidget(QWidget *parent)
	: QWidget(parent)
{
	m_pBaseWidget = (SKBaseWidget *)parent;

	Init();
	InitUi();
}

CTopWidget::~CTopWidget()
{

}

void CTopWidget::Init()
{
	m_mainLayout = NULL;
	m_iMainMenu = NULL;
	m_bFixed = false;
	m_bBackgroundImaged = false;

	setObjectName("CTopWidget");
	QString	qss = QString("QWidget#CTopWidget{background:%1;border-top-left-radius:4px;border-top-right-radius:4px;}").arg(COLOR_NW);
	setStyleSheet(qss);
	setMouseTracking(true);
}

void CTopWidget::InitUi()
{
	m_btnLogo = new QPushButton(this);
	m_labelTitle = new QLabel(this);
	m_btnFullScreen = new QPushButton(this);
	m_btnMainMenu = new QPushButton(this);
	m_btnMinimize = new QPushButton(this);
	m_btnMaximize = new QPushButton(this);
	m_btnClose = new QPushButton(this);

#ifdef WIN32
	m_btnLogo->setStyleSheet("background:transparent;");
	m_btnLogo->setFixedSize(TOPWIDGET_HEIGHT,TOPWIDGET_HEIGHT);
	m_btnLogo->setIconSize(QSize(18,18));
	m_btnLogo->setIcon(QIcon(":/image/Resources/qt.png"));
#else
	m_btnLogo->setFixedSize(18,18);
	m_btnLogo->setStyleSheet(
		"QPushButton{border-image:url(:/image/Resources/qt.png);}"
		"QPushButton::hover{border-image:url(:/image/Resources/qt.png);}"
		"QPushButton::pressed{border-image:url(:/image/Resources/qt.png);}");
#endif

	QFont m_font;
	m_font.setFamily("Microsoft YaHei");
	m_font.setPixelSize(13);
	m_font.setBold(true);
	m_labelTitle->setFont(m_font);
	m_labelTitle->setStyleSheet("color:white;");
	m_labelTitle->setText(tr("Title"));

	m_btnFullScreen->setFixedSize(14,14);
	m_btnFullScreen->setToolTip(tr("Full Screen"));
	m_btnFullScreen->setStyleSheet(
		"QPushButton{border-image:url(:/image/Resources/full_nor.png);}"
		"QPushButton::hover{border-image:url(:/image/Resources/full_hov.png);}"
		"QPushButton::pressed{border-image:url(:/image/Resources/full_hov.png);}");
	m_btnFullScreen->hide();

	m_btnMainMenu->setFixedSize(14,14);
	m_btnMainMenu->setToolTip(tr("Main Menu"));
	m_btnMainMenu->setStyleSheet(
		"QPushButton{border-image:url(:/image/Resources/more_nor.png);}"
		"QPushButton::hover{border-image:url(:/image/Resources/more_hov.png);}"
		"QPushButton::pressed{border-image:url(:/image/Resources/more_down.png);}");
	m_btnMainMenu->hide();

	m_btnMinimize->setFixedSize(14,14);
	m_btnMinimize->setToolTip(tr("Minimize"));
	m_btnMinimize->setStyleSheet(
		"QPushButton{border-image:url(:/image/Resources/min_nor.png);}"
		"QPushButton::hover{border-image:url(:/image/Resources/min_hov.png);}"
		"QPushButton::pressed{border-image:url(:/image/Resources/min_down.png);}");
	m_btnMinimize->hide();

	m_btnMaximize->setFixedSize(14,14);
	m_btnMaximize->setToolTip(tr("Maximize"));
	m_btnMaximize->setStyleSheet(
		"QPushButton{border-image:url(:/image/Resources/max_nor.png);}"
		"QPushButton::hover{border-image:url(:/image/Resources/max_hov.png);}"
		"QPushButton::pressed{border-image:url(:/image/Resources/max_down.png);}");
	m_btnMaximize->hide();

	m_btnClose->setFixedSize(14,14);
	m_btnClose->setToolTip(tr("Close"));
	m_btnClose->setStyleSheet(
		"QPushButton{border-image:url(:/image/Resources/close_nor.png);}"
		"QPushButton::hover{border-image:url(:/image/Resources/close_hov.png);}"
		"QPushButton::pressed{border-image:url(:/image/Resources/close_down.png);}");

	SetWindowFlags(SKBASEWIDGET_MAXIMIZE | SKBASEWIDGET_MINIMIZE);

	connect(m_btnClose, SIGNAL(clicked()), this, SIGNAL(SigClose()));
	connect(m_btnMinimize, SIGNAL(clicked()), this, SIGNAL(SigMin()));
	connect(m_btnMaximize, SIGNAL(clicked()), this, SIGNAL(SigMax()));
	connect(m_btnMainMenu, SIGNAL(clicked()), this, SLOT(SlotMainMenu()));
	connect(m_btnFullScreen, SIGNAL(clicked()), this, SIGNAL(SigFullScreen()));
}

bool CTopWidget::eventFilter(QObject *o, QEvent *e)
{
	return QObject::eventFilter(o,e);
}

void CTopWidget::mousePressEvent(QMouseEvent *e)
{
	QWidget::mousePressEvent(e);
}

void CTopWidget::mouseReleaseEvent(QMouseEvent *e)
{
	QWidget::mouseReleaseEvent(e);
}

void CTopWidget::mouseMoveEvent(QMouseEvent *e)
{
	setCursor(Qt::ArrowCursor);

	QWidget::mouseMoveEvent(e);
}

void CTopWidget::mouseDoubleClickEvent(QMouseEvent *e)
{
	if (!m_bFixed)
		emit SigMax();
}

void CTopWidget::paintEvent(QPaintEvent *e)
{
	QStyleOption opt;
	opt.init(this);
	QPainter p(this);
	style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}

void CTopWidget::SetWindowIcon(QIcon icon)
{
	if (icon.isNull())
		m_btnLogo->hide();
	else
		m_btnLogo->setIcon(icon);
}

void CTopWidget::SetWindowIcon(QString icon)
{
	if (icon.isEmpty())
		m_btnLogo->hide();
	else
	{
		m_btnLogo->setStyleSheet(tr(
			"QPushButton{border-image:url(%1);}"
			"QPushButton::hover{border-image:url(%1);}"
			"QPushButton::pressed{border-image:url(%1);}").arg(icon));
	}
}

void CTopWidget::SetWindowTitle(QString title)
{
	m_labelTitle->setText(title);
}

void CTopWidget::SetWindowFlags(int flags)
{
	if (m_mainLayout)
	{
		delete m_mainLayout;
		m_mainLayout = 0;
	}

	QHBoxLayout *flagsLayout = new QHBoxLayout();
	if ((flags & SKBASEWIDGET_FULLSCREEN) == SKBASEWIDGET_FULLSCREEN  && !m_bFixed)
	{
		m_btnFullScreen->show();
		flagsLayout->addWidget(m_btnFullScreen,0,Qt::AlignVCenter);
	}
	else
	{
		m_btnFullScreen->hide();
	}

	if ((flags & SKBASEWIDGET_MAINMENU) == SKBASEWIDGET_MAINMENU)
	{
		m_btnMainMenu->show();
		flagsLayout->addWidget(m_btnMainMenu,0,Qt::AlignVCenter);
	}
	else
	{
		m_btnMainMenu->hide();
	}

	if ((flags & SKBASEWIDGET_MINIMIZE) == SKBASEWIDGET_MINIMIZE)
	{
		m_btnMinimize->show();
		flagsLayout->addWidget(m_btnMinimize,0,Qt::AlignVCenter);
	}
	else
	{
		m_btnMinimize->hide();
	}

	if ((flags & SKBASEWIDGET_MAXIMIZE) == SKBASEWIDGET_MAXIMIZE && !m_bFixed)
	{
		m_btnMaximize->show();
		flagsLayout->addWidget(m_btnMaximize,0,Qt::AlignVCenter);
	}
	else
	{
		m_btnMaximize->hide();
	}

	flagsLayout->addWidget(m_btnClose,0,Qt::AlignVCenter);
	flagsLayout->setSpacing(15);
	flagsLayout->setContentsMargins(0,0,0,0);

	m_mainLayout = new QHBoxLayout();
	m_mainLayout->addWidget(m_btnLogo,0,Qt::AlignVCenter);
	m_mainLayout->addWidget(m_labelTitle,0,Qt::AlignVCenter);
	m_mainLayout->addStretch();
	m_mainLayout->addLayout(flagsLayout);
	m_mainLayout->setSpacing(0);
	m_mainLayout->setContentsMargins(2,0,7,0);

	setLayout(m_mainLayout);
	setFixedHeight(TOPWIDGET_HEIGHT);
}

void CTopWidget::SetWindowBackgroundImaged(bool b)
{
	m_bBackgroundImaged = b;

	QString qss;
	if (m_bBackgroundImaged)
		qss = "QWidget#CTopWidget{background:transparent;border-top-left-radius:4px;border-top-right-radius:4px;}";
	else
		qss = QString("QWidget#CTopWidget{background:%1;border-top-left-radius:4px;border-top-right-radius:4px;}").arg(COLOR_NW);
	setStyleSheet(qss);
}

void CTopWidget::SlotMainMenu()
{
	QPoint p = m_btnMainMenu->pos();
	p.setY(TOPWIDGET_HEIGHT+1);
	if (m_iMainMenu)
		m_iMainMenu->popup(this->mapToGlobal(p));
}
