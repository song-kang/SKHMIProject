#include "drawview.h"
#include "skdraw.h"
#include "drawscene.h"

DrawView::DrawView(QGraphicsScene *scene)
	: QGraphicsView(scene)
{
	m_pScene = scene;

	setRenderHint(QPainter::Antialiasing);
	//setCacheMode(QGraphicsView::CacheBackground);
	//setOptimizationFlags(QGraphicsView::DontSavePainterState);
	setViewportUpdateMode(QGraphicsView::FullViewportUpdate);
	setTransformationAnchor(QGraphicsView::AnchorUnderMouse);
	setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	setViewport(new QWidget);

	//m_pHorRuler = new QtRuleBar(Qt::Horizontal,this,this);
	//m_pVerRuler = new QtRuleBar(Qt::Vertical,this,this);
	//m_pBox = new QtCornerBox(this);
	m_bMouseTranslate = false;
	m_scale = 1.0;
	m_zoomDelta = 0.1;
	m_isUntitled = true;

	connect(horizontalScrollBar(),SIGNAL(valueChanged(int)),this,SLOT(SlotScrollBarValueChanged(int)));
	connect(verticalScrollBar(),SIGNAL(valueChanged(int)),this,SLOT(SlotScrollBarValueChanged(int)));
}

DrawView::~DrawView()
{

}

