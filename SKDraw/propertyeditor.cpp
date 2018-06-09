#include "propertyeditor.h"

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
	m_pVariantmanager = new QtVariantPropertyManager(this);
	QtEnumEditorFactory *enumFactory = new QtEnumEditorFactory(this);
	QtVariantEditorFactory *variantFactory = new QtVariantEditorFactory(this);
	m_pBrowser->setFactoryForManager(m_pEnumManager, enumFactory);
	m_pBrowser->setFactoryForManager(m_pVariantmanager, variantFactory);

	connect(m_pEnumManager, SIGNAL(valueChanged(QtProperty *, int)), this, SLOT(SlotValueChanged(QtProperty *, int)));
	connect(m_pVariantmanager, SIGNAL(valueChanged(QtProperty *, const QVariant &)), this, SLOT(SlotValueChanged(QtProperty *, const QVariant &)));
}

PropertyEditor::~PropertyEditor()
{

}

void PropertyEditor::InitMap()
{
	m_mapTranslate.insert("Text", "�ı�");
	m_mapTranslate.insert("Width", "���");
	m_mapTranslate.insert("Height", "�߶�");
	m_mapTranslate.insert("Position", "λ��");
	m_mapTranslate.insert("PenColor", "������ɫ");
	m_mapTranslate.insert("PenWidth", "���ʴ�ϸ");
	m_mapTranslate.insert("PenStyle", "���ʷ��");
	m_mapTranslate.insert("BrushColor", "��ˢ��ɫ");
	m_mapTranslate.insert("BrushStyle", "��ˢ���");
	m_mapTranslate.insert("RoundRadius", "Բ�ǰ뾶");
}

void PropertyEditor::SlotValueChanged(QtProperty *property, int value)
{
	if (!m_propertyToIndex.contains(property))
		return;

	int idx = m_propertyToIndex.value(property);
	const QMetaObject *metaObject = m_pObject->metaObject();
	QMetaProperty metaProperty = metaObject->property(idx);
	if (metaProperty.isEnumType())
		metaProperty.write(m_pObject, value);

	UpdateProperties(metaObject);
}

void PropertyEditor::SlotValueChanged(QtProperty *property, const QVariant &value)
{
	if (!m_propertyToIndex.contains(property))
		return;

	int idx = m_propertyToIndex.value(property);
	const QMetaObject *metaObject = m_pObject->metaObject();
	QMetaProperty metaProperty = metaObject->property(idx);
	if (!metaProperty.isEnumType())
		metaProperty.write(m_pObject, value);

	UpdateProperties(metaObject);
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
		else if (m_pVariantmanager->isPropertyTypeSupported(type))
		{
			subProperty = m_pVariantmanager->addProperty(type, m_mapTranslate.value(metaProperty.name()));
		}

		QtBrowserItem *item = NULL;
		if (subProperty)
		{
			m_propertyToIndex[subProperty] = idx;
			subProperty->setValue(metaProperty.read(m_pObject));
			m_lstTopLevelProperties.append(subProperty);
			item = m_pBrowser->addProperty(subProperty);
		}

		if (enumProperty)
		{
			m_propertyToIndex[enumProperty] = idx;
			m_pEnumManager->setValue(enumProperty, metaProperty.read(m_pObject).toInt());
			m_lstTopLevelProperties.append(enumProperty);
			item = m_pBrowser->addProperty(enumProperty);
		}

		m_classToIndexToProperty[metaObject][idx] = subProperty;
		((QtTreePropertyBrowser*)m_pBrowser)->setExpanded(item,false);
	}
}

void PropertyEditor::UpdateProperties(const QMetaObject *metaObject)
{
	if (!metaObject || QString("%1").arg(metaObject->className()) == "QObject")
		return;

	UpdateProperties(metaObject->superClass());

	for (int idx = metaObject->propertyOffset(); idx < metaObject->propertyCount(); idx++)
	{
		QMetaProperty metaProperty = metaObject->property(idx);
		if (m_classToIndexToProperty.contains(metaObject) && m_classToIndexToProperty[metaObject].contains(idx))
		{
			QtVariantProperty *subProperty = m_classToIndexToProperty[metaObject][idx];
			if (!metaProperty.isEnumType())
				subProperty->setValue(metaProperty.read(m_pObject));
		}
	}
}

