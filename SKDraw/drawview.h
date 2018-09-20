#ifndef DRAWVIEW_H
#define DRAWVIEW_H

#include "skhead.h"
#include "rulebar.h"
#include "drawobj.h"

#define SAVE_MODE_DB		1
#define SAVE_MODE_FILE		2

class SKDraw;
class DrawView : public QGraphicsView
{
	Q_OBJECT

public:
	DrawView(QGraphicsScene *scene);
	~DrawView();

	void SetApp(SKDraw *app) { m_app = app; }
	void SetMouseTranslate(bool b) { m_bMouseTranslate = b; }
	SKDraw* GetApp() { return m_app; }

	bool Save();
	bool SaveAs();
	bool SaveFile(const QString fileName);
	bool SaveDB();
	bool LoadFile(const QString fileName);
	bool Load(char *content);
	void SetSymbolName(QString name) { m_sSymbolName = name; }
	void SetSymbolCursor(QCursor cursor) { m_cursorSymbol = cursor; }
	void SetModified(bool b) { m_isModified = b; }
	void SetSaveMode(int mode) { m_iSaveMode = mode; }
	bool GetIsModified() { return m_isModified; }
	int  GetSaveMode() { return m_iSaveMode; }

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

private:
	QtRuleBar *m_pHorRuler;
	QtRuleBar *m_pVerRuler;
	QtCornerBox *m_pBox;
	QPoint m_lastMousePos;
	bool m_bMouseTranslate;
	qreal m_scale;
	qreal m_zoomDelta;
	bool m_isUntitled;
	bool m_isModified;
	QString m_sFileName;
	QString m_sSymbolName;
	QCursor m_cursorSymbol;
	int m_iSaveMode;

private:
	void UpdateRuler();
	bool LoadCanvas(QXmlStreamReader *xml);
	void LoadSymbol(QPoint point);

public:
	GraphicsItemGroup* LoadGroupFromXML(QXmlStreamReader *xml);

signals:
	void SigPositionChanged(int x, int y);
	void SigMouseRightButton(QPoint p);

private:
	SKDraw *m_app;
	QGraphicsScene *m_pScene;

signals:
	void SigSaveDB();

private slots:
	void SlotScrollBarValueChanged(int pos);
	bool SlotSaveDB();
	
};

#endif // DRAWVIEW_H
