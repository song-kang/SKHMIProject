#include "drawtool.h"
#include "drawview.h"
#include "skdraw.h"

QList<DrawTool*> DrawTool::c_tools;
QPointF DrawTool::c_down;
QPointF DrawTool::c_last;

DrawShape DrawTool::c_drawShape = eDrawSelection;

static DrawSelectTool	selectTool;
static DrawRotationTool rotationTool;
static DrawPolygonTool	lineTool(eDrawLine);

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
int m_nDragHandle = Handle_None;

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
	m_pDashRect = NULL;
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
		if (m_nDragHandle != Handle_None && m_nDragHandle <= Handle_Left)
		{
			m_selectMode = eModeSize;

			//opposite_ = item->opposite(nDragHandle);
			//if( opposite_.x() == 0 )
			//	opposite_.setX(1);
			//if (opposite_.y() == 0 )
			//	opposite_.setY(1);
		}
		else if ( m_nDragHandle > Handle_Left )
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
			if (m_nDragHandle != Handle_None && m_selectMode == eModeSize)
			{
				//if (opposite_.isNull())
				//{
				//	opposite_ = item->opposite(m_nDragHandle);
				//	if( opposite_.x() == 0 )
				//		opposite_.setX(1);
				//	if (opposite_.y() == 0 )
				//		opposite_.setY(1);
				//}

				//QPointF new_delta = item->mapFromScene(c_last) - opposite_;
				//QPointF initial_delta = item->mapFromScene(c_down) - opposite_;

				//double sx = new_delta.x() / initial_delta.x();
				//double sy = new_delta.y() / initial_delta.y();

				//item->Stretch(m_nDragHandle, sx , sy , opposite_);

				//emit scene->itemResize(item,m_nDragHandle,QPointF(sx,sy));
			} 
			else if (m_nDragHandle > Handle_Left && m_selectMode == eModeEditor)
			{
				item->Control(m_nDragHandle,c_last);
				//emit scene->itemControl(item,m_nDragHandle,c_last,c_down);
			}
			else if (m_nDragHandle == Handle_None)
			{
				int handle = item->CollidesWithHandle(event->scenePos());
				if (handle != Handle_None)
				{
					SetCursor(scene,Qt::OpenHandCursor);
					if (handle > Handle_Left)
						SetCursor(scene,Qt::CrossCursor); //线段手势
					//m_bHoverSizer = true;
				}
				else if (m_selectMode == eModeMove)
				{
					item->setPos(m_initialPositions + c_last - c_down); //线段移动
				}
				else
				{
					SetCursor(scene,Qt::ArrowCursor);
					//m_bHoverSizer = false;
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
			//item->UpdateCoordinate();
		}
	}
	else if (items.count() > 1 && m_selectMode == eModeMove && c_last != c_down)
	{
		//emit scene->itemMoved(NULL , c_last - c_down );
	}

	m_selectMode = eModeNone;
	m_nDragHandle = Handle_None;
	m_bHoverSizer = false;
	//opposite_ = QPointF();
	scene->MouseEvent(event);
}

///////////////////////// DrawRotationTool /////////////////////////
DrawRotationTool::DrawRotationTool()
	: DrawTool(eDrawRotation)
{
	m_lastAngle = 0;
	m_pDashRect = NULL;
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
	
}

DrawRectTool::~DrawRectTool()
{

}

void DrawRectTool::mousePressEvent(QGraphicsSceneMouseEvent *event, DrawScene *scene)
{

}

void DrawRectTool::mouseMoveEvent(QGraphicsSceneMouseEvent *event, DrawScene *scene)
{

}

void DrawRectTool::mouseReleaseEvent(QGraphicsSceneMouseEvent *event, DrawScene *scene)
{

}

///////////////////////// DrawPolygonTool /////////////////////////
DrawPolygonTool::DrawPolygonTool(DrawShape shape)
	: DrawTool(shape)
{
	item = NULL;
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

	if (item == NULL)
	{
		scene->clearSelection();

		//if ( c_drawShape == polygon )
		//	item = new GraphicsPolygonItem(NULL);
		//else if (c_drawShape == bezier )
		//	item = new GraphicsBezier();
		//else if ( c_drawShape == polyline )
		//	item = new GraphicsBezier(false);
		/*else */if (c_drawShape == eDrawLine)
			item = new GraphicsLineItem(NULL);

		item->SetScene(scene);
		item->setPos(event->scenePos());
		scene->addItem(item);
		initialPositions = c_down;
		item->AddPoint(c_down);
		m_nPoints++;
		item->AddPoint(c_down + QPoint(1,0));
		m_nPoints++;
		m_selectMode = eModeSize ;
		m_nDragHandle = item->HandleCount();
	}
	else if (c_drawShape == eDrawLine)
	{
		item->EndPoint(event->scenePos());
		item->UpdateCoordinate();
		item->setSelected(true);
		emit scene->itemAdded(item);
		item = NULL;
		
		m_selectMode = eModeNone;
		m_nPoints = 0;
	}
}

void DrawPolygonTool::mouseMoveEvent(QGraphicsSceneMouseEvent *event, DrawScene *scene)
{
	DrawTool::mouseMoveEvent(event,scene);

	if (item != 0)
	{
		if (m_nDragHandle != Handle_None && m_selectMode == eModeSize )
			item->Control(m_nDragHandle,c_last);
	}
}

void DrawPolygonTool::mouseReleaseEvent(QGraphicsSceneMouseEvent *event, DrawScene *scene)
{
	DrawTool::mousePressEvent(event,scene);
}

void DrawPolygonTool::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event, DrawScene *scene)
{
	DrawTool::mouseDoubleClickEvent(event,scene);

	item->EndPoint(event->scenePos());
	item->UpdateCoordinate();
	//emit scene->itemAdded(item);

	item = NULL;
	m_selectMode = eModeNone;
	c_drawShape = eDrawSelection;
	m_nPoints = 0;
}
