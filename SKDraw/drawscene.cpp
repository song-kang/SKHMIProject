#include "drawscene.h"
#include "drawtool.h"
#include "skdraw.h"

#define DEFAULT_WIDTH		800
#define DEFAULT_HEIGHT		600

///////////////////////// GridTool /////////////////////////
GridTool::GridTool(const QSize &grid , const QSize &space)
	:m_sizeGrid(grid)
	,m_sizeGridSpace(space)
{
	QColor c(Qt::black);
	SetBackColor(c);
}

GridTool::~GridTool()
{

}

void GridTool::PaintGrid(QPainter *painter, const QRect &rect)
{
	QColor c(Qt::darkCyan);
	QPen p(c);
	p.setStyle(Qt::DashLine);
	p.setWidthF(0.2);
	painter->setPen(p);

	painter->save();
	painter->setRenderHints(QPainter::Antialiasing,false);

	painter->fillRect(rect,m_backColor);
	for (int x = rect.left(); x < rect.right(); x += (int)(m_sizeGridSpace.width()))
		painter->drawLine(x,rect.top(),x,rect.bottom());

	for (int y = rect.top(); y < rect.bottom(); y += (int)(m_sizeGridSpace.height()))
		painter->drawLine(rect.left(),y,rect.right(),y);

	painter->restore();
}

///////////////////////// DrawScene /////////////////////////
DrawScene::DrawScene(QObject *parent)
	: QGraphicsScene(parent)
{
	m_pView = NULL;
	m_pGrid = new GridTool();

	SetWidth(DEFAULT_WIDTH);
	SetHeight(DEFAULT_HEIGHT);
	SetPressShift(false);

	setSceneRect(QRectF(0, 0, m_iWidth, m_iHeight));
	setBackgroundBrush(Qt::darkGray);

	m_dx = m_dy = 0;
}

DrawScene::~DrawScene()
{
	delete m_pGrid;
}

void DrawScene::drawBackground(QPainter *painter, const QRectF &rect)
{
	QGraphicsScene::drawBackground(painter,rect);

	if (m_pGrid)
		m_pGrid->PaintGrid(painter, sceneRect().toRect());
}

void DrawScene::MouseEvent(QGraphicsSceneMouseEvent *mouseEvent)
{
	switch(mouseEvent->type())
	{
	case QEvent::GraphicsSceneMousePress:
		QGraphicsScene::mousePressEvent(mouseEvent);
		break;
	case QEvent::GraphicsSceneMouseMove:
		QGraphicsScene::mouseMoveEvent(mouseEvent);
		break;
	case QEvent::GraphicsSceneMouseRelease:
		QGraphicsScene::mouseReleaseEvent(mouseEvent);
		break;
	}
}

void DrawScene::mousePressEvent(QGraphicsSceneMouseEvent *mouseEvent)
{
	DrawTool *tool = DrawTool::findTool(DrawTool::c_drawShape);
	if (tool)
		tool->mousePressEvent(mouseEvent,this);
}

void DrawScene::mouseMoveEvent(QGraphicsSceneMouseEvent *mouseEvent)
{
	DrawTool *tool = DrawTool::findTool(DrawTool::c_drawShape);
	if (tool)
		tool->mouseMoveEvent(mouseEvent,this);
}

void DrawScene::mouseReleaseEvent(QGraphicsSceneMouseEvent *mouseEvent)
{
	DrawTool *tool = DrawTool::findTool(DrawTool::c_drawShape);
	if (tool)
		tool->mouseReleaseEvent(mouseEvent,this);
}

void DrawScene::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *mouseEvent)
{
	DrawTool *tool = DrawTool::findTool(DrawTool::c_drawShape);
	if (tool)
		tool->mouseDoubleClickEvent(mouseEvent,this);
}

void DrawScene::keyPressEvent(QKeyEvent *e)
{
	qreal dx = 0;
	qreal dy = 0;
	m_bMoved = false;

	switch(e->key())
	{
	case Qt::Key_Up:
		dx = 0;
		dy = -1;
		m_bMoved = true;
		break;
	case Qt::Key_Down:
		dx = 0;
		dy = 1;
		m_bMoved = true;
		break;
	case Qt::Key_Left:
		dx = -1;
		dy = 0;
		m_bMoved = true;
		break;
	case Qt::Key_Right:
		dx = 1;
		dy = 0;
		m_bMoved = true;
		break;
	}

	m_dx += dx;
	m_dy += dy;
	if (m_bMoved)
	{
		foreach (QGraphicsItem *item, selectedItems()) 
			item->moveBy(dx,dy);
	}
}

void DrawScene::keyReleaseEvent(QKeyEvent *e)
{
	//if (m_bMoved && selectedItems().count() > 0)
	//	emit itemMoved(NULL, QPointF(m_dx,m_dy));

	m_dx = m_dy = 0;
}
