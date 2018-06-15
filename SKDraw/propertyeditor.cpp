#include "propertyeditor.h"
#include "drawobj.h"
#include "drawscene.h"
#include "drawview.h"
#include "commands.h"
#include "skdraw.h"

PropertyEditor::PropertyEditor(QWidget *parent)
	: QWidget(parent)
{
	m_pObject = NULL;

	InitMap();

	QtTreePropertyBrowser *browser = new QtTreePropertyBrowser(this);
	browser->setRootIsDecorated(true);
	m_pBrowser = browser;

	QVBoxLayout *layout = new QVBoxLayout(this);
	layout->setMargin(0);
	layout->addWidget(m_pBrowser);

	m_pEnumManager = new QtEnumPropertyManager(this);
	m_pVariantManager = new QtVariantPropertyManager(this);
	m_pCustomManager = new QtVariantPropertyManager(this);
	QtEnumEditorFactory *enumFactory = new QtEnumEditorFactory(this);
	QtVariantEditorFactory *variantFactory = new QtVariantEditorFactory(this);
	QtVariantEditorFactory *customFactory = new QtVariantEditorFactory(this);
	m_pBrowser->setFactoryForManager(m_pEnumManager, enumFactory);
	m_pBrowser->setFactoryForManager(m_pVariantManager, variantFactory);
	m_pBrowser->setFactoryForManager(m_pCustomManager, customFactory);

	connect(m_pEnumManager, SIGNAL(valueChanged(QtProperty *, int)), this, SLOT(SlotValueChanged(QtProperty *, int)));
	connect(m_pVariantManager, SIGNAL(valueChanged(QtProperty *, const QVariant &)), this, SLOT(SlotValueChanged(QtProperty *, const QVariant &)));
	connect(m_pCustomManager, SIGNAL(valueChanged(QtProperty *, const QVariant &)), this, SLOT(SlotCustomValueChanged(QtProperty *, const QVariant &)));
}

PropertyEditor::~PropertyEditor()
{

}

void PropertyEditor::InitMap()
{
	m_mapTranslate.insert("Text", "文本");
	m_mapTranslate.insert("Font", "字体");
	m_mapTranslate.insert("Width", "宽度");
	m_mapTranslate.insert("Height", "高度");
	m_mapTranslate.insert("Position", "位置");
	m_mapTranslate.insert("Scale", "缩放比例");
	m_mapTranslate.insert("Tooltip", "提示信息");
	m_mapTranslate.insert("Rotation", "旋转角度");
	m_mapTranslate.insert("PenColor", "画笔颜色");
	m_mapTranslate.insert("PenWidth", "画笔粗细");
	m_mapTranslate.insert("PenStyle", "画笔风格");
	m_mapTranslate.insert("BrushColor", "画刷颜色");
	m_mapTranslate.insert("BrushStyle", "画刷风格");
	m_mapTranslate.insert("RoundRadius", "圆角半径");
}

void PropertyEditor::SlotValueChanged(QtProperty *property, int value)
{
	if (!m_pObject || !m_propertyToIndex.contains(property))
		return;

	int idx = m_propertyToIndex.value(property);
	const QMetaObject *metaObject = m_pObject->metaObject();
	QMetaProperty metaProperty = metaObject->property(idx);
	if (metaProperty.isEnumType())
	{
		QPen pen = ((GraphicsItem*)m_pObject)->GetPen();
		QBrush brush = ((GraphicsItem*)m_pObject)->GetBrush();
		
		metaProperty.write(m_pObject, value);

		PropertyCommand(property, value, pen);
		PropertyCommand(property, value, brush);
		
	}
}

