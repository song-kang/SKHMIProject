#ifndef DRAWVIEW_H
#define DRAWVIEW_H

#include "skhead.h"
#include "drawscene.h"
#include "drawobj.h"
#include "sk_database.h"

#ifdef SKBASEWIDGET_LIB
# define SKBASEWIDGET_EXPORT Q_DECL_EXPORT
#else
# define SKBASEWIDGET_EXPORT Q_DECL_IMPORT
#endif

#define LINKDB_NONE			0
#define LINKDB_STATE		1
#define LINKDB_MEASURE		2

#define SVG_TYPE_ITEM		1
#define SVG_TYPE_3DITEM		2
#define SVG_TYPE_PIX		3
#define SVG_TYPE_GIF		4

class SKBASEWIDGET_EXPORT DrawView : public QGraphicsView
{
	Q_OBJECT

public:
	DrawView(QGraphicsScene *scene = NULL);
	~DrawView();

	bool Load(char *content);
	bool Load(const QString fileName);
	void ZoomIn();
	void ZoomOut();
	void ZoomOrg();
	void Translate(QPointF delta);
	void RefreshStateFromDB();
	void RefreshMeasureFromDB();
	void RefreshStateByKey(QString key,int val);
	void ClearSelection() { m_pScene->clearSelection(); }
	void Redraw() { viewport()->update(); }
	GraphicsItem *SearchItemByCustom(QString custom);

public:
	virtual void OnClicked(QList<QGraphicsItem*> list) {};

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
	DrawScene *m_pScene;
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
	QMap<QString, QList<GraphicsItem *>*> m_mapLinkDBState;
	QMap<QString, QList<GraphicsItem *>*> m_mapLinkDBMeasure;

private:
	bool LoadCanvas(QXmlStreamReader *xml);
	void LoadSymbol(QPoint point);
	void ScaleToScreenWidth();
	void ScaleToScreenHeight();
	void LoadPicture(QXmlStreamReader *xml, AbstractShape *shape);
	void InitDrawobj();
	void InsertMapLinkDBState(QString key, GraphicsItem *item);
	void InsertMapLinkDBMeasure(QString key, GraphicsItem *item);

public:
	GraphicsItemGroup* LoadGroupFromXML(QXmlStreamReader *xml);

signals:
	void SigPositionChanged(int x, int y);
	void SigMouseRightButton(QPoint p);
	
};

#endif // DRAWVIEW_H
