#include "shaprelate.h"
#include "skdraw.h"
#include "seldb.h"

///////////////////////// PropertyState /////////////////////////
PropertyState::PropertyState(QWidget *parent)
	: QWidget(parent)
{
	Init();
}

PropertyState::~PropertyState()
{

}

void PropertyState::Init()
{
	QtTreePropertyBrowser *browser = new QtTreePropertyBrowser(this);
	browser->setRootIsDecorated(true);
	m_pBrowser = browser;

	QVBoxLayout *layout = new QVBoxLayout(this);
	layout->setMargin(0);
	layout->addWidget(m_pBrowser);

	m_pEnumManager = new QtEnumPropertyManager(this);
	m_pVariantManager = new QtVariantPropertyManager(this);
	QtEnumEditorFactory *enumFactory = new QtEnumEditorFactory(this);
	QtVariantEditorFactory *variantFactory = new QtVariantEditorFactory(this);
	m_pBrowser->setFactoryForManager(m_pEnumManager, enumFactory);
	m_pBrowser->setFactoryForManager(m_pVariantManager, variantFactory);
}

void PropertyState::InitSlot()
{
	connect(m_pEnumManager, SIGNAL(valueChanged(QtProperty *, int)), this, SLOT(SlotValueChanged(QtProperty *, int)));
	connect(m_pVariantManager, SIGNAL(valueChanged(QtProperty *, const QVariant &)), this, SLOT(SlotValueChanged(QtProperty *, const QVariant &)));
}

void PropertyState::SlotValueChanged(QtProperty *property, int value)
{
	QString name = property->propertyName();
	if (name == "���ʷ��")
		m_pen.setStyle((Qt::PenStyle)value);
	else if (name == "��ˢ���")
		m_brush.setStyle((Qt::BrushStyle)value);
}

void PropertyState::SlotValueChanged(QtProperty *property, const QVariant &value)
{
	QString name = property->propertyName();
	if (name == "���ű���")
		m_scale = value.toFloat();
	else if (name == "��ת�Ƕ�")
		m_rotation = value.toFloat();
	else if (name == "������ɫ")
		m_pen.setColor(value.value<QColor>());
	else if (name == "���ʴ�ϸ")
		m_pen.setWidthF(value.toFloat());
	else if (name == "��ˢ��ɫ")
		m_brush.setColor(value.value<QColor>());
	else if (name == "����")
		m_text = value.toString();
	else if (name == "����")
		m_font = value.value<QFont>();
	else if (name == "ͼƬ·��")
		m_picturePath = value.toString();
}

void PropertyState::SetProperty(QString name)
{
	QtProperty *enumProperty = NULL;
	QtVariantProperty *subProperty = NULL;

	SetCommProperty();
	if (name == "�߶�ͼԪ" || name == "����ͼԪ")
	{
		SetPenProperty();
	}
	else if (name == "�����ͼԪ" || name.contains("����ͼԪ") || name.contains("Բ��ͼԪ") || name.contains("������ͼԪ") || name.contains("����ͼԪ"))
	{
		SetPenProperty();
		SetBrushProperty();
	}
	else if (name == "����ͼԪ")
	{
		subProperty = m_pVariantManager->addProperty(QVariant::Color,tr("������ɫ"));
		subProperty->setValue(m_pen.color());
		AddProperty(subProperty);

		subProperty = m_pVariantManager->addProperty(QVariant::String,tr("����"));
		subProperty->setValue(m_text);
		AddProperty(subProperty);

		subProperty = m_pVariantManager->addProperty(QVariant::Font,tr("����"));
		subProperty->setValue(m_font);
		AddProperty(subProperty);
	}
	else if (name == "ͼ��ͼԪ")
	{
		subProperty = m_pVariantManager->addProperty(QVariant::String,tr("ͼƬ·��"));
		subProperty->setValue(m_picturePath);
		AddProperty(subProperty);
	}
}

void PropertyState::AddProperty(QtVariantProperty *property)
{
	QtBrowserItem *item = m_pBrowser->addProperty(property);
	((QtTreePropertyBrowser*)m_pBrowser)->setExpanded(item, false);
}

void PropertyState::AddProperty(QtProperty *property)
{
	QtBrowserItem *item = m_pBrowser->addProperty(property);
	((QtTreePropertyBrowser*)m_pBrowser)->setExpanded(item, false);
}

