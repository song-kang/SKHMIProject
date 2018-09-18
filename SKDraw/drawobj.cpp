#include "drawobj.h"
#include "drawscene.h"
#include "drawview.h"
#include "skdraw.h"

///////////////////////// ShapeMimeData /////////////////////////
ShapeMimeData::ShapeMimeData(QList<QGraphicsItem *> items)
{
	foreach (QGraphicsItem *item, items)
	{
		AbstractShape *sp = qgraphicsitem_cast<AbstractShape*>(item);
		m_items.append(sp->Duplicate());
	}
}

ShapeMimeData::~ShapeMimeData()
{
	foreach (QGraphicsItem *item, m_items)
		delete item;
	m_items.clear();
}

QList<QGraphicsItem *> ShapeMimeData::items() const
{
	return m_items;
}

///////////////////////// GraphicsItem /////////////////////////
GraphicsItem::GraphicsItem(QGraphicsItem *parent)
    :AbstractShapeType<QGraphicsItem>(parent)
{
	setAcceptHoverEvents(true);
    setFlag(QGraphicsItem::ItemIsMovable, true);
    setFlag(QGraphicsItem::ItemIsSelectable, true);
    setFlag(QGraphicsItem::ItemSendsGeometryChanges, true);

	m_handles.reserve(eHandleLeft);
	for (int i = eHandleLeftTop; i <= eHandleLeft; i++)
	{
		SizeHandleRect *shr = new SizeHandleRect(this, i);
		m_handles.push_back(shr);
	}

	m_pScene = NULL;
}

GraphicsItem::~GraphicsItem()
{

}

void GraphicsItem::Image(QPainter *painter, QPointF point)
{
	QPointF p = pos() - point;
	painter->translate(p.x(),p.y());
	paint(painter,NULL);
	painter->translate(-p.x(),-p.y());
}

void GraphicsItem::UpdateHandles()
{
	const QRectF geom = this->boundingRect();
	for (Handles::iterator it = m_handles.begin(); it != m_handles.end(); it++)
	{
		SizeHandleRect *handle = *it;
		switch (handle->GetDirect()) 
		{
		case eHandleLeftTop:
			handle->Move(geom.x(), geom.y());
			break;
		case eHandleTop:
			handle->Move(geom.x() + geom.width() / 2, geom.y());
			break;
		case eHandleRightTop:
			handle->Move(geom.x() + geom.width(), geom.y());
			break;
		case eHandleRight:
			handle->Move(geom.x() + geom.width(), geom.y() + geom.height() / 2);
			break;
		case eHandleRightBottom:
			handle->Move(geom.x() + geom.width(), geom.y() + geom.height());
			break;
		case eHandleBottom:
			handle->Move(geom.x() + geom.width() / 2, geom.y() + geom.height());
			break;
		case eHandleLeftBottom:
			handle->Move(geom.x(), geom.y() + geom.height());
			break;
		case eHandleLeft:
			handle->Move(geom.x(), geom.y() + geom.height() / 2);
			break;
		default:
			break;
		}
	}
}

void GraphicsItem::contextMenuEvent(QGraphicsSceneContextMenuEvent *event)
{
	Q_UNUSED(event);
}

void GraphicsItem::hoverEnterEvent(QGraphicsSceneHoverEvent *event)
{
	QGraphicsItem::hoverEnterEvent(event);
}

void GraphicsItem::hoverLeaveEvent(QGraphicsSceneHoverEvent *event)
{
	m_pScene->GetView()->setCursor(Qt::ArrowCursor);

	QGraphicsItem::hoverLeaveEvent(event);
}

void GraphicsItem::hoverMoveEvent(QGraphicsSceneHoverEvent *event)
{
	m_pScene->GetView()->setCursor(Qt::PointingHandCursor);

	QGraphicsItem::hoverMoveEvent(event);
}

QVariant GraphicsItem::itemChange(QGraphicsItem::GraphicsItemChange change, const QVariant &value)
{
	if (change == QGraphicsItem::ItemSelectedHasChanged)
	{
		QGraphicsItemGroup *g = dynamic_cast<QGraphicsItemGroup*>(parentItem());
		if (!g)
		{
			SetState(value.toBool() ? eSelectionHandleActive : eSelectionHandleOff);
		}
		else
		{
			setSelected(false);
			return QVariant::fromValue<bool>(false);
		}
	}

    return QGraphicsItem::itemChange(change, value);
}

bool GraphicsItem::ReadBaseAttributes(QXmlStreamReader *xml)
{
	qreal x = xml->attributes().value(tr("x")).toString().toDouble();
	qreal y = xml->attributes().value(tr("y")).toString().toDouble();
	m_width = xml->attributes().value("width").toString().toDouble();
	m_height = xml->attributes().value("height").toString().toDouble();
	setZValue(xml->attributes().value("z").toString().toDouble());
	setRotation(xml->attributes().value("rotate").toString().toDouble());
	setScale(xml->attributes().value("scale").toString().toDouble());
	setToolTip(xml->attributes().value("tooltip").toString());
	setPos(x,y);

	QColor color;
	color.setNamedColor(xml->attributes().value("penColor").toString());
	color.setAlpha(xml->attributes().value("penAlpha").toString().toInt());
	m_pen.setColor(color);
	m_pen.setWidthF(xml->attributes().value("penWidth").toString().toDouble());
	m_pen.setStyle((Qt::PenStyle)xml->attributes().value("penStyle").toString().toInt());

	color.setNamedColor(xml->attributes().value("brushColor").toString());
	color.setAlpha(xml->attributes().value("brushAlpha").toString().toInt());
	m_brush.setColor(color);
	m_brush.setStyle((Qt::BrushStyle)xml->attributes().value("brushStyle").toString().toInt());

	m_iShowType = xml->attributes().value(tr("showtype")).toString().toInt();
	m_iShowState = xml->attributes().value(tr("showst")).toString().toInt();
	m_sLinkDB = xml->attributes().value(tr("linkdb")).toString();
	m_sLinkScene = xml->attributes().value(tr("linkscene")).toString();

	QString strval = xml->attributes().value(tr("showstyle")).toString();
	if (!strval.isEmpty())
	{
		int index = 0;
		QStringList list = strval.split(";");
		foreach (QString s, list)
			m_mapShowStyle.insert(index++, s);
	}
	
	return true;
}

