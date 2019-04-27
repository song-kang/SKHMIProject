#include "drawobj.h"
#include "drawscene.h"

#define PI			3.1415926
#define RADIAN1		PI / 180.0
#define ANGLE		180 / PI

static QPainterPath qt_graphicsItem_shapeFromPath(const QPainterPath &path, const QPen &pen)
{
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
	//p.addPath(path);
	return p;
}

///////////////////////// GraphicsItem /////////////////////////
GraphicsItem::GraphicsItem(QGraphicsItem *parent)
    :AbstractShapeType<QGraphicsItem>(parent)
{
	setAcceptHoverEvents(true);
    setFlag(QGraphicsItem::ItemIsSelectable, true);

	m_pScene = NULL;

	connect(this, SIGNAL(SigAttribute(QString,QString)), this, SLOT(SlotAttribute(QString,QString)));
}

GraphicsItem::~GraphicsItem()
{

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
	if (!GetLinkScene().isEmpty() || GetShowType() == 2 || GetShowType() == 3)
		m_pScene->GetView()->setCursor(Qt::PointingHandCursor);

	QGraphicsItem::hoverMoveEvent(event);
}

void GraphicsItem::Image(QPainter *painter, QPointF point)
{
	QPointF p = pos() - point;
	painter->translate(p.x(),p.y());
	paint(painter,NULL);
	painter->translate(-p.x(),-p.y());
}

void GraphicsItem::contextMenuEvent(QGraphicsSceneContextMenuEvent *event)
{
	Q_UNUSED(event);
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
	m_sCustom = xml->attributes().value(tr("custom")).toString();

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

void GraphicsItem::SetStyleFromState(int state)
{
	QMap<int,QString>::iterator it = m_mapShowStyle.find(state);
	if (it == m_mapShowStyle.constEnd())
		return;

	QColor color;
	QString style = it.value();
	QList<QString> l = style.split(":").at(1).split("|");
	if (m_sName == "多边形图元" || m_sName == "圆角矩形图元" || m_sName == "矩形图元" || m_sName == "圆形图元" || m_sName == "椭圆形图元" ||
		m_sName == "三角形图元" || m_sName == "菱形图元")
	{
		color.setNamedColor(l.at(0).split(",").at(0));
		color.setAlpha(l.at(0).split(",").at(1).toInt());
		m_pen.setColor(color);
		m_pen.setWidthF(l.at(0).split(",").at(2).toDouble());
		m_pen.setStyle((Qt::PenStyle)l.at(0).split(",").at(3).toInt());

		color.setNamedColor(l.at(1).split(",").at(0));
		color.setAlpha(l.at(1).split(",").at(1).toInt());
		m_brush.setColor(color);
		m_brush.setStyle((Qt::BrushStyle)l.at(1).split(",").at(2).toInt());

		emit SigAttribute(l.at(2), l.at(3));
	}
	else if (m_sName == "线段图元" || m_sName == "折线图元")
	{
		color.setNamedColor(l.at(0).split(",").at(0));
		color.setAlpha(l.at(0).split(",").at(1).toInt());
		m_pen.setColor(color);
		m_pen.setWidthF(l.at(0).split(",").at(2).toDouble());
		m_pen.setStyle((Qt::PenStyle)l.at(0).split(",").at(3).toInt());

		emit SigAttribute(l.at(1), l.at(2));
	}
	else if (m_sName == "文字图元")
	{
		color.setNamedColor(l.at(0).split(",").at(0));
		color.setAlpha(l.at(0).split(",").at(1).toInt());
		m_pen.setColor(color);

		((GraphicsTextItem*)this)->SetStyle(l.at(1), l.at(2));

		emit SigAttribute(l.at(3), l.at(4));
	}
	else if (m_sName == "图像图元")
	{
	}
}

void GraphicsItem::SlotAttribute(QString rotation,QString scale)
{
	setRotation(rotation.toDouble());
	setScale(scale.toDouble());
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

void GraphicsPolygonItem::UpdateCoordinate()
{
	QPointF pt1,pt2,delta;
	QPolygonF pts = mapToScene(m_points);
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
	}

	m_initialPoints = m_points;
}

QRectF GraphicsPolygonItem::boundingRect() const
{
	return shape().controlPointRect();
}

QPainterPath GraphicsPolygonItem::shape() const
{
	QPainterPath path;

	path.addPolygon(m_points);
	path.closeSubpath();

	return path;
}

void GraphicsPolygonItem::AddPoint(const QPointF &point)
{
	m_points.append(mapFromScene(point));
}

void GraphicsPolygonItem::EndPoint(const QPointF &point)
{
	Q_UNUSED(point);

	int nPoints = m_points.count();
	if (nPoints > 2 && (m_points[nPoints-1] == m_points[nPoints-2] ||
		m_points[nPoints-1].x() == m_points[nPoints-2].x() ||
		m_points[nPoints-1].y() == m_points[nPoints-2].y()))
	{
		m_points.remove(nPoints-1);
	}

	m_initialPoints = m_points;
}

void GraphicsPolygonItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
	if (m_isShow == false)
		return;

	if (m_isFlash)
	{
		if (GetScene()->GetSwap())
		{
			painter->setPen(GetPen());
			painter->setBrush(GetBrush());
		}
		else
		{
			QPen pen = GetPen();
			QColor color = GetPenColor();
			color.setAlpha(color.alpha() - 150 > 0 ? color.alpha() - 150 : 0);
			pen.setColor(color);
			painter->setPen(pen);

			QBrush brush = GetBrush();
			color = GetBrushColor();
			color.setAlpha(color.alpha() - 150 > 0 ? color.alpha() - 150 : 0);
			brush.setColor(color);
			painter->setBrush(brush);
		}
	}
	else
	{
		painter->setPen(GetPen());
		painter->setBrush(GetBrush());
	}

	painter->drawPolygon(m_points);
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
	SetName("线段图元");

	//startTimer(150);
	m_fScale = 1.5;
	m_fStep = 4.0;
	m_iStepCnt = 1;
}

