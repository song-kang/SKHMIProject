#include "drawobj.h"

static QPainterPath qt_graphicsItem_shapeFromPath(const QPainterPath &path, const QPen &pen)
{
	// We unfortunately need this hack as QPainterPathStroker will set a width of 1.0
	// if we pass a value of 0.0 to QPainterPathStroker::setWidth()
	const qreal penWidthZero = qreal(0.00000001);

	if (path == QPainterPath() || pen == Qt::NoPen)
		return path;

	QPainterPathStroker ps;
	ps.setCapStyle(pen.capStyle());
	if (pen.widthF() <= 0.0)
		ps.setWidth(penWidthZero);
	else
		ps.setWidth(pen.widthF());

	ps.setJoinStyle(pen.joinStyle());
	ps.setMiterLimit(pen.miterLimit());
	QPainterPath p = ps.createStroke(path);
	p.addPath(path);

	return p;
}

///////////////////////// GraphicsItem /////////////////////////
GraphicsItem::GraphicsItem(QGraphicsItem *parent)
    :AbstractShapeType<QGraphicsItem>(parent)
{
    setFlag(QGraphicsItem::ItemIsMovable, true);
    setFlag(QGraphicsItem::ItemIsSelectable, true);
    setFlag(QGraphicsItem::ItemSendsGeometryChanges, true);
    this->setAcceptHoverEvents(true);
}

GraphicsItem::~GraphicsItem()
{

}

void GraphicsItem::UpdateHandles()
{

}

void GraphicsItem::contextMenuEvent(QGraphicsSceneContextMenuEvent *event)
{
	Q_UNUSED(event);
}

QVariant GraphicsItem::itemChange(QGraphicsItem::GraphicsItemChange change, const QVariant &value)
{
    return QGraphicsItem::itemChange(change, value);
}

///////////////////////// GraphicsPolygonItem /////////////////////////
GraphicsPolygonItem::GraphicsPolygonItem(QGraphicsItem *parent)
	:GraphicsItem(parent)
{

}

GraphicsPolygonItem::~GraphicsPolygonItem()
{

}

void GraphicsPolygonItem::Move(const QPointF &point)
{

}

void GraphicsPolygonItem::Control(int direct, const QPointF &delta)
{

}

void GraphicsPolygonItem::Stretch(int handle, double sx, double sy, const QPointF &origin)
{

}

bool GraphicsPolygonItem::SaveToXml(QXmlStreamWriter *xml)
{
	return true;
}

bool GraphicsPolygonItem::LoadFromXml(QXmlStreamReader *xml)
{
	return true;
}

void GraphicsPolygonItem::UpdateHandles()
{

}

void GraphicsPolygonItem::UpdateCoordinate()
{

}

QGraphicsItem* GraphicsPolygonItem::Duplicate()
{
	return 0;
}

QRectF GraphicsPolygonItem::boundingRect() const
{
	return Shape().controlPointRect();
}

QPainterPath GraphicsPolygonItem::Shape() const
{
	QPainterPath path;

	path.addPolygon(m_points);
	path.closeSubpath();

	return qt_graphicsItem_shapeFromPath(path, GetPen());
}

void GraphicsPolygonItem::AddPoint(const QPointF &point)
{

}

void GraphicsPolygonItem::EndPoint(const QPointF &point)
{

}

void GraphicsPolygonItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{

}

///////////////////////// GraphicsLineItem /////////////////////////
GraphicsLineItem::GraphicsLineItem(QGraphicsItem *parent)
	:GraphicsPolygonItem(parent)
{
	m_handles.reserve(Handle_Left);
	for (Handles::iterator it = m_handles.begin(); it != m_handles.end(); ++it)
		delete (*it);
	m_handles.clear();
}

GraphicsLineItem::~GraphicsLineItem()
{

}

void GraphicsLineItem::Move(const QPointF &point)
{

}

void GraphicsLineItem::Control(int direct, const QPointF &delta)
{
	QPointF pt = mapFromScene(delta);
	if (direct <= Handle_Left)
		return ;

	m_points[direct - Handle_Left - 1] = pt;
	prepareGeometryChange();
	m_localRect = m_points.boundingRect();
	m_width = m_localRect.width();
	m_height = m_localRect.height();
	m_initialPoints = m_points;
}

void GraphicsLineItem::Stretch(int handle, double sx, double sy, const QPointF &origin)
{

}

bool GraphicsLineItem::SaveToXml(QXmlStreamWriter *xml)
{
	return true;
}

bool GraphicsLineItem::LoadFromXml(QXmlStreamReader *xml)
{
	return true;
}

void GraphicsLineItem::UpdateHandles()
{
	for (int i = 0; i < m_points.size(); ++i)
	{
		m_handles[i]->Move(m_points[i].x() ,m_points[i].y() );
	}
}

void GraphicsLineItem::UpdateCoordinate()
{

}

QGraphicsItem* GraphicsLineItem::Duplicate()
{
	return 0;
}

QPointF GraphicsLineItem::Opposite(int handle)
{
	QPointF pt;

	return pt;
}

QPainterPath GraphicsLineItem::Shape() const
{
	QPainterPath path;

	if (m_points.size() > 1)
	{
		path.moveTo(m_points.at(0));
		path.lineTo(m_points.at(1));
	}

	return qt_graphicsItem_shapeFromPath(path, GetPen());
}

void GraphicsLineItem::AddPoint(const QPointF &point)
{
	m_points.append(mapFromScene(point));

	int direct = m_points.count();
	SizeHandleRect *shr = new SizeHandleRect(this, direct + Handle_Left, direct == 1 ? false : true);
	shr->SetState(SelectionHandleActive);
	m_handles.push_back(shr);
}

void GraphicsLineItem::EndPoint(const QPointF &point)
{
	Q_UNUSED(point);

	int nPoints = m_points.count();
	if (nPoints > 2 && (m_points[nPoints-1] == m_points[nPoints-2] ||
		m_points[nPoints-1].x() - 1 == m_points[nPoints-2].x() &&
		m_points[nPoints-1].y() == m_points[nPoints-2].y()))
	{
		delete m_handles[nPoints-1];
		m_points.remove(nPoints-1);
		m_handles.resize(nPoints-1);
	}

	m_initialPoints = m_points;
}

void GraphicsLineItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
	Q_UNUSED(option);
	Q_UNUSED(widget);

	painter->setPen(GetPen());
	if (m_points.size() > 1)
		painter->drawLine(m_points.at(0),m_points.at(1));
}

