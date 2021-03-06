#ifndef DRAWOBJ_H
#define DRAWOBJ_H

#include "skhead.h"

#ifdef SKBASEWIDGET_LIB
# define SKBASEWIDGET_EXPORT Q_DECL_EXPORT
#else
# define SKBASEWIDGET_EXPORT Q_DECL_IMPORT
#endif

class DrawScene;
///////////////////////// AbstractShapeType /////////////////////////
template <typename BaseType = QGraphicsItem>
class AbstractShapeType : public BaseType
{
public:
	explicit AbstractShapeType(QGraphicsItem * parent = 0)
		:BaseType(parent)
	{
		m_width = 0.0;
		m_height = 0.0;
		m_pen = QPen(Qt::white);
		m_pen.setWidthF(1.0);
		m_pen.setStyle(Qt::SolidLine);
		m_pen.setJoinStyle(Qt::RoundJoin);
		m_pen.setCapStyle(Qt::RoundCap);
		m_brush = QBrush(Qt::NoBrush);
		m_iShowType = 0;
		m_iShowState = -1;
		m_iRealState = -1;
		m_isFlash = false;
		m_isShow = true;
	}
	virtual ~AbstractShapeType(){}

	QPen   GetPen() const { return m_pen; }
	QColor GetPenColor() const { return m_pen.color(); }
	qreal  GetPenWidth() const { return m_pen.widthF(); }
	Qt::PenStyle GetPenStyle() const { return m_pen.style(); }
	QBrush GetBrush() const { return m_brush; }
	QColor GetBrushColor() const { return m_brush.color(); }
	Qt::BrushStyle GetBrushStyle() const { return m_brush.style(); }
	qreal  GetWidth() const { return m_width ; }
	qreal  GetHeight() const {return m_height;}
	QRectF GetRect() const { return m_localRect; }
	qint32 GetShowType() { return m_iShowType; }
	qint32  GetShowState() { return m_iShowState; }
	qint32 GetRealState() { return m_iRealState; }
	QString GetLinkDB() { return m_sLinkDB; }
	QString GetLinkScene() { return m_sLinkScene; }
	bool GetIsFlash() { return m_isFlash; } 
	QString GetCustom() { return m_sCustom; }
	bool GetIsShow() { return m_isShow; }

	void SetPen(const QPen & pen) { m_pen = pen; }
	void SetPenColor(const QColor & color) { m_pen.setColor(color); update(); }
	void SetPenWidth(const qreal width) { m_pen.setWidthF(width); update(); }
	void SetPenStyle(const Qt::PenStyle style) { m_pen.setStyle(style); update(); }
	void SetBrush(const QBrush & brush ) { m_brush = brush; }
	void SetBrushColor(const QColor & color ) { m_brush.setColor(color); update(); }
	void SetBrushStyle(const Qt::BrushStyle style) { m_brush.setStyle(style); update(); }
	void SetWidth(qreal width) { m_width = width; UpdateCoordinate(); }
	void SetHeight(qreal height) { m_height = height; UpdateCoordinate(); }
	void SetRect(QRectF rect) { m_localRect = rect; }
	void SetShowType(qint32 type) { m_iShowType = type; }
	void SetShowState(qint32 state) { m_iShowState = state; }
	void SetLinkDB(QString link) { m_sLinkDB = link; }
	void SetRealState(qint32 state) { m_iRealState = state; }
	void SetLinkScene(QString scene) { m_sLinkScene = scene; }
	void SetIsFlash(bool isFlash) { m_isFlash = isFlash; } 
	void SetCustom(QString custom) { m_sCustom = custom; }
	void SetIsShow(bool isShow) { m_isShow = isShow; }

public:
	QPen   m_pen;
	QBrush m_brush;
	qreal  m_width;
	qreal  m_height;
	QRectF m_localRect;
	qint32 m_iShowType;
	qint32 m_iShowState;
	qint32 m_iRealState;
	QString m_sLinkDB;
	QString m_sLinkScene;
	bool m_isFlash;
	QMap<int,QString> m_mapShowStyle;
	QString m_sCustom;
	bool m_isShow;

public:
	virtual bool LoadFromXml(QXmlStreamReader *xml) = 0;
	virtual void UpdateCoordinate() = 0;
	virtual QString DisplayName() const { return QString("抽象图元"); }	

};

typedef AbstractShapeType <QGraphicsItem> AbstractShape;

///////////////////////// GraphicsItem /////////////////////////
class SKBASEWIDGET_EXPORT GraphicsItem : public QObject, public AbstractShapeType <QGraphicsItem>
{
	Q_OBJECT

public:
	GraphicsItem(QGraphicsItem * parent);
	~GraphicsItem();