void PropertyState::SetCommProperty()
{
	QtVariantProperty *subProperty = NULL;

	subProperty = m_pVariantManager->addProperty(QVariant::Double,tr("���ű���"));
	subProperty->setValue(m_scale);
	AddProperty(subProperty);

	subProperty = m_pVariantManager->addProperty(QVariant::Double,tr("��ת�Ƕ�"));
	subProperty->setValue(m_rotation);
	AddProperty(subProperty);
}

void PropertyState::SetPenProperty()
{
	QtProperty *enumProperty = NULL;
	QtVariantProperty *subProperty = NULL;

	subProperty = m_pVariantManager->addProperty(QVariant::Color,tr("������ɫ"));
	subProperty->setValue(m_pen.color());
	AddProperty(subProperty);

	subProperty = m_pVariantManager->addProperty(QVariant::Double,tr("���ʴ�ϸ"));
	subProperty->setValue(m_pen.widthF());
	AddProperty(subProperty);

	enumProperty = m_pEnumManager->addProperty("���ʷ��");
	QStringList enumNames;
	QMap<int, QIcon> enumIcons;
	enumNames.append("��");
	enumIcons.insert(0, QIcon(":/images/none"));
	enumNames.append("ʵ��");
	enumIcons.insert(1, QIcon(":/images/SolidLine"));
	enumNames.append("����");
	enumIcons.insert(2, QIcon(":/images/DashLine"));
	enumNames.append("����");
	enumIcons.insert(3, QIcon(":/images/DotLine"));
	enumNames.append("�����");
	enumIcons.insert(4, QIcon(":/images/DashDotLine"));
	enumNames.append("������");
	enumIcons.insert(5, QIcon(":/images/DashDotDotLine"));
	m_pEnumManager->setEnumNames(enumProperty, enumNames);
	m_pEnumManager->setEnumIcons(enumProperty, enumIcons);
	m_pEnumManager->setValue(enumProperty, (int)m_pen.style());
	AddProperty(enumProperty);
}

void PropertyState::SetBrushProperty()
{
	QtProperty *enumProperty = NULL;
	QtVariantProperty *subProperty = NULL;

	subProperty = m_pVariantManager->addProperty(QVariant::Color,tr("��ˢ��ɫ"));
	subProperty->setValue(m_brush.color());
	AddProperty(subProperty);

	enumProperty = m_pEnumManager->addProperty("��ˢ���");
	QStringList enumNames;
	QMap<int, QIcon> enumIcons;
	enumNames.append("��");
	enumIcons.insert(0, QIcon(":/images/none"));
	enumNames.append("ʵ��");
	enumIcons.insert(1, QIcon(":/images/SolidPattern"));
	enumNames.append("�ܼ�1");
	enumIcons.insert(2, QIcon(":/images/Dense1Pattern"));
	enumNames.append("�ܼ�2");
	enumIcons.insert(3, QIcon(":/images/Dense2Pattern"));
	enumNames.append("�ܼ�3");
	enumIcons.insert(4, QIcon(":/images/Dense3Pattern"));
	enumNames.append("�ܼ�4");
	enumIcons.insert(5, QIcon(":/images/Dense4Pattern"));
	enumNames.append("�ܼ�5");
	enumIcons.insert(6, QIcon(":/images/Dense5Pattern"));
	enumNames.append("�ܼ�6");
	enumIcons.insert(7, QIcon(":/images/Dense6Pattern"));
	enumNames.append("�ܼ�7");
	enumIcons.insert(8, QIcon(":/images/Dense7Pattern"));
	enumNames.append("ˮƽ��");
	enumIcons.insert(9, QIcon(":/images/HorPattern"));
	enumNames.append("��ֱ��");
	enumIcons.insert(10, QIcon(":/images/VerPattern"));
	enumNames.append("������");
	enumIcons.insert(11, QIcon(":/images/CrossPattern"));
	enumNames.append("����Խ���");
	enumIcons.insert(12, QIcon(":/images/BDiagPattern"));
	enumNames.append("ǰ��Խ���");
	enumIcons.insert(13, QIcon(":/images/FDiagPattern"));
	enumNames.append("����Խ���");
	enumIcons.insert(14, QIcon(":/images/DiagCrossPattern"));
	m_pEnumManager->setEnumNames(enumProperty, enumNames);
	m_pEnumManager->setEnumIcons(enumProperty, enumIcons);
	m_pEnumManager->setValue(enumProperty, (int)m_brush.style());
	AddProperty(enumProperty);
}

