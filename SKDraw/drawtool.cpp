#include "drawtool.h"
#include "drawview.h"
#include "skdraw.h"

QList<DrawTool*> DrawTool::c_tools;
QPointF DrawTool::c_down;
QPointF DrawTool::c_last;

DrawShape DrawTool::c_drawShape = eDrawSelection;

static DrawSelectTool	c_selectTool;
static DrawRotationTool c_rotationTool;

static DrawPolygonTool	c_lineTool(eDrawLine);
static DrawPolygonTool	c_polygonlineTool(eDrawPolyline);
static DrawPolygonTool	c_polygonTool(eDrawPolygon);

static DrawRectTool		c_rectTool(eDrawRectangle);
static DrawRectTool		c_roundRectTool(eDrawRoundrect);
static DrawRectTool		c_ellipseTool(eDrawEllipse);
static DrawRectTool		c_circleTool(eDrawCircle);
static DrawRectTool		c_textTool(eDrawText);

enum SelectMode
{
	eModeNone = 0,
	eModeNetSelect,
	eModeMove,
	eModeSize,
	eModeRotate,
	eModeEditor,
};

SelectMode m_selectMode = eModeNone;
int m_nDragHandle = eHandleNone;

static void SetCursor(DrawScene *scene, const QCursor &cursor)
{
	QGraphicsView *view = scene->GetView();
	if (view)
		view->setCursor(cursor);
}

///////////////////////// DrawTool /////////////////////////
DrawTool::DrawTool(DrawShape shape)
	: QObject(),
	m_drawShape(shape)
{
	m_bHoverSizer = false;
	c_tools.append(this);
}

DrawTool::~DrawTool()
{

}

void DrawTool::mousePressEvent(QGraphicsSceneMouseEvent *event, DrawScene *scene)
{
	c_down = event->scenePos();
	c_last = event->scenePos();
}

void DrawTool::mouseMoveEvent(QGraphicsSceneMouseEvent *event, DrawScene *scene)
{
	c_last = event->scenePos();
}

void DrawTool::mouseReleaseEvent(QGraphicsSceneMouseEvent *event, DrawScene *scene)
{
	//setCursor(scene,Qt::ArrowCursor);
}

void DrawTool::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event,DrawScene *scene)
{

}

DrawTool* DrawTool::findTool(DrawShape drawShape)
{
	QList<DrawTool*>::const_iterator iter;
	for (iter = c_tools.constBegin(); iter != c_tools.constEnd(); iter++)
	{
		if ((*iter)->m_drawShape == (drawShape == eDrawSelectionArea ? eDrawSelection : drawShape))
			return (*iter);
	}

	return 0;
}

///////////////////////// DrawSelectTool /////////////////////////
DrawSelectTool::DrawSelectTool()
	: DrawTool(eDrawSelection)
{
	m_opposite = QPointF();
}

DrawSelectTool::~DrawSelectTool()
{

}

void DrawSelectTool::mousePressEvent(QGraphicsSceneMouseEvent *event, DrawScene *scene)
{
	DrawTool::mousePressEvent(event,scene);

	if (event->button() != Qt::LeftButton)
		return;

	if (!m_bHoverSizer)
		scene->MouseEvent(event);

	QList<QGraphicsItem *> items = scene->selectedItems();
	AbstractShape *item = NULL;
	if (items.count() == 1)
		item = qgraphicsitem_cast<AbstractShape*>(items.first());

	if (item != NULL)
	{
		m_nDragHandle = item->CollidesWithHandle(event->scenePos());
		if (m_nDragHandle != eHandleNone && m_nDragHandle <= eHandleLeft)
		{
			m_selectMode = eModeSize;
			m_opposite = item->Opposite(m_nDragHandle);
			if( m_opposite.x() == 0 )
				m_opposite.setX(1);
			if (m_opposite.y() == 0 )
				m_opposite.setY(1);
		}
		else if ( m_nDragHandle > eHandleLeft )
			m_selectMode = eModeEditor;
		else
			m_selectMode =  eModeMove;
	}
	else if (items.count() > 1)
		m_selectMode =  eModeMove;

	if (m_selectMode == eModeMove && items.count() == 1)
		m_initialPositions = item->pos();
}