	enum { Type = UserType + 1 };
	virtual int type() const { return Type; }
	void Image(QPainter *painter, QPointF point);

	void SetScene(DrawScene *scene) { m_pScene = scene; }
	void SetName(QString name) { m_sName = name; }
	void SetScale(qreal val) { setScale(val); }
	void SetTooltip(QString tip) { setToolTip(tip); }
	void SetRotation(qreal angle) { setRotation(angle); }

	DrawScene* GetScene() { return m_pScene; }
	QString GetName() { return m_sName; }
	qreal GetScale() { return scale(); }
	QString GetTooltip() { return toolTip(); }
	qreal GetRotation() { return rotation(); }

public:
	virtual void contextMenuEvent(QGraphicsSceneContextMenuEvent *event);
	virtual void hoverEnterEvent(QGraphicsSceneHoverEvent *event);
	virtual void hoverLeaveEvent(QGraphicsSceneHoverEvent *event);
	virtual void hoverMoveEvent(QGraphicsSceneHoverEvent *event);

private:
	DrawScene *m_pScene;
	QString m_sName;

public:
	bool ReadBaseAttributes(QXmlStreamReader *xml);
	bool WriteBaseAttributes(QXmlStreamWriter *xml);
	void SetStyleFromState(int state);

signals:
	void SigSelectedChange(QGraphicsItem *item);
	void SigAttribute(QString rotation,QString scale);

private slots:
	void SlotAttribute(QString rotation,QString scale);

};

///////////////////////// GraphicsPolygonItem /////////////////////////
class GraphicsPolygonItem : public GraphicsItem
{
public:
	GraphicsPolygonItem(QGraphicsItem * parent = 0);
	~GraphicsPolygonItem();

public:
	virtual void UpdateCoordinate();
	virtual QRectF boundingRect() const;
	virtual QPainterPath shape() const;
	virtual void AddPoint(const QPointF &point);
	virtual void EndPoint(const QPointF &point);
	virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
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
	virtual void timerEvent(QTimerEvent *event);
	virtual QPainterPath shape() const;
	virtual void AddPoint(const QPointF &point);
	virtual void EndPoint(const QPointF &point);
	virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
	virtual bool LoadFromXml(QXmlStreamReader *xml);

public:
	int m_iStepCnt;
	float m_fStep;
	float m_fScale;
	QPolygonF m_arrowPoints;

};

///////////////////////// GraphicsPolygonLineItem /////////////////////////
class GraphicsPolygonLineItem : public GraphicsPolygonItem
{
public:
	GraphicsPolygonLineItem(QGraphicsItem * parent = 0);
	~GraphicsPolygonLineItem();

public:
	virtual QPainterPath shape() const;
	virtual void EndPoint(const QPointF &point);
	virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
	virtual bool LoadFromXml(QXmlStreamReader *xml);

};

///////////////////////// GraphicsRectItem /////////////////////////
class GraphicsRectItem : public GraphicsItem
{
	Q_OBJECT

public:
	GraphicsRectItem(const QRect &rect, bool isRound = false, QGraphicsItem * parent = 0);
	~GraphicsRectItem();

	void SetRound(QSize size) { m_round = size; update(); }
	QSize GetRound() { return m_round; }
	QRectF GetLocalRect() const { return m_localRect; }

public:
	virtual void UpdateCoordinate();
	virtual QRectF boundingRect() const;
	virtual QPainterPath shape() const;
	virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
	virtual bool LoadFromXml(QXmlStreamReader *xml);

public:
	bool m_isRound;
	QSize m_round;
	QRectF m_initialRect;
	QPointF m_opposite;
	QPointF m_originPoint;

};

///////////////////////// GraphicsTriangleItem /////////////////////////
class GraphicsTriangleItem : public GraphicsRectItem
{
public:
	GraphicsTriangleItem(const QRect &rect, GraphicsRectItem * parent = 0);
	~GraphicsTriangleItem();

	QRectF GetLocalRect() const { return m_localRect; }

public:
	virtual QPainterPath shape() const;
	virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
	virtual bool LoadFromXml(QXmlStreamReader *xml);

public:
	QPolygonF m_points;
};

///////////////////////// GraphicsRhombusItem /////////////////////////
class GraphicsRhombusItem : public GraphicsRectItem
{
public:
	GraphicsRhombusItem(const QRect &rect, GraphicsRectItem * parent = 0);
	~GraphicsRhombusItem();

