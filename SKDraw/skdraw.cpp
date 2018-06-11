#include "skdraw.h"
#include "commands.h"

SKDraw::SKDraw(QWidget *parent, Qt::WFlags flags)
	: QMainWindow(parent, flags)
{
	ui.setupUi(this);

	Init();
	InitUi();
}

SKDraw::~SKDraw()
{

}

void SKDraw::Init()
{
	m_pView = NULL;
	m_pScene = NULL;
	m_pUndoStack = new QUndoStack(this);
	m_pEditMenu = new QMenu(this);
	m_pEditMenu->addAction(ui.actionSelect);
	m_pEditMenu->addAction(ui.actionSelectArea);
	m_pEditMenu->addSeparator();
	m_pEditMenu->addAction(ui.actionCut);
	m_pEditMenu->addAction(ui.actionCopy);
	m_pEditMenu->addAction(ui.actionPaste);

	m_pPropertyEditor = new PropertyEditor(this);
	ui.dockWidgetProperty->setWidget(m_pPropertyEditor);

	m_pFontComboBox = new QFontComboBox(this);
	m_pFontComboBox->setCurrentFont(QFont("宋体"));
	m_pFontSizeComboBox = new QComboBox(this);
	m_pFontSizeComboBox->setEditable(false);
	m_pFontSizeComboBox->setEditable(true);
	for (int i = 6,j = 0; j < 7; i++,j++)
		m_pFontSizeComboBox->addItem(QString().setNum(i));
	for (int i = 14,j = 0; j < 8; i = i + 2,j++)
		m_pFontSizeComboBox->addItem(QString().setNum(i));
	m_pFontSizeComboBox->addItem(QString().setNum(36));
	m_pFontSizeComboBox->addItem(QString().setNum(48));
	m_pFontSizeComboBox->addItem(QString().setNum(72));
	m_pFontSizeComboBox->setCurrentIndex(8);
	m_pFontColorBtn = new QPushButton("",this);
	m_pFontColorBtn->setFixedWidth(28);
	m_pFontColorBtn->setFlat(false);
	m_pFontColorBtn->setStyleSheet(tr("QPushButton{background:%2;}").arg("#00FFFF"));
	ui.fontToolBar->addWidget(m_pFontComboBox);
	ui.fontToolBar->addWidget(m_pFontSizeComboBox);
	ui.fontToolBar->addAction(ui.actionBold);
	ui.fontToolBar->addAction(ui.actionItalic);
	ui.fontToolBar->addAction(ui.actionUnderline);
	ui.fontToolBar->addWidget(m_pFontColorBtn);

	m_font = m_pFontComboBox->currentFont();
	m_font.setPointSize(m_pFontSizeComboBox->currentText().toInt());
	m_font.setBold(ui.actionBold->isChecked());
	m_font.setItalic(ui.actionItalic->isChecked());
	m_font.setUnderline(ui.actionItalic->isChecked());
	m_fontColor.setNamedColor("#00FFFF");

	//propertyToolBar

	UpdateActions();
}

void SKDraw::InitUi()
{
	setStyleSheet(tr("QWidget#%1{background:rgb(240,240,240,255);}").arg(objectName()));
}