GraphicsLineItem::~GraphicsLineItem()
{

}

void GraphicsLineItem::timerEvent(QTimerEvent *event)
{
	QPointF p0 = m_points.at(0);
	QPointF p1 = m_points.at(1);
	float x = abs(p1.x() - p0.x());
	float y = abs(p1.y() - p0.y());
	float angle = atan(y / x)/* * ANGLE*/;
	float len = m_fStep * m_iStepCnt * m_fScale;
	if (p1.x() > p0.x() && p1.y() == p0.y()) //X正轴
	{
		float x = p0.x() + m_fStep * m_iStepCnt * m_fScale;
		if (x > p1.x())
		{
			m_iStepCnt = 1;
			x = p1.x();
		}
		p1.setX(x);
	}
	else if (p1.x() < p0.x() && p1.y() == p0.y()) //X负轴
	{
		float x = p0.x() - m_fStep * m_iStepCnt * m_fScale;
		if (x < p1.x())
		{
			m_iStepCnt = 1;
			x = p1.x();
		}
		p1.setX(x);
	}
	else if (p1.x() == p0.x() && p1.y() < p0.y()) //Y正轴
	{
		float y = p0.y() - m_fStep * m_iStepCnt * m_fScale;
		if (y < p1.y())
		{
			m_iStepCnt = 1;
			y = p1.y();
		}
		p1.setY(y);
	}
	else if (p1.x() == p0.x() && p1.y() > p0.y()) //Y负轴
	{
		float y = p0.y() + m_fStep * m_iStepCnt * m_fScale;
		if (y > p1.y())
		{
			m_iStepCnt = 1;
			y = p1.y();
		}
		p1.setY(y);
	}
	else if (p1.x() > p0.x() && p1.y() < p0.y()) //第一象限
	{
		x = p0.x() + len * cos(/*RADIAN1 * */angle);
		if (x > p1.x())
		{
			m_iStepCnt = 1;
			x = p1.x();
		}
		p1.setX(x);

		y = p0.y() - len * sin(angle);
		if (y < p1.y())
		{
			m_iStepCnt = 1;
			y = p1.y();
		}
		p1.setY(y);
	}
	else if (p1.x() < p0.x() && p1.y() < p0.y()) //第二象限
	{
		x = p0.x() - len * cos(angle);
		if (x < p1.x())
		{
			m_iStepCnt = 1;
			x = p1.x();
		}
		p1.setX(x);

		y = p0.y() - len * sin(angle);
		if (y < p1.y())
		{
			m_iStepCnt = 1;
			y = p1.y();
		}
		p1.setY(y);
	}
	else if (p1.x() < p0.x() && p1.y() > p0.y()) //第三象限
	{
		x = p0.x() - len * cos(angle);
		if (x < p1.x())
		{
			m_iStepCnt = 1;
			x = p1.x();
		}
		p1.setX(x);

		y = p0.y() + len * sin(angle);
		if (y > p1.y())
		{
			m_iStepCnt = 1;
			y = p1.y();
		}
		p1.setY(y);
	}
	else if (p1.x() > p0.x() && p1.y() > p0.y()) //第四象限
	{
		x = p0.x() + len * cos(angle);
		if (x > p1.x())
		{
			m_iStepCnt = 1;
			x = p1.x();
		}
		p1.setX(x);

		y = p0.y() + len * sin(angle);
		if (y > p1.y())
		{
			m_iStepCnt = 1;
			y = p1.y();
		}
		p1.setY(y);
	}
	else if (p0.x() == p1.x() && p0.y() == p1.y())
		return;
	m_iStepCnt++;
	
	float cx = p1.x();
	float cy = p1.y();
	float rx = p0.x() - p1.x();
	float ry = p0.y() - p1.y();
	float dis = sqrt(rx * rx + ry * ry);
	if (dis < 0.01)
		return;

	bool bConv = false;
	if (p1.y() > p0.y())
		bConv = true;

	float fcos = (bConv ? -rx : rx) / dis;
	float radian = (float)(acos(fcos) - PI);
	if (bConv)
		radian += (float)PI;

	float x3 = cx - 10 * m_fScale;
	float y3 = cy - 3 * m_fScale;
	float x4 = cx - 10 * m_fScale;
	float y4 = cy + 3 * m_fScale;

	float x30 = (float)(x3 - cx) * cos(radian) - (y3 - cy) * sin(radian) + cx;
	float x40 = (float)(x4 - cx) * cos(radian) - (y4 - cy) * sin(radian) + cx;
	float y30 = (float)(x3 - cx) * sin(radian) + (y3 - cy) * cos(radian) + cy;
	float y40 = (float)(x4 - cx) * sin(radian) + (y4 - cy) * cos(radian) + cy;

	m_arrowPoints.clear();
	QPointF f1(cx,cy);
	m_arrowPoints.append(f1);
	QPointF f2(x30,y30);
	m_arrowPoints.append(f2);
	QPointF f3(x40,y40);
	m_arrowPoints.append(f3);

	//update();
}