void PropertyEditor::SlotValueChanged(QtProperty *property, const QVariant &value)
{
	if (!m_pObject || !m_propertyToIndex.contains(property))
		return;

	int idx = m_propertyToIndex.value(property);
	const QMetaObject *metaObject = m_pObject->metaObject();
	QMetaProperty metaProperty = metaObject->property(idx);
	if (!metaProperty.isEnumType())
	{
		QPen pen = ((GraphicsItem*)m_pObject)->GetPen();
		QBrush brush = ((GraphicsItem*)m_pObject)->GetBrush();
		qreal scale = ((GraphicsItem*)m_pObject)->scale();
		QFont font;
		if (((GraphicsItem*)m_pObject)->GetName() == "文字图元")
			font = ((GraphicsTextItem*)m_pObject)->GetFont();

		metaProperty.write(m_pObject, value);

		PropertyCommand(property, value, pen);
		PropertyCommand(property, value, brush);
		PropertyCommand(property, value, scale);
		if (((GraphicsItem*)m_pObject)->GetName() == "文字图元")
			PropertyCommand(property, value, font);
	}
}

void PropertyEditor::SlotCustomValueChanged(QtProperty *property, const QVariant &value)
{
	QString name = property->propertyName();
	if (name == "画幅宽度")
	{
		m_pScene->SetWidth(value.toInt());
		m_pScene->setSceneRect(QRectF(0, 0, m_pScene->GetWidth(), m_pScene->GetHeight()));
	}
	else if (name == "画幅高度")
	{
		m_pScene->SetHeight(value.toInt());
		m_pScene->setSceneRect(QRectF(0, 0, m_pScene->GetWidth(), m_pScene->GetHeight()));
	}
	else if (name == "背景色")
	{
		QString a = value.toString();
		m_pScene->GetGridTool()->SetBackColor(value.value<QColor>());
	}
}

void PropertyEditor::Clear()
{
	m_pObject = NULL;

	QListIterator<QtProperty *> it(m_lstTopLevelProperties);
	while (it.hasNext())
		m_pBrowser->removeProperty(it.next());
	m_lstTopLevelProperties.clear();
}

void PropertyEditor::SetObject(QObject *object)
{
	if (m_pObject == object)
		return;

	if (m_pObject)
		Clear();

	m_pObject = object;
	if (!m_pObject)
		return;

	AddProperties(m_pObject->metaObject());
}

void PropertyEditor::SetBackground()
{
	QtVariantProperty *subProperty = m_pCustomManager->addProperty(QVariant::Int, tr("画幅宽度"));
	subProperty->setValue(m_pScene->GetWidth());
	m_pCustomManager->setAttribute(subProperty, "maximum", 10000);
	m_pCustomManager->setAttribute(subProperty, "minimum", 100);
	AddCustomProperty(subProperty);

	subProperty = m_pCustomManager->addProperty(QVariant::Int,tr("画幅高度"));
	subProperty->setValue(m_pScene->GetHeight());
	m_pCustomManager->setAttribute(subProperty, "maximum", 10000);
	m_pCustomManager->setAttribute(subProperty, "minimum", 100);
	AddCustomProperty(subProperty);

	subProperty = m_pCustomManager->addProperty(QVariant::Color,tr("背景色"));
	subProperty->setValue(m_pScene->GetGridTool()->GetBackColor());
	AddCustomProperty(subProperty);
}

