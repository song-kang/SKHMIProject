#include "sk_svg_xml.h"
#include "math.h"

//================ CSKItem =======================
CSKItem::CSKItem()
{
	m_eType = eItemNone;
	m_width = 0.0;
	m_height = 0.0;
	m_scale = 1.0;
	m_rotation = 0.0;
	m_penWidth = 1.0;
	m_penStyle = ItemSolidLine;
	m_penColor = "#FFFFFF";
	m_penAlpha = 255;
	m_brushStyle = ItemSolidPattern;
	m_brushColor = "#008000";
	m_brushAlpha = 255;
	m_tooltip = "";
	m_startAngle = 0.0;
	m_endAngle = 0.0;
	m_crossAngle = 0.0;
	m_z = 0.0;
	m_showType = 0;
	m_linkDB = "";
	m_linkScene = "";
}

CSKItem::~CSKItem()
{
	for (int i = 0; i < m_items.count(); i++)
		delete m_items.at(i);

	for (int i = 0; i < m_points.count(); i++)
		delete m_points.at(i);
}

//================ CSKTextItem =======================
CSKTextItem::CSKTextItem()
{
	m_desc = "文字";
	m_timeStyle = "yyyy-MM-dd hh:mm:ss";
	m_family = "宋体";
	m_pointSize = 12;
	m_isBold = false;
	m_isItalic = false;
	m_isUnderline = false;
	m_isStrikeOut = false;
	m_isKerning = false;
	m_alignment = ItemAlignCenter;
}

CSKTextItem::~CSKTextItem()
{

}

//================ CSKSvgXml =======================
CSKSvgXml::CSKSvgXml()
{
	m_backgroundWidth = 1900;
	m_backgroundHeight = 900;
	m_backgroundColor = "#000000";
	m_backgroundAlpha = 255;
}

CSKSvgXml::~CSKSvgXml()
{
	for (int i = 0; i < m_items.count(); i++)
		delete m_items.at(i);
}

CSKItem* CSKSvgXml::CreateLine(SPtrList<stuPoint> *points, SString color, float width)
{
	if (points->count() <= 1 || points->count() > 2)
		return NULL;

	CSKItem *item = new CSKItem;
	if (item)
	{
		stuPoint *point1 = points->at(0);
		stuPoint *point2 = points->at(1);
		item->m_eType = eItemLine;
		item->m_points.append(point1);
		item->m_points.append(point2);
		item->m_width  = abs(point2->m_x - point1->m_x);
		item->m_height = abs(point2->m_y - point1->m_y);
		item->m_penColor = color;
		item->m_penWidth = width;
		m_items.append(item);
	}

	return item;
}

CSKItem* CSKSvgXml::CreatePolygonLine(SPtrList<stuPoint> *points, SString color, float width)
{
	if (points->count() < 2)
		return NULL;

	CSKItem *item = new CSKItem;
	if (item)
	{
		float min_x = points->at(0)->m_x;
		float min_y = points->at(0)->m_y;
		float max_x = points->at(0)->m_x;
		float max_y = points->at(0)->m_y;
		item->m_points.append(points->at(0));
		for (int i = 1; i < points->count(); i++)
		{
			stuPoint *p = points->at(i);
			if (p->m_x < min_x)
				min_x = p->m_x;
			else if (p->m_x > max_x)
				max_x = p->m_x;

			if (p->m_y < min_y)
				min_y = p->m_y;
			else if (p->m_y > max_y)
				max_y = p->m_y;

			item->m_points.append(p);
		}

		item->m_eType = eItemPolygonLine;
		item->m_width  = abs(max_x - min_x);
		item->m_height = abs(max_y - min_y);
		item->m_penColor = color;
		item->m_penWidth = width;
		m_items.append(item);
	}

	return item;
}

CSKItem* CSKSvgXml::CreateArcCircle(stuPoint point, float diameter, float startAngle, float endAngle, SString color, float width)
{
	CSKItem *item = new CSKItem;
	if (item)
	{
		item->m_eType = eItemArcCircle;
		item->m_origin.m_x = point.m_x + diameter / 2;
		item->m_origin.m_y = point.m_y + diameter / 2;
		item->m_width  = diameter;
		item->m_height = diameter;
		item->m_startAngle = startAngle;
		item->m_endAngle = endAngle;
		item->m_penColor = color;
		item->m_penWidth = width;
		m_items.append(item);
	}

	return item;
}