void PropertyEditor::EnumEditor(int index, QString name, QStringList &enumNames, QMap<int, QIcon> &enumIcons)
{
	if (name == "NoPen")
	{
		enumNames.append("��");
		enumIcons.insert(index, QIcon(":/images/none"));
	}
	else if (name == "SolidLine")
	{
		enumNames.append("ʵ��");
		enumIcons.insert(index, QIcon(":/images/SolidLine"));
	}
	else if (name == "DashLine")
	{
		enumNames.append("����");
		enumIcons.insert(index, QIcon(":/images/DashLine"));
	}
	else if (name == "DotLine")
	{
		enumNames.append("����");
		enumIcons.insert(index, QIcon(":/images/DotLine"));
	}
	else if (name == "DashDotLine")
	{
		enumNames.append("�����");
		enumIcons.insert(index, QIcon(":/images/DashDotLine"));
	}
	else if (name == "DashDotDotLine")
	{
		enumNames.append("������");
		enumIcons.insert(index, QIcon(":/images/DashDotDotLine"));
	}
	else if (name == "NoBrush")
	{
		enumNames.append("��");
		enumIcons.insert(index, QIcon(":/images/none"));
	}
	else if (name == "SolidPattern")
	{
		enumNames.append("ʵ��");
		enumIcons.insert(index, QIcon(":/images/SolidPattern"));
	}
	else if (name == "Dense1Pattern")
	{
		enumNames.append("�ܼ�1");
		enumIcons.insert(index, QIcon(":/images/Dense1Pattern"));
	}
	else if (name == "Dense2Pattern")
	{
		enumNames.append("�ܼ�2");
		enumIcons.insert(index, QIcon(":/images/Dense2Pattern"));
	}
	else if (name == "Dense3Pattern")
	{
		enumNames.append("�ܼ�3");
		enumIcons.insert(index, QIcon(":/images/Dense3Pattern"));
	}
	else if (name == "Dense4Pattern")
	{
		enumNames.append("�ܼ�4");
		enumIcons.insert(index, QIcon(":/images/Dense4Pattern"));
	}
	else if (name == "Dense5Pattern")
	{
		enumNames.append("�ܼ�5");
		enumIcons.insert(index, QIcon(":/images/Dense5Pattern"));
	}
	else if (name == "Dense6Pattern")
	{
		enumNames.append("�ܼ�6");
		enumIcons.insert(index, QIcon(":/images/Dense6Pattern"));
	}
	else if (name == "Dense7Pattern")
	{
		enumNames.append("�ܼ�7");
		enumIcons.insert(index, QIcon(":/images/Dense7Pattern"));
	}
	else if (name == "HorPattern")
	{
		enumNames.append("ˮƽ��");
		enumIcons.insert(index, QIcon(":/images/VerPattern"));
	}
	else if (name == "VerPattern")
	{
		enumNames.append("��ֱ��");
		enumIcons.insert(index, QIcon(":/images/HorPattern"));
	}
	else if (name == "CrossPattern")
	{
		enumNames.append("������");
		enumIcons.insert(index, QIcon(":/images/CrossPattern"));
	}
	else if (name == "BDiagPattern")
	{
		enumNames.append("����Խ���");
		enumIcons.insert(index, QIcon(":/images/BDiagPattern"));
	}
	else if (name == "FDiagPattern")
	{
		enumNames.append("ǰ��Խ���");
		enumIcons.insert(index, QIcon(":/images/FDiagPattern"));
	}
	else if (name == "DiagCrossPattern")
	{
		enumNames.append("����Խ���");
		enumIcons.insert(index, QIcon(":/images/DiagCrossPattern"));
	}
}