QPainterPath GraphicsLineItem::shape() const
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
}

void GraphicsLineItem::EndPoint(const QPointF &point)
{
	Q_UNUSED(point);

	int nPoints = m_points.count();
	if (nPoints > 2 && (m_points[nPoints-1] == m_points[nPoints-2] ||
		m_points[nPoints-1].x() == m_points[nPoints-2].x() &&
		m_points[nPoints-1].y() == m_points[nPoints-2].y()))
	{
		m_points.remove(nPoints-1);
	}

	m_initialPoints = m_points;
}

void GraphicsLineItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
	if (m_isShow == false)
		return;

	if (m_isFlash)
	{
		if (GetScene()->GetSwap())
		{
			painter->setPen(GetPen());
		}
		else
		{
			QPen pen = GetPen();
			QColor color = GetPenColor();
			color.setAlpha(color.alpha() - 150 > 0 ? color.alpha() - 150 : 0);
			pen.setColor(color);
			painter->setPen(pen);
		}
	}
	else
	{
		painter->setPen(GetPen());
	}
	//painter->setBrush(Qt::NoBrush);
	painter->setBrush(GetBrush());

	if (m_points.size() > 1)
	{
		painter->drawLine(m_points.at(0),m_points.at(1));
		painter->drawPolygon(m_arrowPoints);
	}
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
			xml->skipCurrentElement();
		}
		else
			xml->skipCurrentElement();
	}

	UpdateCoordinate();
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

