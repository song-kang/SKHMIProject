#ifndef DRAWOBJ_H
#define DRAWOBJ_H

#include "skhead.h"
#include "sizehandle.h"

class DrawScene;
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

///////////////////////// AbstractShapeType /////////////////////////
template <typename BaseType = QGraphicsItem>
class AbstractShapeType : public BaseType
{
public:
	explicit AbstractShapeType(QGraphicsItem * parent = 0)
		:BaseType(parent)
	{
		m_penWidth = 1.0;
		m_penStyle = Qt::SolidLine;
		m_pen = QPen(Qt::white);
		m_pen.setWidthF(m_penWidth);
		m_pen.setStyle(m_penStyle);

		m_brushStyle = Qt::NoBrush;
		m_brush = QBrush(m_brushStyle);

		m_width = 0.0;
		m_height = 0.0;
	}
	virtual ~AbstractShapeType(){}

	QPen   GetPen() const { return m_pen; }
	QColor GetPenColor() const { return m_pen.color(); }
	qreal  GetPenWidth() const { return m_penWidth; }
	Qt::PenStyle GetPenStyle() const { return m_penStyle; }
	QBrush GetBrush() const { return m_brush; }
	QColor GetBrushColor() const { return m_brush.color(); }
	Qt::BrushStyle GetBrushStyle() const { return m_brush.style(); }
	qreal  GetWidth() const { return m_width ; }
	qreal  GetHeight() const {return m_height;}
	QRectF GetRect() const { return m_localRect; }

	void SetPen(const QPen & pen) { m_pen = pen; }
	void SetPenColor(const QColor & color) { m_pen.setColor(color); update(); }
	void SetPenWidth(const qreal width) { m_pen.setWidthF(width); update(); }
	void SetPenStyle(const Qt::PenStyle style) { m_pen.setStyle(style); update(); }
	void SetBrush(const QBrush & brush ) { m_brush = brush ; }
	void SetBrushColor(const QColor & color ) { m_brush.setColor(color); update(); }
	void SetBrushStyle(const Qt::BrushStyle style) { m_brush.setStyle(style); update(); }
	void SetWidth(qreal width) { m_width = width; UpdateCoordinate(); }
	void SetHeight(qreal height) { m_height = height; UpdateCoordinate(); }
	void SetRect(QRectF rect) { m_localRect = rect; }

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

		return eHandleNone;
	}

	QPointF Opposite(int handle)
	{
		QPointF pt;
		switch (handle)
		{
		case eHandleRight:
			pt = m_handles.at(eHandleLeft-1)->pos();
			break;
		case eHandleRightTop:
			pt = m_handles[eHandleLeftBottom-1]->pos();
			break;
		case eHandleRightBottom:
			pt = m_handles[eHandleLeftTop-1]->pos();
			break;
		case eHandleLeftBottom:
			pt = m_handles[eHandleRightTop-1]->pos();
			break;
		case eHandleBottom:
			pt = m_handles[eHandleTop-1]->pos();
			break;
		case eHandleLeftTop:
			pt = m_handles[eHandleRightBottom-1]->pos();
			break;
		case eHandleLeft:
			pt = m_handles[eHandleRight-1]->pos();
			break;
		case eHandleTop:
			pt = m_handles[eHandleBottom-1]->pos();
			break;
		}

		return pt;
	}

	void DrawOutline(QPainter *painter)
	{
		qreal itemPenWidth = GetPen().widthF();
		const qreal pad = itemPenWidth / 2;
		const qreal penWidth = 0;

		painter->setPen(QPen(Qt::black, penWidth, Qt::SolidLine));
		painter->setBrush(Qt::NoBrush);
		painter->drawRect(boundingRect().adjusted(pad, pad, -pad, -pad));

		painter->setPen(QPen(Qt::white, 0, Qt::DotLine));
		painter->setBrush(Qt::NoBrush);
		painter->drawRect(boundingRect().adjusted(pad, pad, -pad, -pad));
	}

public:
	QPen   m_pen;
	qreal  m_penWidth;
	Qt::PenStyle m_penStyle;
	QBrush m_brush;
	Qt::BrushStyle m_brushStyle;

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
	virtual QString DisplayName() const { return QString("抽象图元"); }	
	virtual QGraphicsItem* Duplicate() = 0;

};

typedef AbstractShapeType <QGraphicsItem> AbstractShape;

///////////////////////// GraphicsItem /////////////////////////
class GraphicsItem : public QObject, public AbstractShapeType <QGraphicsItem>
{
	Q_OBJECT

