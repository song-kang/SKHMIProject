#include "drawview.h"
#include "skdraw.h"
#include "drawscene.h"

DrawView::DrawView(QGraphicsScene *scene)
	: QGraphicsView(scene)
{
	m_pScene = scene;

	setRenderHint(QPainter::Antialiasing);
	//setTransform(transform().scale(1,-1)); //原点改为左下方

	//setCacheMode(QGraphicsView::CacheBackground);
	//setOptimizationFlags(QGraphicsView::DontSavePainterState);
	setViewportUpdateMode(QGraphicsView::FullViewportUpdate);
	setTransformationAnchor(QGraphicsView::AnchorUnderMouse);
	setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
	setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
	setViewport(new QWidget);

	m_pHorRuler = new QtRuleBar(Qt::Horizontal,this,this);
	m_pVerRuler = new QtRuleBar(Qt::Vertical,this,this);
	m_pBox = new QtCornerBox(this);
	m_bMouseTranslate = false;
	m_scale = 1.0;
	m_zoomDelta = 0.1;
	m_zoomCount = 0;

	connect(horizontalScrollBar(),SIGNAL(valueChanged(int)),this,SLOT(SlotScrollBarValueChanged(int)));
	connect(verticalScrollBar(),SIGNAL(valueChanged(int)),this,SLOT(SlotScrollBarValueChanged(int)));
}

DrawView::~DrawView()
{

}

void DrawView::SlotScrollBarValueChanged(int pos)
{
	UpdateRuler();
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

void DrawView::keyPressEvent(QKeyEvent *event)
{
	switch (event->key())
	{
	case Qt::Key_Up:
		Translate(QPointF(0, 10));
		break;
	case Qt::Key_Down:
		Translate(QPointF(0, -10));
		break;
	case Qt::Key_Left:
		Translate(QPointF(10, 0));
		break;
	case Qt::Key_Right:
		Translate(QPointF(-10, 0));
		break;
	case Qt::Key_Equal:
		ZoomIn();
		break;
	case Qt::Key_Minus:
		ZoomOut();
		break;
	case Qt::Key_Escape:
		m_app->SlotKeyEscape();
		break;
	case Qt::Key_Shift:
		((DrawScene*)m_pScene)->SetPressShift(true);
		break;
	}

	QGraphicsView::keyPressEvent(event);
}

void DrawView::keyReleaseEvent(QKeyEvent *event)
{
	switch (event->key())
	{
	case Qt::Key_Shift:
		((DrawScene*)m_pScene)->SetPressShift(false);
		break;
	}

	QGraphicsView::keyReleaseEvent(event);
}

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
	if (event->button() == Qt::LeftButton && dragMode() != QGraphicsView::RubberBandDrag)
	{
		QPointF point = mapToScene(event->pos());
		if (scene()->itemAt(point, transform()) == NULL)
		{
			m_bMouseTranslate = true;
			m_lastMousePos = event->pos();
			setCursor(Qt::ClosedHandCursor);
		}
	}
	else if (event->button() == Qt::RightButton)
	{
		SigMouseRightButton(event->pos());
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
	if (((DrawScene*)m_pScene)->selectedItems().count() > 0 || DrawTool::c_drawShape != eDrawSelection)
		return;
	
	delta *= m_scale;
	QPoint newCenter(viewport()->rect().width() / 2 - delta.x(), viewport()->rect().height() / 2 - delta.y());
	centerOn(mapToScene(newCenter));

	UpdateRuler();
}

void DrawView::ZoomIn()
{
	scale(1+m_zoomDelta, 1+m_zoomDelta);
	m_scale *= 1+m_zoomDelta;

	m_zoomCount += 1;
	UpdateRuler();
}

void DrawView::ZoomOut()
{
	scale(1/(1+m_zoomDelta), 1/(1+m_zoomDelta));
	m_scale *= 1/(1+m_zoomDelta);

	m_zoomCount -= 1;
	UpdateRuler();
}

void DrawView::ZoomOrg()
{
	if (m_zoomCount == 0)
		return;

	for (int i = 0; 
		 m_zoomCount > 0 ? i < m_zoomCount : i > m_zoomCount; 
		 m_zoomCount > 0 ? i++ : i--)
	{
		if (m_zoomCount > 0)
		{
			scale(1/(1+m_zoomDelta), 1/(1+m_zoomDelta));
			m_scale *= 1/(1+m_zoomDelta);
		}
		else
		{
			scale(1+m_zoomDelta, 1+m_zoomDelta);
			m_scale *= 1+m_zoomDelta;
		}
	}

	m_zoomCount = 0;
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