CSKItem* CSKSvgXml::CreateArcEllipse(stuPoint point, float longAxis, float shortAxis, float startAngle, float endAngle, SString color, float width)
{
	CSKItem *item = new CSKItem;
	if (item)
	{
		item->m_eType = eItemArcEllipse;
		item->m_origin.m_x = point.m_x + longAxis / 2;
		item->m_origin.m_y = point.m_y + shortAxis / 2;
		item->m_width  = longAxis;
		item->m_height = shortAxis;
		item->m_startAngle = startAngle;
		item->m_endAngle = endAngle;
		item->m_penColor = color;
		item->m_penWidth = width;
		m_items.append(item);
	}

	return item;
}

CSKItem* CSKSvgXml::CreateRect(stuPoint point, float width, float height)
{
	CSKItem *item = new CSKItem;
	if (item)
	{
		item->m_eType = eItemRect;
		item->m_origin.m_x = point.m_x + width / 2;
		item->m_origin.m_y = point.m_y + height / 2;
		item->m_width  = width;
		item->m_height = height;
		m_items.append(item);
	}

	return item;
}

CSKItem* CSKSvgXml::CreateRoundedRect(stuPoint point, float width, float height, float rx, float ry)
{
	CSKItem *item = new CSKItem;
	if (item)
	{
		item->m_eType = eItemRoundedRect;
		item->m_origin.m_x = point.m_x + width / 2;
		item->m_origin.m_y = point.m_y + height / 2;
		item->m_width  = width;
		item->m_height = height;
		item->m_rx = rx;
		item->m_ry = ry;
		m_items.append(item);
	}

	return item;
}

CSKItem* CSKSvgXml::CreateCircle(stuPoint point, float diameter)
{
	CSKItem *item = new CSKItem;
	if (item)
	{
		item->m_eType = eItemCircle;
		item->m_origin.m_x = point.m_x + diameter / 2;
		item->m_origin.m_y = point.m_y + diameter / 2;
		item->m_width  = diameter;
		item->m_height = diameter;
		m_items.append(item);
	}

	return item;
}

CSKItem* CSKSvgXml::CreateEllipse(stuPoint point, float longAxis, float shortAxis)
{
	CSKItem *item = new CSKItem;
	if (item)
	{
		item->m_eType = eItemEllipse;
		item->m_origin.m_x = point.m_x + longAxis / 2;
		item->m_origin.m_y = point.m_y + shortAxis / 2;
		item->m_width  = longAxis;
		item->m_height = shortAxis;
		m_items.append(item);
	}

	return item;
}

CSKItem* CSKSvgXml::CreateTriangle(stuPoint point, float width, float height)
{
	CSKItem *item = new CSKItem;
	if (item)
	{
		item->m_eType = eItemTriangle;
		item->m_origin.m_x = point.m_x + width / 2;
		item->m_origin.m_y = point.m_y + height / 2;
		item->m_width  = width;
		item->m_height = height;
		m_items.append(item);
	}

	return item;
}

CSKItem* CSKSvgXml::CreateRhombus(stuPoint point, float width, float height)
{
	CSKItem *item = new CSKItem;
	if (item)
	{
		item->m_eType = eItemRhombus;
		item->m_origin.m_x = point.m_x + width / 2;
		item->m_origin.m_y = point.m_y + height / 2;
		item->m_width  = width;
		item->m_height = height;
		m_items.append(item);
	}

	return item;
}

CSKItem* CSKSvgXml::CreateParallelogram(stuPoint point, float width, float height, float crossAngle)
{
	CSKItem *item = new CSKItem;
	if (item)
	{
		item->m_eType = eItemParallelogram;
		item->m_origin.m_x = point.m_x + width / 2;
		item->m_origin.m_y = point.m_y + height / 2;
		item->m_width  = width;
		item->m_height = height;
		item->m_crossAngle = crossAngle;
		m_items.append(item);
	}

	return item;
}

