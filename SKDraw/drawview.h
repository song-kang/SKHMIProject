#ifndef DRAWVIEW_H
#define DRAWVIEW_H

#include "skhead.h"
#include "rulebar.h"

class SKDraw;
class DrawView : public QGraphicsView
{
	Q_OBJECT

public:
	DrawView(QGraphicsScene *scene);
	~DrawView();

	void SetApp(SKDraw *app) { m_app = app; }
	void SetMouseTranslate(bool b) { m_bMouseTranslate = b; }

public:
	void ZoomIn();
	void ZoomOut();
	void Translate(QPointF delta);

protected:
	virtual void wheelEvent(QWheelEvent *event);
	virtual void resizeEvent(QResizeEvent *event);
	virtual void keyPressEvent(QKeyEvent *event);
	virtual void keyReleaseEvent(QKeyEvent *event);
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

private:
	void UpdateRuler();

signals:
	void SigPositionChanged(int x ,int y);

private:
	SKDraw *m_app;
	QGraphicsScene *m_pScene;

private slots:
	void SlotScrollBarValueChanged(int pos);
	
};

#endif // DRAWVIEW_H
