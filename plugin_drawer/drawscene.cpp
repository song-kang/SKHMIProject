#include "drawscene.h"
#include "view_plugin_drawer.h"

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
	m_app = (view_plugin_drawer*)parent;
	m_pView = NULL;
	m_pGrid = new GridTool();

	SetWidth(DEFAULT_WIDTH);
	SetHeight(DEFAULT_HEIGHT);
	SetPressShift(false);

	setSceneRect(QRectF(0, 0, m_iWidth, m_iHeight));
	setBackgroundBrush(QBrush("#013E53"));

	m_dx = m_dy = 0;
	m_pAlignItem = NULL;

	m_pSwapIntervalTimer = new QTimer(this);
	m_pSwapIntervalTimer->setInterval(1000);
	m_pSwapIntervalTimer->start();

	connect(m_pSwapIntervalTimer, SIGNAL(timeout()), this, SLOT(SlotSwapIntervalTimer()));
	connect(this, SIGNAL(selectionChanged()), this, SLOT(SlotSelectionChanged()));
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
	QGraphicsScene::mousePressEvent(mouseEvent);
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
	//if (m_bMoved)
	//{
	//	foreach (QGraphicsItem *item, selectedItems())
	//	{
	//		item->moveBy(dx,dy);
	//		((DrawView*)m_pView)->GetApp()->GetPropertyEditor()->UpdateProperties(((GraphicsItem*)item)->metaObject());
	//	}
	//}
}

void DrawScene::keyReleaseEvent(QKeyEvent *e)
{
	//if (m_bMoved && selectedItems().count() > 0)
	//	emit SigItemMoved(NULL, QPointF(m_dx,m_dy));

	m_dx = m_dy = 0;
}

void DrawScene::SlotSelectionChanged()
{
	QList<QGraphicsItem*> l = selectedItems();
	if (l.count() == 0)
	{
		SetAlignItem(NULL);
	}
	else if (l.count() == 1 && l.first()->isSelected())
	{
		AbstractShape *sp = qgraphicsitem_cast<AbstractShape*>(l.first());
		if (sp == m_pAlignItem)
			SetAlignItem(NULL);
		else
		{
			SetAlignItem(sp);
			//for (Handles::iterator it = sp->m_handles.begin(); it != sp->m_handles.end(); ++it)
			//	(*it)->SetBorderColor(Qt::black);
		}
	}
	else if (l.count() > 1)
	{
		bool bFind = false;
		foreach (QGraphicsItem *item, selectedItems())
		{
			AbstractShape *sp = qgraphicsitem_cast<AbstractShape*>(item);
			if (sp != m_pAlignItem)
			{
				//for (Handles::iterator it = sp->m_handles.begin(); it != sp->m_handles.end(); ++it)
				//	(*it)->SetBorderColor(Qt::gray);
			}
			bFind = true;
		}

		if (!bFind && m_pAlignItem)
			clearSelection();
	}

}

void DrawScene::SlotSwapIntervalTimer()
{
	m_bSwap = !m_bSwap;
	m_app->RefreshMeasureFromDB();

	m_pView->viewport()->update();
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