void DrawSelectTool::mouseMoveEvent(QGraphicsSceneMouseEvent *event, DrawScene *scene)
{
	DrawTool::mouseMoveEvent(event,scene);

	AbstractShape *item = NULL;
	QList<QGraphicsItem *> items = scene->selectedItems();
	if (items.count() == 1)
	{
		item = qgraphicsitem_cast<AbstractShape*>(items.first());
		if (item != NULL)
		{
			if (m_nDragHandle != eHandleNone && m_selectMode == eModeSize)
			{
				if (m_opposite.isNull())
				{
					m_opposite = item->Opposite(m_nDragHandle);
					if (m_opposite.x() == 0)
						m_opposite.setX(1);
					if (m_opposite.y() == 0)
						m_opposite.setY(1);
				}

				QPointF new_delta = item->mapFromScene(c_last) - m_opposite;
				QPointF initial_delta = item->mapFromScene(c_down) - m_opposite;
				double sx = new_delta.x() / initial_delta.x();
				double sy = new_delta.y() / initial_delta.y();
				item->Stretch(m_nDragHandle, sx, sy, m_opposite);
				//emit scene->itemResize(item,m_nDragHandle,QPointF(sx,sy));
			} 
			else if (m_nDragHandle > eHandleLeft && m_selectMode == eModeEditor)
			{
				item->Control(m_nDragHandle,c_last);
				//emit scene->itemControl(item,m_nDragHandle,c_last,c_down);
			}
			else if (m_nDragHandle == eHandleNone)
			{
				int handle = item->CollidesWithHandle(event->scenePos());
				if (handle != eHandleNone)
				{
					if (handle > eHandleLeft)
						SetCursor(scene,Qt::CrossCursor); //线段手势
					else if (handle == eHandleLeft || handle == eHandleRight)
						SetCursor(scene,Qt::SizeHorCursor);
					else if (handle == eHandleTop || handle == eHandleBottom)
						SetCursor(scene,Qt::SizeVerCursor);
					else if (handle == eHandleLeftTop || handle == eHandleRightBottom)
						SetCursor(scene,Qt::SizeFDiagCursor);
					else if (handle == eHandleLeftBottom || handle == eHandleRightTop)
						SetCursor(scene,Qt::SizeBDiagCursor);
					m_bHoverSizer = true;
				}
				else if (m_selectMode == eModeMove)
				{
					item->setPos(m_initialPositions + c_last - c_down); //线段移动
				}
				else
				{
					SetCursor(scene,Qt::ArrowCursor);
					m_bHoverSizer = false;
				}
			}
		}
	}

	if (m_selectMode != eModeSize && items.count() > 1)
		scene->MouseEvent(event); //多个物体移动
}

void DrawSelectTool::mouseReleaseEvent(QGraphicsSceneMouseEvent *event, DrawScene *scene)
{
	DrawTool::mouseReleaseEvent(event,scene);

	if (event->button() != Qt::LeftButton)
		return;

	QList<QGraphicsItem *> items = scene->selectedItems();
	if (items.count() == 1)
	{
		AbstractShape *item = qgraphicsitem_cast<AbstractShape*>(items.first());
		if (item != 0  && m_selectMode == eModeMove && c_last != c_down)
		{
			item->setPos(m_initialPositions + c_last - c_down);
			//emit scene->itemMoved(item , c_last - c_down );
		}
		else if (item !=0 && (m_selectMode == eModeSize || m_selectMode == eModeEditor) && c_last != c_down)
		{
			item->UpdateCoordinate();
		}
	}
	else if (items.count() > 1 && m_selectMode == eModeMove && c_last != c_down)
	{
		//emit scene->itemMoved(NULL , c_last - c_down );
	}

	m_selectMode = eModeNone;
	m_nDragHandle = eHandleNone;
	m_bHoverSizer = false;
	m_opposite = QPointF();
	scene->MouseEvent(event);
}

///////////////////////// DrawRotationTool /////////////////////////
DrawRotationTool::DrawRotationTool()
	: DrawTool(eDrawRotation)
{
	m_lastAngle = 0;
}

DrawRotationTool::~DrawRotationTool()
{

}

void DrawRotationTool::mousePressEvent(QGraphicsSceneMouseEvent *event, DrawScene *scene)
{

}

void DrawRotationTool::mouseMoveEvent(QGraphicsSceneMouseEvent *event, DrawScene *scene)
{

}

void DrawRotationTool::mouseReleaseEvent(QGraphicsSceneMouseEvent *event, DrawScene *scene)
{

}

///////////////////////// DrawRectTool /////////////////////////
DrawRectTool::DrawRectTool(DrawShape shape)
	: DrawTool(shape)
{
	m_pItem = NULL;
	m_opposite = QPointF();
}

DrawRectTool::~DrawRectTool()
{
	
}

