#include "skbasewidget.h"
#include "ctopwidget.h"
#include "cmainwidget.h"

#define SHADOW_WIDTH	6

SKBaseWidget::SKBaseWidget(QWidget *parent,QWidget *w)
	: AbsFrameLessAutoSize(parent)
	, m_centerWidget(w)
{
	Init();
	InitUi();
	InitSlot();
}

SKBaseWidget::~SKBaseWidget()
{

}

void SKBaseWidget::Init()
{
	SetWindowSize(600,400);
	m_iLocation = geometry();
	m_bIsMax = false;
	m_bIsFull = false;
	m_bIsDrag = true;
	m_bIsTopDrag = false;
}

void SKBaseWidget::InitUi()
{
	setAttribute(Qt::WA_TranslucentBackground);

	m_mainWidget = new CMainWidget();
	m_mainGridLayout = new QGridLayout;
	m_mainGridLayout->addWidget((CMainWidget*)m_mainWidget);
	m_mainGridLayout->setContentsMargins(SHADOW_WIDTH,SHADOW_WIDTH,SHADOW_WIDTH,SHADOW_WIDTH);
	setLayout(m_mainGridLayout);

	m_topWidget = new CTopWidget(this);
	QVBoxLayout *topLayout = new QVBoxLayout;
	topLayout->addWidget((CTopWidget*)m_topWidget);
	topLayout->setSpacing(0);
	topLayout->setContentsMargins(0,0,0,0);

	m_vBoxLyout = new QVBoxLayout;
	m_vBoxLyout->addLayout(topLayout);
	if (m_centerWidget)
		m_vBoxLyout->addWidget(m_centerWidget);
	else
		m_vBoxLyout->addStretch();
	m_vBoxLyout->setContentsMargins(0,0,0,0);
	m_vBoxLyout->setSpacing(0);
	((CMainWidget*)m_mainWidget)->setLayout(m_vBoxLyout);
}

void SKBaseWidget::InitSlot()
{
	connect((CTopWidget*)m_topWidget,SIGNAL(SigMax()),this,SLOT(SlotShowMax()));
	connect((CTopWidget*)m_topWidget,SIGNAL(SigMin()),this,SLOT(SlotShowMin()));
	connect((CTopWidget*)m_topWidget,SIGNAL(SigClose()),this,SLOT(SlotClose()));
	connect((CTopWidget*)m_topWidget,SIGNAL(SigFullScreen()),this,SLOT(SlotFullScreen()));
	connect(m_centerWidget,SIGNAL(SigClose()),this,SLOT(SlotClose()));
}

void SKBaseWidget::mouseMoveEvent(QMouseEvent *e)
{
	QPoint p = mapFromGlobal(e->globalPos());
	mouseMoveRect(p);

	if (m_bIsFull || !m_bIsDrag)
		return;
	if (m_bIsTopDrag && p.y() > TOPWIDGET_HEIGHT)
		return;
	if (m_bIsTopDrag && ((CTopWidget*)m_topWidget)->isHidden())
		return;

	if (m_state.MousePressed)
	{
		if (m_bIsMax)
		{
			SlotShowMax();
			int curMonitor = QApplication::desktop()->screenNumber(this);
			QRect rect = QApplication::desktop()->availableGeometry(curMonitor);
			float xs;
			int fx;
			if (curMonitor <= 0) //主显示屏
			{
				xs = (float)m_state.MousePos.x()/(float)rect.right();
				fx = int((float)(m_iLocation.right()-m_iLocation.left()) * xs);
			}
			else //分显示屏
			{
				xs = (float)(m_state.MousePos.x()-rect.left())/(float)(rect.right()-rect.left());
				fx = int((float)(m_iLocation.right()-m_iLocation.left()) * xs);
			}

			xs = (float)m_state.MousePos.y()/(float)rect.bottom();
			int fy = int((float)(m_iLocation.bottom()-m_iLocation.top()) * xs);

			m_state.WindowPos.setX(m_state.MousePos.x()-fx);
			m_state.WindowPos.setY(m_state.MousePos.y()-fy);
		}

		move(m_state.WindowPos + (e->globalPos() - m_state.MousePos));
	}
}

void SKBaseWidget::mousePressEvent(QMouseEvent *e)
{
	AbsFrameLessAutoSize::mousePressEvent(e);
}