bool GraphicsItem::WriteBaseAttributes(QXmlStreamWriter *xml)
{
	xml->writeAttribute(tr("x"),QString("%1").arg(pos().x()));
	xml->writeAttribute(tr("y"),QString("%1").arg(pos().y()));
	xml->writeAttribute(tr("z"),QString("%1").arg(zValue()));
	xml->writeAttribute(tr("width"),QString("%1").arg(m_width));
	xml->writeAttribute(tr("height"),QString("%1").arg(m_height));
	xml->writeAttribute(tr("rotate"),QString("%1").arg(rotation()));
	xml->writeAttribute(tr("scale"),QString("%1").arg(scale()));
	xml->writeAttribute(tr("tooltip"),QString("%1").arg(toolTip()));
	xml->writeAttribute(tr("penColor"),QString("%1").arg(GetPen().color().name()));
	xml->writeAttribute(tr("penAlpha"),QString("%1").arg(GetPen().color().alpha()));
	xml->writeAttribute(tr("penWidth"),QString("%1").arg(GetPen().widthF()));
	xml->writeAttribute(tr("penStyle"),QString("%1").arg(GetPen().style()));
	xml->writeAttribute(tr("brushColor"),QString("%1").arg(GetBrush().color().name()));
	xml->writeAttribute(tr("brushAlpha"),QString("%1").arg(GetBrush().color().alpha()));
	xml->writeAttribute(tr("brushStyle"),QString("%1").arg(GetBrush().style()));
	xml->writeAttribute(tr("showtype"),QString("%1").arg(GetShowType()));
	xml->writeAttribute(tr("showst"),QString("%1").arg(GetShowState()));
	xml->writeAttribute(tr("linkdb"),QString("%1").arg(GetLinkDB()));
	xml->writeAttribute(tr("linkscene"),QString("%1").arg(GetLinkScene()));

	QString s;
	QMap<int,QString>::const_iterator iter;
	for (iter = m_mapShowStyle.constBegin(); iter != m_mapShowStyle.constEnd(); iter++)
		s += iter.value() + ";";
	s = s.left(s.length()-1);
	xml->writeAttribute(tr("showstyle"),s);

	return true;
}

///////////////////////// GraphicsPolygonItem /////////////////////////
GraphicsPolygonItem::GraphicsPolygonItem(QGraphicsItem *parent)
	:GraphicsItem(parent)
{
	SetName("多边形图元");
}

GraphicsPolygonItem::~GraphicsPolygonItem()
{

}

void GraphicsPolygonItem::Move(const QPointF &point)
{

}

void GraphicsPolygonItem::Control(int direct, const QPointF &delta)
{
	QPointF pt = mapFromScene(delta);
	if (direct <= eHandleLeft)
		return;

	prepareGeometryChange();

	m_points[direct - eHandleLeft - 1] = pt;
	m_localRect = m_points.boundingRect();
	m_width = m_localRect.width();
	m_height = m_localRect.height();
	m_initialPoints = m_points;

	UpdateHandles();
}

void GraphicsPolygonItem::Stretch(int handle, double sx, double sy, const QPointF &origin)
{
	QTransform trans;
	switch (handle)
	{
	case eHandleRight:
	case eHandleLeft:
		sy = 1;
		break;
	case eHandleTop:
	case eHandleBottom:
		sx = 1;
		break;
	default:
		break;
	}
	trans.translate(origin.x(), origin.y());
	trans.scale(sx,sy);
	trans.translate(-origin.x(), -origin.y());

	prepareGeometryChange();
	m_points = trans.map(m_initialPoints);
	m_localRect = m_points.boundingRect();
	m_width = m_localRect.width();
	m_height = m_localRect.height();

	UpdateHandles();
}

void GraphicsPolygonItem::UpdateHandles()
{
	GraphicsItem::UpdateHandles();

	for (int i = 0; i < m_points.size(); i++)
		m_handles[eHandleLeft + i]->Move(m_points[i].x(), m_points[i].y());
}

void GraphicsPolygonItem::UpdateCoordinate()
{
	QPointF pt1,pt2,delta;
	QPolygonF pts;
	if (GetName() == "线段图元")
	{
		pt1 = m_points.at(0);
		if (m_points.at(1).x() > m_points.at(0).x() && m_points.at(1).y() > m_points.at(0).y() ||
			m_points.at(1).x() > m_points.at(0).x() && m_points.at(1).y() == m_points.at(0).y() ||
			m_points.at(1).x() == m_points.at(0).x() && m_points.at(1).y() > m_points.at(0).y())
			pt2 = m_points.at(0) + QPointF(m_width,m_height);
		else if (m_points.at(1).x() < m_points.at(0).x()  && m_points.at(1).y() < m_points.at(0).y() ||
				 m_points.at(1).x() < m_points.at(0).x()  && m_points.at(1).y() == m_points.at(0).y() ||
				 m_points.at(1).x() == m_points.at(0).x() && m_points.at(1).y() < m_points.at(0).y()) 
			pt2 = m_points.at(0) - QPointF(m_width,m_height);
		else if (m_points.at(1).x() > m_points.at(0).x() && m_points.at(1).y() < m_points.at(0).y())
			pt2 = QPointF(m_points.at(0).x() + m_width, m_points.at(0).y() - m_height);
		else if (m_points.at(1).x() < m_points.at(0).x()  && m_points.at(1).y() > m_points.at(0).y())
			pt2 = QPointF(m_points.at(0).x() - m_width, m_points.at(0).y() + m_height);
		
		m_points.clear();
		m_points.append(pt1);
		m_points.append(pt2);
		pts = mapToScene(m_points);
	}
	else
		pts = mapToScene(m_points);

	if (parentItem() == NULL)
	{
		pt1 = mapToScene(transformOriginPoint());
		pt2 = mapToScene(boundingRect().center());
		delta = pt1 - pt2;

		for (int i = 0; i < pts.count(); i++)
			pts[i] += delta;

		prepareGeometryChange();
		m_points = mapFromScene(pts);
		m_localRect = m_points.boundingRect();
		m_width = m_localRect.width();
		m_height = m_localRect.height();

		setTransform(transform().translate(delta.x(), delta.y()));
		moveBy(-delta.x(), -delta.y());
		setTransform(transform().translate(-delta.x(), -delta.y()));
		UpdateHandles();
	}

	m_initialPoints = m_points;
}

QGraphicsItem* GraphicsPolygonItem::Duplicate()
{
	GraphicsPolygonItem *item = new GraphicsPolygonItem();

	item->m_width = GetWidth();
	item->m_height = GetHeight();
	item->m_points = m_points;
	for ( int i = 0 ; i < m_points.size() ; ++i )
		item->m_handles.push_back(new SizeHandleRect(item, eHandleLeft + i + 1, true));

	item->SetScene(GetScene());
	item->setPos(pos().x(),pos().y());
	item->SetPen(GetPen());
	item->SetBrush(GetBrush());
	item->setTransform(transform());
	item->setTransformOriginPoint(transformOriginPoint());
	item->setRotation(rotation());
	item->setScale(scale());
	item->setZValue(zValue()+0.1);
	item->SetName(GetName());
	item->UpdateCoordinate();

	return item;
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

	return path;
}