QPainterPath GraphicsPolygonLineItem::shape() const
{
	QPainterPath path;
	path.moveTo(m_points.at(0));

	int i = 1;
	while (i < m_points.size())
	{
		path.lineTo(m_points.at(i));
		i++;
	}

	return qt_graphicsItem_shapeFromPath(path, GetPen());
}

void GraphicsPolygonLineItem::EndPoint(const QPointF &point)
{
	Q_UNUSED(point);

	int nPoints = m_points.count();
	if (nPoints > 2 && (m_points[nPoints-1] == m_points[nPoints-2] ||
		m_points[nPoints-1].x() == m_points[nPoints-2].x() ||
		m_points[nPoints-1].y() == m_points[nPoints-2].y()))
	{
		m_points.remove(nPoints-1);
	}

	m_initialPoints = m_points;
}

void GraphicsPolygonLineItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
	if (m_isShow == false)
		return;

	if (m_isFlash)
	{
		if (GetScene()->GetSwap())
		{
			painter->setPen(GetPen());
		}
		else
		{
			QPen pen = GetPen();
			QColor color = GetPenColor();
			color.setAlpha(color.alpha() - 150 > 0 ? color.alpha() - 150 : 0);
			pen.setColor(color);
			painter->setPen(pen);
		}
	}
	else
	{
		painter->setPen(GetPen());
	}
	painter->setBrush(Qt::NoBrush);

	QPainterPath path;
	path.moveTo(m_points.at(0));
	int i = 1;
	while (i < m_points.size())
	{
		path.lineTo(m_points.at(i));
		i++;
	}

	painter->drawPath(path);
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
	}

	m_initialRect = m_localRect;
}

QRectF GraphicsRectItem::boundingRect() const
{
	return m_localRect;
}

QPainterPath GraphicsRectItem::shape() const
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
	if (m_isShow == false)
		return;

	if (m_isFlash)
	{
		if (GetScene()->GetSwap())
		{
			painter->setPen(GetPen());
			painter->setBrush(GetBrush());
		}
		else
		{
			QPen pen = GetPen();
			QColor color = GetPenColor();
			color.setAlpha(color.alpha() - 150 > 0 ? color.alpha() - 150 : 0);
			pen.setColor(color);
			painter->setPen(pen);

			QBrush brush = GetBrush();
			color = GetBrushColor();
			color.setAlpha(color.alpha() - 150 > 0 ? color.alpha() - 150 : 0);
			brush.setColor(color);
			painter->setBrush(brush);
		}
	}
	else
	{
		painter->setPen(GetPen());
		painter->setBrush(GetBrush());
	}

	if (m_isRound)
		painter->drawRoundedRect(m_localRect,m_round.width(),m_round.height());
	else
		painter->drawRect(m_localRect);
}

bool GraphicsRectItem::LoadFromXml(QXmlStreamReader *xml)
{
	m_isRound = (xml->name() == tr("roundrect"));
	if (m_isRound)
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

QPainterPath GraphicsTriangleItem::shape() const
{
	QPainterPath path;

	path.addPolygon(m_points);
	path.closeSubpath();

	return path;
}

void GraphicsTriangleItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
	if (m_isShow == false)
		return;

	painter->setPen(GetPen());
	painter->setBrush(GetBrush());

	m_points.clear();
	m_points.append(QPointF(m_localRect.x()+m_localRect.width()/2,m_localRect.y()));
	m_points.append(QPointF(m_localRect.x(),m_localRect.y()+m_localRect.height()));
	m_points.append(QPointF(m_localRect.x()+m_localRect.width(),m_localRect.y()+m_localRect.height()));
	painter->drawPolygon(m_points);
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

QPainterPath GraphicsRhombusItem::shape() const
{
	QPainterPath path;

	path.addPolygon(m_points);
	path.closeSubpath();

	return path;
}

void GraphicsRhombusItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
	if (m_isShow == false)
		return;

	painter->setPen(GetPen());
	painter->setBrush(GetBrush());

	m_points.clear();
	m_points.append(QPointF(m_localRect.x()+m_localRect.width()/2,m_localRect.y()));
	m_points.append(QPointF(m_localRect.x(),m_localRect.y()+m_localRect.height()/2));
	m_points.append(QPointF(m_localRect.x()+m_localRect.width()/2,m_localRect.y()+m_localRect.height()));
	m_points.append(QPointF(m_localRect.x()+m_localRect.width(),m_localRect.y()+m_localRect.height()/2));
	painter->drawPolygon(m_points);
}

