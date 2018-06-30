#ifndef SHAPRELATE_H
#define SHAPRELATE_H

#include "skhead.h"
#include "ui_shaprelate.h"
#include "skwidget.h"
#include "drawobj.h"
#include "qtpropertybrowser.h"
#include "qtpropertymanager.h"
#include "qteditorfactory.h"
#include "qttreepropertybrowser.h"
#include "qtvariantproperty.h"

///////////////////////// PropertyState /////////////////////////
class PropertyState : public QWidget
{
	Q_OBJECT

public:
	PropertyState(QWidget *parent = 0);
	~PropertyState();

	void Init();
	void InitSlot();
	void SetProperty(QString name);

public:
	QtAbstractPropertyBrowser *m_pBrowser;
	QtEnumPropertyManager	  *m_pEnumManager;
	QtVariantPropertyManager  *m_pVariantManager;

	QPen m_pen;
	QBrush m_brush;
	qreal m_rotation;
	qreal m_scale;
	QFont m_font;
	QString m_text;
	QString m_picturePath;

private:
	void AddProperty(QtProperty *property);
	void AddProperty(QtVariantProperty *property);
	void SetCommProperty();
	void SetPenProperty();
	void SetBrushProperty();

private slots:
	void SlotValueChanged(QtProperty *property, int value);
	void SlotValueChanged(QtProperty *property, const QVariant &value);

};

///////////////////////// ShapRelate /////////////////////////
class ShapRelate : public SKWidget
{
	Q_OBJECT

public:
	ShapRelate(QWidget *parent = 0);
	~ShapRelate();

	void Start();
	void SetItem(QGraphicsItem *item) { m_pShape = item; }

private:
	Ui::ShapRelate ui;

	QGraphicsItem *m_pShape;

public:
	QString m_sShapeName;
	QPen m_pen;
	QBrush m_brush;
	qreal m_rotation;
	qreal m_scale;
	QFont m_font;
	QString m_text;
	QString m_picturePath;

protected:
	virtual void paintEvent(QPaintEvent *);

private:
	void Init();
	void InitUi();
	void InitSlot();
	void SetPenFromString(PropertyState *ps, QString strPen);
	void SetBrushFromString(PropertyState *ps, QString strBrush);
	void SetFontFromString(PropertyState *ps, QString strFont);
	void OkGroup(QGraphicsItem *item);

signals:
	void SigClose();

private slots:
	void SlotOk();
	void SlotLinkDB();
	void SlotLinkScene();
	void SlotStateNumChanged(int index);

};

#endif // SHAPRELATE_H
