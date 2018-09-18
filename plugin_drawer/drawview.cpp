#include "drawview.h"
#include "drawscene.h"
#include "view_plugin_drawer.h"

#define MAX_ZOOM_LEVEL		5

DrawView::DrawView(QGraphicsScene *scene)
	: QGraphicsView(scene)
{
	m_pScene = scene;

	setRenderHint(QPainter::Antialiasing);
	setViewportUpdateMode(QGraphicsView::FullViewportUpdate);
	setTransformationAnchor(QGraphicsView::AnchorUnderMouse);
	setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	setStyleSheet("background: transparent;border:0px");
	setViewport(new QWidget);

	m_bMouseTranslate = false;
	m_scale = 1.0;
	m_zoomDelta = 0.5;
	m_iZoomLevel = 0;
	m_isUntitled = true;
	m_bScaleToScreen = true;

	connect(horizontalScrollBar(),SIGNAL(valueChanged(int)),this,SLOT(SlotScrollBarValueChanged(int)));
	connect(verticalScrollBar(),SIGNAL(valueChanged(int)),this,SLOT(SlotScrollBarValueChanged(int)));
}

DrawView::~DrawView()
{

}

void DrawView::SlotScrollBarValueChanged(int pos)
{
}

void DrawView::wheelEvent(QWheelEvent *event)
{
	if (event->delta() > 0)
		ZoomIn();
	else
		ZoomOut();
}

void DrawView::resizeEvent(QResizeEvent *event)
{
	setViewportMargins(0,0,0,0);
	QGraphicsView::resizeEvent(event);
}

void DrawView::keyPressEvent(QKeyEvent *event)
{
	switch (event->key())
	{
	case Qt::Key_Up:
		Translate(QPointF(0, 10));
		break;
	case Qt::Key_Down:
		Translate(QPointF(0, -10));
		break;
	case Qt::Key_Left:
		Translate(QPointF(10, 0));
		break;
	case Qt::Key_Right:
		Translate(QPointF(-10, 0));
		break;
	case Qt::Key_Equal:
		ZoomIn();
		break;
	case Qt::Key_Minus:
		ZoomOut();
		break;
	}

	QGraphicsView::keyPressEvent(event);
}

void DrawView::keyReleaseEvent(QKeyEvent *event)
{
	switch (event->key())
	{
	case Qt::Key_Shift:
		((DrawScene*)m_pScene)->SetPressShift(false);
		break;
	}

	QGraphicsView::keyReleaseEvent(event);
}

void DrawView::mouseMoveEvent(QMouseEvent *event)
{
	QPointF pt = mapToScene(event->pos());
	emit SigPositionChanged(pt.x(), pt.y());

	if (m_bMouseTranslate)
	{
		QPointF mouseDelta = mapToScene(event->pos()) - mapToScene(m_lastMousePos);
		Translate(mouseDelta);
	}

	m_lastMousePos = event->pos();

	QGraphicsView::mouseMoveEvent(event);
}

void DrawView::mousePressEvent(QMouseEvent *event)
{
	if (event->button() == Qt::LeftButton && dragMode() != QGraphicsView::RubberBandDrag)
	{
		QPointF point = mapToScene(event->pos());
		if (scene()->itemAt(point, transform()) == NULL)
		{
			m_bMouseTranslate = true;
			m_lastMousePos = event->pos();
			setCursor(Qt::ClosedHandCursor);
		}
	}
	else if (event->button() == Qt::RightButton)
	{
		SigMouseRightButton(mapToGlobal(event->pos()));
	}

	QGraphicsView::mousePressEvent(event);
}

void DrawView::mouseReleaseEvent(QMouseEvent *event)
{
	if (event->button() == Qt::LeftButton)
	{
		m_bMouseTranslate = false;
		if (m_sSymbolName.isEmpty())
			setCursor(Qt::ArrowCursor);
		else
			setCursor(m_cursorSymbol);
	}

	QGraphicsView::mouseReleaseEvent(event);
}

void DrawView::drawBackground(QPainter *painter, const QRectF &rect)
{
	QGraphicsView::drawBackground(painter,rect);

	if (m_bScaleToScreen)
	{
		ScaleToScreenWidth();
		m_bScaleToScreen = false;
	}
}