void SKDraw::InitSlot()
{
	connect(ui.actionNew,SIGNAL(triggered()),this,SLOT(SlotNew()));
	connect(ui.actionClose,SIGNAL(triggered()),this,SLOT(SlotClose()));

	connect(ui.actionCopy,SIGNAL(triggered()),this,SLOT(SlotCopy()));
	connect(ui.actionCut,SIGNAL(triggered()),this,SLOT(SlotCut()));
	connect(ui.actionPaste,SIGNAL(triggered()),this,SLOT(SlotPaste()));
	connect(ui.actionDelete,SIGNAL(triggered()),this,SLOT(SlotDelete()));
	connect(QApplication::clipboard(),SIGNAL(dataChanged()),this,SLOT(SlotDataChanged()));

	connect(ui.actionUndo,SIGNAL(triggered()),this,SLOT(SlotUndo()));
	connect(ui.actionRedo,SIGNAL(triggered()),this,SLOT(SlotRedo()));
	connect(ui.actionZoomin,SIGNAL(triggered()),this,SLOT(SlotZoomin()));
	connect(ui.actionZoomout,SIGNAL(triggered()),this,SLOT(SlotZoomout()));
	connect(ui.actionZoomorg,SIGNAL(triggered()),this,SLOT(SlotZoomorg()));

	connect(ui.actionSelect,SIGNAL(triggered()),this,SLOT(SlotAddShape()));
	connect(ui.actionSelectArea,SIGNAL(triggered()),this,SLOT(SlotAddShape()));
	connect(ui.actionRotate,SIGNAL(triggered()),this,SLOT(SlotAddShape()));
	connect(ui.actionLine,SIGNAL(triggered()),this,SLOT(SlotAddShape()));
	connect(ui.actionPolyline,SIGNAL(triggered()),this,SLOT(SlotAddShape()));
	connect(ui.actionRectangle,SIGNAL(triggered()),this,SLOT(SlotAddShape()));
	connect(ui.actionRoundRect,SIGNAL(triggered()),this,SLOT(SlotAddShape()));
	connect(ui.actionCircle,SIGNAL(triggered()),this,SLOT(SlotAddShape()));
	connect(ui.actionEllipse,SIGNAL(triggered()),this,SLOT(SlotAddShape()));
	connect(ui.actionPolygon,SIGNAL(triggered()),this,SLOT(SlotAddShape()));
	connect(ui.actionText,SIGNAL(triggered()),this,SLOT(SlotAddShape()));
	connect(ui.actionPicture,SIGNAL(triggered()),this,SLOT(SlotAddShape()));

	connect(ui.actionLeft,SIGNAL(triggered()),this,SLOT(SlotAlign()));
	connect(ui.actionRight,SIGNAL(triggered()),this,SLOT(SlotAlign()));
	connect(ui.actionTop,SIGNAL(triggered()),this,SLOT(SlotAlign()));
	connect(ui.actionBottom,SIGNAL(triggered()),this,SLOT(SlotAlign()));
	connect(ui.actionVCenter,SIGNAL(triggered()),this,SLOT(SlotAlign()));
	connect(ui.actionHCenter,SIGNAL(triggered()),this,SLOT(SlotAlign()));
	connect(ui.actionVSpace,SIGNAL(triggered()),this,SLOT(SlotAlign()));
	connect(ui.actionHSpace,SIGNAL(triggered()),this,SLOT(SlotAlign()));
	connect(ui.actionHeight,SIGNAL(triggered()),this,SLOT(SlotAlign()));
	connect(ui.actionWidth,SIGNAL(triggered()),this,SLOT(SlotAlign()));
	connect(ui.actionSize,SIGNAL(triggered()),this,SLOT(SlotAlign()));

	connect(ui.actionfront,SIGNAL(triggered()),this,SLOT(SlotBringToFront()));
	connect(ui.actionback,SIGNAL(triggered()),this,SLOT(SlotSendToBack()));

	connect(ui.actionGroup,SIGNAL(triggered()),this,SLOT(SlotGroup()));
	connect(ui.actionUngroup,SIGNAL(triggered()),this,SLOT(SlotUngroup()));

	connect(m_pFontComboBox, SIGNAL(currentFontChanged(QFont)), this, SLOT(SlotCurrentFontChanged(QFont)));
	connect(m_pFontSizeComboBox, SIGNAL(currentIndexChanged(QString)), this, SLOT(SlotFontSizeChanged(QString)));
	connect(ui.actionBold,SIGNAL(triggered()),this,SLOT(SlotBold()));
	connect(ui.actionItalic,SIGNAL(triggered()),this,SLOT(SlotItalic()));
	connect(ui.actionUnderline,SIGNAL(triggered()),this,SLOT(SlotUnderline()));
	connect(m_pFontColorBtn,SIGNAL(clicked()),this,SLOT(SlotBtnFontColor()));

	connect(m_app, SIGNAL(SigKeyUp()), this, SLOT(SlotKeyUp()));
	connect(m_app, SIGNAL(SigKeyDown()), this, SLOT(SlotKeyDown()));
	connect(m_app, SIGNAL(SigKeyLeft()), this, SLOT(SlotKeyLeft()));
	connect(m_app, SIGNAL(SigKeyRight()), this, SLOT(SlotKeyRight()));
	connect(m_app, SIGNAL(SigKeyEqual()), this, SLOT(SlotKeyEqual()));
	connect(m_app, SIGNAL(SigKeyMinus()), this, SLOT(SlotKeyMinus()));
	connect(m_app, SIGNAL(SigKeyShift()), this, SLOT(SlotKeyShift()));
	connect(m_app, SIGNAL(SigReleaseKeyShift()), this, SLOT(SlotReleaseKeyShift()));
	connect(m_app, SIGNAL(SigKeyEscape()), this, SLOT(SlotKeyEscape()));
	//connect(m_app, SIGNAL(SigMouseRightButton(QPoint)), this, SLOT(SlotMouseRightButton(QPoint)));
}