CSKItem* CSKSvgXml::CreatePolygon(SPtrList<stuPoint> *points)
{
	if (points->count() < 2)
		return NULL;

	CSKItem *item = new CSKItem;
	if (item)
	{
		float min_x = points->at(0)->m_x;
		float min_y = points->at(0)->m_y;
		float max_x = points->at(0)->m_x;
		float max_y = points->at(0)->m_y;
		item->m_points.append(points->at(0));
		for (int i = 1; i < points->count(); i++)
		{
			stuPoint *p = points->at(i);
			if (p->m_x < min_x)
				min_x = p->m_x;
			else if (p->m_x > max_x)
				max_x = p->m_x;

			if (p->m_y < min_y)
				min_y = p->m_y;
			else if (p->m_y > max_y)
				max_y = p->m_y;

			item->m_points.append(p);
		}

		item->m_eType = eItemPolygon;
		item->m_width  = abs(max_x - min_x);
		item->m_height = abs(max_y - min_y);
		m_items.append(item);
	}

	return item;
}

CSKTextItem* CSKSvgXml::CreateText(stuPoint point, float width, float height, SString desc)
{
	CSKTextItem *item = new CSKTextItem;
	if (item)
	{
		item->m_eType = eItemText;
		item->m_origin.m_x = point.m_x + width / 2;
		item->m_origin.m_y = point.m_y + height / 2;
		item->m_width  = width;
		item->m_height = height;
		item->m_desc = desc;
		m_items.append(item);
	}

	return item;
}

CSKTextItem* CSKSvgXml::CreateTextTime(stuPoint point, float width, float height, SString style)
{
	CSKTextItem *item = new CSKTextItem;
	if (item)
	{
		item->m_eType = eItemTextTime;
		item->m_origin.m_x = point.m_x + width / 2;
		item->m_origin.m_y = point.m_y + height / 2;
		item->m_width  = width;
		item->m_height = height;
		item->m_timeStyle = style;
		m_items.append(item);
	}

	return item;
}

void CSKSvgXml::CreateArrowLine(SPtrList<stuPoint> *points, float arrowScale, SString color, float width, float z)
{
	CSKItem *item = CreateLine(points, color, width);
	item->m_z = z;

	float cx = points->at(1)->m_x;
	float cy = points->at(1)->m_y;
	float rx = points->at(0)->m_x-points->at(1)->m_x;
	float ry = points->at(0)->m_y-points->at(1)->m_y;
	float dis = sqrt(rx*rx+ry*ry);
	if (dis < 0.01)
		return;

	bool bConv = false;
	if (points->at(1)->m_y > points->at(0)->m_y)
		bConv = true;

	float fcos = (bConv ? -rx : rx) / dis;
	float radian = (float)(acos(fcos)-3.1415926);
	if (bConv)
		radian += (float)3.1415926;

	float x3 = cx-10*arrowScale;
	float y3 = cy-3*arrowScale;
	float x4 = cx-10*arrowScale;
	float y4 = cy+3*arrowScale;

	float x30 = (float)(x3-cx)*cos(radian)-(y3-cy)*sin(radian)+cx;
	float x40 = (float)(x4-cx)*cos(radian)-(y4-cy)*sin(radian)+cx;
	float y30 = (float)(x3-cx)*sin(radian)+(y3-cy)*cos(radian)+cy;
	float y40 = (float)(x4-cx)*sin(radian)+(y4-cy)*cos(radian)+cy;

	SPtrList<stuPoint> lstPoint;
	stuPoint *p = new stuPoint;
	p->m_x = cx;
	p->m_y = cy;
	lstPoint.append(p);
	p = new stuPoint;
	p->m_x = x30;
	p->m_y = y30;
	lstPoint.append(p);
	p = new stuPoint;
	p->m_x = x40;
	p->m_y = y40;
	lstPoint.append(p);
	item = CreatePolygon(&lstPoint);
	item->m_penColor = color;
	item->m_penWidth = width;
	item->m_brushColor = color;
	item->m_z = z;
}