bool GraphicsRhombusItem::LoadFromXml(QXmlStreamReader *xml)
{
	ReadBaseAttributes(xml);
	UpdateCoordinate();
	xml->skipCurrentElement();
	return true;
}

///////////////////////// GraphicsParallelogramItem /////////////////////////
GraphicsParallelogramItem::GraphicsParallelogramItem(const QRect &rect, GraphicsRectItem *parent)
	:GraphicsRectItem(rect, parent)
{
	SetName("平形四边形图元");
}

GraphicsParallelogramItem::~GraphicsParallelogramItem()
{

}

QPainterPath GraphicsParallelogramItem::shape() const
{
	QPainterPath path;

	path.addPolygon(m_points);
	path.closeSubpath();

	return path;
}

void GraphicsParallelogramItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
	if (m_isShow == false)
		return;

	painter->setPen(GetPen());
	painter->setBrush(GetBrush());

	if (m_angle > 90.0 && m_angle <= 180.0)
	{
		m_points.clear();
		m_points.append(QPointF(m_localRect.x(),m_localRect.y()));
		m_points.append(QPointF(m_localRect.x()+m_localRect.height()/tan(RADIAN1*(180.0-m_angle)),m_localRect.y()+m_localRect.height()));
		m_points.append(QPointF(m_localRect.x()+m_localRect.width(),m_localRect.y()+m_localRect.height()));
		m_points.append(QPointF(m_localRect.x()+(m_localRect.width()-m_localRect.height()/tan(RADIAN1*(180.0-m_angle))),m_localRect.y()));
	}
	else if (m_angle >= 0.0 && m_angle < 90.0)
	{
		m_points.clear();
		m_points.append(QPointF(m_localRect.x()+m_localRect.width(),m_localRect.y()));
		m_points.append(QPointF(m_localRect.x()+m_localRect.height()/tan(RADIAN1*m_angle),m_localRect.y()));
		m_points.append(QPointF(m_localRect.x(),m_localRect.y()+m_localRect.height()));
		m_points.append(QPointF(m_localRect.x()+(m_localRect.width()-m_localRect.height()/tan(RADIAN1*m_angle)),m_localRect.y()+m_localRect.height()));
	}

	painter->drawPolygon(m_points);
}

bool GraphicsParallelogramItem::LoadFromXml(QXmlStreamReader *xml)
{
	ReadBaseAttributes(xml);
	m_angle = xml->attributes().value(tr("angle")).toString().toDouble();
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

QRectF GraphicsEllipseItem::boundingRect() const
{
	return shape().controlPointRect();
}

QPainterPath GraphicsEllipseItem::shape() const
{
	QPainterPath path;

	path.addEllipse(m_localRect);
	path.closeSubpath();

	return path;
}

void GraphicsEllipseItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
	if (m_isShow == false)
		return;

	if (m_isFlash)
	{
		if (GetScene()->GetSwap())
		{
			painter->setPen(GetPen());
			painter->setBrush(GetBrush());
		}
		else
		{
			QPen pen = GetPen();
			QColor color = GetPenColor();
			color.setAlpha(color.alpha() - 150 > 0 ? color.alpha() - 150 : 0);
			pen.setColor(color);
			painter->setPen(pen);

			QBrush brush = GetBrush();
			color = GetBrushColor();
			color.setAlpha(color.alpha() - 150 > 0 ? color.alpha() - 150 : 0);
			brush.setColor(color);
			painter->setBrush(brush);
		}
	}
	else
	{
		painter->setPen(GetPen());
		painter->setBrush(GetBrush());
	}

	painter->drawEllipse(m_localRect);
}

bool GraphicsEllipseItem::LoadFromXml(QXmlStreamReader *xml)
{
	ReadBaseAttributes(xml);
	xml->skipCurrentElement();

	UpdateCoordinate();
	return true;
}