	Q_PROPERTY(QPointF Position READ pos WRITE setPos)
	Q_PROPERTY(qreal Width READ GetWidth WRITE SetWidth)
	Q_PROPERTY(qreal Height READ GetHeight WRITE SetHeight)
	Q_PROPERTY(QColor PenColor READ GetPenColor WRITE SetPenColor)
	Q_PROPERTY(qreal PenWidth READ GetPenWidth WRITE SetPenWidth)
	Q_PROPERTY(Qt::PenStyle PenStyle READ GetPenStyle WRITE SetPenStyle)
	Q_PROPERTY(QColor BrushColor READ GetBrushColor WRITE SetBrushColor)
	Q_PROPERTY(Qt::BrushStyle BrushStyle READ GetBrushStyle WRITE SetBrushStyle)

public:
	GraphicsItem(QGraphicsItem * parent);
	~GraphicsItem();

	enum { Type = UserType + 1 };
	virtual int type() const { return Type; }

	void SetScene(DrawScene *scene) { m_pScene = scene; }
	DrawScene* GetScene() { return m_pScene; }

public:
	virtual void UpdateHandles();
	virtual void contextMenuEvent(QGraphicsSceneContextMenuEvent *event);
	virtual QVariant itemChange(QGraphicsItem::GraphicsItemChange change, const QVariant &value);

private:
	DrawScene *m_pScene;

signals:
	void SigSelectedChange(QGraphicsItem *item);

};

///////////////////////// GraphicsPolygonItem /////////////////////////
class GraphicsPolygonItem : public GraphicsItem
{
public:
	GraphicsPolygonItem(QGraphicsItem * parent = 0);
	~GraphicsPolygonItem();

public:
	virtual void Move(const QPointF &point);
	virtual void Control(int direct, const QPointF &delta);
	virtual void Stretch(int handle, double sx, double sy, const QPointF &origin);
	virtual void UpdateHandles();
	virtual void UpdateCoordinate();
	virtual QString DisplayName() const { return tr("多边形图元"); }	
	virtual QGraphicsItem* Duplicate();

	virtual QRectF boundingRect() const;
	virtual QPainterPath Shape() const;
	virtual void AddPoint(const QPointF &point);
	virtual void EndPoint(const QPointF &point);
	virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

	virtual bool SaveToXml(QXmlStreamWriter *xml);
	virtual bool LoadFromXml(QXmlStreamReader *xml);

public:
	QPolygonF m_points;
	QPolygonF m_initialPoints;

};

///////////////////////// GraphicsLineItem /////////////////////////
class GraphicsLineItem : public GraphicsPolygonItem
{
public:
	GraphicsLineItem(QGraphicsItem * parent = 0);
	~GraphicsLineItem();

public:
	virtual void Control(int direct, const QPointF &delta);
	virtual void UpdateHandles();
	virtual int HandleCount() const { return m_handles.size() + eHandleLeft; }
	virtual QString DisplayName() const { return tr("线段图元"); }	
	virtual QGraphicsItem* Duplicate();

	virtual QPainterPath Shape() const;
	virtual void AddPoint(const QPointF &point);
	virtual void EndPoint(const QPointF &point);
	virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

	virtual bool SaveToXml(QXmlStreamWriter *xml);
	virtual bool LoadFromXml(QXmlStreamReader *xml);

};

///////////////////////// GraphicsPolygonLineItem /////////////////////////
class GraphicsPolygonLineItem : public GraphicsPolygonItem
{
public:
	GraphicsPolygonLineItem(QGraphicsItem * parent = 0);
	~GraphicsPolygonLineItem();

public:
	virtual void Control(int direct, const QPointF &delta);
	virtual QString DisplayName() const { return tr("折线图元"); }
	virtual QGraphicsItem *Duplicate();

	virtual QPainterPath Shape() const;
	virtual void EndPoint(const QPointF &point);
	virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

	virtual bool SaveToXml(QXmlStreamWriter *xml);
	virtual bool LoadFromXml(QXmlStreamReader *xml);

};

///////////////////////// GraphicsRectItem /////////////////////////
class GraphicsRectItem : public GraphicsItem
{
	Q_OBJECT

	Q_PROPERTY(QSize RoundRadius READ GetRound WRITE SetRound)

public:
	GraphicsRectItem(const QRect &rect, bool isRound = false, QGraphicsItem * parent = 0);
	~GraphicsRectItem();

	//void SetScene(DrawScene *scene) { m_pScene = scene; }
	void SetRound(QSize size) { m_round = size; }
	//void SetRx(qreal r) { m_rx = r; }
	//void SetRy(qreal r) { m_ry = r; }
	//DrawScene* GetScene() { return m_pScene; }
	QSize GetRound() { return m_round; }
	//qreal GetRx() { return m_rx; }
	//qreal GetRy() { return m_ry; }
	QRectF GetLocalRect() const { return m_localRect; }

public:
	virtual void Move(const QPointF & point);
	virtual void Control(int dir, const QPointF & delta);
	virtual void Stretch(int handle, double sx, double sy, const QPointF &origin);
	virtual void UpdateCoordinate();
	virtual QString DisplayName();
	virtual QGraphicsItem *Duplicate();