void PropertyEditor::AddProperties(const QMetaObject *metaObject)
{
	if (!metaObject || QString("%1").arg(metaObject->className()) == "QObject")
		return;

	AddProperties(metaObject->superClass());

	for (int idx = metaObject->propertyOffset(); idx < metaObject->propertyCount(); idx++)
	{
		QtProperty *enumProperty = NULL;
		QtVariantProperty *subProperty = NULL;

		QMetaProperty metaProperty = metaObject->property(idx);
		if (!IsVisibleProperty(metaProperty.name()))
			continue;

		int type = metaProperty.userType();
		if (metaProperty.isEnumType())
		{
			enumProperty = m_pEnumManager->addProperty(m_mapTranslate.value(metaProperty.name()));
			QMetaEnum metaEnum = metaProperty.enumerator();
			QStringList enumNames;
			QMap<int, QIcon> enumIcons;
			for (int i = 0; i < metaEnum.keyCount(); i++)
				EnumEditor(i, metaEnum.key(i), enumNames, enumIcons);
			m_pEnumManager->setEnumNames(enumProperty, enumNames);
			m_pEnumManager->setEnumIcons(enumProperty, enumIcons);
		}
		else if (m_pVariantManager->isPropertyTypeSupported(type))
		{
			subProperty = m_pVariantManager->addProperty(type, m_mapTranslate.value(metaProperty.name()));
		}

		QtBrowserItem *item = NULL;
		if (subProperty)
		{
			m_propertyToIndex[subProperty] = idx;
			subProperty->setValue(metaProperty.read(m_pObject));
			m_lstTopLevelProperties.append(subProperty);
			item = m_pBrowser->addProperty(subProperty);
			m_classToIndexToProperty[metaObject][idx] = subProperty;
		}

		if (enumProperty)
		{
			m_propertyToIndex[enumProperty] = idx;
			m_pEnumManager->setValue(enumProperty, metaProperty.read(m_pObject).toInt());
			m_lstTopLevelProperties.append(enumProperty);
			item = m_pBrowser->addProperty(enumProperty);
			m_classToIndexToEnumProperty[metaObject][idx] = enumProperty;
		}

		//m_classToIndexToProperty[metaObject][idx] = subProperty;
		((QtTreePropertyBrowser*)m_pBrowser)->setExpanded(item,false);
	}
}

void PropertyEditor::AddCustomProperty(QtVariantProperty *property)
{
	QtBrowserItem *item = m_pBrowser->addProperty(property);
	((QtTreePropertyBrowser*)m_pBrowser)->setExpanded(item,false);
	m_lstTopLevelProperties.append(property);
}

void PropertyEditor::UpdateProperties(const QMetaObject *metaObject)
{
	if (!metaObject || QString("%1").arg(metaObject->className()) == "QObject")
		return;

	UpdateProperties(metaObject->superClass());

	for (int idx = metaObject->propertyOffset(); idx < metaObject->propertyCount(); idx++)
	{
		QMetaProperty metaProperty = metaObject->property(idx);
		if (!IsVisibleProperty(metaProperty.name()))
			continue;

		if (m_classToIndexToProperty.contains(metaObject) && m_classToIndexToProperty[metaObject].contains(idx))
		{
			QtVariantProperty *subProperty = m_classToIndexToProperty[metaObject][idx];
			if (!metaProperty.isEnumType())
				subProperty->setValue(metaProperty.read(m_pObject));
		}
		else if (m_classToIndexToEnumProperty.contains(metaObject) && m_classToIndexToEnumProperty[metaObject].contains(idx))
		{
			QtProperty *enumProperty = m_classToIndexToEnumProperty[metaObject][idx];
			if (metaProperty.isEnumType())
				m_pEnumManager->setValue(enumProperty, metaProperty.read(m_pObject).toInt());
		}
		
	}
}