void SKDraw::Start()
{
	SlotNew();
}

void SKDraw::SlotNew()
{
	DrawView *view = CreateView();
	ui.gridLayoutCentral->addWidget(view);
	view->show();

	UpdateActions();
}

void SKDraw::SlotClose()
{
	delete m_pView;
	delete m_pScene;
	m_pView = NULL;
	m_pScene = NULL;

	UpdateActions();
}

void SKDraw::SlotCopy()
{
	QList<QGraphicsItem *> items = m_pScene->selectedItems();
	ShapeMimeData *data = new ShapeMimeData(m_pScene->selectedItems());
	QApplication::clipboard()->setMimeData(data);

	UpdateActions();
}

void SKDraw::SlotCut()
{
	QList<QGraphicsItem *> copylist;
	foreach (QGraphicsItem *item, m_pScene->selectedItems())
	{
		AbstractShape *sp = qgraphicsitem_cast<AbstractShape*>(item);
		QGraphicsItem *copy = sp->Duplicate();
		if (copy)
			copylist.append(copy);
	}

	QUndoCommand *removeCommand = new RemoveShapeCommand(m_pScene);
	m_pUndoStack->push(removeCommand);
	if (copylist.count() > 0)
	{
		ShapeMimeData *data = new ShapeMimeData(copylist);
		QApplication::clipboard()->setMimeData(data);
	}

	foreach (QGraphicsItem *item, copylist)
		delete item;
	copylist.clear();

	UpdateActions();
}

void SKDraw::SlotPaste()
{
	QMimeData *mp = const_cast<QMimeData *>(QApplication::clipboard()->mimeData());
	ShapeMimeData *data = dynamic_cast<ShapeMimeData*>(mp);
	if (data)
	{
		m_pScene->clearSelection();
		foreach (QGraphicsItem * item, data->items())
		{
			AbstractShape *sp = qgraphicsitem_cast<AbstractShape*>(item);
			QGraphicsItem *copy = sp->Duplicate();
			if (copy)
			{
				copy->setSelected(true);
				copy->moveBy(10, -10);
				QUndoCommand *addCommand = new AddShapeCommand(copy, m_pScene);
				m_pUndoStack->push(addCommand);
			}
		}
	}

	UpdateActions();
}

void SKDraw::SlotDelete()
{
	if (m_pScene == NULL || m_pScene->selectedItems().isEmpty())
		return;

	QUndoCommand *removeCommand = new RemoveShapeCommand(m_pScene);
	m_pUndoStack->push(removeCommand);

	UpdateActions();
}

void SKDraw::SlotDataChanged()
{
	ui.actionPaste->setEnabled(true);

	UpdateActions();
}

void SKDraw::SlotUndo()
{
	m_pUndoStack->undo();

	UpdateActions();
}

void SKDraw::SlotRedo()
{
	m_pUndoStack->redo();

	UpdateActions();
}

void SKDraw::SlotZoomin()
{
	if (m_pView)
		m_pView->ZoomIn();
}

void SKDraw::SlotZoomout()
{
	if (m_pView)
		m_pView->ZoomOut();
}