void SKBaseWidget::mouseReleaseEvent(QMouseEvent *e)
{
	AbsFrameLessAutoSize::mouseReleaseEvent(e);
}

void SKBaseWidget::mouseDoubleClickEvent(QMouseEvent *e)
{
	AbsFrameLessAutoSize::mouseDoubleClickEvent(e);
}

void SKBaseWidget::keyPressEvent(QKeyEvent *e)
{
	if (e->key() == Qt::Key_Escape && m_bIsFull)
	{
		SlotShowNormal();
	}
}

void SKBaseWidget::closeEvent(QCloseEvent *e)
{
	e->accept();
}

void SKBaseWidget::paintEvent(QPaintEvent *e)
{
	QPainterPath path;
	path.setFillRule(Qt::WindingFill);
	if (m_bIsMax || m_bIsFull)
		path.addRect(0,0,this->width(),this->height());
	else
		path.addRoundRect(SHADOW_WIDTH,SHADOW_WIDTH,this->width()-SHADOW_WIDTH*2,this->height()-SHADOW_WIDTH*2,3);

	QPainter painter(this);
	painter.setRenderHint(QPainter::Antialiasing, true);
	//painter.fillPath(path,QBrush(QColor(240,240,240,255)));
	painter.fillPath(path,QBrush(QColor(COLOR_NW)));

	QColor color(0, 0, 0);
	for (int i = 0; i < SHADOW_WIDTH; i++)
	{
		QPainterPath path;
		path.setFillRule(Qt::WindingFill);
		if (m_bIsMax || m_bIsFull)
			path.addRect(0,0,this->width(),this->height());
		else
			path.addRoundRect(SHADOW_WIDTH-i, SHADOW_WIDTH-i, this->width()-(SHADOW_WIDTH-i)*2, this->height()-(SHADOW_WIDTH-i)*2,2);
		color.setAlpha(50 - qSqrt(i)*15);
		painter.setPen(color);
		painter.drawPath(path);
	}
}

void SKBaseWidget::changeEvent(QEvent *e)
{
#if (QT_VERSION <= QT_VERSION_CHECK(5,1,0))
	if (e->type() == QEvent::WindowStateChange)
	{
		if(windowState() & Qt::WindowMinimized )
		{
			//do something after minimize
		}
		else
		{
			setWindowFlags(Qt::Window);//set normal window flag
			setWindowFlags(Qt::Window | Qt::FramelessWindowHint | Qt::WindowMinimizeButtonHint);//and return to your old flags
			showNormal();
		}
	}
#endif
}

void SKBaseWidget::SetWindowSize(int width,int height)
{
	int curMonitor = QApplication::desktop()->screenNumber(this);
	QRect rect = QApplication::desktop()->availableGeometry(curMonitor);

	if (width > rect.width())
		width = rect.width();
	if (height > rect.height())
		height = rect.height();

	setGeometry((rect.width()-width)/2+rect.x(),(rect.height()-height)/2,width,height);
	m_iLocation = geometry();
}

void SKBaseWidget::SetWindowFixSize(int width,int height)
{
	setFixedSize(width,height);
	SetWindowSize(width,height);
	SetFixed(true);
	((CTopWidget*)m_topWidget)->SetWindowFixed(true);
}

void SKBaseWidget::SetWindowIcon(QIcon icon)
{
	((CTopWidget*)m_topWidget)->SetWindowIcon(icon);
}

void SKBaseWidget::SetWindowTitle(QString title)
{
	((CTopWidget*)m_topWidget)->SetWindowTitle(title);
}

void SKBaseWidget::SetWindowFlags(int flags)
{
	((CTopWidget*)m_topWidget)->SetWindowFlags(flags);
}

void SKBaseWidget::SetWindowMainMenu(QMenu *menu)
{
	((CTopWidget*)m_topWidget)->SetWindowMainMenu(menu);
}

void SKBaseWidget::SetWindowBackgroundImage(QPixmap pix)
{
	((CMainWidget*)m_mainWidget)->SetPixmap(pix);
	((CTopWidget*)m_topWidget)->SetWindowBackgroundImaged(true);
	m_vBoxLyout->setContentsMargins(1,0,1,1);
}

void SKBaseWidget::HideTopFrame()
{
	((CTopWidget*)m_topWidget)->hide();
	//m_vBoxLyout->setContentsMargins(1,1,1,1);
	m_vBoxLyout->setContentsMargins(0,0,0,0);
}