void DrawView::Translate(QPointF delta)
{
	if (((DrawScene*)m_pScene)->selectedItems().count() > 0)
		return;
	
	delta *= m_scale;
	QPoint newCenter(viewport()->rect().width() / 2 - delta.x(), viewport()->rect().height() / 2 - delta.y());
	centerOn(mapToScene(newCenter));
}

void DrawView::ZoomIn()
{
	if (m_iZoomLevel >= MAX_ZOOM_LEVEL)
		return;
	
	if (++m_iZoomLevel == 0)
	{
		ScaleToScreenWidth();
	}
	else
	{
		scale(1+m_zoomDelta, 1+m_zoomDelta);
		m_scale *= 1+m_zoomDelta;
	}
}

void DrawView::ZoomOut()
{
	if (m_iZoomLevel == 0)
		return;

	if (--m_iZoomLevel == 0)
	{
		ScaleToScreenWidth();
	}
	else
	{
		scale(1/(1+m_zoomDelta), 1/(1+m_zoomDelta));
		m_scale *= 1/(1+m_zoomDelta);
	}
}

void DrawView::ZoomOrg()
{
	resetMatrix();
	m_scale = 1.;
}

void DrawView::ScaleToScreenWidth()
{
	ZoomOrg();
	QRect size = viewport()->rect();
	qreal ratio = (qreal)size.width() / (qreal)m_iSceneWidth;
	scale(ratio, ratio);
	m_scale *= ratio;
	centerOn(mapToScene(QPoint(0,0)));
}

void DrawView::ScaleToScreenHeight()
{
	ZoomOrg();
	QRect size = viewport()->rect();
	qreal ratio = (qreal)size.height() / (qreal)m_iSceneHeight;
	scale(ratio, ratio);
	m_scale *= ratio;
	centerOn(mapToScene(QPoint(0,0)));
}

bool DrawView::Load(char *content)
{
	QXmlStreamReader xml(content);
	if (xml.readNextStartElement())
	{
		if (xml.name() == tr("canvas"))
		{
			QColor color;
			color.setNamedColor(xml.attributes().value(tr("color")).toString());
			color.setAlpha(xml.attributes().value(tr("alpha")).toString().toInt());
			m_iSceneWidth = xml.attributes().value(tr("width")).toString().toInt();
			m_iSceneHeight = xml.attributes().value(tr("height")).toString().toInt();
			((DrawScene*)m_pScene)->GetGridTool()->SetBackColor(color);
			((DrawScene*)m_pScene)->SetWidth(m_iSceneWidth);
			((DrawScene*)m_pScene)->SetHeight(m_iSceneHeight);
			scene()->setSceneRect(0,0,m_iSceneWidth,m_iSceneHeight);
			scene()->setBackgroundBrush(QBrush(color));
			if (!LoadCanvas(&xml))
				return false;
		}
	}

	return true;
}

bool DrawView::Load(const QString fileName)
{
	if (fileName == m_sFileName)
		return true;

	QFile file(fileName);
	if (!file.open(QFile::ReadOnly | QFile::Text))
	{
		QMessageBox::warning(this, tr("告警"), tr("文件【%1】读模式打开失败\n%2.").arg(fileName).arg(file.errorString()));
		return false;
	}

	if (!Load(file.readAll().data()))
		return false;

	m_sFileName = fileName;
	return true;
}