void CSKSvgXml::CreateDoubleArrowLine(SPtrList<stuPoint> *points, float arrowScale, SString color, float width, float z)
{
	CSKItem *item = CreateLine(points, color, width);
	item->m_z = z;

	//前箭头
	float cx = points->at(1)->m_x;
	float cy = points->at(1)->m_y;
	float rx = points->at(0)->m_x-points->at(1)->m_x;
	float ry = points->at(0)->m_y-points->at(1)->m_y;
	float dis = sqrt(rx*rx+ry*ry);
	if (dis < 0.01)
		return;

	bool bConv = false;
	if (points->at(1)->m_y > points->at(0)->m_y)
		bConv = true;

	float fcos = (bConv ? -rx : rx) / dis;
	float radian = (float)(acos(fcos)-3.1415926);
	if (bConv)
		radian += (float)3.1415926;

	float x3 = cx-10*arrowScale;
	float y3 = cy-3*arrowScale;
	float x4 = cx-10*arrowScale;
	float y4 = cy+3*arrowScale;

	float x30 = (float)(x3-cx)*cos(radian)-(y3-cy)*sin(radian)+cx;
	float x40 = (float)(x4-cx)*cos(radian)-(y4-cy)*sin(radian)+cx;
	float y30 = (float)(x3-cx)*sin(radian)+(y3-cy)*cos(radian)+cy;
	float y40 = (float)(x4-cx)*sin(radian)+(y4-cy)*cos(radian)+cy;

	SPtrList<stuPoint> lstPoint;
	stuPoint *p = new stuPoint;
	p->m_x = cx;
	p->m_y = cy;
	lstPoint.append(p);
	p = new stuPoint;
	p->m_x = x30;
	p->m_y = y30;
	lstPoint.append(p);
	p = new stuPoint;
	p->m_x = x40;
	p->m_y = y40;
	lstPoint.append(p);
	item = CreatePolygon(&lstPoint);
	item->m_penColor = color;
	item->m_penWidth = width;
	item->m_brushColor = color;
	item->m_z = z;

	//后箭头
	cx = points->at(0)->m_x;
	cy = points->at(0)->m_y;
	rx = points->at(1)->m_x-points->at(0)->m_x;
	ry = points->at(1)->m_y-points->at(0)->m_y;
	dis = sqrt(rx*rx+ry*ry);
	if (dis < 0.01)
		return;

	bConv = false;
	if (points->at(0)->m_y > points->at(1)->m_y)
		bConv = true;

	fcos = (bConv ? -rx : rx) / dis;
	radian = (float)(acos(fcos)-3.1415926);
	if (bConv)
		radian += (float)3.1415926;

	x3 = cx-10*arrowScale;
	y3 = cy-3*arrowScale;
	x4 = cx-10*arrowScale;
	y4 = cy+3*arrowScale;

	x30 = (float)(x3-cx)*cos(radian)-(y3-cy)*sin(radian)+cx;
	x40 = (float)(x4-cx)*cos(radian)-(y4-cy)*sin(radian)+cx;
	y30 = (float)(x3-cx)*sin(radian)+(y3-cy)*cos(radian)+cy;
	y40 = (float)(x4-cx)*sin(radian)+(y4-cy)*cos(radian)+cy;

	SPtrList<stuPoint> lstPoint1;
	p = new stuPoint;
	p->m_x = cx;
	p->m_y = cy;
	lstPoint1.append(p);
	p = new stuPoint;
	p->m_x = x30;
	p->m_y = y30;
	lstPoint1.append(p);
	p = new stuPoint;
	p->m_x = x40;
	p->m_y = y40;
	lstPoint1.append(p);
	item = CreatePolygon(&lstPoint1);
	item->m_penColor = color;
	item->m_penWidth = width;
	item->m_brushColor = color;
	item->m_z = z;
}