void SKBaseWidget::Show()
{
	show();
}

void SKBaseWidget::ShowMaximized()
{
	SlotShowMax();
	Show();
}

void SKBaseWidget::ShowFullScreen()
{
	m_bIsFull = true;
	((CTopWidget*)m_topWidget)->hide();
	SlotShowMax();
	Show();
}

void SKBaseWidget::SlotClose()
{
	close();
}

void SKBaseWidget::SlotShowMax()
{
	int curMonitor = QApplication::desktop()->screenNumber(this);
	if (m_bIsFull)
	{
		m_mainGridLayout->setContentsMargins(0,0,0,0);
		setGeometry(QApplication::desktop()->screenGeometry(curMonitor));
		return;
	}

	if (m_bIsMax)
	{
		setGeometry(m_iLocation); //还原窗口大小和位置
		m_mainGridLayout->setContentsMargins(SHADOW_WIDTH,SHADOW_WIDTH,SHADOW_WIDTH,SHADOW_WIDTH);
		((CTopWidget*)m_topWidget)->GetBtnMaximize()->setToolTip(tr("Maximize"));
		((CTopWidget*)m_topWidget)->GetBtnMaximize()->setStyleSheet(
			"QPushButton{border-image:url(:/image/Resources/max_nor.png);}"
			"QPushButton::hover{border-image:url(:/image/Resources/max_hov.png);}"
			"QPushButton::pressed{border-image:url(:/image/Resources/max_down.png);}");
	}
	else
	{
		m_iLocation = geometry(); //最大化前，记录窗口大小和位置
		setGeometry(QApplication::desktop()->availableGeometry(curMonitor));
		m_mainGridLayout->setContentsMargins(0,0,0,0);
		((CTopWidget*)m_topWidget)->GetBtnMaximize()->setToolTip(tr("Restore"));
		((CTopWidget*)m_topWidget)->GetBtnMaximize()->setStyleSheet(
			"QPushButton{border-image:url(:/image/Resources/max_nor1.png);}"
			"QPushButton::hover{border-image:url(:/image/Resources/max_hov1.png);}"
			"QPushButton::pressed{border-image:url(:/image/Resources/max_down1.png);}");
	}

	m_bIsMax = !m_bIsMax;
}

void SKBaseWidget::SlotShowMin()
{
	showMinimized();
}

void SKBaseWidget::SlotFullScreen()
{
	if (!m_bIsMax) //非最大化状态下，记录窗口大小和位置
		m_iLocation = geometry();

	ShowFullScreen();
}

void SKBaseWidget::SlotShowNormal()
{
	if (m_bIsMax)
	{
		int curMonitor = QApplication::desktop()->screenNumber(this);
		setGeometry(QApplication::desktop()->availableGeometry(curMonitor));
		m_mainGridLayout->setContentsMargins(0,0,0,0);
		((CTopWidget*)m_topWidget)->GetBtnMaximize()->setToolTip(tr("Restore"));
		((CTopWidget*)m_topWidget)->GetBtnMaximize()->setStyleSheet(
			"QPushButton{border-image:url(:/image/Resources/max_nor1.png);}"
			"QPushButton::hover{border-image:url(:/image/Resources/max_hov1.png);}"
			"QPushButton::pressed{border-image:url(:/image/Resources/max_down1.png);}");
		((CTopWidget*)m_topWidget)->show();
		m_bIsFull = false;
		return;
	}

	QRect rect;
	rect.setX(m_iLocation.x());
	rect.setY(m_iLocation.y());
	rect.setWidth(m_iLocation.width());
	rect.setHeight(m_iLocation.height());
	setGeometry(rect);
	m_iLocation = rect;
	m_mainGridLayout->setContentsMargins(SHADOW_WIDTH,SHADOW_WIDTH,SHADOW_WIDTH,SHADOW_WIDTH);
	((CTopWidget*)m_topWidget)->GetBtnMaximize()->setStyleSheet(
		"QPushButton{border-image:url(:/image/Resources/max_nor.png);}"
		"QPushButton::hover{border-image:url(:/image/Resources/max_hov.png);}"
		"QPushButton::pressed{border-image:url(:/image/Resources/max_down.png);}");
	((CTopWidget*)m_topWidget)->show();
	m_bIsFull = false;
}