	virtual QRectF boundingRect() const;
	virtual QPainterPath Shape() const;
	virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

	virtual bool SaveToXml(QXmlStreamWriter *xml);
	virtual bool LoadFromXml(QXmlStreamReader *xml);

public:
	bool m_isRound;
	//qreal m_rx;
	//qreal m_ry;
	QSize m_round;
	QRectF m_initialRect;
	QPointF m_opposite;
	QPointF m_originPoint;

//private:
//	DrawScene *m_pScene;

};

///////////////////////// GraphicsEllipseItem /////////////////////////
class GraphicsEllipseItem :public GraphicsRectItem
{
public:
	GraphicsEllipseItem(const QRect &rect, bool isCircle = false, QGraphicsItem *parent = 0);
	~GraphicsEllipseItem();

public:
	virtual void Stretch(int handle, double sx, double sy, const QPointF &origin);
	virtual void UpdateHandles();
	virtual QString DisplayName();
	virtual QGraphicsItem *Duplicate();
	
	virtual QRectF boundingRect() const;
	virtual QPainterPath Shape() const;
	virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

	virtual bool SaveToXml(QXmlStreamWriter *xml);
	virtual bool LoadFromXml(QXmlStreamReader *xml);

public:
	bool m_isCircle;

};

///////////////////////// GraphicsTextItem /////////////////////////
class GraphicsTextItem :public GraphicsRectItem
{
	Q_OBJECT

	Q_PROPERTY(QString Text READ GetText WRITE SetText)

public:
	GraphicsTextItem(const QRect &rect, QGraphicsItem *parent = 0);
	~GraphicsTextItem();

	void SetFont(QFont font) { m_font = font; }
	void SetText(QString text) { m_text = text; }
	void SetOption(QTextOption option) { m_option = option; }
	QFont GetFont() { return m_font; }
	QString GetText() { return m_text; }
	QTextOption GetOption() { return m_option; }

public:
	virtual void Stretch(int handle, double sx, double sy, const QPointF &origin);
	virtual QString DisplayName() { return tr("文字图元"); }
	virtual QGraphicsItem *Duplicate();

	virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

	virtual bool SaveToXml(QXmlStreamWriter *xml);
	virtual bool LoadFromXml(QXmlStreamReader *xml);

public:
	QFont m_font;
	QString m_text;
	QTextOption m_option;

};

///////////////////////// GraphicsPictureItem /////////////////////////
class GraphicsPictureItem :public GraphicsRectItem
{
public:
	GraphicsPictureItem(const QRect &rect, QPixmap &pix, QGraphicsItem *parent = 0);
	~GraphicsPictureItem();

public:
	virtual void Stretch(int handle, double sx, double sy, const QPointF &origin);
	virtual QString DisplayName() { return tr("图像图元"); }
	virtual QGraphicsItem *Duplicate();

	virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

	virtual bool SaveToXml(QXmlStreamWriter *xml);
	virtual bool LoadFromXml(QXmlStreamReader *xml);

public:
	QPixmap m_picture;

};

///////////////////////// GraphicsItemGroup /////////////////////////
class GraphicsItemGroup : public QObject,
	public AbstractShapeType <QGraphicsItemGroup>
{
	Q_OBJECT

public:
	explicit GraphicsItemGroup(QGraphicsItem *parent = 0);
	~GraphicsItemGroup();

	enum { Type = UserType + 2 };
	int  type() const { return Type; }
	
public:
	virtual void Move(const QPointF & point) {};
	virtual void Control(int dir, const QPointF & delta) {};
	virtual void Stretch( int handle, double sx, double sy, const QPointF &origin);
	virtual void UpdateCoordinate();
	virtual void UpdateHandles();
	virtual QString DisplayName() const { return tr("组合图元"); }
	virtual QGraphicsItem *Duplicate();
	virtual QRectF boundingRect() const;

	virtual bool SaveToXml(QXmlStreamWriter *xml);
	virtual bool LoadFromXml(QXmlStreamReader *xml);

protected:
	QGraphicsItem *m_parent;
	QRectF m_itemsBoundingRect;
	QRectF m_initialRect;

protected:
	GraphicsItemGroup *CreateGroup(const QList<QGraphicsItem *> &items) const;
	QList<QGraphicsItem*> DuplicateItems() const;
	
	virtual QVariant itemChange(QGraphicsItem::GraphicsItemChange change, const QVariant &value);
	virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = 0);

signals:
	void SigSelectedChange(QGraphicsItem *item);

};

#endif // DRAWOBJ_H
