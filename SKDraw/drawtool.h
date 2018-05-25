#ifndef DRAWTOOL_H
#define DRAWTOOL_H

#include <QObject>
#include "skhead.h"
#include "drawscene.h"
#include "drawobj.h"

enum DrawShape
{
	eDrawSelection,
	eDrawRotation,
	eDrawLine,
	eDrawRectangle,
	eDrawRoundrect,
	eDrawCiricle,
	eDrawEllipse,
	eDrawPolygon,
	eDrawPolyline,
};

///////////////////////// DrawTool ////////////////////////
class DrawTool : public QObject
{
	Q_OBJECT

public:
	DrawTool(DrawShape shape);
	~DrawTool();

	DrawShape m_drawShape;

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
	QGraphicsPathItem *m_pDashRect;
	//GraphicsItemGroup *selLayer;
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
	qreal m_lastAngle;
	QGraphicsPathItem *m_pDashRect;
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
	GraphicsItem *item;
};

///////////////////////// DrawPolygonTool ////////////////////////
class DrawPolygonTool : public DrawTool
{
public:
	DrawPolygonTool(DrawShape shape );
	~DrawPolygonTool();

public:
	virtual void mousePressEvent(QGraphicsSceneMouseEvent *event, DrawScene *scene);
	virtual void mouseMoveEvent(QGraphicsSceneMouseEvent *event, DrawScene *scene);
	virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent *event, DrawScene *scene);
	virtual void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event,DrawScene *scene);

public:
	int m_nPoints;	
	QPointF initialPositions;
	GraphicsPolygonItem *item;

};

#endif // DRAWTOOL_H