///////////////////////// ShapRelate /////////////////////////
ShapRelate::ShapRelate(QWidget *parent)
	: SKWidget(parent)
{
	ui.setupUi(this);

	m_app = (SKDraw *)parent;

	Init();
	InitUi();
	InitSlot();
}

ShapRelate::~ShapRelate()
{

}

void ShapRelate::Init()
{
	QStringList list;
	list << "0" << "1" << "2" << "3" << "4";
	ui.comboBoxStateNum->addItems(list);
	ui.comboBoxStateNum->setCurrentIndex(0);

	ui.lineEditLinkDB->setReadOnly(true);
	ui.lineEditLinkScene->setReadOnly(true);
	ui.comboBoxStateNum->setEnabled(false);
	ui.comboBoxShowState->setEnabled(false);
	ui.tabWidgetState->setEnabled(false);
	ui.btnLinkDB->setEnabled(false);
	m_pSelDBWidget = NULL;
}

void ShapRelate::InitUi()
{
	setStyleSheet(tr("QWidget#%1{background:rgb(255,255,255,220);border-bottom-left-radius:6px;border-bottom-right-radius:6px;}").arg(objectName()));
}

void ShapRelate::InitSlot()
{
	connect(ui.btnOk, SIGNAL(clicked()), this, SLOT(SlotOk()));
	connect(ui.btnCancel, SIGNAL(clicked()), this, SLOT(SlotCancel()));
	connect(ui.btnLinkDB, SIGNAL(clicked()), this, SLOT(SlotLinkDB()));
	connect(ui.btnLinkScene, SIGNAL(clicked()), this, SLOT(SlotLinkScene()));
	connect(ui.comboBoxShowType, SIGNAL(currentIndexChanged(int)), this, SLOT(SlotShowTypeChanged(int)));
	connect(ui.comboBoxStateNum, SIGNAL(currentIndexChanged(int)), this, SLOT(SlotStateNumChanged(int)));
}

void ShapRelate::Start()
{
	QStringList list;
	list << "������" << "״̬����";
	ui.comboBoxShowType->addItems(list);
	ui.comboBoxShowType->setCurrentIndex(0);

	if (((GraphicsItem*)m_pShape)->DisplayName() == "���ͼԪ")
	{
		ui.comboBoxStateNum->setEnabled(false);
		ui.comboBoxShowState->setEnabled(false);
		ui.tabWidgetState->setEnabled(false);
		ui.btnLinkDB->setEnabled(true);
		ui.lineEditLinkDB->setText(((GraphicsItem*)m_pShape)->GetLinkDB());
		ui.lineEditLinkScene->setText(((GraphicsItem*)m_pShape)->GetLinkScene());
		return;
	}

	m_sShapeName = ((GraphicsItem*)m_pShape)->GetName();
	m_pen = ((GraphicsItem*)m_pShape)->GetPen();
	m_brush = ((GraphicsItem*)m_pShape)->GetBrush();
	m_rotation = ((GraphicsItem*)m_pShape)->GetRotation();
	m_scale = ((GraphicsItem*)m_pShape)->GetScale();
	if (m_sShapeName == "����ͼԪ")
	{
		ui.comboBoxShowType->addItem("��������");
		m_font = ((GraphicsTextItem*)m_pShape)->GetFont();
		m_text = ((GraphicsTextItem*)m_pShape)->GetText();
	}
	else if (m_sShapeName == "ͼ��ͼԪ")
	{
		m_picturePath = ((GraphicsPictureItem*)m_pShape)->GetFileName();
	}

	int count = ((GraphicsItem*)m_pShape)->m_mapShowStyle.count();
	ui.comboBoxStateNum->setCurrentIndex(count);
	ui.comboBoxShowType->setCurrentIndex(((GraphicsItem*)m_pShape)->GetShowType());
	ui.comboBoxShowState->setCurrentIndex(((GraphicsItem*)m_pShape)->GetShowState()+1);
	ui.lineEditLinkDB->setText(((GraphicsItem*)m_pShape)->GetLinkDB());
	ui.lineEditLinkScene->setText(((GraphicsItem*)m_pShape)->GetLinkScene());
}

