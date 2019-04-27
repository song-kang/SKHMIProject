#include "drawscene.h"
#include "drawview.h"

#define DEFAULT_WIDTH		1880
#define DEFAULT_HEIGHT		960

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
	painter->save();
	painter->setRenderHints(QPainter::Antialiasing,false);
	painter->fillRect(rect,m_backColor);
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
	setBackgroundBrush(QBrush("#013E53"));

	m_pSwapIntervalTimer = new QTimer(this);
	m_pSwapIntervalTimer->setInterval(1000);
	m_pSwapIntervalTimer->start();

	m_pGifTimer = new QTimer(this);
	m_pGifTimer->setInterval(200);

	connect(this, SIGNAL(selectionChanged()), this, SLOT(SlotSelectionChanged()));
	connect(m_pSwapIntervalTimer, SIGNAL(timeout()), this, SLOT(SlotSwapIntervalTimer()));
	connect(m_pGifTimer, SIGNAL(timeout()), this, SLOT(SlotGifTimer()));
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
	QGraphicsScene::mousePressEvent(mouseEvent);
}

void DrawScene::mouseMoveEvent(QGraphicsSceneMouseEvent *mouseEvent)
{
	QGraphicsScene::mouseMoveEvent(mouseEvent);
}

void DrawScene::mouseReleaseEvent(QGraphicsSceneMouseEvent *mouseEvent)
{
	QGraphicsScene::mouseReleaseEvent(mouseEvent);
}

void DrawScene::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *mouseEvent)
{
	QGraphicsScene::mouseDoubleClickEvent(mouseEvent);
}

void DrawScene::keyPressEvent(QKeyEvent *e)
{

}

void DrawScene::keyReleaseEvent(QKeyEvent *e)
{

}

void DrawScene::SlotSelectionChanged()
{
	QList<QGraphicsItem*> list = selectedItems();
	if (list.count() != 1)
		return;

	((DrawView*)m_pView)->OnClicked(list);
}

void DrawScene::SlotSwapIntervalTimer()
{
	m_bSwap = !m_bSwap;
	((DrawView*)m_pView)->RefreshMeasureFromDB();

	((DrawView*)m_pView)->Redraw();
}

void DrawScene::SlotGifTimer()
{
	((DrawView*)m_pView)->Redraw();
}

GraphicsItemGroup* DrawScene::CreateGroup(const QList<QGraphicsItem *> &items, bool isAdd)
{
	QList<QGraphicsItem *> ancestors;
	int n = 0;
	QPointF pt = items.first()->pos();
	if (!items.isEmpty())
	{
		QGraphicsItem *parent = items.at(n++);
		while ((parent = parent->parentItem()))
			ancestors.append(parent);
	}

	QGraphicsItem *commonAncestor = 0;
	if (!ancestors.isEmpty())
	{
		while (n < items.size())
		{
			int commonIndex = -1;
			QGraphicsItem *parent = items.at(n++);
			do
			{
				int index = ancestors.indexOf(parent, qMax(0, commonIndex));
				if (index != -1)
				{
					commonIndex = index;
					break;
				}
			} while ((parent = parent->parentItem()));

			if (commonIndex == -1)
			{
				commonAncestor = 0;
				break;
			}

			commonAncestor = ancestors.at(commonIndex);
		}
	}

	GraphicsItemGroup *group = new GraphicsItemGroup(commonAncestor);
	if (!commonAncestor && isAdd)
		addItem(group);

	foreach (QGraphicsItem *item, items)
	{
		item->setSelected(false);
		QGraphicsItemGroup *g = dynamic_cast<QGraphicsItemGroup*>(item->parentItem());
		if (!g)
			group->addToGroup(item);
	}

	group->UpdateCoordinate();
	return group;
}