bool DrawView::LoadCanvas(QXmlStreamReader *xml)
{
	if (!xml->isStartElement() || xml->name() != "canvas")
	{
		QMessageBox::warning(NULL,tr("告警"),tr("文件格式异常"));
		return false;
	}

	while (xml->readNextStartElement())
	{
		AbstractShape *item = NULL;
		if (xml->name() == tr("rect"))
			item = new GraphicsRectItem(QRect(0,0,0,0));
		else if (xml->name() == tr("roundrect"))
			item = new GraphicsRectItem(QRect(0,0,0,0), true);
		else if (xml->name() == tr("ellipse"))
			item = new GraphicsEllipseItem(QRect(0,0,0,0));
		else if (xml->name() == tr("circle"))
			item = new GraphicsEllipseItem(QRect(0,0,0,0), true);
		else if (xml->name() == tr("text"))
			item = new GraphicsTextItem(QRect(0, 0, 0, 0));
		else if (xml->name() == tr("picture"))
			item = new GraphicsPictureItem(QRect(0, 0, 0, 0), QString::null);
		else if (xml->name()==tr("polygon"))
			item = new GraphicsPolygonItem();
		else if (xml->name()==tr("polyline"))
			item = new GraphicsPolygonLineItem();
		else if (xml->name() == tr("line"))
			item = new GraphicsLineItem();
		else if (xml->name() == tr("triangle"))
			item = new GraphicsTriangleItem(QRect(0,0,0,0));
		else if (xml->name() == tr("rhombus"))
			item = new GraphicsTriangleItem(QRect(0,0,0,0));
		else if (xml->name() == tr("group"))
			item = qgraphicsitem_cast<AbstractShape*>(LoadGroupFromXML(xml));
		else
			xml->skipCurrentElement();

		if (item && item->LoadFromXml(xml))
		{
			((GraphicsItem*)item)->SetScene((DrawScene*)m_pScene);
			scene()->addItem(item);

			if (!item->GetLinkDB().isEmpty())
				m_app->InsertMapLinkDB(item->GetLinkDB(),(GraphicsItem*)item);
		}
		else if (item)
			delete item;
	}

	m_app->InitDrawobj();
	return true;
}

GraphicsItemGroup* DrawView::LoadGroupFromXML(QXmlStreamReader *xml)
{
	QList<QGraphicsItem*> items;
	qreal angle = xml->attributes().value(tr("rotate")).toString().toDouble();
	QString linkdb = xml->attributes().value(tr("linkdb")).toString();
	QString linkscene = xml->attributes().value(tr("linkscene")).toString();
	while (xml->readNextStartElement())
	{
		AbstractShape * item = NULL;
		if (xml->name() == tr("rect"))
			item = new GraphicsRectItem(QRect(0,0,0,0));
		else if (xml->name() == tr("roundrect"))
			item = new GraphicsRectItem(QRect(0,0,0,0), true);
		else if (xml->name() == tr("ellipse"))
			item = new GraphicsEllipseItem(QRect(0,0,0,0));
		else if (xml->name() == tr("circle"))
			item = new GraphicsEllipseItem(QRect(0,0,0,0), true);
		else if (xml->name() == tr("text"))
			item = new GraphicsTextItem(QRect(0, 0, 0, 0));
		else if (xml->name() == tr("picture"))
			item = new GraphicsPictureItem(QRect(0, 0, 0, 0), QString::null);
		else if (xml->name()==tr("polygon"))
			item = new GraphicsPolygonItem();
		else if (xml->name()==tr("polyline"))
			item = new GraphicsPolygonLineItem();
		else if (xml->name() == tr("line"))
			item = new GraphicsLineItem();
		else if (xml->name() == tr("triangle"))
			item = new GraphicsTriangleItem(QRect(0,0,0,0));
		else if (xml->name() == tr("rhombus"))
			item = new GraphicsTriangleItem(QRect(0,0,0,0));
		else if (xml->name() == tr("group"))
			item = qgraphicsitem_cast<AbstractShape*>(LoadGroupFromXML(xml));
		else
			xml->skipCurrentElement();

		if (item && item->LoadFromXml(xml))
		{
			((GraphicsItem*)item)->SetScene((DrawScene*)m_pScene);
			scene()->addItem(item);
			items.append(item);

			if (!item->GetLinkDB().isEmpty())
				m_app->InsertMapLinkDB(item->GetLinkDB(),(GraphicsItem*)item);
		}
		else if (item)
			delete item;
	}

	if (items.count() > 0)
	{
		DrawScene *s = dynamic_cast<DrawScene*>(scene());
		GraphicsItemGroup *group = s->CreateGroup(items,false);
		if (group)
		{
			group->setRotation(angle);
			group->SetLinkDB(linkdb);
			group->SetLinkScene(linkscene);
			group->UpdateCoordinate();
		}

		return group;
	}

	return NULL;
}