void ShapRelate::paintEvent(QPaintEvent *e)
{
	SKWidget::paintEvent(e);
}

void ShapRelate::SlotShowTypeChanged(int index)
{
	switch (index)
	{
	case 0: //������
		ui.comboBoxStateNum->setEnabled(false);
		ui.comboBoxStateNum->setCurrentIndex(0);
		ui.comboBoxShowState->setEnabled(false);
		ui.comboBoxShowState->clear();
		ui.lineEditLinkDB->clear();
		ui.btnLinkDB->setEnabled(false);
		ui.tabWidgetState->setEnabled(false);
		ui.tabWidgetState->clear();
		break;
	case 1: //״̬����
		ui.comboBoxStateNum->setEnabled(true);
		ui.comboBoxShowState->setEnabled(true);
		ui.lineEditLinkDB->clear();
		ui.btnLinkDB->setEnabled(true);
		ui.tabWidgetState->setEnabled(true);
		break;
	case 2: //��������
		ui.comboBoxStateNum->setEnabled(false);
		ui.comboBoxStateNum->setCurrentIndex(0);
		ui.comboBoxShowState->setEnabled(false);
		ui.comboBoxShowState->clear();
		ui.lineEditLinkDB->clear();
		ui.btnLinkDB->setEnabled(true);
		ui.tabWidgetState->setEnabled(true);
		ui.tabWidgetState->clear();
		break;
	default:
		break;
	}
}

void ShapRelate::SlotStateNumChanged(int index)
{
	int num = ui.comboBoxStateNum->currentText().toInt();
	if (num == 0)
	{
		ui.comboBoxShowState->clear();
		ui.comboBoxShowState->setEnabled(false);
		ui.btnLinkDB->setEnabled(false);
		ui.tabWidgetState->clear();
		ui.tabWidgetState->setEnabled(false);
		return;
	}

	QStringList list;
	list.append(tr("���ݿ��ʼ״̬"));
	for (int i = 0; i < num; i++)
		list.append(tr("��ʾ״̬%1").arg(i));
	ui.comboBoxShowState->clear();
	ui.comboBoxShowState->addItems(list);
	ui.comboBoxShowState->setEnabled(true);
	ui.btnLinkDB->setEnabled(true);

	ui.tabWidgetState->clear();
	ui.tabWidgetState->setEnabled(true);
	for (int i = 0; i < num; i++)
	{
		QString value = ((GraphicsItem*)m_pShape)->m_mapShowStyle.value(i);
		if (value.isEmpty())
		{
			PropertyState *ps = new PropertyState(this);
			ps->m_pen = m_pen;
			ps->m_brush = m_brush;
			ps->m_font = m_font;
			ps->m_rotation = m_rotation;
			ps->m_scale = m_scale;
			ps->m_text = m_text;
			ps->m_picturePath = m_picturePath;
			ps->SetProperty(m_sShapeName);
			ps->InitSlot();
			ui.tabWidgetState->addTab(ps,tr("״̬%1").arg(i));
		}
		else
		{
			PropertyState *ps = new PropertyState(this);
			if (m_sShapeName == "�߶�ͼԪ" || m_sShapeName == "����ͼԪ")
			{
				QString strPen = value.split(":").at(1).split("|").at(0);
				SetPenFromString(ps, strPen);
				ps->m_rotation = value.split(":").at(1).split("|").at(1).toFloat();
				ps->m_scale = value.split(":").at(1).split("|").at(2).toFloat();
			}
			else if (m_sShapeName == "�����ͼԪ" || m_sShapeName == "����ͼԪ" || m_sShapeName == "Բ��ͼԪ" ||
					 m_sShapeName == "������ͼԪ" || m_sShapeName == "����ͼԪ")
			{
				QString strPen = value.split(":").at(1).split("|").at(0);
				QString strBrush = value.split(":").at(1).split("|").at(1);
				SetPenFromString(ps, strPen);
				SetBrushFromString(ps, strBrush);
				ps->m_rotation = value.split(":").at(1).split("|").at(2).toFloat();
				ps->m_scale = value.split(":").at(1).split("|").at(3).toFloat();
			}
			else if (m_sShapeName == "����ͼԪ")
			{
				QString strPen = value.split(":").at(1).split("|").at(0);				
				QString strFont = value.split(":").at(1).split("|").at(1);
				SetPenFromString(ps, strPen);
				SetFontFromString(ps, strFont);
				ps->m_text = value.split(":").at(1).split("|").at(2);
				ps->m_rotation = value.split(":").at(1).split("|").at(3).toFloat();
				ps->m_scale = value.split(":").at(1).split("|").at(4).toFloat();
			}
			else if (m_sShapeName == "ͼ��ͼԪ")
			{
				ps->m_picturePath = value.split(":").at(1).split("|").at(0);
				ps->m_rotation = value.split(":").at(1).split("|").at(1).toFloat();
				ps->m_scale = value.split(":").at(1).split("|").at(2).toFloat();
			}
			ps->SetProperty(m_sShapeName);
			ps->InitSlot();
			ui.tabWidgetState->addTab(ps,tr("״̬%1").arg(i));
		}
	}
}

