#ifndef DRAWSCENE_H
#define DRAWSCENE_H

#include "skhead.h"
#include "drawobj.h"

#ifdef SKBASEWIDGET_LIB
# define SKBASEWIDGET_EXPORT Q_DECL_EXPORT
#else
# define SKBASEWIDGET_EXPORT Q_DECL_IMPORT
#endif

///////////////////////// GridTool /////////////////////////
class GridTool
{
public:
	GridTool(const QSize &grid = QSize(8000,6000), const QSize &space = QSize(20,20));
	~GridTool();

	void SetBackColor(QColor c) { m_backColor = c; }
	QColor GetBackColor() { return m_backColor; }

public:
	void PaintGrid(QPainter *painter, const QRect &rect);

private:
	QSize m_sizeGrid;
	QSize m_sizeGridSpace;
	QColor m_backColor;
};

///////////////////////// DrawScene /////////////////////////
class SKBASEWIDGET_EXPORT DrawScene : public QGraphicsScene
{
	Q_OBJECT

public:
	DrawScene(QObject *parent = NULL);
	~DrawScene();

	void SetView(QGraphicsView *view ) { m_pView = view; }
	void SetWidth(int w) { m_iWidth = w; }
	void SetHeight(int h) { m_iHeight = h; }
	void SetPressShift(bool b) { m_bPressShift = b; }

	bool GetPressShift() { return m_bPressShift; }
	QGraphicsView* GetView() { return m_pView; }
	int GetWidth() { return m_iWidth; }
	int GetHeight() { return m_iHeight; }
	GridTool* GetGridTool() { return m_pGrid; }
	bool GetSwap() { return m_bSwap; }

	GraphicsItemGroup* CreateGroup(const QList<QGraphicsItem *> &items, bool isAdd = true);

public:
	void MouseEvent(QGraphicsSceneMouseEvent *mouseEvent);

protected:
	virtual void drawBackground(QPainter *painter, const QRectF &rect);
	virtual void mousePressEvent(QGraphicsSceneMouseEvent *mouseEvent);
	virtual void mouseMoveEvent(QGraphicsSceneMouseEvent *mouseEvent);
	virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent *mouseEvent);
	virtual void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *mouseEvent);
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

public:
	QTimer *m_pSwapIntervalTimer;
	QTimer *m_pGifTimer;
	bool m_bSwap;

private slots:
	void SlotSelectionChanged();
	void SlotSwapIntervalTimer();
	void SlotGifTimer();
	
};

#endif // DRAWSCENE_H
