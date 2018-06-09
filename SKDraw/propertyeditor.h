#ifndef PROPERTYEDITOR_H
#define PROPERTYEDITOR_H

#include "skhead.h"
#include "qtpropertybrowser.h"
#include "qtpropertymanager.h"
#include "qteditorfactory.h"
#include "qttreepropertybrowser.h"
#include "qtvariantproperty.h"

class PropertyEditor : public QWidget
{
	Q_OBJECT

public:
	PropertyEditor(QWidget *parent = 0);
	~PropertyEditor();

	void InitMap();

public:
	QtAbstractPropertyBrowser *m_pBrowser;
	QtEnumPropertyManager	  *m_pEnumManager;
	QtVariantPropertyManager  *m_pVariantmanager;

public:
	void Clear();
	void SetObject(QObject *object);
	QObject* GetObject() const { return m_pObject; }

	void AddProperties(const QMetaObject *metaObject);
	void UpdateProperties(const QMetaObject *metaObject);

private:
	QObject *m_pObject;
	QList<QtProperty*> m_lstTopLevelProperties;
	QMap<QtProperty *, int> m_propertyToIndex;
	QMap<const QMetaObject*, QMap<int, QtVariantProperty*>> m_classToIndexToProperty;
	QMap<QString, QString> m_mapTranslate;

private:
	void EnumEditor(int index, QString name, QStringList &enumNames, QMap<int, QIcon> &enumIcons);

private slots:
	void SlotValueChanged(QtProperty *property, int value);
	void SlotValueChanged(QtProperty *property, const QVariant &value);
	
};

#endif // PROPERTYEDITOR_H
