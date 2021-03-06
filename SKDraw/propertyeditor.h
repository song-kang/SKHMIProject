#ifndef PROPERTYEDITOR_H
#define PROPERTYEDITOR_H

#include "skhead.h"
#include "qtpropertybrowser.h"
#include "qtpropertymanager.h"
#include "qteditorfactory.h"
#include "qttreepropertybrowser.h"
#include "qtvariantproperty.h"

class DrawScene;
class DrawView;
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
	QtVariantPropertyManager  *m_pVariantManager;
	QtVariantPropertyManager  *m_pCustomManager;

public:
	void Clear();
	void SetObject(QObject *object);
	void SetBackground();
	void SetScene(DrawScene *scene) { m_pScene = scene; }
	QObject* GetObject() const { return m_pObject; }

	void AddProperties(const QMetaObject *metaObject);
	void AddCustomProperty(QtVariantProperty *property);
	void UpdateProperties(const QMetaObject *metaObject);

private:
	QObject *m_pObject;
	QList<QtProperty*> m_lstTopLevelProperties;
	QMap<QtProperty *, int> m_propertyToIndex;
	QMap<const QMetaObject*, QMap<int, QtVariantProperty*> > m_classToIndexToProperty;
	QMap<const QMetaObject*, QMap<int, QtProperty*> > m_classToIndexToEnumProperty;
	QMap<QString, QString> m_mapTranslate;
	DrawScene *m_pScene;

private:
	bool IsVisibleProperty(QString property);
	void EnumEditor(int index, QString name, QStringList &enumNames, QMap<int, QIcon> &enumIcons);
	void PropertyCommand(QtProperty *property, const QVariant &value, QPen oldPen);
	void PropertyCommand(QtProperty *property, const QVariant &value, QBrush oldBrush);
	void PropertyCommand(QtProperty *property, const QVariant &value, QFont oldFont);
	void PropertyCommand(QtProperty *property, const QVariant &value, qreal oldScale);

private slots:
	void SlotValueChanged(QtProperty *property, int value);
	void SlotValueChanged(QtProperty *property, const QVariant &value);
	void SlotCustomValueChanged(QtProperty *property, const QVariant &value);
	
};

#endif // PROPERTYEDITOR_H
