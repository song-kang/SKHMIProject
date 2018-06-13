#include "drawscene.h"
#include "drawtool.h"
#include "skdraw.h"

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

///////////////////////// BBoxSort /////////////////////////
class BBoxSort
{
public:
	BBoxSort( QGraphicsItem * item , const QRectF & rect , eAlignType alignType )
		:item_(item),box(rect),align(alignType)
	{
		min_ = alignType == eAlignHSpace ? box.topLeft().x() : box.topLeft().y();
		max_ = alignType == eAlignHSpace ? box.bottomRight().x() : box.bottomRight().y();
		extent_ = alignType == eAlignHSpace ? box.width() : box.height();
		anchor = min_ * 0.5 + max_ * 0.5;
	}
	qreal min() { return min_; }
	qreal max() { return max_; }
	qreal extent() { return extent_; }
	QGraphicsItem * item_;
	qreal anchor;
	qreal min_;
	qreal max_;
	qreal extent_;
	QRectF box;
	eAlignType align ;
};

bool operator < (const BBoxSort &a, const BBoxSort &b)
{
	return (a.anchor < b.anchor);
}

///////////////////////// DrawScene /////////////////////////
DrawScene::DrawScene(QObject *parent)
	: QGraphicsScene(parent)
{
	m_app = (SKDraw*)parent;
	m_pView = NULL;
	m_pGrid = new GridTool();

	SetWidth(DEFAULT_WIDTH);
	SetHeight(DEFAULT_HEIGHT);
	SetPressShift(false);

	setSceneRect(QRectF(0, 0, m_iWidth, m_iHeight));
	setBackgroundBrush(Qt::darkGray);

	m_dx = m_dy = 0;
	m_pAlignItem = NULL;

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
		{
			item->moveBy(dx,dy);
			((DrawView*)m_pView)->GetApp()->GetPropertyEditor()->UpdateProperties(((GraphicsItem*)item)->metaObject());
		}
	}
}

void DrawScene::keyReleaseEvent(QKeyEvent *e)
{
	if (m_bMoved && selectedItems().count() > 0)
		emit SigItemMoved(NULL, QPointF(m_dx,m_dy));

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
			for (Handles::iterator it = sp->m_handles.begin(); it != sp->m_handles.end(); ++it)
				(*it)->SetBorderColor(Qt::black);
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
				for (Handles::iterator it = sp->m_handles.begin(); it != sp->m_handles.end(); ++it)
					(*it)->SetBorderColor(Qt::gray);
			}
			bFind = true;
		}

		if (!bFind && m_pAlignItem)
			clearSelection();
	}

}

void DrawScene::Align(eAlignType alignType)
{
	if (!m_pAlignItem)
		return;

	QRectF rectref = m_pAlignItem->mapRectToScene(m_pAlignItem->boundingRect());
	int nLeft, nRight, nTop, nBottom;
	nLeft = nRight = rectref.center().x();
	nTop = nBottom = rectref.center().y();
	QPointF pt = rectref.center();
	qreal width = m_pAlignItem->GetWidth();
	qreal height = m_pAlignItem->GetHeight();

	if (alignType == eAlignHSpace || alignType == eAlignVSpace)
	{
		std::vector<BBoxSort> sorted;
		foreach (QGraphicsItem *item , selectedItems())
		{
			QGraphicsItemGroup *g = dynamic_cast<QGraphicsItemGroup*>(item->parentItem());
			if (g)
				continue;

			sorted.push_back(BBoxSort(item,item->mapRectToScene(item->boundingRect()),alignType));
		}

		std::sort(sorted.begin(), sorted.end());

		unsigned int len = sorted.size();
		bool changed = false;
		float dist = (sorted.back().max()-sorted.front().min());
		float span = 0;
		for (unsigned int i = 0; i < len; i++)
			span += sorted[i].extent();

		float step = (dist - span) / (len - 1);
		float pos = sorted.front().min();
		for (std::vector<BBoxSort>::iterator it(sorted.begin()); it < sorted.end(); ++it)
		{
			QPointF t;
			if (alignType == eAlignHSpace)
				t.setX(pos - it->min());
			else
				t.setY(pos - it->min());

			it->item_->moveBy(t.x(),t.y());
			emit SigItemMoved(it->item_, t);
			changed = true;

			pos += it->extent();
			pos += step;
		}
	}
	else
	{
		foreach (QGraphicsItem *item, selectedItems())
		{
			QGraphicsItemGroup *g = dynamic_cast<QGraphicsItemGroup*>(item->parentItem());
			if (g)
				continue;

			QRectF rectItem = item->mapRectToScene(item->boundingRect());
			QPointF ptNew = rectItem.center();
			switch (alignType)
			{
			case eAlignTop:
				ptNew.setY(nTop + (rectItem.height()-rectref.height())/2);
				break;
			case eAlignHCenter:
				ptNew.setY(pt.y());
				break;
			case eAlignVCenter:
				ptNew.setX(pt.x());
				break;
			case eAlignBottom:
				ptNew.setY(nBottom - (rectItem.height()-rectref.height())/2);
				break;
			case eAlignLeft:
				ptNew.setX(nLeft - (rectref.width()-rectItem.width())/2);
				break;
			case eAlignRight:
				ptNew.setX(nRight + (rectref.width()-rectItem.width())/2);
				break;
			case eAlignSize:
				{
					AbstractShape * aitem = qgraphicsitem_cast<AbstractShape*>(item);
					if ( aitem ){
						qreal fx = width / aitem->GetWidth();
						qreal fy = height / aitem->GetHeight();
						if ( fx == 1.0 && fy == 1.0 ) break;
						aitem->Stretch(eHandleRightBottom,fx,fy,aitem->Opposite(eHandleRightBottom));
						aitem->UpdateCoordinate();
						emit SigItemResize(aitem, eHandleRightBottom, QPointF(fx,fy));
					}
				}
				break;
			case eAlignWidth:
				{
					AbstractShape * aitem = qgraphicsitem_cast<AbstractShape*>(item);
					if ( aitem ){
						qreal fx = width / aitem->GetWidth();
						if ( fx == 1.0 ) break;
						aitem->Stretch(eHandleRight,fx,1,aitem->Opposite(eHandleRight));
						aitem->UpdateCoordinate();
						emit SigItemResize(aitem, eHandleRight, QPointF(fx,1));
					}
				}
				break;
			case eAlignHeight:
				{
					AbstractShape * aitem = qgraphicsitem_cast<AbstractShape*>(item);
					if ( aitem ){

						qreal fy = height / aitem->GetHeight();
						if (fy == 1.0 ) break ;
						aitem->Stretch(eHandleBottom,1,fy,aitem->Opposite(eHandleBottom));
						aitem->UpdateCoordinate();
						emit SigItemResize(aitem, eHandleBottom, QPointF(1,fy));
					}
				}
				break;
			}

			QPointF ptLast= rectItem.center();
			QPointF ptMove = ptNew - ptLast;
			if (!ptMove.isNull())
			{
				item->moveBy(ptMove.x(), ptMove.y());
				emit SigItemMoved(item, ptMove);
			}
		}
	}
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