void SKDraw::SlotZoomorg()
{
	if (m_pView)
		m_pView->ZoomOrg();
}

void SKDraw::SlotAddShape()
{
	if (sender() == ui.actionSelectArea)
	{
		DrawTool::c_drawShape = eDrawSelectionArea;
		m_pView->setDragMode(QGraphicsView::RubberBandDrag);
	}
	else
	{
		if (sender() == ui.actionSelect)
			DrawTool::c_drawShape = eDrawSelection;
		else if (sender() == ui.actionRotate)
			DrawTool::c_drawShape = eDrawRotation;
		else if (sender() == ui.actionLine)
			DrawTool::c_drawShape = eDrawLine;
		else if (sender() == ui.actionPolyline)
			DrawTool::c_drawShape = eDrawPolyline;
		else if (sender() == ui.actionRectangle)
			DrawTool::c_drawShape = eDrawRectangle;
		else if (sender() == ui.actionRoundRect)
			DrawTool::c_drawShape = eDrawRoundrect;
		else if (sender() == ui.actionCircle)
			DrawTool::c_drawShape = eDrawCircle;
		else if (sender() == ui.actionEllipse)
			DrawTool::c_drawShape = eDrawEllipse ;
		else if (sender() == ui.actionPolygon)
			DrawTool::c_drawShape = eDrawPolygon;
		else if (sender() == ui.actionText)
			DrawTool::c_drawShape = eDrawText;
		else if (sender() == ui.actionPicture)
			DrawTool::c_drawShape = eDrawPicture;

		m_pView->setDragMode(QGraphicsView::NoDrag);
	}
	
	if (sender() != ui.actionSelect && sender() != ui.actionSelectArea && sender() != ui.actionRotate)
		m_pScene->clearSelection();

	UpdateActions();
}

void SKDraw::SlotAlign()
{
	if (sender() == ui.actionLeft)
		m_pScene->Align(eAlignLeft);
	else if (sender() == ui.actionRight)
		m_pScene->Align(eAlignRight);
	else if (sender() == ui.actionTop)
		m_pScene->Align(eAlignTop);
	else if (sender() == ui.actionBottom)
		m_pScene->Align(eAlignBottom);
	else if (sender() == ui.actionVCenter)
		m_pScene->Align(eAlignVCenter);
	else if (sender() == ui.actionHCenter)
		m_pScene->Align(eAlignHCenter);
	else if (sender() == ui.actionVSpace)
		m_pScene->Align(eAlignVSpace);
	else if (sender()==ui.actionHSpace)
		m_pScene->Align(eAlignHSpace);
	else if (sender() == ui.actionHeight)
		m_pScene->Align(eAlignHeight);
	else if (sender() == ui.actionWidth)
		m_pScene->Align(eAlignWidth);
	else if (sender () == ui.actionSize)
		m_pScene->Align(eAlignSize);
}

void SKDraw::SlotBringToFront()
{
	if (m_pScene->selectedItems().isEmpty() || m_pScene->selectedItems().count() != 1)
		return;

	QGraphicsItem *selectedItem = m_pScene->selectedItems().first();

	qreal zValue = 0;
	QList<QGraphicsItem *> overlapItems = selectedItem->collidingItems();
	foreach (QGraphicsItem *item, overlapItems)
	{
		if (item->zValue() >= zValue && item->type() == GraphicsItem::Type)
			zValue = item->zValue() + 0.1;
	}

	selectedItem->setZValue(zValue);
}

void SKDraw::SlotSendToBack()
{
	if (m_pScene->selectedItems().isEmpty() || m_pScene->selectedItems().count() != 1)
		return;

	QGraphicsItem *selectedItem = m_pScene->selectedItems().first();

	qreal zValue = 0;
	QList<QGraphicsItem *> overlapItems = selectedItem->collidingItems();
	foreach (QGraphicsItem *item, overlapItems)
	{
		if (item->zValue() <= zValue && item->type() == GraphicsItem::Type)
			zValue = item->zValue() - 0.1;
	}

	selectedItem->setZValue(zValue);
}

