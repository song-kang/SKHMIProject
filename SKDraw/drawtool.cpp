#include "drawtool.h"

QList<DrawTool*> DrawTool::c_tools;
QPointF DrawTool::c_down;
QPointF DrawTool::c_last;

DrawShape DrawTool::c_drawShape = eDrawSelection;

static DrawSelectTool	selectTool;
static DrawRotationTool rotationTool;
static DrawPolygonTool	lineTool(eDrawLine);

enum SelectMode
{
	none = 0,
	netSelect,
	move,
	size,
	rotate,
	editor,
};

SelectMode m_selectMode = none;
int m_nDragHandle = Handle_None;

///////////////////////// DrawTool /////////////////////////
DrawTool::DrawTool(DrawShape shape)
	: QObject(),
	m_drawShape(shape)
{
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
		if ((*iter)->m_drawShape == drawShape)
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

}

void DrawSelectTool::mouseMoveEvent(QGraphicsSceneMouseEvent *event, DrawScene *scene)
{

}

void DrawSelectTool::mouseReleaseEvent(QGraphicsSceneMouseEvent *event, DrawScene *scene)
{

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
		//if ( c_drawShape == polygon )
		//	item = new GraphicsPolygonItem(NULL);
		//else if (c_drawShape == bezier )
		//	item = new GraphicsBezier();
		//else if ( c_drawShape == polyline )
		//	item = new GraphicsBezier(false);
		/*else */if (c_drawShape == eDrawLine)
			item = new GraphicsLineItem(NULL);

		item->setPos(event->scenePos());
		scene->addItem(item);
		initialPositions = c_down;
		item->AddPoint(c_down);
		item->setSelected(true);
		m_nPoints++;
	}
	else if ( c_down == c_last )
	{
		/*
		if ( item != NULL )
		{
		scene->removeItem(item);
		delete item;
		item = NULL ;
		c_drawShape = selection;
		selectMode = none;
		return ;
		}
		*/
	}

	item->AddPoint(c_down + QPoint(1,0));
	m_nPoints++;
	m_selectMode = size ;
	m_nDragHandle = item->HandleCount();
}

void DrawPolygonTool::mouseMoveEvent(QGraphicsSceneMouseEvent *event, DrawScene *scene)
{
	DrawTool::mouseMoveEvent(event,scene);

	if (item != 0)
	{
		if (m_nDragHandle != Handle_None && m_selectMode == size )
			item->Control(m_nDragHandle,c_last);
	}
}

void DrawPolygonTool::mouseReleaseEvent(QGraphicsSceneMouseEvent *event, DrawScene *scene)
{
	DrawTool::mousePressEvent(event,scene);

	if (c_drawShape == eDrawLine)
	{
		item->EndPoint(event->scenePos());
		item->UpdateCoordinate();
		//emit scene->itemAdded(item);

		item = NULL;
		m_selectMode = none;
		c_drawShape = eDrawSelection;
		m_nPoints = 0;
	}
}

void DrawPolygonTool::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event, DrawScene *scene)
{
	DrawTool::mouseDoubleClickEvent(event,scene);

	item->EndPoint(event->scenePos());
	item->UpdateCoordinate();
	//emit scene->itemAdded(item);

	item = NULL;
	m_selectMode = none;
	c_drawShape = eDrawSelection;
	m_nPoints = 0;
}