void ShapRelate::SetPenFromString(PropertyState *ps, QString strPen)
{
	QStringList list = strPen.split(",");
	for (int i = 0; i < list.count(); i++)
	{
		if (i == 0)
		{
			QColor color;
			color.setNamedColor(list.at(i));
			color.setAlpha(list.at(i+1).toInt());
			ps->m_pen.setColor(color);
		}
		else if (i == 2)
			ps->m_pen.setWidthF(list.at(i).toFloat());
		else if (i == 3)
			ps->m_pen.setStyle((Qt::PenStyle)list.at(i).toInt());
	}
}

void ShapRelate::SetBrushFromString(PropertyState *ps, QString strBrush)
{
	QStringList list = strBrush.split(",");
	for (int i = 0; i < list.count(); i++)
	{
		if (i == 0)
		{
			QColor color;
			color.setNamedColor(list.at(i));
			color.setAlpha(list.at(i+1).toInt());
			ps->m_brush.setColor(color);
		}
		else if (i == 2)
			ps->m_brush.setStyle((Qt::BrushStyle)list.at(i).toInt());
	}
}

void ShapRelate::SetFontFromString(PropertyState *ps, QString strFont)
{
	QStringList list = strFont.split(",");
	for (int i = 0; i < list.count(); i++)
	{
		if (i == 0)
			ps->m_font.setFamily(list.at(i));
		else if (i == 1)
			ps->m_font.setPointSize(list.at(i).toInt());
		else if (i == 2)
			ps->m_font.setBold(list.at(i).toInt());
		else if (i == 3)
			ps->m_font.setItalic(list.at(i).toInt());
		else if (i == 4)
			ps->m_font.setUnderline(list.at(i).toInt());
		else if (i == 5)
			ps->m_font.setStrikeOut(list.at(i).toInt());
		else if (i == 6)
			ps->m_font.setKerning(list.at(i).toInt());
	}
}

