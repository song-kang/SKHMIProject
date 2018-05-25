#ifndef DRAWVIEW_H
#define DRAWVIEW_H

#include "skhead.h"
#include "rulebar.h"

class DrawView : public QGraphicsView
{
	Q_OBJECT

public:
	DrawView(QGraphicsScene *scene);
	~DrawView();

public:
	void ZoomIn();
	void ZoomOut();
	void UpdateRuler();
	void Translate(QPointF delta);
	//void KeyPressEvent(QKeyEvent *event);

protected:
	virtual void wheelEvent(QWheelEvent *event);
	virtual void resizeEvent(QResizeEvent *event);
	virtual void keyPressEvent(QKeyEvent *event);
	virtual void mouseMoveEvent(QMouseEvent *event);
	virtual void mousePressEvent(QMouseEvent *event);
	virtual void mouseReleaseEvent(QMouseEvent *event);

private:
	QtRuleBar *m_pHorRuler;
	QtRuleBar *m_pVerRuler;
	QtCornerBox *m_pBox;
	QPoint m_lastMousePos;
	bool m_bMouseTranslate;
	qreal m_scale;
	qreal m_zoomDelta;

signals:
	void SigPositionChanged(int x ,int y);
	
};

#endif // DRAWVIEW_H