///////////////////////// GraphicsArcEllipseItem /////////////////////////
GraphicsArcEllipseItem::GraphicsArcEllipseItem(const QRect &rect, bool isCircle, QGraphicsItem *parent)
	:GraphicsRectItem(rect, parent)
	,m_isCircle(isCircle)
{
	if (isCircle)
		SetName("圆弧线图元");
	else
		SetName("椭圆弧线图元");
}

GraphicsArcEllipseItem::~GraphicsArcEllipseItem()
{

}

QRectF GraphicsArcEllipseItem::boundingRect() const
{
	return shape().controlPointRect();
}

QPainterPath GraphicsArcEllipseItem::shape() const
{
	QPainterPath path;
	path.moveTo(m_localRect.center());
	path.arcTo(m_localRect, m_startAngle * 16, (m_endAngle - m_startAngle) * 16);

	return qt_graphicsItem_shapeFromPath(path, GetPen());
}

void GraphicsArcEllipseItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
	if (m_isShow == false)
		return;

	painter->setPen(GetPen());
	painter->drawArc(m_localRect, m_startAngle * 16, (m_endAngle - m_startAngle) * 16);
}

bool GraphicsArcEllipseItem::LoadFromXml(QXmlStreamReader *xml)
{
	ReadBaseAttributes(xml);
	m_startAngle = xml->attributes().value(tr("startAngle")).toString().toDouble();
	m_endAngle = xml->attributes().value(tr("endAngle")).toString().toDouble();

	xml->skipCurrentElement();
	UpdateCoordinate();
	return true;
}

///////////////////////// GraphicsTextItem /////////////////////////
GraphicsTextItem::GraphicsTextItem(const QRect &rect, QGraphicsItem *parent)
	:GraphicsRectItem(rect, parent)
{
	m_text = "文字";
	m_option.setAlignment(Qt::AlignCenter);
	m_option.setWrapMode(QTextOption::WordWrap);

	SetName("文字图元");
}

GraphicsTextItem::~GraphicsTextItem()
{

}

void GraphicsTextItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
	if (m_isShow == false)
		return;

	if (m_isFlash)
	{
		if (GetScene()->GetSwap())
		{
			painter->setPen(GetPen());
		}
		else
		{
			QPen pen = GetPen();
			QColor color = GetPenColor();
			color.setAlpha(color.alpha() - 150 > 0 ? color.alpha() - 150 : 0);
			pen.setColor(color);
			painter->setPen(pen);
		}
	}
	else
	{
		painter->setPen(GetPen());
	}
	
	painter->setFont(m_font);
	painter->drawText(m_localRect, m_text, m_option);
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
	m_option.setAlignment((Qt::Alignment)xml->attributes().value(tr("alignment")).toString().toInt());

	xml->skipCurrentElement();
	UpdateCoordinate();
	return true;
}

void GraphicsTextItem::SetStyle(QString sFont, QString text)
{
	m_text = text;
	m_font.setFamily(sFont.split(",").at(0));
	m_font.setPointSize(sFont.split(",").at(1).toInt());
	m_font.setBold(sFont.split(",").at(2).toInt());
	m_font.setItalic(sFont.split(",").at(3).toInt());
	m_font.setUnderline(sFont.split(",").at(4).toInt());
	m_font.setStrikeOut(sFont.split(",").at(5).toInt());
	m_font.setKerning(sFont.split(",").at(6).toInt());
}

///////////////////////// GraphicsTextTimeItem /////////////////////////
GraphicsTextTimeItem::GraphicsTextTimeItem(const QRect &rect, QGraphicsItem *parent)
	:GraphicsTextItem(rect, parent)
{
	m_sStyle = "yyyy-MM-dd hh:mm:ss";

	m_option.setAlignment(Qt::AlignCenter);
	m_option.setWrapMode(QTextOption::WordWrap);

	SetName("文本时间");
}

GraphicsTextTimeItem::~GraphicsTextTimeItem()
{

}

void GraphicsTextTimeItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
	if (m_isShow == false)
		return;

	QDateTime dt = QDateTime::currentDateTime();
	QString sTime = dt.toString(m_sStyle);

	painter->setFont(m_font);
	painter->setPen(GetPen());
	painter->drawText(m_localRect, sTime, m_option);
}

