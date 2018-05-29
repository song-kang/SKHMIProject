#ifndef DRAWSCENE_H
#define DRAWSCENE_H

#include "skhead.h"

///////////////////////// GridTool /////////////////////////
class GridTool
{
public:
	GridTool(const QSize &grid = QSize(8000,6000), const QSize &space = QSize(20,20));
	~GridTool();

	void SetBackColor(QColor c) { m_backColor = c; }

public:
	void PaintGrid(QPainter *painter, const QRect &rect);

private:
	QSize m_sizeGrid;
	QSize m_sizeGridSpace;
	QColor m_backColor;
};

///////////////////////// DrawScene /////////////////////////
class DrawTool;
class DrawScene : public QGraphicsScene
{
	Q_OBJECT

	friend class DrawRectTool;
	friend class DrawPolygonTool;

public:
	DrawScene(QObject *parent);
	~DrawScene();

	void SetView(QGraphicsView *view ) { m_pView = view; }
	void SetWidth(int w) { m_iWidth = w; }
	void SetHeight(int h) { m_iHeight = h; }
	void SetPressShift(bool b) { m_bPressShift = b; }
	bool GetPressShift() { return m_bPressShift; }
	QGraphicsView *GetView() { return m_pView; }

public:
	void MouseEvent(QGraphicsSceneMouseEvent *mouseEvent);

protected:
	virtual void drawBackground(QPainter *painter, const QRectF &rect);
	virtual void mousePressEvent(QGraphicsSceneMouseEvent *mouseEvent);
	virtual void mouseMoveEvent(QGraphicsSceneMouseEvent *mouseEvent);
	virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent *mouseEvent);
	virtual void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *mouseEvet);
	virtual void keyPressEvent(QKeyEvent *e);
	virtual void keyReleaseEvent(QKeyEvent *e);

private:
	GridTool *m_pGrid;
	QGraphicsView *m_pView;
	int	m_iWidth;
	int m_iHeight;
	bool m_bPressShift;
	qreal m_dx;
	qreal m_dy;
	bool m_bMoved;

signals:
	void itemAdded(QGraphicsItem *item);
	void itemMoved(QGraphicsItem *item, const QPointF &oldPosition);
	void itemRotate(QGraphicsItem *item, const qreal oldAngle);
	void itemResize(QGraphicsItem *item , int handle, const QPointF &scale);
	void itemControl(QGraphicsItem *item , int handle, const QPointF &newPos, const QPointF &lastPos_);
	
};

#endif // DRAWSCENE_H