void SKDraw::SlotGroup()
{
	QList<QGraphicsItem *> selectedItems = m_pScene->selectedItems();
	if (selectedItems.count() > 1)
	{
		GraphicsItemGroup *group = m_pScene->CreateGroup(selectedItems);
		QUndoCommand *groupCommand = new GroupShapeCommand(group,m_pScene);
		m_pUndoStack->push(groupCommand);
	}
}

void SKDraw::SlotUngroup()
{
	QGraphicsItem *selectedItem = m_pScene->selectedItems().first();
	GraphicsItemGroup *group = dynamic_cast<GraphicsItemGroup*>(selectedItem);
	if (group)
	{
		QUndoCommand *unGroupCommand = new UnGroupShapeCommand(group, m_pScene);
		m_pUndoStack->push(unGroupCommand);
	}
}

void SKDraw::SlotCurrentFontChanged(QFont font)
{
	m_font = font;
	m_font.setPointSize(m_pFontSizeComboBox->currentText().toInt());
	SlotBold();
	SlotItalic();
	SlotUnderline();
}

void SKDraw::SlotFontSizeChanged(QString size)
{
	m_font.setPointSize(size.toInt());
}

void SKDraw::SlotBold()
{
	ui.actionBold->setChecked(ui.actionBold->isChecked());
	m_font.setBold(ui.actionBold->isChecked());
}

void SKDraw::SlotItalic()
{
	ui.actionItalic->setChecked(ui.actionItalic->isChecked());
	m_font.setItalic(ui.actionItalic->isChecked());
}

void SKDraw::SlotUnderline()
{
	ui.actionUnderline->setChecked(ui.actionUnderline->isChecked());
	m_font.setUnderline(ui.actionUnderline->isChecked());
}

void SKDraw::SlotBtnFontColor()
{
	QColor color = QColorDialog::getColor(m_fontColor,this,tr("选择颜色"));
	if (color.isValid())
	{
		m_fontColor = color.name();
		m_pFontColorBtn->setStyleSheet(tr("QPushButton{background:%2;}").arg(m_fontColor.name()));
	}
}