void GraphicsPolygonItem::AddPoint(const QPointF &point)
{
	m_points.append(mapFromScene(point));

	int direct = m_points.count();
	SizeHandleRect *shr = new SizeHandleRect(this, direct + eHandleLeft, true);
	m_handles.push_back(shr);
}

void GraphicsPolygonItem::EndPoint(const QPointF &point)
{
	Q_UNUSED(point);

	int nPoints = m_points.count();
	if (nPoints > 2 && (m_points[nPoints-1] == m_points[nPoints-2] ||
		m_points[nPoints-1].x() == m_points[nPoints-2].x() ||
		m_points[nPoints-1].y() == m_points[nPoints-2].y()))
	{
		delete m_handles[eHandleLeft+nPoints-1];
		m_points.remove(nPoints-1);
		m_handles.resize(eHandleLeft+nPoints-1);
	}

	m_initialPoints = m_points;
}

void GraphicsPolygonItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
	Q_UNUSED(option);
	Q_UNUSED(widget);

	painter->setPen(GetPen());
	painter->setBrush(GetBrush());
	painter->drawPolygon(m_points);

	if (option && (option->state & QStyle::State_Selected))
		DrawOutline(painter);
}

bool GraphicsPolygonItem::SaveToXml(QXmlStreamWriter *xml)
{
	xml->writeStartElement("polygon");
	WriteBaseAttributes(xml);
	for (int i = 0 ; i < m_points.count(); i++)
	{
		xml->writeStartElement("point");
		xml->writeAttribute("x",QString("%1").arg(m_points[i].x()));
		xml->writeAttribute("y",QString("%1").arg(m_points[i].y()));
		xml->writeEndElement();
	}

	xml->writeEndElement();
	return true;
}

bool GraphicsPolygonItem::LoadFromXml(QXmlStreamReader *xml)
{
	ReadBaseAttributes(xml);
	while(xml->readNextStartElement())
	{
		if (xml->name() == "point")
		{
			qreal x = xml->attributes().value("x").toString().toDouble();
			qreal y = xml->attributes().value("y").toString().toDouble();
			m_points.append(QPointF(x,y));
			int dir = m_points.count();
			SizeHandleRect *shr = new SizeHandleRect(this, dir + eHandleLeft, true);
			m_handles.push_back(shr);
			xml->skipCurrentElement();
		}
		else
			xml->skipCurrentElement();
	}

	UpdateCoordinate();
	return true;
}

///////////////////////// GraphicsLineItem /////////////////////////
GraphicsLineItem::GraphicsLineItem(QGraphicsItem *parent)
	:GraphicsPolygonItem(parent)
{
	for (Handles::iterator it = m_handles.begin(); it != m_handles.end(); ++it)
		delete (*it);
	m_handles.clear();

	SetName("线段图元");
}

GraphicsLineItem::~GraphicsLineItem()
{

}

void GraphicsLineItem::Control(int direct, const QPointF &delta)
{
	QPointF pt = mapFromScene(delta);
	if (direct <= eHandleLeft)
		return;

	prepareGeometryChange();
	if (GetScene()->GetPressShift() == false)
	{
		if (direct == eHandleLeft + 1)
		{
			if (abs(pt.x() - m_points.at(1).x()) > abs(pt.y() - m_points.at(1).y()))
				pt.setY(m_points.at(1).y());
			else
				pt.setX(m_points.at(1).x());
		}
		else if (direct == eHandleLeft + 2)
		{
			if (abs(pt.x() - m_points.at(0).x()) > abs(pt.y() - m_points.at(0).y()))
				pt.setY(m_points.at(0).y());
			else
				pt.setX(m_points.at(0).x());
		}
	}

	m_points[direct - eHandleLeft - 1] = pt;
	m_localRect = m_points.boundingRect();
	m_width = m_localRect.width();
	m_height = m_localRect.height();
	m_initialPoints = m_points;

	UpdateHandles();
}

void GraphicsLineItem::UpdateHandles()
{
	for (int i = 0; i < m_points.size(); i++)
		m_handles[i]->Move(m_points[i].x() ,m_points[i].y());
}

QGraphicsItem* GraphicsLineItem::Duplicate()
{
	GraphicsLineItem *item = new GraphicsLineItem();

	item->m_width = GetWidth();
	item->m_height = GetHeight();
	item->m_points = m_points;
	item->m_initialPoints = m_initialPoints;
	for (int i = 0; i < m_points.size(); i++)
		item->m_handles.push_back(new SizeHandleRect(item, eHandleLeft+i+1, true));

	item->SetScene(GetScene());
	item->setPos(pos().x(), pos().y());
	item->SetPen(GetPen());
	item->SetBrush(GetBrush());
	item->setTransform(transform());
	item->setTransformOriginPoint(transformOriginPoint());
	item->setRotation(rotation());
	item->setScale(scale());
	item->setZValue(zValue()+0.1);
	item->SetName(GetName());
	item->UpdateHandles();

	return item;
}

QPainterPath GraphicsLineItem::Shape() const
{
	QPainterPath path;

	if (m_points.size() > 1)
	{
		path.moveTo(m_points.at(0));
		path.lineTo(m_points.at(1));
	}

	return path;
}

void GraphicsLineItem::AddPoint(const QPointF &point)
{
	m_points.append(mapFromScene(point));

	int direct = m_points.count();
	SizeHandleRect *shr = new SizeHandleRect(this, direct + eHandleLeft, true);
	m_handles.push_back(shr);
}