bool CSKSvgXml::SaveToXml(SString &content, SString encoding)
{
	SXmlConfig xml;
	xml.SetNodeName("canvas");
	xml.SetAttribute("width",SString::toFormat("%d",m_backgroundWidth));
	xml.SetAttribute("height",SString::toFormat("%d",m_backgroundHeight));
	xml.SetAttribute("color",m_backgroundColor);
	xml.SetAttribute("alpha",SString::toFormat("%d",m_backgroundAlpha));

	for (int i = 0; i < m_items.count(); i++)
	{
		CSKItem *item = m_items.at(i);
		switch (item->m_eType)
		{
		case eItemNone:
			break;
		case eItemLine:				//线段
			LineToXml(item, &xml);
			break;
		case eItemPolygonLine:		//折线
			PolygonLineToXml(item, &xml);
			break;
		case eItemArcCircle:		//圆弧线
			ArcCircleToXml(item, &xml);
			break;
		case eItemArcEllipse:		//椭圆弧线
			ArcEllipseToXml(item, &xml);
			break;
		case eItemRect:				//矩形
			RectToXml(item, &xml);
			break;
		case eItemRoundedRect:		//圆角矩形
			RoundedRectToXml(item, &xml);
			break;
		case eItemCircle:			//圆形
			CircleToXml(item, &xml);
			break;
		case eItemEllipse:			//椭圆形
			EllipseToXml(item, &xml);
			break;
		case eItemTriangle:			//三角形
			TriangleToXml(item, &xml);
			break;
		case eItemRhombus:			//菱形
			RhombusToXml(item, &xml);
			break;
		case eItemParallelogram:	//平行四边形
			ParallelogramToXml(item, &xml);
			break;
		case eItemPolygon:			//不规则多边形
			PolygonToXml(item, &xml);
			break;
		case eItemPicture:			//图片
			PictureToXml(item, &xml);
			break;
		case eItemText:				//文本
			TextToXml(item, &xml);
			break;
		case eItemTextTime:			//文本时间
			TextTimeToXml(item, &xml);
			break;
		default:
			break;
		}
	}

	return xml.SaveConfigToText(content, encoding);
}

bool CSKSvgXml::BaseToXml(CSKItem *item, SBaseConfig *base)
{
	base->SetAttribute("x",SString::toFormat("%f",item->m_origin.m_x));
	base->SetAttribute("y",SString::toFormat("%f",item->m_origin.m_y));
	base->SetAttribute("z",SString::toFormat("%f",item->m_z));
	base->SetAttribute("width",SString::toFormat("%f",item->m_width));
	base->SetAttribute("height",SString::toFormat("%f",item->m_height));
	base->SetAttribute("rotate",SString::toFormat("%f",item->m_rotation));
	base->SetAttribute("scale",SString::toFormat("%f",item->m_scale));
	base->SetAttribute("tooltip",item->m_tooltip);
	base->SetAttribute("penWidth",SString::toFormat("%f",item->m_penWidth));
	base->SetAttribute("penColor",item->m_penColor);
	base->SetAttribute("penAlpha",SString::toFormat("%d",item->m_penAlpha));
	base->SetAttribute("penStyle",SString::toFormat("%d",item->m_penStyle));
	base->SetAttribute("brushColor",item->m_brushColor);
	base->SetAttribute("brushAlpha",SString::toFormat("%d",item->m_brushAlpha));
	base->SetAttribute("brushStyle",SString::toFormat("%d",item->m_brushStyle));
	base->SetAttribute("showtype",SString::toFormat("%d",item->m_showType));
	base->SetAttribute("linkdb",item->m_linkDB);
	base->SetAttribute("linkscene",item->m_linkScene);

	return true;
}

bool CSKSvgXml::FontToXml(CSKTextItem *item, SBaseConfig *base)
{
	base->SetAttribute("family",item->m_family);
	base->SetAttribute("pointSize",SString::toFormat("%d",item->m_pointSize));
	base->SetAttribute("bold",SString::toFormat("%d",item->m_isBold));
	base->SetAttribute("italic",SString::toFormat("%d",item->m_isItalic));
	base->SetAttribute("underline",SString::toFormat("%d",item->m_isUnderline));
	base->SetAttribute("strikeOut",SString::toFormat("%d",item->m_isStrikeOut));
	base->SetAttribute("kerning",SString::toFormat("%d",item->m_isKerning));
	base->SetAttribute("alignment",SString::toFormat("%d",item->m_alignment));

	return true;
}

bool CSKSvgXml::LineToXml(CSKItem *item, SBaseConfig *canvas)
{
	SBaseConfig *base = canvas->AddChildNode("line");
	BaseToXml(item, base);
	for (int i = 0; i < item->m_points.count(); i++)
	{
		stuPoint *p = item->m_points.at(i);
		base->AddChildNode("point",SString::toFormat("x=%f;y=%f",p->m_x,p->m_y));
	}

	return true;
}