void SKDraw::UpdateActions()
{
	ui.actionNew->setEnabled(m_pScene ? false : true);
	ui.actionOpen->setEnabled(m_pScene ? false : true);
	ui.actionClose->setEnabled(m_pScene);
	ui.actionClose->setEnabled(m_pScene);

	ui.actionCopy->setEnabled(m_pScene && m_pScene->selectedItems().count() > 0);
	ui.actionCut->setEnabled(m_pScene && m_pScene->selectedItems().count() > 0);

	ui.actionSelect->setEnabled(m_pScene);
	ui.actionSelectArea->setEnabled(m_pScene);
	ui.actionRotate->setEnabled(m_pScene);
	ui.actionLine->setEnabled(m_pScene);
	ui.actionRectangle->setEnabled(m_pScene);
	ui.actionRoundRect->setEnabled(m_pScene);
	ui.actionEllipse->setEnabled(m_pScene);
	ui.actionCircle->setEnabled(m_pScene);
	ui.actionPolygon->setEnabled(m_pScene);
	ui.actionPolyline->setEnabled(m_pScene);
	ui.actionText->setEnabled(m_pScene);
	ui.actionPicture->setEnabled(m_pScene);

	ui.actionZoomin->setEnabled(m_pScene);
	ui.actionZoomout->setEnabled(m_pScene);

	ui.actionSelect->setChecked(DrawTool::c_drawShape == eDrawSelection);
	ui.actionSelectArea->setChecked(DrawTool::c_drawShape == eDrawSelectionArea);
	ui.actionRotate->setChecked(DrawTool::c_drawShape == eDrawRotation);
	ui.actionLine->setChecked(DrawTool::c_drawShape == eDrawLine);
	ui.actionRectangle->setChecked(DrawTool::c_drawShape == eDrawRectangle);
	ui.actionRoundRect->setChecked(DrawTool::c_drawShape == eDrawRoundrect);
	ui.actionEllipse->setChecked(DrawTool::c_drawShape == eDrawEllipse);
	ui.actionCircle->setChecked(DrawTool::c_drawShape == eDrawCircle);
	ui.actionPolygon->setChecked(DrawTool::c_drawShape == eDrawPolygon);
	ui.actionPolyline->setChecked(DrawTool::c_drawShape == eDrawPolyline);
	ui.actionText->setChecked(DrawTool::c_drawShape == eDrawText);
	ui.actionPicture->setChecked(DrawTool::c_drawShape == eDrawPicture);

	ui.actionUndo->setEnabled(m_pUndoStack->canUndo());
	ui.actionRedo->setEnabled(m_pUndoStack->canRedo());

	ui.actionLeft->setEnabled(m_pScene && m_pScene->selectedItems().count() > 1 && m_pScene->GetAlignItem());
	ui.actionRight->setEnabled(m_pScene && m_pScene->selectedItems().count() > 1 && m_pScene->GetAlignItem());
	ui.actionTop->setEnabled(m_pScene && m_pScene->selectedItems().count() > 1 && m_pScene->GetAlignItem());
	ui.actionBottom->setEnabled(m_pScene && m_pScene->selectedItems().count() > 1 && m_pScene->GetAlignItem());
	ui.actionVCenter->setEnabled(m_pScene && m_pScene->selectedItems().count() > 1 && m_pScene->GetAlignItem());
	ui.actionHCenter->setEnabled(m_pScene && m_pScene->selectedItems().count() > 1 && m_pScene->GetAlignItem());
	ui.actionVSpace->setEnabled(m_pScene && m_pScene->selectedItems().count() > 1 && m_pScene->GetAlignItem());
	ui.actionHSpace->setEnabled(m_pScene && m_pScene->selectedItems().count() > 1 && m_pScene->GetAlignItem());
	ui.actionWidth->setEnabled(m_pScene && m_pScene->selectedItems().count() > 1 && m_pScene->GetAlignItem());
	ui.actionHeight->setEnabled(m_pScene && m_pScene->selectedItems().count() > 1 && m_pScene->GetAlignItem());
	ui.actionSize->setEnabled(m_pScene && m_pScene->selectedItems().count() > 1 && m_pScene->GetAlignItem());

	ui.actionfront->setEnabled(false);
	ui.actionback->setEnabled(false);
	if (m_pScene && m_pScene->selectedItems().count() == 1)
	{
		QGraphicsItem *selectedItem = m_pScene->selectedItems().first();
		QList<QGraphicsItem *> overlapItems = selectedItem->collidingItems();
		foreach (QGraphicsItem *item, overlapItems)
		{
			if (item->type() == GraphicsItem::Type)
			{
				ui.actionfront->setEnabled(true);
				ui.actionback->setEnabled(true);
			}
		}
	}

	ui.actionGroup->setEnabled(m_pScene && m_pScene->selectedItems().count() > 1);
	ui.actionUngroup->setEnabled(m_pScene && m_pScene->selectedItems().count() == 1 && dynamic_cast<GraphicsItemGroup*>(m_pScene->selectedItems().first()));
}

DrawView* SKDraw::CreateView()
{
	m_pScene = new DrawScene(this);
	connect(m_pScene, SIGNAL(selectionChanged()),											 this, SLOT(SlotItemSelected()));
	connect(m_pScene, SIGNAL(itemAdded(QGraphicsItem*)),									 this, SLOT(SlotItemAdded(QGraphicsItem*)));
	connect(m_pScene, SIGNAL(itemMoved(QGraphicsItem*,QPointF)),							 this, SLOT(SlotItemMoved(QGraphicsItem*,QPointF)));
	connect(m_pScene, SIGNAL(itemRotate(QGraphicsItem*,qreal)),								 this, SLOT(SlotItemRotate(QGraphicsItem*,qreal)));
	connect(m_pScene, SIGNAL(itemResize(QGraphicsItem*,int,const QPointF&)),				 this, SLOT(SlotItemResize(QGraphicsItem*,int,QPointF)));
	connect(m_pScene, SIGNAL(itemControl(QGraphicsItem*,int,const QPointF&,const QPointF&)), this, SLOT(SlotItemControl(QGraphicsItem*,int,QPointF,QPointF)));

	m_pView = new DrawView(m_pScene);
	m_pView->SetApp(this);
	m_pScene->SetView(m_pView);
	connect(m_pView, SIGNAL(SigPositionChanged(int,int)), this, SLOT(SlotPositionChanged(int,int)));
	connect(m_pView, SIGNAL(SigMouseRightButton(QPoint)), this, SLOT(SlotMouseRightButton(QPoint)));

	return m_pView;
}