void DrawView::SlotScrollBarValueChanged(int pos)
{
	//UpdateRuler();
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
	//setViewportMargins(RULER_SIZE - 1, RULER_SIZE - 1, 0, 0);
	setViewportMargins(1, 1, 1, 1);

	//m_pHorRuler->resize(this->size().width() - RULER_SIZE - 1, RULER_SIZE);
	//m_pHorRuler->move(RULER_SIZE, 0);

	//m_pVerRuler->resize(RULER_SIZE, this->size().height() - RULER_SIZE - 1);
	//m_pVerRuler->move(0,RULER_SIZE);

	//m_pBox->resize(RULER_SIZE,RULER_SIZE);
	//m_pBox->move(0,0);

	//UpdateRuler();

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
	case Qt::Key_Escape:
		m_app->SlotKeyEscape();
		break;
	case Qt::Key_Shift:
		((DrawScene*)m_pScene)->SetPressShift(true);
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
	//m_pHorRuler->updatePosition(event->pos());
	//m_pVerRuler->updatePosition(event->pos());

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
	LoadSymbol(event->pos());
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

void DrawView::Translate(QPointF delta)
{
	if (((DrawScene*)m_pScene)->selectedItems().count() > 0 || DrawTool::c_drawShape != eDrawSelection)
		return;
	
	delta *= m_scale;
	QPoint newCenter(viewport()->rect().width() / 2 - delta.x(), viewport()->rect().height() / 2 - delta.y());
	centerOn(mapToScene(newCenter));

	//UpdateRuler();
}

void DrawView::ZoomIn()
{
	scale(1+m_zoomDelta, 1+m_zoomDelta);
	m_scale *= 1+m_zoomDelta;

	//UpdateRuler();
}

void DrawView::ZoomOut()
{
	scale(1/(1+m_zoomDelta), 1/(1+m_zoomDelta));
	m_scale *= 1/(1+m_zoomDelta);

	//UpdateRuler();
}

void DrawView::ZoomOrg()
{
	resetMatrix();
	m_scale = 1.;

	//UpdateRuler();
}

void DrawView::UpdateRuler()
{
	if (!scene())
		return;

	QRectF viewbox = this->rect();
	QPointF offset = mapFromScene(scene()->sceneRect().topLeft());

	double factor = 1./transform().m11();
	double lower_x = factor * (viewbox.left() - offset.x());
	double upper_x = factor * (viewbox.right() - RULER_SIZE - offset.x());
	//m_pHorRuler->setRange(lower_x,upper_x, upper_x - lower_x);
	//m_pHorRuler->update();

	double lower_y = factor * (viewbox.top() - offset.y()) * -1;
	double upper_y = factor * (viewbox.bottom() - RULER_SIZE - offset.y()) * -1;
	//m_pVerRuler->setRange(lower_y, upper_y, upper_y - lower_y);
	//m_pVerRuler->update();
}

bool DrawView::Save()
{
	if (m_isUntitled)
		return SaveAs();
	else if (!m_sFileName.isEmpty())
		return SaveFile(m_sFileName);

	return false;
}

bool DrawView::SaveAs()
{
	QString fileName = QFileDialog::getSaveFileName(this, tr("保存"), m_sFileName, "*.sdw");
	if (fileName.isEmpty())
		return false;

	return SaveFile(fileName);
}

bool DrawView::SaveFile(const QString fileName)
{
	QFile file(fileName);
	if (!file.open(QFile::WriteOnly | QFile::Text))
	{
		QMessageBox::warning(this, tr("告警"), tr("文件【%1】写模式打开失败\n%2.").arg(fileName).arg(file.errorString()));
		return false;
	}

	QXmlStreamWriter xml(&file);
	xml.setAutoFormatting(true);
	xml.writeStartDocument();
	xml.writeDTD("<!DOCTYPE SKDRAW>");
	xml.writeStartElement("canvas");
	xml.writeAttribute("width",QString("%1").arg(scene()->width()));
	xml.writeAttribute("height",QString("%1").arg(scene()->height()));
	xml.writeAttribute("color",QString("%1").arg(((DrawScene*)m_pScene)->GetGridTool()->GetBackColor().name()));
	xml.writeAttribute("alpha",QString("%1").arg(((DrawScene*)m_pScene)->GetGridTool()->GetBackColor().alpha()));
	foreach (QGraphicsItem *item , scene()->items())
	{
		AbstractShape *ab = qgraphicsitem_cast<AbstractShape*>(item);
		QGraphicsItemGroup *g = dynamic_cast<QGraphicsItemGroup*>(item->parentItem());
		if (ab &&!qgraphicsitem_cast<SizeHandleRect*>(ab) && !g)
			ab->SaveToXml(&xml);
	}
	xml.writeEndElement();
	xml.writeEndDocument();

	m_sFileName = QFileInfo(fileName).canonicalFilePath();
	m_isUntitled = false;
	m_app->GetApp()->SetWindowTitle("SKDraw - " + tr("%1").arg(fileName));

	return true;
}

bool DrawView::LoadFile(const QString fileName)
{
	if (fileName == m_sFileName)
		return true;

	QFile file(fileName);
	if (!file.open(QFile::ReadOnly | QFile::Text))
	{
		QMessageBox::warning(this, tr("告警"), tr("文件【%1】读模式打开失败\n%2.").arg(fileName).arg(file.errorString()));
		return false;
	}

	QXmlStreamReader xml(&file);
	if (xml.readNextStartElement())
	{
		if (xml.name() == tr("canvas"))
		{
			QColor color;
			color.setNamedColor(xml.attributes().value(tr("color")).toString());
			color.setAlpha(xml.attributes().value(tr("alpha")).toString().toInt());
			int width = xml.attributes().value(tr("width")).toString().toInt();
			int height = xml.attributes().value(tr("height")).toString().toInt();
			((DrawScene*)m_pScene)->GetGridTool()->SetBackColor(color);
			((DrawScene*)m_pScene)->SetWidth(width);
			((DrawScene*)m_pScene)->SetHeight(height);
			scene()->setSceneRect(0,0,width,height);
			if (!LoadCanvas(&xml))
				return false;

			m_app->GetPropertyEditor()->Clear();
			m_app->GetPropertyEditor()->SetBackground();
		}
	}

	m_sFileName = QFileInfo(fileName).canonicalFilePath();
	m_isUntitled = false;
	m_app->GetApp()->SetWindowTitle("SKDraw - " + tr("%1").arg(fileName));

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
		else if (xml->name() == tr("group"))
			item = qgraphicsitem_cast<AbstractShape*>(LoadGroupFromXML(xml));
		else
			xml->skipCurrentElement();

		if (item && item->LoadFromXml(xml))
		{
			((GraphicsItem*)item)->SetScene((DrawScene*)m_pScene);
			scene()->addItem(item);
		}
		else if (item)
			delete item;
	}

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
		else if (xml->name() == tr("group"))
			item = qgraphicsitem_cast<AbstractShape*>(LoadGroupFromXML(xml));
		else
			xml->skipCurrentElement();

		if (item && item->LoadFromXml(xml))
		{
			((GraphicsItem*)item)->SetScene((DrawScene*)m_pScene);
			scene()->addItem(item);
			items.append(item);	
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

void DrawView::LoadSymbol(QPoint point)
{
	if (m_sSymbolName.isEmpty())
		return;

	QFile file(m_sSymbolName);
	if (file.open(QFile::ReadOnly | QFile::Text))
	{
		QXmlStreamReader xml(&file);
		if (xml.readNextStartElement() && xml.name() == "canvas")
		{
			if (xml.readNextStartElement() && xml.name() == "group")
			{
				AbstractShape *item = qgraphicsitem_cast<AbstractShape*>(LoadGroupFromXML(&xml));
				if (item && item->LoadFromXml(&xml))
				{
					item->setPos(mapToScene(point));
					((GraphicsItem*)item)->SetScene((DrawScene*)m_pScene);
					scene()->addItem(item);
				}
				else if (item)
					delete item;
			}
		}
	}
}