bool CSKSvgXml::PolygonLineToXml(CSKItem *item, SBaseConfig *canvas)
{
	SBaseConfig *base = canvas->AddChildNode("polyline");
	BaseToXml(item, base);
	for (int i = 0; i < item->m_points.count(); i++)
	{
		stuPoint *p = item->m_points.at(i);
		base->AddChildNode("point",SString::toFormat("x=%f;y=%f",p->m_x,p->m_y));
	}

	return true;
}

bool CSKSvgXml::ArcCircleToXml(CSKItem *item, SBaseConfig *canvas)
{
	SBaseConfig *base = canvas->AddChildNode("arcCircle");
	BaseToXml(item, base);
	base->SetAttribute("startAngle",SString::toFormat("%f",item->m_startAngle));
	base->SetAttribute("endAngle",SString::toFormat("%f",item->m_endAngle));
	return true;
}

bool CSKSvgXml::ArcEllipseToXml(CSKItem *item, SBaseConfig *canvas)
{
	SBaseConfig *base = canvas->AddChildNode("arcEllipse");
	BaseToXml(item, base);
	base->SetAttribute("startAngle",SString::toFormat("%f",item->m_startAngle));
	base->SetAttribute("endAngle",SString::toFormat("%f",item->m_endAngle));
	return true;
}

bool CSKSvgXml::RectToXml(CSKItem *item, SBaseConfig *canvas)
{
	SBaseConfig *base = canvas->AddChildNode("rect");
	return BaseToXml(item, base);
}

bool CSKSvgXml::RoundedRectToXml(CSKItem *item, SBaseConfig *canvas)
{
	SBaseConfig *base = canvas->AddChildNode("roundrect");
	base->SetAttribute("rx",SString::toFormat("%f",item->m_rx));
	base->SetAttribute("ry",SString::toFormat("%f",item->m_ry));
	return BaseToXml(item, base);
}

bool CSKSvgXml::CircleToXml(CSKItem *item, SBaseConfig *canvas)
{
	SBaseConfig *base = canvas->AddChildNode("circle");
	return BaseToXml(item, base);
}

bool CSKSvgXml::EllipseToXml(CSKItem *item, SBaseConfig *canvas)
{
	SBaseConfig *base = canvas->AddChildNode("ellipse");
	return BaseToXml(item, base);
}

bool CSKSvgXml::TriangleToXml(CSKItem *item, SBaseConfig *canvas)
{
	SBaseConfig *base = canvas->AddChildNode("triangle");
	return BaseToXml(item, base);
}

bool CSKSvgXml::RhombusToXml(CSKItem *item, SBaseConfig *canvas)
{
	SBaseConfig *base = canvas->AddChildNode("rhombus");
	return BaseToXml(item, base);
}

bool CSKSvgXml::ParallelogramToXml(CSKItem *item, SBaseConfig *canvas)
{
	SBaseConfig *base = canvas->AddChildNode("parallelogram");
	BaseToXml(item, base);
	base->SetAttribute("angle",SString::toFormat("%f",item->m_crossAngle));
	return true;
}

bool CSKSvgXml::PolygonToXml(CSKItem *item, SBaseConfig *canvas)
{
	SBaseConfig *base = canvas->AddChildNode("polygon");
	BaseToXml(item, base);
	for (int i = 0; i < item->m_points.count(); i++)
	{
		stuPoint *p = item->m_points.at(i);
		base->AddChildNode("point",SString::toFormat("x=%f;y=%f",p->m_x,p->m_y));
	}

	return true;
}

bool CSKSvgXml::PictureToXml(CSKItem *item, SBaseConfig *canvas)
{
	SBaseConfig *base = canvas->AddChildNode("picture");
	return BaseToXml(item, base);
}

bool CSKSvgXml::TextToXml(CSKItem *item, SBaseConfig *canvas)
{
	SBaseConfig *base = canvas->AddChildNode("text");
	BaseToXml(item, base);
	base->SetAttribute("desc",((CSKTextItem*)item)->m_desc);
	FontToXml((CSKTextItem*)item, base);
	return true;
}

bool CSKSvgXml::TextTimeToXml(CSKItem *item, SBaseConfig *canvas)
{
	SBaseConfig *base = canvas->AddChildNode("textTime");
	BaseToXml(item, base);
	base->SetAttribute("style",((CSKTextItem*)item)->m_timeStyle);
	FontToXml((CSKTextItem*)item, base);
	return true;
}
