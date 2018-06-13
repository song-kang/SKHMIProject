#ifndef SKDRAW_H
#define SKDRAW_H

#include "ui_skdraw.h"
#include "skhead.h"
#include "skbasewidget.h"
#include "drawtool.h"
#include "drawscene.h"
#include "drawview.h"
#include "propertyeditor.h"

class SKDraw : public QMainWindow
{
	Q_OBJECT

public:
	SKDraw(QWidget *parent = 0, Qt::WFlags flags = 0);
	~SKDraw();

	void InitSlot();
	void Start();

	void SetApp(SKBaseWidget *app) { m_app = app; }
	PropertyEditor* GetPropertyEditor() { return m_pPropertyEditor; }
	QFont GetFont() { return m_font; }
	QColor GetFontColor() { return m_fontColor; }
	QPen GetPen() { return m_pen; }
	QBrush GetBrush() { return m_brush; }

public:
	void UpdateActions();

private:
	Ui::SKDrawClass ui;

	SKBaseWidget	*m_app;
	DrawView		*m_pView;
	DrawScene		*m_pScene;
	QUndoStack		*m_pUndoStack;
	QMenu			*m_pEditMenu;
	QObject			*m_pControlledObject;
	PropertyEditor	*m_pPropertyEditor;
	QFontComboBox   *m_pFontComboBox;
	QComboBox		*m_pFontSizeComboBox;
	QPushButton		*m_pFontColorBtn;
	QComboBox		*m_pPenStyleComboBox;
	QComboBox		*m_pPenWidthComboBox;
	QComboBox		*m_pBrushStyleComboBox;
	QPushButton		*m_pPenColorBtn;
	QPushButton		*m_pBrushColorBtn;

	QFont m_font;
	QColor m_fontColor;
	QPen m_pen;
	QBrush m_brush;

	QLabel *m_pLabelLogo;

private:
	void Init();
	void InitUi();
	DrawView* CreateView();

signals:
	void SigClose();

public slots:
	void SlotKeyEscape();

private slots:
	void SlotNew();
	void SlotOpen();
	void SlotSave();
	void SlotSaveas();
	void SlotClose();
	void SlotExit();

	void SlotCopy();
	void SlotCut();
	void SlotPaste();
	void SlotDelete();
	void SlotDataChanged();

	void SlotUndo();
	void SlotRedo();
	void SlotZoomin();
	void SlotZoomout();
	void SlotZoomorg();
	void SlotAddShape();
	void SlotAlign();
	void SlotBringToFront();
	void SlotSendToBack();
	void SlotGroup();
	void SlotUngroup();

	void SlotCurrentFontChanged(QFont font);
	void SlotFontSizeChanged(QString size);
	void SlotBold();
	void SlotItalic();
	void SlotUnderline();
	void SlotBtnFontColor();

	void SlotPenStyleChanged(QString val);
	void SlotPenWidthChanged(QString val);
	void SlotBrushStyleChanged(QString val);
	void SlotBtnPentColor();
	void SlotBtnBrushColor();

	void SlotItemSelected();
	void SlotItemAdded(QGraphicsItem *item);
	void SlotItemMoved(QGraphicsItem *item, const QPointF &oldPosition);
	void SlotItemRotate(QGraphicsItem *item, const qreal oldAngle);
	void SlotItemResize(QGraphicsItem *item, int handle, const QPointF &scale);
	void SlotItemControl(QGraphicsItem *item,int handle, const QPointF &newPos, const QPointF &lastPos_);
	void SlotPositionChanged(int x, int y);

	void SlotKeyUp();
	void SlotKeyDown();
	void SlotKeyLeft();
	void SlotKeyRight();
	void SlotKeyEqual();
	void SlotKeyMinus();
	void SlotKeyShift();
	void SlotReleaseKeyShift();

	void SlotMouseRightButton(QPoint p);

};

#endif // SKDRAW_H
