#ifndef DRAWTOOL_H
#define DRAWTOOL_H

#include <QObject>
#include "skhead.h"
#include "drawobj.h"

enum DrawShape
{
	eDrawSelection,
	eDrawSelectionArea,
	eDrawRotation,
	eDrawLine,
	eDrawRectangle,
	eDrawRoundrect,
	eDrawCircle,
	eDrawEllipse,
	eDrawPolygon,
	eDrawPolyline,
	eDrawText,
	eDrawPicture,
};

///////////////////////// DrawTool ////////////////////////
class DrawTool : public QObject
{
	Q_OBJECT

public:
	DrawTool(DrawShape shape);
	~DrawTool();

	DrawShape m_drawShape;
	bool m_bHoverSizer;

public:
	virtual void mousePressEvent(QGraphicsSceneMouseEvent *event, DrawScene *scene);
	virtual void mouseMoveEvent(QGraphicsSceneMouseEvent *event, DrawScene *scene);
	virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent *event, DrawScene *scene);
	virtual void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event,DrawScene *scene);

public:
	static QList<DrawTool*> c_tools;
	static QPointF c_down;
	static QPointF c_last;
	static DrawShape c_drawShape;
	static DrawTool* findTool(DrawShape drawShape);
};

///////////////////////// DrawSelectTool ////////////////////////
class DrawSelectTool : public DrawTool
{
	Q_OBJECT

public:
	DrawSelectTool();
	~DrawSelectTool();

public:
	virtual void mousePressEvent(QGraphicsSceneMouseEvent *event, DrawScene *scene);
	virtual void mouseMoveEvent(QGraphicsSceneMouseEvent *event, DrawScene *scene);
	virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent *event, DrawScene *scene);

public:
	QPointF m_initialPositions;
	QPointF m_opposite;
	//GraphicsItemGroup *selLayer;

private:
	void UpdatePropertyEditor(DrawScene *scene, GraphicsItem *item);
};

///////////////////////// DrawRotationTool ////////////////////////
class DrawRotationTool : public DrawTool
{
public:
	DrawRotationTool();
	~DrawRotationTool();

public:
	virtual void mousePressEvent(QGraphicsSceneMouseEvent *event, DrawScene *scene);
	virtual void mouseMoveEvent(QGraphicsSceneMouseEvent *event, DrawScene *scene);
	virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent *event, DrawScene *scene);

public:
	qreal m_oldAngle;
	qreal m_lastAngle;
	
};

///////////////////////// DrawRectTool ////////////////////////
class DrawRectTool : public DrawTool
{
public:
	DrawRectTool(DrawShape drawShape);
	~DrawRectTool();

public:
	virtual void mousePressEvent(QGraphicsSceneMouseEvent *event, DrawScene *scene);
	virtual void mouseMoveEvent(QGraphicsSceneMouseEvent *event, DrawScene *scene);
	virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent *event, DrawScene *scene);

public:
	GraphicsItem *m_pItem;
	QPointF m_opposite;
};

///////////////////////// DrawPolygonTool ////////////////////////
class DrawPolygonTool : public DrawTool
{
public:
	DrawPolygonTool(DrawShape shape);
	~DrawPolygonTool();

public:
	virtual void mousePressEvent(QGraphicsSceneMouseEvent *event, DrawScene *scene);
	virtual void mouseMoveEvent(QGraphicsSceneMouseEvent *event, DrawScene *scene);
	virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent *event, DrawScene *scene);
	virtual void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event,DrawScene *scene);

public:
	int m_nPoints;	
	QPointF m_initialPositions;
	GraphicsPolygonItem *m_pItem;

};

#endif // DRAWTOOL_H