	QRectF GetLocalRect() const { return m_localRect; }

public:
	virtual QPainterPath shape() const;
	virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
	virtual bool LoadFromXml(QXmlStreamReader *xml);

public:
	QPolygonF m_points;
};

///////////////////////// GraphicsParallelogramItem /////////////////////////
class GraphicsParallelogramItem : public GraphicsRectItem
{
public:
	GraphicsParallelogramItem(const QRect &rect, GraphicsRectItem * parent = 0);
	~GraphicsParallelogramItem();

	QRectF GetLocalRect() const { return m_localRect; }

public:
	virtual QPainterPath shape() const;
	virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
	virtual bool LoadFromXml(QXmlStreamReader *xml);

public:
	QPolygonF m_points;
	qreal m_angle;
};

///////////////////////// GraphicsEllipseItem /////////////////////////
class GraphicsEllipseItem :public GraphicsRectItem
{
public:
	GraphicsEllipseItem(const QRect &rect, bool isCircle = false, QGraphicsItem *parent = 0);
	~GraphicsEllipseItem();

public:
	virtual QRectF boundingRect() const;
	virtual QPainterPath shape() const;
	virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
	virtual bool LoadFromXml(QXmlStreamReader *xml);

public:
	bool m_isCircle;

};

///////////////////////// GraphicsArcEllipseItem /////////////////////////
class GraphicsArcEllipseItem :public GraphicsRectItem
{
public:
	GraphicsArcEllipseItem(const QRect &rect, bool isCircle = false, QGraphicsItem *parent = 0);
	~GraphicsArcEllipseItem();

public:
	virtual QRectF boundingRect() const;
	virtual QPainterPath shape() const;
	virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
	virtual bool LoadFromXml(QXmlStreamReader *xml);

public:
	bool m_isCircle;
	qreal m_startAngle;
	qreal m_endAngle;

};

///////////////////////// GraphicsTextItem /////////////////////////
class GraphicsTextItem :public GraphicsRectItem
{
	Q_OBJECT

public:
	GraphicsTextItem(const QRect &rect, QGraphicsItem *parent = 0);
	~GraphicsTextItem();

	void SetFont(QFont font) { m_font = font; update(); }
	void SetFontColor(QColor c) { m_pen.setColor(c); update(); }
	void SetText(QString text) { m_text = text; update(); }
	void SetOption(QTextOption option) { m_option = option; }
	QFont GetFont() { return m_font; }
	QString GetText() { return m_text; }
	QTextOption GetOption() { return m_option; }

public:
	virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
	virtual bool LoadFromXml(QXmlStreamReader *xml);

public:
	void SetStyle(QString sFont, QString text);

public:
	QFont m_font;
	QString m_text;
	QTextOption m_option;

};

///////////////////////// GraphicsTextTimeItem /////////////////////////
class GraphicsTextTimeItem :public GraphicsTextItem
{
public:
	GraphicsTextTimeItem(const QRect &rect, QGraphicsItem *parent = 0);
	~GraphicsTextTimeItem();

	void SetStyle(QString style) { m_sStyle = style; }
	QString GetStyle() { return m_sStyle; }

public:
	virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
	virtual bool LoadFromXml(QXmlStreamReader *xml);

public:
	QString m_sStyle;

};

///////////////////////// GraphicsPictureItem /////////////////////////
class GraphicsPictureItem :public GraphicsRectItem
{
public:
	GraphicsPictureItem(const QRect &rect, QString fileName, QGraphicsItem *parent = 0);
	~GraphicsPictureItem();

	QString GetFileName() { return m_fileName; }

	void LoadPicture(int sn);
	void LoadGif(int sn);

public:
	virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
	virtual bool LoadFromXml(QXmlStreamReader *xml);

public:
	QPixmap m_picture;
	QString m_fileName;
	QMovie *m_gif;

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
	QPixmap Image();
	
public:
	virtual void UpdateCoordinate();
	virtual QString DisplayName() const { return tr("组合图元"); }
	virtual QRectF boundingRect() const;
	virtual bool LoadFromXml(QXmlStreamReader *xml);

protected:
	QGraphicsItem *m_parent;
	QRectF m_itemsBoundingRect;
	QRectF m_initialRect;

protected:
	GraphicsItemGroup *CreateGroup(const QList<QGraphicsItem *> &items) const;
	virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = 0);

signals:
	void SigSelectedChange(QGraphicsItem *item);
	void SigAttribute(QString rotation,QString scale);

private slots:
	void SlotAttribute(QString rotation,QString scale);

};

#endif // DRAWOBJ_H
