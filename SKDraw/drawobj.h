#ifndef DRAWOBJ_H
#define DRAWOBJ_H

#include "skhead.h"
#include "sizehandle.h"
#include "drawscene.h"

class ShapeMimeData : public QMimeData
{
	Q_OBJECT

public:
	ShapeMimeData(QList<QGraphicsItem *> items);
	~ShapeMimeData();

	QList<QGraphicsItem *> items() const;

private:
	QList<QGraphicsItem *> m_items;

};

typedef std::vector<SizeHandleRect*> Handles;

template <typename BaseType = QGraphicsItem>
class AbstractShapeType : public BaseType
{
public:
	explicit AbstractShapeType(QGraphicsItem * parent = 0)
		:BaseType(parent)
	{
		m_pen = QPen(Qt::black);
		m_pen.setWidthF(1.0);
		m_brush = QBrush(Qt::NoBrush);
		m_width = m_height = 0.0;
	}
	virtual ~AbstractShapeType(){}

	QPen   GetPen() const { return m_pen; }
	QColor GetPenColor() const { return m_pen.color(); }
	QBrush GetBrush() const { return m_brush; }
	QColor GetBrushColor() const { return m_brush.color(); }
	qreal  GetWidth() const { return m_width ; }
	qreal  GetHeight() const {return m_height;}
	QRectF GetRect() const { return m_localRect; }

	void SetPen(const QPen & pen) { m_pen = pen; }
	void SetPenColor(const QColor & color) { m_pen.setColor(color); }
	void SetBrush( const QBrush & brush ) { m_brush = brush ; }
	void SetBrushColor( const QColor & color ) { m_brush.setColor(color);}
	void SetWidth(qreal width) { m_width = width; UpdateCoordinate(); }
	void SetHeight(qreal height) { m_height = height; UpdateCoordinate(); }

	void SetState(SelectionHandleState st)
	{
		for (Handles::iterator it = m_handles.begin(); it != m_handles.end(); ++it)
			(*it)->SetState(st);
	}

	int CollidesWithHandle(const QPointF &point) const
	{
		const Handles::const_reverse_iterator hend = m_handles.rend();
		for (Handles::const_reverse_iterator it = m_handles.rbegin(); it != hend; ++it)
		{
			QPointF pt = (*it)->mapFromScene(point);
			if ((*it)->contains(pt))
				return (*it)->GetDirect();
		}

		return Handle_None;
	}

public:
	QPen   m_pen ;
	QBrush m_brush;
	qreal  m_width;
	qreal  m_height;
	QRectF m_localRect;
	Handles m_handles;

public:
	virtual void Move(const QPointF &point) = 0;
	virtual void Control(int direct, const QPointF &delta) = 0;
	virtual void Stretch(int handle, double sx, double sy, const QPointF &origin) = 0;
	virtual bool SaveToXml(QXmlStreamWriter *xml) = 0;
	virtual bool LoadFromXml(QXmlStreamReader *xml) = 0;
	virtual int HandleCount() const { return m_handles.size(); }
	virtual void UpdateHandles() = 0;
	virtual void UpdateCoordinate() = 0;
	virtual QString DisplayName() const { return QString("抽象形态"); }	
	virtual QGraphicsItem* Duplicate() = 0;

};

typedef AbstractShapeType <QGraphicsItem> AbstractShape;

///////////////////////// GraphicsItem /////////////////////////
class GraphicsItem : public QObject, public AbstractShapeType <QGraphicsItem>
{
	Q_OBJECT

public:
	GraphicsItem(QGraphicsItem * parent);
	~GraphicsItem();

	enum { Type = UserType + 1 };
	int  GetType() const { return Type; }

public:
	virtual void UpdateHandles();
	virtual void contextMenuEvent(QGraphicsSceneContextMenuEvent *event);
	virtual QVariant itemChange(QGraphicsItem::GraphicsItemChange change, const QVariant &value);

signals:
	void SigSelectedChange(QGraphicsItem *item);

};

///////////////////////// GraphicsPolygonItem /////////////////////////
class GraphicsPolygonItem : public GraphicsItem
{
public:
	GraphicsPolygonItem(QGraphicsItem * parent = 0);
	~GraphicsPolygonItem();

	void SetScene(DrawScene *scene) { m_pScene = scene; }
	DrawScene *GetScene() { return m_pScene; }

public:
	virtual void Move(const QPointF &point);
	virtual void Control(int direct, const QPointF &delta);
	virtual void Stretch(int handle, double sx, double sy, const QPointF &origin);
	virtual bool SaveToXml(QXmlStreamWriter *xml);
	virtual bool LoadFromXml(QXmlStreamReader *xml);
	virtual void UpdateHandles();
	virtual void UpdateCoordinate();
	virtual QString DisplayName() const { return tr("多边形"); }	
	virtual QGraphicsItem* Duplicate();

	virtual QRectF boundingRect() const;
	virtual QPainterPath Shape() const;
	virtual void AddPoint(const QPointF &point);
	virtual void EndPoint(const QPointF &point);
	virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

public:
	QPolygonF m_points;
	QPolygonF m_initialPoints;

private:
	DrawScene *m_pScene;

};

///////////////////////// GraphicsLineItem /////////////////////////
class GraphicsLineItem : public GraphicsPolygonItem
{
public:
	GraphicsLineItem(QGraphicsItem * parent = 0);
	~GraphicsLineItem();

public:
	virtual void Move(const QPointF &point);
	virtual void Control(int direct, const QPointF &delta);
	virtual void Stretch(int handle, double sx, double sy, const QPointF &origin);
	virtual bool SaveToXml(QXmlStreamWriter *xml);
	virtual bool LoadFromXml(QXmlStreamReader *xml);
	virtual void UpdateHandles();
	virtual void UpdateCoordinate();
	virtual int HandleCount() const { return m_handles.size() + Handle_Left;}
	virtual QString DisplayName() const { return tr("线段"); }	
	virtual QGraphicsItem* Duplicate();

	virtual QPointF Opposite(int handle);
	virtual QPainterPath Shape() const;
	virtual void AddPoint(const QPointF &point);
	virtual void EndPoint(const QPointF &point);
	virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

};

#endif // DRAWOBJ_H
