#include "skwidget.h"

SKWidget::SKWidget(QWidget *parent)
	: QWidget(parent)
{
	setCursor(Qt::ArrowCursor);
}

SKWidget::~SKWidget()
{
	
}

void SKWidget::paintEvent(QPaintEvent *e)
{
	QWidget::paintEvent(e);

	QStyleOption opt;
	opt.init(this);
	QPainter p(this);
	style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}

//void SKWidget::mouseMoveEvent(QMouseEvent *e)
//{
//	
//}