void GraphicsLineItem::EndPoint(const QPointF &point)
{
	Q_UNUSED(point);

	int nPoints = m_points.count();
	if (nPoints > 2 && (m_points[nPoints-1] == m_points[nPoints-2] ||
		m_points[nPoints-1].x() == m_points[nPoints-2].x() &&
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
	painter->setBrush(Qt::NoBrush);
	if (m_points.size() > 1)
		painter->drawLine(m_points.at(0),m_points.at(1));
}

bool GraphicsLineItem::SaveToXml(QXmlStreamWriter *xml)
{
	xml->writeStartElement("line");
	WriteBaseAttributes(xml);
	for (int i = 0 ; i < m_points.count(); i++)
	{
		xml->writeStartElement("point");
		xml->writeAttribute("x",QString("%1").arg(m_points[i].x()));
		xml->writeAttribute("y",QString("%1").arg(m_points[i].y()));
		xml->writeEndElement();
	}

	xml->writeEndElement();
	return true;
}

bool GraphicsLineItem::LoadFromXml(QXmlStreamReader *xml)
{
	ReadBaseAttributes(xml);
	while(xml->readNextStartElement())
	{
		if (xml->name()=="point")
		{
			qreal x = xml->attributes().value("x").toString().toDouble();
			qreal y = xml->attributes().value("y").toString().toDouble();
			m_points.append(QPointF(x,y));
			int dir = m_points.count();
			SizeHandleRect *shr = new SizeHandleRect(this, dir + eHandleLeft, true);
			m_handles.push_back(shr);
			xml->skipCurrentElement();
		}
		else
			xml->skipCurrentElement();
	}

	UpdateHandles();
	return true;
}

///////////////////////// GraphicsPolygonLineItem /////////////////////////
GraphicsPolygonLineItem::GraphicsPolygonLineItem(QGraphicsItem *parent)
	:GraphicsPolygonItem(parent)
{
	SetName("折线图元");
}

GraphicsPolygonLineItem::~GraphicsPolygonLineItem()
{

}

void GraphicsPolygonLineItem::Control(int direct, const QPointF &delta)
{
	QPointF pt = mapFromScene(delta);
	if (direct <= eHandleLeft)
		return;

	prepareGeometryChange();
	if (GetScene()->GetPressShift() == false)
	{
		if (direct == eHandleLeft + 1)
		{
			if (abs(pt.x() - m_points.at(1).x()) > abs(pt.y() - m_points.at(1).y()))
				pt.setY(m_points.at(1).y());
			else
				pt.setX(m_points.at(1).x());
		}
		else if (direct > eHandleLeft + 1)
		{
			if (abs(pt.x() - m_points.at(direct - 10).x()) > abs(pt.y() - m_points.at(direct - 10).y()))
				pt.setY(m_points.at(direct - 10).y());
			else
				pt.setX(m_points.at(direct - 10).x());
		}
	}

	m_points[direct - eHandleLeft - 1] = pt;
	m_localRect = m_points.boundingRect();
	m_width = m_localRect.width();
	m_height = m_localRect.height();
	m_initialPoints = m_points;

	UpdateHandles();
}

QGraphicsItem* GraphicsPolygonLineItem::Duplicate()
{
	GraphicsPolygonLineItem *item = new GraphicsPolygonLineItem();

	item->m_width = GetWidth();
	item->m_height = GetHeight();
	item->m_points = m_points;
	for (int i = 0; i < m_points.size(); i++)
		item->m_handles.push_back(new SizeHandleRect(item, eHandleLeft + i + 1, true));

	item->SetScene(GetScene());
	item->setPos(pos().x(), pos().y());
	item->SetPen(GetPen());
	item->SetBrush(GetBrush());
	item->setTransform(transform());
	item->setTransformOriginPoint(transformOriginPoint());
	item->setRotation(rotation());
	item->setScale(scale());
	item->setZValue(zValue()+0.1);
	item->SetName(GetName());
	item->UpdateCoordinate();
	item->UpdateHandles();

	return item;
}

QPainterPath GraphicsPolygonLineItem::Shape() const
{
	QPainterPath path;
	path.moveTo(m_points.at(0));

	int i = 1;
	while (i < m_points.size())
	{
		path.lineTo(m_points.at(i));
		i++;
	}

	return path;
}

void GraphicsPolygonLineItem::EndPoint(const QPointF &point)
{
	Q_UNUSED(point);

	int nPoints = m_points.count();
	if (nPoints > 2 && (m_points[nPoints-1] == m_points[nPoints-2] ||
		m_points[nPoints-1].x() == m_points[nPoints-2].x() ||
		m_points[nPoints-1].y() == m_points[nPoints-2].y()))
	{
		delete m_handles[eHandleLeft+nPoints-1];
		m_points.remove(nPoints-1);
		m_handles.resize(eHandleLeft+nPoints-1);
	}

	m_initialPoints = m_points;
}

void GraphicsPolygonLineItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
	Q_UNUSED(option);
	Q_UNUSED(widget);

	QPainterPath path;
	painter->setPen(GetPen());
	painter->setBrush(Qt::NoBrush);
	path.moveTo(m_points.at(0));

	int i = 1;
	while (i < m_points.size())
	{
		path.lineTo(m_points.at(i));
		i++;
	}

	painter->drawPath(path);
}

bool GraphicsPolygonLineItem::SaveToXml(QXmlStreamWriter *xml)
{
	xml->writeStartElement("polyline");
	WriteBaseAttributes(xml);
	for (int i = 0 ; i < m_points.count(); i++)
	{
		xml->writeStartElement("point");
		xml->writeAttribute("x",QString("%1").arg(m_points[i].x()));
		xml->writeAttribute("y",QString("%1").arg(m_points[i].y()));
		xml->writeEndElement();
	}

	xml->writeEndElement();
	return true;
}

bool GraphicsPolygonLineItem::LoadFromXml(QXmlStreamReader *xml)
{
	ReadBaseAttributes(xml);
	while(xml->readNextStartElement())
	{
		if (xml->name() == "point")
		{
			qreal x = xml->attributes().value("x").toString().toDouble();
			qreal y = xml->attributes().value("y").toString().toDouble();
			m_points.append(QPointF(x,y));
			int dir = m_points.count();
			SizeHandleRect *shr = new SizeHandleRect(this, dir + eHandleLeft, true);
			m_handles.push_back(shr);
			xml->skipCurrentElement();
		}
		else
			xml->skipCurrentElement();
	}

	UpdateCoordinate();
	return true;
}

///////////////////////// GraphicsRectItem /////////////////////////
GraphicsRectItem::GraphicsRectItem(const QRect &rect, bool isRound, QGraphicsItem *parent)
	:GraphicsItem(parent)
	,m_isRound(isRound)
{
	m_round.setWidth(5);
	m_round.setHeight(5);
	m_localRect = rect;
	m_initialRect = rect;
	m_width = rect.width();
	m_height = rect.height();
	m_originPoint = QPointF(0,0);

	if (isRound)
		SetName("圆角矩形图元");
	else
		SetName("矩形图元");
}

GraphicsRectItem::~GraphicsRectItem()
{

}

void GraphicsRectItem::Move(const QPointF & point)
{

}

void GraphicsRectItem::Control(int dir, const QPointF & delta)
{

}

void GraphicsRectItem::Stretch(int handle, double sx, double sy, const QPointF &origin)
{
	QTransform trans;
	switch (handle)
	{
	case eHandleRight:
	case eHandleLeft:
		sy = 1;
		break;
	case eHandleTop:
	case eHandleBottom:
		sx = 1;
		break;
	default:
		break;
	}

	m_opposite = origin;
	trans.translate(origin.x(), origin.y());
	trans.scale(sx, sy);
	trans.translate(-origin.x(), -origin.y());

	prepareGeometryChange();
	m_localRect = trans.mapRect(m_initialRect);
	m_width = m_localRect.width();
	m_height = m_localRect.height();

	UpdateHandles();
}

void GraphicsRectItem::UpdateCoordinate()
{
	QPointF pt1,pt2,delta;
	pt1 = mapToScene(transformOriginPoint());
	pt2 = mapToScene(m_localRect.center());
	delta = pt1 - pt2;

	if (!parentItem())
	{
		prepareGeometryChange();
		m_localRect = QRectF(-m_width/2, -m_height/2, m_width,m_height);
		m_width = m_localRect.width();
		m_height = m_localRect.height();
		setTransform(transform().translate(delta.x(), delta.y()));
		setTransformOriginPoint(m_localRect.center());
		moveBy(-delta.x(), -delta.y());
		setTransform(transform().translate(-delta.x(), -delta.y()));
		m_opposite = QPointF(0,0);
		UpdateHandles();
	}

	m_initialRect = m_localRect;
}

QGraphicsItem* GraphicsRectItem::Duplicate()
{
	GraphicsRectItem * item = new GraphicsRectItem(m_localRect.toRect(), m_isRound);

	item->m_width = GetWidth();
	item->m_height = GetHeight();
	item->SetScene(GetScene());
	item->setPos(pos().x(), pos().y());
	item->SetPen(GetPen());
	item->SetBrush(GetBrush());
	item->setTransform(transform());
	item->setTransformOriginPoint(transformOriginPoint());
	item->setRotation(rotation());
	item->setScale(scale());
	item->setZValue(zValue()+0.1);
	item->SetName(GetName());
	item->UpdateCoordinate();

	return item;
}

QRectF GraphicsRectItem::boundingRect() const
{
	return m_localRect;
}

QPainterPath GraphicsRectItem::Shape() const
{
	QPainterPath path;

	if (m_isRound)
		path.addRoundedRect(m_localRect,m_round.width(),m_round.height());
	else
		path.addRect(m_localRect);

	return path;
}

void GraphicsRectItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
	painter->setPen(GetPen());
	painter->setBrush(GetBrush());

	if (m_isRound)
		painter->drawRoundedRect(m_localRect,m_round.width(),m_round.height());
	else
		painter->drawRect(m_localRect);

	if (option && (option->state & QStyle::State_Selected))
		DrawOutline(painter);
}

