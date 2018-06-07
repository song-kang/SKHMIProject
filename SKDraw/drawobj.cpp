#include "drawobj.h"
#include "drawscene.h"

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
}

GraphicsItem::~GraphicsItem()
{

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

///////////////////////// GraphicsPolygonItem /////////////////////////
GraphicsPolygonItem::GraphicsPolygonItem(QGraphicsItem *parent)
	:GraphicsItem(parent)
{
	m_pScene = NULL;
	SetBrush(Qt::green);
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
}

bool GraphicsPolygonItem::SaveToXml(QXmlStreamWriter *xml)
{
	return true;
}

bool GraphicsPolygonItem::LoadFromXml(QXmlStreamReader *xml)
{
	return true;
}

///////////////////////// GraphicsLineItem /////////////////////////
GraphicsLineItem::GraphicsLineItem(QGraphicsItem *parent)
	:GraphicsPolygonItem(parent)
{
	for (Handles::iterator it = m_handles.begin(); it != m_handles.end(); ++it)
		delete (*it);
	m_handles.clear();
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
	return true;
}

bool GraphicsLineItem::LoadFromXml(QXmlStreamReader *xml)
{
	return true;
}

///////////////////////// GraphicsPolygonLineItem /////////////////////////
GraphicsPolygonLineItem::GraphicsPolygonLineItem(QGraphicsItem *parent)
	:GraphicsPolygonItem(parent)
{

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
	return true;
}

bool GraphicsPolygonLineItem::LoadFromXml(QXmlStreamReader *xml)
{
	return true;
}

///////////////////////// GraphicsRectItem /////////////////////////
GraphicsRectItem::GraphicsRectItem(const QRect &rect, bool isRound, QGraphicsItem *parent)
	:GraphicsItem(parent)
	,m_isRound(isRound)
{
	m_rx = m_ry = 5;
	m_localRect = rect;
	m_initialRect = rect;
	m_width = rect.width();
	m_height = rect.height();
	m_originPoint = QPointF(0,0);

	SetBrush(Qt::green);
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

QString GraphicsRectItem::DisplayName()
{
	QString name;

	if (m_isRound)
		name = tr("圆角矩形图元");
	else
		name = tr("矩形图元");

	return name;
}

QGraphicsItem* GraphicsRectItem::Duplicate()
{
	GraphicsRectItem * item = new GraphicsRectItem(m_localRect.toRect(), m_isRound);

	item->m_width = GetWidth();
	item->m_height = GetHeight();
	item->setPos(pos().x(), pos().y());
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

QRectF GraphicsRectItem::boundingRect() const
{
	return m_localRect;
}

QPainterPath GraphicsRectItem::Shape() const
{
	QPainterPath path;

	if (m_isRound)
		path.addRoundedRect(m_localRect,m_rx,m_ry);
	else
		path.addRect(m_localRect);

	return path;
}

void GraphicsRectItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
	painter->setPen(GetPen());
	painter->setBrush(GetBrush());

	if (m_isRound)
		painter->drawRoundedRect(m_localRect,m_rx,m_ry);
	else
		painter->drawRect(m_localRect.toRect());
}

bool GraphicsRectItem::SaveToXml(QXmlStreamWriter *xml)
{
	return true;
}

bool GraphicsRectItem::LoadFromXml(QXmlStreamReader *xml)
{
	return true;
}

///////////////////////// GraphicsEllipseItem /////////////////////////
GraphicsEllipseItem::GraphicsEllipseItem(const QRect &rect, bool isCircle, QGraphicsItem *parent)
	:GraphicsRectItem(rect, parent)
	,m_isCircle(isCircle)
{
	
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

QString GraphicsEllipseItem::DisplayName()
{
	QString name;

	if (m_isCircle)
		name = tr("圆形图元");
	else
		name = tr("椭圆形图元");

	return name;
}

QGraphicsItem* GraphicsEllipseItem::Duplicate()
{
	GraphicsEllipseItem * item = new GraphicsEllipseItem(m_localRect.toRect(), m_isCircle);

	item->m_width = GetWidth();
	item->m_height = GetHeight();
	item->setPos(pos().x(), pos().y());
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
}

bool GraphicsEllipseItem::SaveToXml(QXmlStreamWriter *xml)
{
	return true;
}

bool GraphicsEllipseItem::LoadFromXml(QXmlStreamReader *xml)
{
	return true;
}

///////////////////////// GraphicsTextItem /////////////////////////
GraphicsTextItem::GraphicsTextItem(const QRect &rect, QGraphicsItem *parent)
	:GraphicsRectItem(rect, parent)
{
	SetPen(QPen(Qt::blue));
	m_text = "文字";
	setToolTip(m_text);
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
	item->UpdateCoordinate();

	return item;
}

void GraphicsTextItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
	QFont font;
	font.setFamily("Microsoft YaHei");					//字体
	font.setPointSize(16);								//大小
	font.setBold(true);									//粗体
	font.setItalic(false);								//斜体
	font.setUnderline(false);							//设置下划线
	font.setOverline(false);							//设置上划线
	font.setCapitalization(QFont::SmallCaps);			//设置字母大小写
	font.setLetterSpacing(QFont::AbsoluteSpacing, 2);	//设置字符间距

	m_option.setAlignment(Qt::AlignCenter);
	//m_option.setWrapMode(QTextOption::NoWrap);
	m_option.setWrapMode(QTextOption::WordWrap);

	painter->setFont(font);
	painter->setPen(GetPen());
	painter->setBrush(GetBrush());
	painter->drawText(m_localRect, m_text, m_option);

	if (option->state & QStyle::State_Selected)
		DrawOutline(painter);
}

bool GraphicsTextItem::SaveToXml(QXmlStreamWriter *xml)
{
	return true;
}

bool GraphicsTextItem::LoadFromXml(QXmlStreamReader *xml)
{
	return true;
}

///////////////////////// GraphicsPictureItem /////////////////////////
GraphicsPictureItem::GraphicsPictureItem(const QRect &rect, QPixmap &pix, QGraphicsItem *parent)
	:GraphicsRectItem(rect, parent),
	m_picture(pix)
{
	m_width = pix.width();
	m_height = pix.height();
	m_localRect.setWidth(m_width);
	m_localRect.setHeight(m_height);
	m_initialRect = m_localRect;
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
	GraphicsPictureItem * item = new GraphicsPictureItem(m_localRect.toRect(), m_picture);

	item->m_width = GetWidth();
	item->m_height = GetHeight();
	item->setPos(pos().x(), pos().y());
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

void GraphicsPictureItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
	painter->drawPixmap(m_localRect.toRect(), m_picture);
}

bool GraphicsPictureItem::SaveToXml(QXmlStreamWriter *xml)
{
	return true;
}

bool GraphicsPictureItem::LoadFromXml(QXmlStreamReader *xml)
{
	return true;
}

///////////////////////// GraphicsItemGroup /////////////////////////
GraphicsItemGroup::GraphicsItemGroup(QGraphicsItem *parent)
	:AbstractShapeType <QGraphicsItemGroup>(parent),
	m_parent(parent)
{
	m_itemsBoundingRect = QRectF();
	m_handles.reserve(eHandleLeft);
	for (int i = eHandleLeftTop; i <= eHandleLeft; i++)
	{
		SizeHandleRect *shr = new SizeHandleRect(this, i);
		m_handles.push_back(shr);
	}

	setFlag(QGraphicsItem::ItemIsMovable, true);
	setFlag(QGraphicsItem::ItemIsSelectable, true);
	setFlag(QGraphicsItem::ItemSendsGeometryChanges, true);
	this->setAcceptHoverEvents(true);
}

GraphicsItemGroup::~GraphicsItemGroup()
{

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

	if (option->state & QStyle::State_Selected)
		DrawOutline(painter);
}