bool PropertyEditor::IsVisibleProperty(QString property)
{
	if (m_pObject == NULL)
		return true;

	if (property == "Position" || property == "Width" || property == "Height" || property == "Scale" || property == "Tooltip" || property == "Rotation")
		return true;

	QString shape = ((GraphicsItem*)m_pObject)->GetName();
	if (shape == "多边形图元")
	{
		if (property == "PenColor" || property == "PenWidth" || property == "PenStyle" || property == "BrushColor" || property == "BrushStyle")
			return true;
		else
			return false;
	}
	else if (shape == "线段图元" || shape == "折线图元")
	{
		if (property == "PenColor" || property == "PenWidth" || property == "PenStyle")
			return true;
		else
			return false;
	}
	else if (shape == "圆角矩形图元")
	{
		if (property == "PenColor" || property == "PenWidth" || property == "PenStyle" || property == "BrushColor" || property == "BrushStyle" || property == "RoundRadius")
			return true;
		else
			return false;
	}
	else if (shape == "矩形图元")
	{
		if (property == "PenColor" || property == "PenWidth" || property == "PenStyle" || property == "BrushColor" || property == "BrushStyle")
			return true;
		else
			return false;
	}
	else if (shape == "圆形图元")
	{
		if (property == "PenColor" || property == "PenWidth" || property == "PenStyle" || property == "BrushColor" || property == "BrushStyle")
			return true;
		else
			return false;
	}
	else if (shape == "椭圆形图元")
	{
		if (property == "PenColor" || property == "PenWidth" || property == "PenStyle" || property == "BrushColor" || property == "BrushStyle")
			return true;
		else
			return false;
	}
	else if (shape == "文字图元")
	{
		if (property == "PenColor" || property == "Text" || property == "Font" || property == "TextOption")
			return true;
		else
			return false;
	}
	else if (shape == "图像图元")
	{
		return false;
	}

	return false;
}

void PropertyEditor::EnumEditor(int index, QString name, QStringList &enumNames, QMap<int, QIcon> &enumIcons)
{
	if (name == "NoPen")
	{
		enumNames.append("无");
		enumIcons.insert(index, QIcon(":/images/none"));
	}
	else if (name == "SolidLine")
	{
		enumNames.append("实线");
		enumIcons.insert(index, QIcon(":/images/SolidLine"));
	}
	else if (name == "DashLine")
	{
		enumNames.append("虚线");
		enumIcons.insert(index, QIcon(":/images/DashLine"));
	}
	else if (name == "DotLine")
	{
		enumNames.append("点线");
		enumIcons.insert(index, QIcon(":/images/DotLine"));
	}
	else if (name == "DashDotLine")
	{
		enumNames.append("虚点线");
		enumIcons.insert(index, QIcon(":/images/DashDotLine"));
	}
	else if (name == "DashDotDotLine")
	{
		enumNames.append("虚点点线");
		enumIcons.insert(index, QIcon(":/images/DashDotDotLine"));
	}
	else if (name == "NoBrush")
	{
		enumNames.append("无");
		enumIcons.insert(index, QIcon(":/images/none"));
	}
	else if (name == "SolidPattern")
	{
		enumNames.append("实体");
		enumIcons.insert(index, QIcon(":/images/SolidPattern"));
	}
	else if (name == "Dense1Pattern")
	{
		enumNames.append("密集1");
		enumIcons.insert(index, QIcon(":/images/Dense1Pattern"));
	}
	else if (name == "Dense2Pattern")
	{
		enumNames.append("密集2");
		enumIcons.insert(index, QIcon(":/images/Dense2Pattern"));
	}
	else if (name == "Dense3Pattern")
	{
		enumNames.append("密集3");
		enumIcons.insert(index, QIcon(":/images/Dense3Pattern"));
	}
	else if (name == "Dense4Pattern")
	{
		enumNames.append("密集4");
		enumIcons.insert(index, QIcon(":/images/Dense4Pattern"));
	}
	else if (name == "Dense5Pattern")
	{
		enumNames.append("密集5");
		enumIcons.insert(index, QIcon(":/images/Dense5Pattern"));
	}
	else if (name == "Dense6Pattern")
	{
		enumNames.append("密集6");
		enumIcons.insert(index, QIcon(":/images/Dense6Pattern"));
	}
	else if (name == "Dense7Pattern")
	{
		enumNames.append("密集7");
		enumIcons.insert(index, QIcon(":/images/Dense7Pattern"));
	}
	else if (name == "HorPattern")
	{
		enumNames.append("水平线");
		enumIcons.insert(index, QIcon(":/images/HorPattern"));
	}
	else if (name == "VerPattern")
	{
		enumNames.append("垂直线");
		enumIcons.insert(index, QIcon(":/images/VerPattern"));
	}
	else if (name == "CrossPattern")
	{
		enumNames.append("交叉线");
		enumIcons.insert(index, QIcon(":/images/CrossPattern"));
	}
	else if (name == "BDiagPattern")
	{
		enumNames.append("后向对角线");
		enumIcons.insert(index, QIcon(":/images/BDiagPattern"));
	}
	else if (name == "FDiagPattern")
	{
		enumNames.append("前向对角线");
		enumIcons.insert(index, QIcon(":/images/FDiagPattern"));
	}
	else if (name == "DiagCrossPattern")
	{
		enumNames.append("交叉对角线");
		enumIcons.insert(index, QIcon(":/images/DiagCrossPattern"));
	}
}

