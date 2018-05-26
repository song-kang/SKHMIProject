#include "drawview.h"

DrawView::DrawView(QGraphicsScene *scene)
	: QGraphicsView(scene)
{
	setRenderHint(QPainter::Antialiasing);
	setTransform(transform().scale(1,-1));

	//setCacheMode(QGraphicsView::CacheBackground);
	//setOptimizationFlags(QGraphicsView::DontSavePainterState);
	//setViewportUpdateMode(QGraphicsView::SmartViewportUpdate);
	//setViewportUpdateMode(QGraphicsView::FullViewportUpdate);
	setTransformationAnchor(QGraphicsView::AnchorUnderMouse);
	setViewport(new QWidget);

	m_pHorRuler = new QtRuleBar(Qt::Horizontal,this,this);
	m_pVerRuler = new QtRuleBar(Qt::Vertical,this,this);
	m_pBox = new QtCornerBox(this);
	m_bMouseTranslate = false;
	m_scale = 1.0;
	m_zoomDelta = 0.1;
}

DrawView::~DrawView()
{

}

void DrawView::wheelEvent(QWheelEvent *event)
{
	if (event->delta() > 0)
		ZoomIn();
	else
		ZoomOut();
}

void DrawView::resizeEvent(QResizeEvent *event)
{
	setViewportMargins(RULER_SIZE - 1, RULER_SIZE - 1, 0, 0);

	m_pHorRuler->resize(this->size().width() - RULER_SIZE - 1, RULER_SIZE);
	m_pHorRuler->move(RULER_SIZE, 0);

	m_pVerRuler->resize(RULER_SIZE, this->size().height() - RULER_SIZE - 1);
	m_pVerRuler->move(0,RULER_SIZE);

	m_pBox->resize(RULER_SIZE,RULER_SIZE);
	m_pBox->move(0,0);

	UpdateRuler();

	QGraphicsView::resizeEvent(event);
}

//void DrawView::keyPressEvent(QKeyEvent *event)
//{
//	switch (event->key())
//	{
//	case Qt::Key_Up:
//		Translate(QPointF(0, -2));
//		break;
//	case Qt::Key_Down:
//		Translate(QPointF(0, 2));
//		break;
//	case Qt::Key_Left:
//		Translate(QPointF(2, 0));
//		break;
//	case Qt::Key_Right:
//		Translate(QPointF(-2, 0));
//		break;
//	case Qt::Key_Equal:
//		ZoomIn();
//		break;
//	case Qt::Key_Minus:
//		ZoomOut();
//		break;
//	}
//
//	QGraphicsView::keyPressEvent(event);
//}

void DrawView::mouseMoveEvent(QMouseEvent *event)
{
	m_pHorRuler->updatePosition(event->pos());
	m_pVerRuler->updatePosition(event->pos());

	QPointF pt = mapToScene(event->pos());
	emit SigPositionChanged(pt.x(), pt.y());

	if (m_bMouseTranslate)
	{
		QPointF mouseDelta = mapToScene(event->pos()) - mapToScene(m_lastMousePos);
		Translate(mouseDelta);
	}

	m_lastMousePos = event->pos();

	QGraphicsView::mouseMoveEvent(event);
}

void DrawView::mousePressEvent(QMouseEvent *event)
{
	if (event->button() == Qt::LeftButton)
	{
		QPointF point = mapToScene(event->pos());
		if (scene()->itemAt(point, transform()) == NULL)
		{
			m_bMouseTranslate = true;
			m_lastMousePos = event->pos();
			setCursor(Qt::ClosedHandCursor);
		}
	}

	QGraphicsView::mousePressEvent(event);
}

void DrawView::mouseReleaseEvent(QMouseEvent *event)
{
	if (event->button() == Qt::LeftButton)
	{
		m_bMouseTranslate = false;
		setCursor(Qt::ArrowCursor);
	}

	QGraphicsView::mouseReleaseEvent(event);
}

void DrawView::Translate(QPointF delta)
{
	delta *= m_scale;

	QPoint newCenter(viewport()->rect().width() / 2 - delta.x(), viewport()->rect().height() / 2 + delta.y());
	centerOn(mapToScene(newCenter));

	UpdateRuler();
}

void DrawView::ZoomIn()
{
	scale(1+m_zoomDelta, 1+m_zoomDelta);
	m_scale *= 1+m_zoomDelta;

	UpdateRuler();
}

void DrawView::ZoomOut()
{
	scale(1-m_zoomDelta, 1-m_zoomDelta);
	m_scale *= 1-m_zoomDelta;

	UpdateRuler();
}

void DrawView::UpdateRuler()
{
	if (!scene())
		return;

	QRectF viewbox = this->rect();
	QPointF offset = mapFromScene(scene()->sceneRect().topLeft());

	double factor = 1./transform().m11();
	double lower_x = factor * (viewbox.left() - offset.x());
	double upper_x = factor * (viewbox.right() - RULER_SIZE - offset.x());
	m_pHorRuler->setRange(lower_x,upper_x, upper_x - lower_x);
	m_pHorRuler->update();

	double lower_y = factor * (viewbox.top() - offset.y()) * -1;
	double upper_y = factor * (viewbox.bottom() - RULER_SIZE - offset.y()) * -1;
	m_pVerRuler->setRange(lower_y, upper_y, upper_y - lower_y);
	m_pVerRuler->update();
}