bool GraphicsTextTimeItem::LoadFromXml(QXmlStreamReader *xml)
{
	ReadBaseAttributes(xml);
	m_text = xml->attributes().value(tr("desc")).toString();
	m_sStyle = xml->attributes().value(tr("style")).toString();
	m_font.setFamily(xml->attributes().value(tr("family")).toString());
	m_font.setPointSize(xml->attributes().value(tr("pointSize")).toString().toInt());
	m_font.setBold(xml->attributes().value(tr("bold")).toString().toInt());
	m_font.setItalic(xml->attributes().value(tr("italic")).toString().toInt());
	m_font.setUnderline(xml->attributes().value(tr("underline")).toString().toInt());
	m_font.setStrikeOut(xml->attributes().value(tr("strikeOut")).toString().toInt());
	m_font.setKerning(xml->attributes().value(tr("kerning")).toString().toInt());
	m_option.setAlignment((Qt::Alignment)xml->attributes().value(tr("alignment")).toString().toInt());

	xml->skipCurrentElement();
	UpdateCoordinate();
	return true;
}

///////////////////////// GraphicsPictureItem /////////////////////////
GraphicsPictureItem::GraphicsPictureItem(const QRect &rect, QString fileName, QGraphicsItem *parent)
	:GraphicsRectItem(rect, parent),
	m_fileName(fileName)
{
	m_gif = NULL;
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
	if (m_gif)
		delete m_gif;
}

void GraphicsPictureItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
	if (m_isShow == false)
		return;

	if (!m_picture.isNull())
		painter->drawPixmap(m_localRect.toRect(), m_picture);
	else
		painter->drawImage(m_localRect.toRect(), m_gif->currentImage());
}

bool GraphicsPictureItem::LoadFromXml(QXmlStreamReader *xml)
{
	ReadBaseAttributes(xml);
	m_fileName = xml->attributes().value(tr("name")).toString();
	if (!m_fileName.isEmpty())
	{
		if (Common::FileExists(Common::GetCurrentAppPath() + "../picture/" + m_fileName))
		{
			if (!m_picture.load(Common::GetCurrentAppPath() + "../picture/" + m_fileName))
				m_picture.load(":/images/fileWarn");
		}
		else
			m_picture.load(":/images/fileWarn");
	}

	xml->skipCurrentElement();
	UpdateCoordinate();
	return true;
}

void GraphicsPictureItem::LoadPicture(int sn)
{
	if (!m_picture.load(Common::GetCurrentAppPath() + tr("../picture/%1").arg(sn)))
		m_picture.load(":/images/fileWarn");
}

void GraphicsPictureItem::LoadGif(int sn)
{
	QPixmap pix;
	bool ret = pix.load(Common::GetCurrentAppPath() + tr("../picture/%1").arg(sn));
	if (!ret)
		m_picture.load(":/images/fileWarn");

	if (ret)
	{
		m_gif = new QMovie(Common::GetCurrentAppPath() + tr("../picture/%1").arg(sn));
		m_gif->start();
	}
}

///////////////////////// GraphicsItemGroup /////////////////////////
GraphicsItemGroup::GraphicsItemGroup(QGraphicsItem *parent)
	:AbstractShapeType <QGraphicsItemGroup>(parent),
	m_parent(parent)
{
	setFlag(QGraphicsItem::ItemIsSelectable, true);
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

	//foreach (QGraphicsItem *shape, childItems())
	//{
	//	AbstractShape *ab = qgraphicsitem_cast<AbstractShape*>(shape);
	//	if (ab && !qgraphicsitem_cast<SizeHandleRect*>(ab))
	//	{
	//		painter.translate(GetWidth()/2, GetHeight()/2);
	//		((GraphicsItem*)ab)->Image(&painter,pos());
	//		painter.translate(-GetWidth()/2, -GetHeight()/2);
	//	}
	//}

	return pixmap;
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
		if (ab)
			ab->UpdateCoordinate();
	}
}

QRectF GraphicsItemGroup::boundingRect() const
{
	return m_itemsBoundingRect;
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

void GraphicsItemGroup::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{

}

void GraphicsItemGroup::SlotAttribute(QString rotation,QString scale)
{

}