bool GraphicsRectItem::SaveToXml(QXmlStreamWriter *xml)
{
	if (m_isRound)
	{
		xml->writeStartElement(tr("roundrect"));
		xml->writeAttribute(tr("rx"),QString("%1").arg(m_round.width()));
		xml->writeAttribute(tr("ry"),QString("%1").arg(m_round.height()));
	}
	else
		xml->writeStartElement(tr("rect"));

	WriteBaseAttributes(xml);
	xml->writeEndElement();
	return true;
}

bool GraphicsRectItem::LoadFromXml(QXmlStreamReader *xml)
{
	m_isRound = (xml->name() == tr("roundrect"));
	if ( m_isRound )
		m_round = QSize(xml->attributes().value(tr("rx")).toString().toDouble(), xml->attributes().value(tr("ry")).toString().toDouble());

	ReadBaseAttributes(xml);
	UpdateCoordinate();
	xml->skipCurrentElement();

	return true;
}

///////////////////////// GraphicsTriangleItem /////////////////////////
GraphicsTriangleItem::GraphicsTriangleItem(const QRect &rect, GraphicsRectItem *parent)
	:GraphicsRectItem(rect, parent)
{
	SetName("三角形图元");
}

GraphicsTriangleItem::~GraphicsTriangleItem()
{

}

QGraphicsItem* GraphicsTriangleItem::Duplicate()
{
	GraphicsTriangleItem * item = new GraphicsTriangleItem(m_localRect.toRect());

	item->m_width = GetWidth();
	item->m_height = GetHeight();
	item->SetScene(GetScene());
	item->setPos(pos().x(), pos().y());
	item->SetPen(GetPen());
	item->SetBrush(GetBrush());
	item->setTransform(transform());
	item->setTransformOriginPoint(transformOriginPoint());
	item->setRotation(rotation());
	item->setScale(scale());
	item->setZValue(zValue()+0.1);
	item->SetName(GetName());
	item->UpdateCoordinate();

	return item;
}

void GraphicsTriangleItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
	painter->setPen(GetPen());
	painter->setBrush(GetBrush());

	m_points.clear();
	m_points.append(QPointF(m_localRect.x()+m_localRect.width()/2,m_localRect.y()));
	m_points.append(QPointF(m_localRect.x(),m_localRect.y()+m_localRect.height()));
	m_points.append(QPointF(m_localRect.x()+m_localRect.width(),m_localRect.y()+m_localRect.height()));
	painter->drawPolygon(m_points);

	if (option && (option->state & QStyle::State_Selected))
		DrawOutline(painter);
}

bool GraphicsTriangleItem::SaveToXml(QXmlStreamWriter *xml)
{
	xml->writeStartElement(tr("triangle"));
	WriteBaseAttributes(xml);
	xml->writeEndElement();
	return true;
}

bool GraphicsTriangleItem::LoadFromXml(QXmlStreamReader *xml)
{
	ReadBaseAttributes(xml);
	UpdateCoordinate();
	xml->skipCurrentElement();
	return true;
}

///////////////////////// GraphicsRhombusItem /////////////////////////
GraphicsRhombusItem::GraphicsRhombusItem(const QRect &rect, GraphicsRectItem *parent)
	:GraphicsRectItem(rect, parent)
{
	SetName("菱形图元");
}

GraphicsRhombusItem::~GraphicsRhombusItem()
{

}

QGraphicsItem* GraphicsRhombusItem::Duplicate()
{
	GraphicsRhombusItem * item = new GraphicsRhombusItem(m_localRect.toRect());

	item->m_width = GetWidth();
	item->m_height = GetHeight();
	item->SetScene(GetScene());
	item->setPos(pos().x(), pos().y());
	item->SetPen(GetPen());
	item->SetBrush(GetBrush());
	item->setTransform(transform());
	item->setTransformOriginPoint(transformOriginPoint());
	item->setRotation(rotation());
	item->setScale(scale());
	item->setZValue(zValue()+0.1);
	item->SetName(GetName());
	item->UpdateCoordinate();

	return item;
}

void GraphicsRhombusItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
	painter->setPen(GetPen());
	painter->setBrush(GetBrush());

	m_points.clear();
	m_points.append(QPointF(m_localRect.x()+m_localRect.width()/2,m_localRect.y()));
	m_points.append(QPointF(m_localRect.x(),m_localRect.y()+m_localRect.height()/2));
	m_points.append(QPointF(m_localRect.x()+m_localRect.width()/2,m_localRect.y()+m_localRect.height()));
	m_points.append(QPointF(m_localRect.x()+m_localRect.width(),m_localRect.y()+m_localRect.height()/2));
	painter->drawPolygon(m_points);

	if (option && (option->state & QStyle::State_Selected))
		DrawOutline(painter);
}

bool GraphicsRhombusItem::SaveToXml(QXmlStreamWriter *xml)
{
	xml->writeStartElement(tr("rhombus"));
	WriteBaseAttributes(xml);
	xml->writeEndElement();
	return true;
}

bool GraphicsRhombusItem::LoadFromXml(QXmlStreamReader *xml)
{
	ReadBaseAttributes(xml);
	UpdateCoordinate();
	xml->skipCurrentElement();
	return true;
}

///////////////////////// GraphicsEllipseItem /////////////////////////
GraphicsEllipseItem::GraphicsEllipseItem(const QRect &rect, bool isCircle, QGraphicsItem *parent)
	:GraphicsRectItem(rect, parent)
	,m_isCircle(isCircle)
{
	if (isCircle)
		SetName("圆形图元");
	else
		SetName("椭圆形图元");
}

GraphicsEllipseItem::~GraphicsEllipseItem()
{

}

void GraphicsEllipseItem::Stretch(int handle, double sx, double sy, const QPointF &origin)
{
	QTransform trans;
	switch (handle)
	{
	case eHandleRight:
	case eHandleLeft:
		sy = 1;
		break;
	case eHandleTop:
	case eHandleBottom:
		sx = 1;
		break;
	default:
		break;
	}

	m_opposite = origin;
	trans.translate(origin.x(), origin.y());
	trans.scale(sx, sy);
	trans.translate(-origin.x(), -origin.y());

	prepareGeometryChange();
	m_localRect = trans.mapRect(m_initialRect);
	if (m_isCircle)
	{
		if (m_localRect.width() > m_localRect.height())
			m_localRect.setHeight(m_localRect.width());
		else
			m_localRect.setWidth(m_localRect.height());
	}
	m_width = m_localRect.width();
	m_height = m_localRect.height();

	UpdateHandles();
}

void GraphicsEllipseItem::UpdateHandles()
{
	GraphicsItem::UpdateHandles();
}

QGraphicsItem* GraphicsEllipseItem::Duplicate()
{
	GraphicsEllipseItem * item = new GraphicsEllipseItem(m_localRect.toRect(), m_isCircle);

	item->m_width = GetWidth();
	item->m_height = GetHeight();
	item->SetScene(GetScene());
	item->setPos(pos().x(), pos().y());
	item->SetPen(GetPen());
	item->SetBrush(GetBrush());
	item->setTransform(transform());
	item->setTransformOriginPoint(transformOriginPoint());
	item->setRotation(rotation());
	item->setScale(scale());
	item->setZValue(zValue()+0.1);
	item->SetName(GetName());
	item->UpdateCoordinate();

	return item;
}

QRectF GraphicsEllipseItem::boundingRect() const
{
	return Shape().controlPointRect();
}

QPainterPath GraphicsEllipseItem::Shape() const
{
	QPainterPath path;

	path.addEllipse(m_localRect);
	path.closeSubpath();

	return path;
}

void GraphicsEllipseItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
	painter->setPen(GetPen());
	painter->setBrush(GetBrush());
	painter->drawEllipse(m_localRect);

	if (option && (option->state & QStyle::State_Selected))
		DrawOutline(painter);
}

bool GraphicsEllipseItem::SaveToXml(QXmlStreamWriter *xml)
{
	if (m_isCircle)
		xml->writeStartElement(tr("circle"));
	else
		xml->writeStartElement(tr("ellipse"));

	WriteBaseAttributes(xml);
	xml->writeEndElement();
	return true;
}

bool GraphicsEllipseItem::LoadFromXml(QXmlStreamReader *xml)
{
	ReadBaseAttributes(xml);
	xml->skipCurrentElement();

	UpdateCoordinate();
	return true;
}

///////////////////////// GraphicsTextItem /////////////////////////
GraphicsTextItem::GraphicsTextItem(const QRect &rect, QGraphicsItem *parent)
	:GraphicsRectItem(rect, parent)
{
	m_text = "文字";

	//m_font.setCapitalization(QFont::SmallCaps);			//设置字母大小写
	//m_font.setLetterSpacing(QFont::AbsoluteSpacing, 2);	//设置字符间距
	m_option.setAlignment(Qt::AlignCenter);
	//m_option.setWrapMode(QTextOption::NoWrap);
	m_option.setWrapMode(QTextOption::WordWrap);

	SetName("文字图元");
}

GraphicsTextItem::~GraphicsTextItem()
{

}

void GraphicsTextItem::Stretch(int handle, double sx, double sy, const QPointF &origin)
{
	QTransform trans;
	switch (handle)
	{
	case eHandleRight:
	case eHandleLeft:
		sy = 1;
		break;
	case eHandleTop:
	case eHandleBottom:
		sx = 1;
		break;
	default:
		break;
	}

	m_opposite = origin;
	trans.translate(origin.x(), origin.y());
	trans.scale(sx, sy);
	trans.translate(-origin.x(), -origin.y());

	prepareGeometryChange();
	m_localRect = trans.mapRect(m_initialRect);
	m_width = m_localRect.width();
	m_height = m_localRect.height();

	UpdateHandles();
}

QGraphicsItem* GraphicsTextItem::Duplicate()
{
	GraphicsTextItem * item = new GraphicsTextItem(m_localRect.toRect());

	item->m_width = GetWidth();
	item->m_height = GetHeight();
	item->SetScene(GetScene());
	item->setPos(pos().x(), pos().y());
	item->SetPen(GetPen());
	item->SetBrush(GetBrush());
	item->SetFont(GetFont());
	item->SetText(GetText());
	item->SetOption(GetOption());
	item->setTransform(transform());
	item->setTransformOriginPoint(transformOriginPoint());
	item->setRotation(rotation());
	item->setScale(scale());
	item->setZValue(zValue()+0.1);
	item->SetName(GetName());
	item->UpdateCoordinate();

	return item;
}

void GraphicsTextItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
	painter->setFont(m_font);
	painter->setPen(GetPen());
	painter->drawText(m_localRect, m_text, m_option);

	if (option && (option->state & QStyle::State_Selected))
		DrawOutline(painter);
}