void PropertyEditor::PropertyCommand(QtProperty *property, const QVariant &value, QPen oldPen)
{
	SKDraw *draw = ((DrawView*)(m_pScene->GetView()))->GetApp();
	if (!draw)
		return;

	QString name = property->propertyName();
	if (name == "画笔粗细")
	{
		qreal penWidth = oldPen.widthF();
		if (penWidth != value.toReal())
		{
			QUndoCommand *penCommand = new PenPropertyCommand((GraphicsItem*)m_pObject, oldPen);
			draw->GetUndoStack()->push(penCommand);
		}
	}
	else if (name == "画笔颜色")
	{
		QColor color = oldPen.color();
		if (color != value.value<QColor>())
		{
			QUndoCommand *command = new PenPropertyCommand((GraphicsItem*)m_pObject, oldPen);
			draw->GetUndoStack()->push(command);
		}
	}
	else if (name == "画笔风格")
	{
		Qt::PenStyle style = oldPen.style();
		if (style != (Qt::PenStyle)value.toInt())
		{
			QUndoCommand *command = new PenPropertyCommand((GraphicsItem*)m_pObject, oldPen);
			draw->GetUndoStack()->push(command);
		}
	}
}

void PropertyEditor::PropertyCommand(QtProperty *property, const QVariant &value, QBrush oldBrush)
{
	SKDraw *draw = ((DrawView*)(m_pScene->GetView()))->GetApp();
	if (!draw)
		return;

	QString name = property->propertyName();
	if (name == "画刷颜色")
	{
		QColor color = oldBrush.color();
		if (color != value.value<QColor>())
		{
			QUndoCommand *command = new BrushPropertyCommand((GraphicsItem*)m_pObject, oldBrush);
			draw->GetUndoStack()->push(command);
		}
	}
	else if (name == "画刷风格")
	{
		Qt::BrushStyle style = oldBrush.style();
		if (style != (Qt::BrushStyle)value.toInt())
		{
			QUndoCommand *command = new BrushPropertyCommand((GraphicsItem*)m_pObject, oldBrush);
			draw->GetUndoStack()->push(command);
		}
	}
}

void PropertyEditor::PropertyCommand(QtProperty *property, const QVariant &value, QFont oldFont)
{
	SKDraw *draw = ((DrawView*)(m_pScene->GetView()))->GetApp();
	if (!draw)
		return;

	QString name = property->propertyName();
	if (name == "字体")
	{
		if (oldFont != value.value<QFont>())
		{
			QUndoCommand *command = new FontPropertyCommand((GraphicsItem*)m_pObject, oldFont);
			draw->GetUndoStack()->push(command);
		}
	}
}

void PropertyEditor::PropertyCommand(QtProperty *property, const QVariant &value, qreal oldScale)
{
	SKDraw *draw = ((DrawView*)(m_pScene->GetView()))->GetApp();
	if (!draw)
		return;

	QString name = property->propertyName();
	if (name == "缩放比例")
	{
		if (oldScale != value.toReal())
		{
			QUndoCommand *command = new ScalePropertyCommand((GraphicsItem*)m_pObject, oldScale);
			draw->GetUndoStack()->push(command);
		}
	}
}
