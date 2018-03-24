#include "cmainwidget.h"

CMainWidget::CMainWidget(QWidget *parent)
	: QWidget(parent)
{
	Init();
	InitUi();
}

CMainWidget::~CMainWidget()
{

}

void CMainWidget::Init()
{
	m_localPix = NULL;
	setAutoFillBackground(true);
}

void CMainWidget::InitUi()
{
	setObjectName("CMainWidget");
	setStyleSheet("QWidget#CMainWidget{background:transparent;border-top-left-radius:4px;border-top-right-radius:4px;}");
}

void CMainWidget::SetPixmap(const QPixmap &pix)
{
	m_localPix = pix;
	update();
}

void CMainWidget::paintEvent(QPaintEvent *e)
{
	QWidget::paintEvent(e);

	if (m_localPix.isNull())
		return;

	QStyleOption opt;
	opt.init(this);
	QPainter p(this);
	style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
	p.setRenderHints(QPainter::SmoothPixmapTransform,true);	//消锯齿

	double d = (double)m_localPix.height() / m_localPix.width();
	int h = d*width();
	int w = height() / d;
	p.drawPixmap(0,0,width(),h,m_localPix);
	m_localPix.scaled(width(),h);
	if (h < height())	//如果图片高度小于窗口高度
	{
		p.drawPixmap(0,0,w,height(),m_localPix);
		m_localPix.scaled(w,height());
	}
}