bool GraphicsTextItem::SaveToXml(QXmlStreamWriter *xml)
{
	xml->writeStartElement("text");
	WriteBaseAttributes(xml);

	xml->writeAttribute(tr("desc"),QString("%1").arg(m_text));
	xml->writeAttribute(tr("family"),QString("%1").arg(m_font.family()));
	xml->writeAttribute(tr("pointSize"),QString("%1").arg(m_font.pointSize()));
	xml->writeAttribute(tr("bold"),QString("%1").arg(m_font.bold()));
	xml->writeAttribute(tr("italic"),QString("%1").arg(m_font.italic()));
	xml->writeAttribute(tr("underline"),QString("%1").arg(m_font.underline()));
	xml->writeAttribute(tr("strikeOut"),QString("%1").arg(m_font.strikeOut()));
	xml->writeAttribute(tr("kerning"),QString("%1").arg(m_font.kerning()));

	xml->writeEndElement();
	return true;
}

bool GraphicsTextItem::LoadFromXml(QXmlStreamReader *xml)
{
	ReadBaseAttributes(xml);
	m_text = xml->attributes().value(tr("desc")).toString();
	m_font.setFamily(xml->attributes().value(tr("family")).toString());
	m_font.setPointSize(xml->attributes().value(tr("pointSize")).toString().toInt());
	m_font.setBold(xml->attributes().value(tr("bold")).toString().toInt());
	m_font.setItalic(xml->attributes().value(tr("italic")).toString().toInt());
	m_font.setUnderline(xml->attributes().value(tr("underline")).toString().toInt());
	m_font.setStrikeOut(xml->attributes().value(tr("strikeOut")).toString().toInt());
	m_font.setKerning(xml->attributes().value(tr("kerning")).toString().toInt());
	
	xml->skipCurrentElement();
	UpdateCoordinate();
	return true;
}

///////////////////////// GraphicsPictureItem /////////////////////////
GraphicsPictureItem::GraphicsPictureItem(const QRect &rect, QString fileName, QGraphicsItem *parent)
	:GraphicsRectItem(rect, parent),
	m_fileName(fileName)
{
	m_picture.load(Common::GetCurrentAppPath() + "../picture/" + fileName);
	m_width = m_picture.width();
	m_height = m_picture.height();
	m_localRect.setWidth(m_width);
	m_localRect.setHeight(m_height);
	m_initialRect = m_localRect;

	SetName("图像图元");
}

GraphicsPictureItem::~GraphicsPictureItem()
{

}

void GraphicsPictureItem::Stretch(int handle, double sx, double sy, const QPointF &origin)
{
	QTransform trans;
	switch (handle)
	{
	case eHandleRight:
	case eHandleLeft:
		sy = 1;
		break;
	case eHandleTop:
	case eHandleBottom:
		sx = 1;
		break;
	default:
		break;
	}

	prepareGeometryChange();
	trans.scale(sx, sy);
	m_localRect = trans.mapRect(m_initialRect);
	m_width = m_localRect.width();
	m_height = m_localRect.height();

	UpdateHandles();
}

QGraphicsItem* GraphicsPictureItem::Duplicate()
{
	GraphicsPictureItem *item = new GraphicsPictureItem(m_localRect.toRect(), m_fileName);

	item->m_width = GetWidth();
	item->m_height = GetHeight();
	item->SetScene(GetScene());
	item->setPos(pos().x(), pos().y());
	item->SetPen(GetPen());
	item->SetBrush(GetBrush());
	item->setTransform(transform());
	item->setTransformOriginPoint(transformOriginPoint());
	item->setRotation(rotation());
	item->setScale(scale());
	item->setZValue(zValue()+0.1);
	item->SetName(GetName());
	item->UpdateCoordinate();

	return item;
}

void GraphicsPictureItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
	painter->drawPixmap(m_localRect.toRect(), m_picture);
}

bool GraphicsPictureItem::SaveToXml(QXmlStreamWriter *xml)
{
	xml->writeStartElement("picture");
	WriteBaseAttributes(xml);
	xml->writeAttribute(tr("name"),QString("%1").arg(m_fileName));
	xml->writeEndElement();
	return true;
}

bool GraphicsPictureItem::LoadFromXml(QXmlStreamReader *xml)
{
	ReadBaseAttributes(xml);
	m_fileName = xml->attributes().value(tr("name")).toString();
	if (!m_fileName.isEmpty())
	{
		if (Common::FileExists(Common::GetCurrentAppPath() + "../picture/" + m_fileName))
			m_picture.load(Common::GetCurrentAppPath() + "../picture/" + m_fileName);
		else
			m_picture.load(Common::GetCurrentAppPath() + "../picture/fileWarn.png");
	}

	xml->skipCurrentElement();
	UpdateCoordinate();
	return true;
}

///////////////////////// GraphicsItemGroup /////////////////////////
GraphicsItemGroup::GraphicsItemGroup(QGraphicsItem *parent)
	:AbstractShapeType <QGraphicsItemGroup>(parent),
	m_parent(parent)
{
#if 0	//组合图元关闭拉伸功能
	m_itemsBoundingRect = QRectF();
	m_handles.reserve(eHandleLeft);
	for (int i = eHandleLeftTop; i <= eHandleLeft; i++)
	{
		SizeHandleRect *shr = new SizeHandleRect(this, i);
		m_handles.push_back(shr);
	}
#endif

	setFlag(QGraphicsItem::ItemIsMovable, true);
	setFlag(QGraphicsItem::ItemIsSelectable, true);
	setFlag(QGraphicsItem::ItemSendsGeometryChanges, true);
	this->setAcceptHoverEvents(true);
}

GraphicsItemGroup::~GraphicsItemGroup()
{

}

QPixmap GraphicsItemGroup::Image()
{
	QPixmap pixmap(GetWidth(), GetHeight());
	pixmap.fill(Qt::transparent);
	QPainter painter(&pixmap);

	foreach (QGraphicsItem *shape, childItems())
	{
		AbstractShape *ab = qgraphicsitem_cast<AbstractShape*>(shape);
		if (ab && !qgraphicsitem_cast<SizeHandleRect*>(ab))
		{
			painter.translate(GetWidth()/2, GetHeight()/2);
			((GraphicsItem*)ab)->Image(&painter,pos());
			painter.translate(-GetWidth()/2, -GetHeight()/2);
		}
	}

	return pixmap;
}

