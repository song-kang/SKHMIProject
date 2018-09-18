#include "drawobj.h"
#include "drawscene.h"
#include "drawview.h"
#include "view_plugin_drawer.h"

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

void GraphicsItem::SetStyleFromState(int state)
{
	QMap<int,QString>::iterator it = m_mapShowStyle.find(state);
	if (it == m_mapShowStyle.constEnd())
		return;

	QColor color;
	QString style = it.value();
	QList<QString> l = style.split(":").at(1).split("|");
	if (m_sName == "多边形图元" || m_sName == "圆角矩形图元" || m_sName == "矩形图元" || m_sName == "圆形图元" || m_sName == "椭圆形图元")
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

	//int direct = m_points.count();
	//SizeHandleRect *shr = new SizeHandleRect(this, direct + eHandleLeft, true);
	//m_handles.push_back(shr);
}

void GraphicsPolygonItem::EndPoint(const QPointF &point)
{
	Q_UNUSED(point);

	int nPoints = m_points.count();
	if (nPoints > 2 && (m_points[nPoints-1] == m_points[nPoints-2] ||
		m_points[nPoints-1].x() == m_points[nPoints-2].x() ||
		m_points[nPoints-1].y() == m_points[nPoints-2].y()))
	{
		//delete m_handles[eHandleLeft+nPoints-1];
		m_points.remove(nPoints-1);
		//m_handles.resize(eHandleLeft+nPoints-1);
	}

	m_initialPoints = m_points;
}

void GraphicsPolygonItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
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
			//SizeHandleRect *shr = new SizeHandleRect(this, dir + eHandleLeft, true);
			//m_handles.push_back(shr);
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
	//for (Handles::iterator it = m_handles.begin(); it != m_handles.end(); ++it)
	//	delete (*it);
	//m_handles.clear();

	SetName("线段图元");
}

GraphicsLineItem::~GraphicsLineItem()
{

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

	//int direct = m_points.count();
	//SizeHandleRect *shr = new SizeHandleRect(this, direct + eHandleLeft, true);
	//m_handles.push_back(shr);
}

void GraphicsLineItem::EndPoint(const QPointF &point)
{
	Q_UNUSED(point);

	int nPoints = m_points.count();
	if (nPoints > 2 && (m_points[nPoints-1] == m_points[nPoints-2] ||
		m_points[nPoints-1].x() == m_points[nPoints-2].x() &&
		m_points[nPoints-1].y() == m_points[nPoints-2].y()))
	{
		//delete m_handles[nPoints-1];
		m_points.remove(nPoints-1);
		//m_handles.resize(nPoints-1);
	}

	m_initialPoints = m_points;
}

void GraphicsLineItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
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

	if (m_points.size() > 1)
		painter->drawLine(m_points.at(0),m_points.at(1));
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
			//SizeHandleRect *shr = new SizeHandleRect(this, dir + eHandleLeft, true);
			//m_handles.push_back(shr);
			xml->skipCurrentElement();
		}
		else
			xml->skipCurrentElement();
	}

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
		//delete m_handles[eHandleLeft+nPoints-1];
		m_points.remove(nPoints-1);
		//m_handles.resize(eHandleLeft+nPoints-1);
	}

	m_initialPoints = m_points;
}

void GraphicsPolygonLineItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
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
			int dir = m_points.count();
			//SizeHandleRect *shr = new SizeHandleRect(this, dir + eHandleLeft, true);
			//m_handles.push_back(shr);
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

void GraphicsTriangleItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
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

void GraphicsPictureItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
	painter->drawPixmap(m_localRect.toRect(), m_picture);
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