void DrawRectTool::mousePressEvent(QGraphicsSceneMouseEvent *event, DrawScene *scene)
{
	DrawTool::mousePressEvent(event,scene);

	if (event->button() != Qt::LeftButton) 
		return;

	if (m_pItem == NULL)
	{
		m_opposite = QPointF();
		scene->clearSelection();
		if (c_drawShape == eDrawRectangle)
			m_pItem = new GraphicsRectItem(QRect(1, 1, 1, 1));
		else if (c_drawShape == eDrawRoundrect)
			m_pItem = new GraphicsRectItem(QRect(1, 1, 1, 1), true);
		else if (c_drawShape == eDrawEllipse)
			m_pItem = new GraphicsEllipseItem(QRect(1, 1, 1, 1));
		else if (c_drawShape == eDrawCircle)
			m_pItem = new GraphicsEllipseItem(QRect(1, 1, 1, 1), true);
		else if (c_drawShape == eDrawText)
			m_pItem = new GraphicsTextItem(QRect(0, 0, 50, 25));

		scene->addItem(m_pItem);
		m_pItem->setPos(event->scenePos());

		if (c_drawShape == eDrawText)
		{
			m_pItem->setSelected(true);
			m_pItem->UpdateCoordinate();
			m_selectMode = eModeNone;
			m_pItem = NULL;
			return;
		}
		
		c_down += QPoint(2, 2);
		m_selectMode = eModeSize;
		m_nDragHandle = eHandleRightBottom;
	}
	else
	{
		if (event->scenePos() == (c_down - QPoint(2,2)) && m_pItem)
		{
			scene->removeItem(m_pItem);
			delete m_pItem ;
		}
		else if (m_pItem)
		{
			m_pItem->setSelected(true);
			m_pItem->UpdateCoordinate();
			m_selectMode = eModeNone;
			//emit scene->itemAdded(m_pItem);
		}

		m_pItem = NULL;
	}
}

void DrawRectTool::mouseMoveEvent(QGraphicsSceneMouseEvent *event, DrawScene *scene)
{
	//c_selectTool.mouseMoveEvent(event, scene);
	DrawTool::mouseMoveEvent(event,scene);

	if (m_pItem != 0)
	{
		if (m_nDragHandle != eHandleNone && m_selectMode == eModeSize)
		{
			if (m_opposite.isNull())
			{
				m_opposite = m_pItem->Opposite(m_nDragHandle);
				if (m_opposite.x() == 0)
					m_opposite.setX(1);
				if (m_opposite.y() == 0)
					m_opposite.setY(1);
			}
			
			QPointF new_delta = m_pItem->mapFromScene(c_last) - m_opposite;
			QPointF initial_delta = m_pItem->mapFromScene(c_down) - m_opposite;
			double sx = new_delta.x() / initial_delta.x();
			double sy = new_delta.y() / initial_delta.y();
			m_pItem->Stretch(m_nDragHandle, sx, sy, m_opposite);
			//emit scene->itemResize(item,m_nDragHandle,QPointF(sx,sy));
		}
	}
}

void DrawRectTool::mouseReleaseEvent(QGraphicsSceneMouseEvent *event, DrawScene *scene)
{
	DrawTool::mouseReleaseEvent(event, scene);
}

///////////////////////// DrawPolygonTool /////////////////////////
DrawPolygonTool::DrawPolygonTool(DrawShape shape)
	: DrawTool(shape)
{
	m_pItem = NULL;
	m_nPoints = 0;
}

DrawPolygonTool::~DrawPolygonTool()
{
	
}

void DrawPolygonTool::mousePressEvent(QGraphicsSceneMouseEvent *event, DrawScene *scene)
{
	DrawTool::mousePressEvent(event,scene);

	if (event->button() != Qt::LeftButton) 
		return;

	if (m_pItem == NULL)
	{
		scene->clearSelection();
		if (c_drawShape == eDrawPolygon)
			m_pItem = new GraphicsPolygonItem();
		else if (c_drawShape == eDrawPolyline)
			m_pItem = new GraphicsPolygonLineItem();
		else if (c_drawShape == eDrawLine)
			m_pItem = new GraphicsLineItem();

		m_initialPositions = c_down;
		scene->addItem(m_pItem);
		m_pItem->SetScene(scene);
		m_pItem->setPos(event->scenePos());
		m_pItem->AddPoint(c_down);
		m_nPoints++;
	}
	else if (c_drawShape == eDrawLine)
	{
		m_pItem->EndPoint(event->scenePos());
		m_pItem->setSelected(true);
		//emit scene->itemAdded(m_pItem);
		m_pItem = NULL;
		
		m_selectMode = eModeNone;
		m_nPoints = 0;
		return;
	}

	m_pItem->AddPoint(c_down);
	m_nPoints++;
	m_selectMode = eModeSize ;
	m_nDragHandle = m_pItem->HandleCount();
}

void DrawPolygonTool::mouseMoveEvent(QGraphicsSceneMouseEvent *event, DrawScene *scene)
{
	DrawTool::mouseMoveEvent(event,scene);

	if (m_pItem != 0)
	{
		if (m_nDragHandle != eHandleNone && m_selectMode == eModeSize)
			m_pItem->Control(m_nDragHandle,c_last);
	}
}

void DrawPolygonTool::mouseReleaseEvent(QGraphicsSceneMouseEvent *event, DrawScene *scene)
{
	DrawTool::mouseReleaseEvent(event,scene);
}

void DrawPolygonTool::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event, DrawScene *scene)
{
	DrawTool::mouseDoubleClickEvent(event,scene);

	m_pItem->EndPoint(event->scenePos());
	m_pItem->UpdateCoordinate();
	m_pItem->setSelected(true);
	//emit scene->itemAdded(m_pItem);
	m_pItem = NULL;

	m_selectMode = eModeNone;
	m_nPoints = 0;
}
