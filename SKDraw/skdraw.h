#ifndef SKDRAW_H
#define SKDRAW_H

#include "ui_skdraw.h"
#include "skhead.h"
#include "skbasewidget.h"
#include "drawtool.h"
#include "drawscene.h"
#include "drawview.h"
#include "propertyeditor.h"
#include "db.h"

class SKDraw : public QMainWindow
{
	Q_OBJECT

public:
	SKDraw(QWidget *parent = 0, Qt::WFlags flags = 0);
	~SKDraw();

	void InitSlot();
	void Start();
	void StartTreeWidgetScene(QList<CWnd*> wnds, QTreeWidgetItem *treeItem);

	void SetApp(SKBaseWidget *app) { m_app = app; }
	PropertyEditor* GetPropertyEditor() { return m_pPropertyEditor; }
	SKBaseWidget* GetApp() { return m_app; }
	QUndoStack* GetUndoStack() { return m_pUndoStack; }
	QFont GetFont() { return m_font; }
	QColor GetFontColor() { return m_fontColor; }
	QPen GetPen() { return m_pen; }
	QBrush GetBrush() { return m_brush; }
	bool GetDBState() { return m_bDBSt; }
	QTreeWidgetItem *GetCurrentTreeWidgetItem() { return m_pCurrentTreeWidgetItem; }
	QTreeWidget *GetWndTreeWidget() { return ui.treeWidgetSence; }
	QList<CWnd*> &GetListWnd() { return m_lstWnd; }
	int GetCurrentWndSn() { return m_iCurrentWndSn; }

public:
	void UpdateActions();
	CWnd* GetWndFromSn(int sn, QList<CWnd*> list);

public:
	SKBaseWidget *m_pLinkDataWidget;
	SKBaseWidget *m_pWndAddWidget;
	SKBaseWidget *m_pWndAttrWidget;

protected:
	virtual bool eventFilter(QObject *obj,QEvent *e);

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
	bool m_isClose;
	bool m_isInitSymbols;
	bool m_bDBSt;

	QList<CWnd*> m_lstWnd;
	QMenu m_menuWnd;
	QTreeWidgetItem *m_pCurrentTreeWidgetItem;
	int m_iCurrentWndSn;

private:
	void Init();
	void InitUi();
	void InitDB();
	void InitDBWnd(CWnd *pWnd);
	void InitSymbols();
	DrawView* CreateView();

signals:
	void SigClose();

public slots:
	void SlotKeyEscape();

public slots:
	void SlotNew();
	void SlotOpen();
	bool SlotSave();
	bool SlotSaveas();
	bool SlotClose();
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
	void SlotSymbolsDQClicked(QListWidgetItem *item);
	void SlotLinkData();
	void SlotLinkDataClose();

	void SlotMenuWnd(QAction *action);
	void SlotWndAddClose();
	void SlotTreeItemDoubleClicked(QTreeWidgetItem *treeItem, int column);

};

#endif // SKDRAW_H