void ShapRelate::SlotOk()
{
	((GraphicsItem*)m_pShape)->SetLinkDB(ui.lineEditLinkDB->text().trimmed());
	((GraphicsItem*)m_pShape)->SetLinkScene(ui.lineEditLinkScene->text().trimmed());
	if (((GraphicsItem*)m_pShape)->DisplayName() == "���ͼԪ")
	{
		OkGroup(m_pShape);
		emit SigClose();
		return;
	}

	if (ui.comboBoxShowState->isEnabled())
		((GraphicsItem*)m_pShape)->SetShowState(ui.comboBoxShowState->currentIndex()-1);
	else
		((GraphicsItem*)m_pShape)->SetShowState(-1);

	((GraphicsItem*)m_pShape)->SetShowType(ui.comboBoxShowType->currentIndex());

	((GraphicsItem*)m_pShape)->m_mapShowStyle.clear();
	int count = ui.tabWidgetState->count();
	for (int i = 0; i < count; i++)
	{
		PropertyState *ps = (PropertyState*)ui.tabWidgetState->widget(i);
		if (!ps)
			continue;

		QString strProperty;
		strProperty += tr("%1:").arg(i);
		if (m_sShapeName == "�߶�ͼԪ" || m_sShapeName == "����ͼԪ")
		{
			strProperty += tr("%1,%2,%3,%4|%5|%6")
				.arg(ps->m_pen.color().name()).arg(ps->m_pen.color().alpha()).arg(ps->m_pen.widthF()).arg(ps->m_pen.style()).arg(ps->m_rotation).arg(ps->m_scale);
		}
		else if (m_sShapeName == "�����ͼԪ" || m_sShapeName == "����ͼԪ" || m_sShapeName == "Բ��ͼԪ" ||
				 m_sShapeName == "������ͼԪ" || m_sShapeName == "����ͼԪ")
		{
			strProperty += tr("%1,%2,%3,%4|%5,%6,%7|%8|%9")
				.arg(ps->m_pen.color().name()).arg(ps->m_pen.color().alpha()).arg(ps->m_pen.widthF()).arg(ps->m_pen.style())
				.arg(ps->m_brush.color().name()).arg(ps->m_brush.color().alpha()).arg(ps->m_brush.style())
				.arg(ps->m_rotation).arg(ps->m_scale);
		}
		else if (m_sShapeName == "����ͼԪ")
		{
			strProperty += tr("%1,%2|%3,%4,%5,%6,%7,%8,%9|%10|%11|%12")
				.arg(ps->m_pen.color().name()).arg(ps->m_pen.color().alpha())
				.arg(ps->m_font.family()).arg(ps->m_font.pointSize()).arg(ps->m_font.bold()).arg(ps->m_font.italic())
				.arg(ps->m_font.underline()).arg(ps->m_font.strikeOut()).arg(ps->m_font.kerning())
				.arg(ps->m_text).arg(ps->m_rotation).arg(ps->m_scale);
		}
		else if (m_sShapeName == "ͼ��ͼԪ")
		{
			strProperty += tr("%1|%2|%3")
				.arg(ps->m_picturePath).arg(ps->m_rotation).arg(ps->m_scale);
		}

		((GraphicsItem*)m_pShape)->m_mapShowStyle.insert(i,strProperty);
	}

	emit SigClose();
}

void ShapRelate::SlotCancel()
{
	emit SigClose();
}

void ShapRelate::OkGroup(QGraphicsItem *item)
{
	GraphicsItemGroup *group = (GraphicsItemGroup*)item;
	foreach (QGraphicsItem *it, group->childItems())
	{
		AbstractShape *ab = qgraphicsitem_cast<AbstractShape*>(it);
		if (ab && !qgraphicsitem_cast<SizeHandleRect*>(ab))
		{
			if (((GraphicsItem*)it)->DisplayName() == "���ͼԪ")
				OkGroup(it);

			((GraphicsItem*)ab)->SetLinkDB(ui.lineEditLinkDB->text().trimmed());
			((GraphicsItem*)ab)->SetLinkScene(ui.lineEditLinkScene->text().trimmed());
		}
	}
}

void ShapRelate::SlotLinkDB()
{
	if (!m_app->GetDBState())
	{
		QMessageBox::warning(NULL,tr("�澯"),tr("���ݿ������쳣"));
		return;
	}
	
	if (!m_pSelDBWidget)
	{
		SelDB *sd = new SelDB(this);
		sd->SetPoint(ui.lineEditLinkDB->text().trimmed());
		m_pSelDBWidget = new SKBaseWidget(NULL,sd);
		m_pSelDBWidget->SetWindowsFlagsDialog();
		m_pSelDBWidget->SetWindowsModal();
		m_pSelDBWidget->SetWindowTitle("ѡ������");
#ifdef WIN32
		m_pSelDBWidget->SetWindowIcon(QIcon(":/images/dblink"));
#else
		m_pSelDBWidget->SetWindowIcon(":/images/dblink");
#endif
		m_pSelDBWidget->SetWindowFlags(0);
		m_pSelDBWidget->SetWindowSize(900,600);
		m_pSelDBWidget->SetIsDrag(true);
		connect(m_pSelDBWidget, SIGNAL(SigClose()), this, SLOT(SlotSelDBClose()));
	}

	((SelDB*)m_pSelDBWidget->GetCenterWidget())->Start();
	m_pSelDBWidget->Show();
}

void ShapRelate::SlotSelDBClose()
{
	ui.lineEditLinkDB->setText(((SelDB*)m_pSelDBWidget->GetCenterWidget())->GetPoint());

	disconnect(m_pSelDBWidget, SIGNAL(SigClose()), this, SLOT(SlotLinkDataClose()));
	delete m_pSelDBWidget;
	m_pSelDBWidget = NULL;
}

void ShapRelate::SlotLinkScene()
{

}