void SKDraw::SlotItemSelected()
{
	QList<QGraphicsItem*> l = m_pScene->selectedItems();
	if (l.count() == 1 && l.first()->isSelected())
	{
		QGraphicsItem *item = l.first();
		m_pControlledObject = dynamic_cast<QObject*>(item);
		m_pPropertyEditor->SetObject(m_pControlledObject);
	}
	else
		m_pPropertyEditor->Clear();

	UpdateActions();
}

void SKDraw::SlotItemAdded(QGraphicsItem *item)
{
	QUndoCommand *addCommand = new AddShapeCommand(item, item->scene());
	m_pUndoStack->push(addCommand);
}

void SKDraw::SlotItemMoved(QGraphicsItem *item, const QPointF &oldPosition)
{

}

void SKDraw::SlotItemRotate(QGraphicsItem *item, const qreal oldAngle)
{

}

void SKDraw::SlotItemResize(QGraphicsItem *item,int, const QPointF &scale)
{

}

void SKDraw::SlotItemControl(QGraphicsItem *item,int, const QPointF &newPos, const QPointF &lastPos_)
{

}

void SKDraw::SlotPositionChanged(int x, int y)
{
	statusBar()->showMessage(tr("坐标（%1,%2）").arg(x).arg(y));
}

void SKDraw::SlotKeyUp()
{
	if (m_pView)
		m_pView->Translate(QPointF(0, 10));
}

void SKDraw::SlotKeyDown()
{
	if (m_pView)
		m_pView->Translate(QPointF(0, -10));
}

void SKDraw::SlotKeyLeft()
{
	if (m_pView)
		m_pView->Translate(QPointF(10, 0));
}

void SKDraw::SlotKeyRight()
{
	if (m_pView)
		m_pView->Translate(QPointF(-10, 0));
}

void SKDraw::SlotKeyEqual()
{
	SlotZoomin();
}

void SKDraw::SlotKeyMinus()
{
	SlotZoomout();
}

void SKDraw::SlotKeyShift()
{
	if (m_pScene)
		m_pScene->SetPressShift(true);
}

void SKDraw::SlotReleaseKeyShift()
{
	if (m_pScene)
		m_pScene->SetPressShift(false);
}

void SKDraw::SlotKeyEscape()
{
	DrawTool *tool = DrawTool::findTool(DrawTool::c_drawShape);
	if (tool &&
		(tool->m_drawShape == eDrawLine || tool->m_drawShape == eDrawPolyline || tool->m_drawShape == eDrawPolygon) &&
		((DrawPolygonTool*)tool)->m_pItem)
	{
		m_pScene->removeItem(((DrawPolygonTool*)tool)->m_pItem);
		delete ((DrawPolygonTool*)tool)->m_pItem;
		((DrawPolygonTool*)tool)->m_pItem = NULL;
	}
	else if (tool &&
			 (tool->m_drawShape == eDrawRectangle || tool->m_drawShape == eDrawRoundrect || 
			  tool->m_drawShape == eDrawEllipse || tool->m_drawShape == eDrawCircle) &&
			 ((DrawRectTool*)tool)->m_pItem)
	{
		m_pScene->removeItem(((DrawRectTool*)tool)->m_pItem);
		delete ((DrawRectTool*)tool)->m_pItem;
		((DrawRectTool*)tool)->m_pItem = NULL;
	}

	m_pScene->clearSelection();
	DrawTool::c_drawShape = eDrawSelection;
	m_pView->setDragMode(QGraphicsView::NoDrag);

	UpdateActions();
}

void SKDraw::SlotMouseRightButton(QPoint p)
{
	m_pEditMenu->popup(p + QPoint(276,136));
}
