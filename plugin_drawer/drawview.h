#ifndef DRAWVIEW_H
#define DRAWVIEW_H

#include "skhead.h"
#include "drawobj.h"

class view_plugin_drawer;
class DrawView : public QGraphicsView
{
	Q_OBJECT

public:
	DrawView(QGraphicsScene *scene);
	~DrawView();

	void SetApp(view_plugin_drawer *app) { m_app = app; }
	void SetMouseTranslate(bool b) { m_bMouseTranslate = b; }
	view_plugin_drawer* GetApp() { return m_app; }

	bool Load(char *content);
	bool Load(const QString fileName);

public:
	void ZoomIn();
	void ZoomOut();
	void ZoomOrg();
	void Translate(QPointF delta);

protected:
	virtual void wheelEvent(QWheelEvent *event);
	virtual void resizeEvent(QResizeEvent *event);
	virtual void keyPressEvent(QKeyEvent *event);
	virtual void keyReleaseEvent(QKeyEvent *event);
	virtual void mouseMoveEvent(QMouseEvent *event);
	virtual void mousePressEvent(QMouseEvent *event);
	virtual void mouseReleaseEvent(QMouseEvent *event);
	virtual void drawBackground(QPainter *painter, const QRectF &rect);

private:
	QPoint m_lastMousePos;
	bool m_bMouseTranslate;
	qreal m_scale;
	qreal m_zoomDelta;
	bool m_isUntitled;
	QString m_sFileName;
	QString m_sSymbolName;
	QCursor m_cursorSymbol;
	int m_iSceneWidth;
	int m_iSceneHeight;
	int m_iZoomLevel;
	bool m_bScaleToScreen;

private:
	bool LoadCanvas(QXmlStreamReader *xml);
	void LoadSymbol(QPoint point);
	void ScaleToScreenWidth();
	void ScaleToScreenHeight();
	void LoadPicture(QXmlStreamReader *xml, AbstractShape *shape);

public:
	GraphicsItemGroup* LoadGroupFromXML(QXmlStreamReader *xml);

signals:
	void SigPositionChanged(int x, int y);
	void SigMouseRightButton(QPoint p);

private:
	view_plugin_drawer *m_app;
	QGraphicsScene *m_pScene;

private slots:
	void SlotScrollBarValueChanged(int pos);
	
};

#endif // DRAWVIEW_H
