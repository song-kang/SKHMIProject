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
class DrawScene : public QGraphicsScene
{
	Q_OBJECT

public:
	DrawScene(QObject *parent);
	~DrawScene();

	void SetView(QGraphicsView *view ) { m_pView = view; }
	void SetWidth(int w) { m_iWidth = w; }
	void SetHeight(int h) { m_iHeight = h; }

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
	
};

#endif // DRAWSCENE_H
