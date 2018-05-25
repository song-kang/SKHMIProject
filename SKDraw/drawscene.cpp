#include "drawscene.h"
#include "drawtool.h"

#define DEFAULT_WIDTH		800
#define DEFAULT_HEIGHT		600

///////////////////////// GridTool /////////////////////////
GridTool::GridTool(const QSize &grid , const QSize &space)
	:m_sizeGrid(grid)
	,m_sizeGridSpace(space)
{
	QColor c(Qt::white);
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
	setSceneRect(QRectF(0, 0, m_iWidth, m_iHeight));
	setBackgroundBrush(Qt::darkGray);
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

void DrawScene::mousePressEvent(QGraphicsSceneMouseEvent *mouseEvent)
{
	DrawTool *tool = DrawTool::findTool(DrawTool::c_drawShape);
	if (tool)
		tool->mousePressEvent(mouseEvent,this);
}

void DrawScene::mouseMoveEvent(QGraphicsSceneMouseEvent *mouseEvent)
{
	DrawTool * tool = DrawTool::findTool(DrawTool::c_drawShape);
	if (tool)
		tool->mouseMoveEvent(mouseEvent,this);
}

void DrawScene::mouseReleaseEvent(QGraphicsSceneMouseEvent *mouseEvent)
{
	DrawTool * tool = DrawTool::findTool(DrawTool::c_drawShape);
	if (tool)
		tool->mouseReleaseEvent(mouseEvent,this);
}

void DrawScene::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *mouseEvet)
{

}

void DrawScene::keyPressEvent(QKeyEvent *e)
{
	int a = 0;
}

void DrawScene::keyReleaseEvent(QKeyEvent *e)
{

}