void GraphicsItemGroup::Stretch( int handle, double sx, double sy, const QPointF &origin)
{
	QTransform trans;
	switch (handle) 
	{
	case eHandleRight:
	case eHandleLeft:
		sy = 1;
		break;
	case eHandleTop:
	case eHandleBottom:
		sx = 1;
		break;
	default:
		break;
	}

	foreach (QGraphicsItem *item, childItems())
	{
		AbstractShape * ab = qgraphicsitem_cast<AbstractShape*>(item);
		if (ab && !qgraphicsitem_cast<SizeHandleRect*>(ab))
			ab->Stretch(handle,sx,sy,ab->mapFromParent(origin));
	}

	trans.translate(origin.x(),origin.y());
	trans.scale(sx,sy);
	trans.translate(-origin.x(),-origin.y());

	prepareGeometryChange();
	m_itemsBoundingRect = trans.mapRect(m_initialRect);
	m_width = m_itemsBoundingRect.width();
	m_height = m_itemsBoundingRect.height();
	UpdateHandles();
}

void GraphicsItemGroup::UpdateCoordinate()
{
	QPointF pt1,pt2,delta;
	if (m_itemsBoundingRect.isNull())
		m_itemsBoundingRect = QGraphicsItemGroup::boundingRect();

	m_initialRect = m_itemsBoundingRect;
	m_width = m_itemsBoundingRect.width();
	m_height = m_itemsBoundingRect.height();
	
	pt1 = mapToScene(transformOriginPoint());
	pt2 = mapToScene(m_itemsBoundingRect.center());
	delta = pt1 - pt2;
	setTransform(transform().translate(delta.x(),delta.y()));
	setTransformOriginPoint(m_itemsBoundingRect.center());
	moveBy(-delta.x(), -delta.y());

	foreach (QGraphicsItem *item, childItems())
	{
		AbstractShape *ab = qgraphicsitem_cast<AbstractShape*>(item);
		if (ab && !qgraphicsitem_cast<SizeHandleRect*>(ab))
			ab->UpdateCoordinate();
	}

	UpdateHandles();
}

void GraphicsItemGroup::UpdateHandles()
{
	const QRectF geom = this->boundingRect();
	for (Handles::iterator it = m_handles.begin(); it != m_handles.end(); it++)
	{
		SizeHandleRect *handle = *it;
		switch (handle->GetDirect()) 
		{
		case eHandleLeftTop:
			handle->Move(geom.x(), geom.y());
			break;
		case eHandleTop:
			handle->Move(geom.x() + geom.width() / 2, geom.y());
			break;
		case eHandleRightTop:
			handle->Move(geom.x() + geom.width(), geom.y());
			break;
		case eHandleRight:
			handle->Move(geom.x() + geom.width(), geom.y() + geom.height() / 2);
			break;
		case eHandleRightBottom:
			handle->Move(geom.x() + geom.width(), geom.y() + geom.height());
			break;
		case eHandleBottom:
			handle->Move(geom.x() + geom.width() / 2, geom.y() + geom.height());
			break;
		case eHandleLeftBottom:
			handle->Move(geom.x(), geom.y() + geom.height());
			break;
		case eHandleLeft:
			handle->Move(geom.x(), geom.y() + geom.height() / 2);
			break;
		default:
			break;
		}
	}
}

QGraphicsItem* GraphicsItemGroup::Duplicate()
{
	GraphicsItemGroup *item = NULL;
	QList<QGraphicsItem*> copylist = DuplicateItems();

	item = CreateGroup(copylist);
	item->m_width = m_width;
	item->m_height = m_height;
	item->setPos(pos().x(),pos().y());
	item->SetPen(GetPen());
	item->SetBrush(GetBrush());
	item->setTransform(transform());
	item->setTransformOriginPoint(transformOriginPoint());
	item->setRotation(rotation());
	item->setScale(scale());
	item->setZValue(zValue()+0.1);
	item->UpdateCoordinate();

	return item;
}

QRectF GraphicsItemGroup::boundingRect() const
{
	return m_itemsBoundingRect;
}

bool GraphicsItemGroup::SaveToXml(QXmlStreamWriter *xml)
{
	xml->writeStartElement("group");
	xml->writeAttribute(tr("x"),QString("%1").arg(pos().x()));
	xml->writeAttribute(tr("y"),QString("%1").arg(pos().y()));
	xml->writeAttribute(tr("rotate"),QString("%1").arg(rotation()));
	xml->writeAttribute(tr("linkdb"),QString("%1").arg(GetLinkDB()));
	xml->writeAttribute(tr("linkscene"),QString("%1").arg(GetLinkScene()));

	foreach (QGraphicsItem *item, childItems())
	{
		removeFromGroup(item);
		AbstractShape *ab = qgraphicsitem_cast<AbstractShape*>(item);
		if (ab && !qgraphicsitem_cast<SizeHandleRect*>(ab))
		{
			ab->UpdateCoordinate();
			ab->SaveToXml(xml);
		}
		addToGroup(item);
	}

	xml->writeEndElement();
	return true;
}

bool GraphicsItemGroup::LoadFromXml(QXmlStreamReader *xml)
{
	return true;
}

GraphicsItemGroup* GraphicsItemGroup::CreateGroup(const QList<QGraphicsItem *> &items) const
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
	foreach (QGraphicsItem *item, items)
	{
		item->setSelected(false);
		QGraphicsItemGroup *g = dynamic_cast<QGraphicsItemGroup*>(item->parentItem());
		if (!g)
			group->addToGroup(item);
	}

	return group;
}

QList<QGraphicsItem*> GraphicsItemGroup::DuplicateItems() const
{
	QList<QGraphicsItem*> copylist ;
	foreach (QGraphicsItem *shape, childItems())
	{
		AbstractShape *ab = qgraphicsitem_cast<AbstractShape*>(shape);
		if (ab && !qgraphicsitem_cast<SizeHandleRect*>(ab))
		{
			QGraphicsItem * cp = ab->Duplicate();
			copylist.append(cp);
		}
	}

	return copylist;
}

QVariant GraphicsItemGroup::itemChange(QGraphicsItem::GraphicsItemChange change, const QVariant &value)
{
	if (change == QGraphicsItem::ItemSelectedHasChanged)
	{
        QGraphicsItemGroup *g = dynamic_cast<QGraphicsItemGroup*>(parentItem());
        if (!g)
		{
            SetState(value.toBool() ? eSelectionHandleActive : eSelectionHandleOff);
		}
        else
		{
            setSelected(false);
            return QVariant::fromValue<bool>(false);
        }

        if(value.toBool())
            UpdateCoordinate();
    }

    return QGraphicsItemGroup::itemChange(change, value);
}

void GraphicsItemGroup::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
	Q_UNUSED(option);
	Q_UNUSED(widget);

	if (option && (option->state & QStyle::State_Selected))
		DrawOutline(painter);
}
