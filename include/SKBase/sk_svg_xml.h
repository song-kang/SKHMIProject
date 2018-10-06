#ifndef __SK_SVG_XML_H__
#define __SK_SVG_XML_H__

#include "SApi.h"
#include "sk_base_inc.h"

#define ItemNoPen			0
#define ItemSolidLine		1
#define ItemDashLine		2
#define ItemDotLine			3
#define ItemDashDotLine		4
#define ItemDashDotDotLine	5

#define ItemNoBrush			0
#define ItemSolidPattern	1

#define ItemAlignLeft		0x0001
#define ItemAlignRight		0x0002
#define ItemAlignHCenter	0x0004
#define ItemAlignTop		0x0020
#define ItemAlignBottom		0x0040
#define ItemAlignVCenter	0x0080
#define ItemAlignCenter		ItemAlignVCenter | ItemAlignHCenter

enum eItemType
{
	eItemNone = 0,
	eItemLine,				//线段
	eItemPolygonLine,		//折线
	eItemArcCircle,			//圆弧线
	eItemArcEllipse,		//椭圆弧线
	eItemRect,				//矩形
	eItemRoundedRect,		//圆角矩形
	eItemCircle,			//圆形
	eItemEllipse,			//椭圆形
	eItemTriangle,			//三角形
	eItemRhombus,			//菱形
	eItemParallelogram,		//平行四边形
	eItemPolygon,			//不规则多边形
	eItemPicture,			//图片
	eItemText,				//文本
	eItemTextTime,			//文本时间
};

struct stuPoint
{
	stuPoint()
	{
		m_x = 0.0;
		m_y = 0.0;
	}

	float m_x;
	float m_y;
};

//================ CSKItem =======================
class SK_BASE_EXPORT CSKItem
{
public:
	explicit CSKItem();
	virtual ~CSKItem();

	void SetType(eItemType type) { m_eType = type; }
	void SetOrigin(stuPoint p) { m_origin = p; }
	void SetPoints(SPtrList<stuPoint> p) { m_points = p; }
	void SetWidth(float width) { m_width = width; }
	void SetHeight(float height) { m_height = height; }
	void SetRx(float rx) { m_rx = rx; }
	void SetRy(float ry) { m_ry = ry; }
	void SetScale(float scale) { m_scale = scale; }
	void SetRotation(float rotation) { m_rotation = rotation; }
	void SetPenWidth(float width) { m_penWidth = width; }
	void SetPenStyle(int style) { m_penStyle = style; }
	void SetPenColor(SString color) { m_penColor = color; }
	void SetPenAlpha(int alpha) { m_penAlpha = alpha; }
	void SetBrushStyle(int style) { m_brushStyle = style; }
	void SetBrushColor(SString color) { m_brushColor = color; }
	void SetBrushAlpha(int alpha) { m_brushAlpha = alpha; }
	void SetTooltip(SString tip) { m_tooltip = tip; }
	void SetStartAngle(float angle) { m_startAngle = angle; }
	void SetEndAngle(float angle) { m_endAngle = angle; }
	void SetCrossAngle(float angle) { m_crossAngle = angle; }
	void SetZ(float z) { m_z = z; }
	void SetShowType(int type) { m_showType = type; }
	void SetLinkDB(SString link) { m_linkDB = link; }
	void SetLinkScene(SString link) { m_linkScene = link; }

public:
	eItemType m_eType;
	stuPoint m_origin;
	SPtrList<stuPoint> m_points;
	float m_width;
	float m_height;
	float m_rx,m_ry;
	float m_scale;
	float m_rotation;
	float m_penWidth;
	int m_penStyle;
	SString m_penColor;
	int m_penAlpha;
	int m_brushStyle;
	SString m_brushColor;
	int m_brushAlpha;
	SString m_tooltip;
	float m_startAngle;
	float m_endAngle;
	float m_crossAngle;
	float m_z;
	int m_showType;
	SString m_linkDB;
	SString m_linkScene;

public:
	SPtrList<CSKItem> m_items;
};

//================ CSKTextItem =======================
class SK_BASE_EXPORT CSKTextItem : public CSKItem
{
public:
	explicit CSKTextItem();
	virtual ~CSKTextItem();

	void SetDesc(SString desc) { m_desc = desc; }
	void SetTimeStyle(SString style) { m_timeStyle = style; }
	void SetFamily(SString family) { m_family = family; }
	void SetPointSize(int size) { m_pointSize = size; }
	void SetBold(bool is) { m_isBold = is; }
	void SetItalic(bool is) { m_isItalic = is; }
	void SetUnderline(bool is) { m_isUnderline = is; }
	void SetStrikeOut(bool is) { m_isStrikeOut = is; }
	void SetKerning(bool is) { m_isKerning = is; }
	void SetAlignment(int alignment) { m_alignment = alignment; }

public:
	SString m_desc;
	SString m_timeStyle;
	SString m_family;
	int m_pointSize;
	bool m_isBold;
	bool m_isItalic;
	bool m_isUnderline;
	bool m_isStrikeOut;
	bool m_isKerning;
	int m_alignment;
};

//================ CSKSvgXml =======================
class SK_BASE_EXPORT CSKSvgXml
{
public:
	explicit CSKSvgXml();
	virtual ~CSKSvgXml();

	void SetBackgroundWidth(int width) { m_backgroundWidth = width; }
	void SetBackgroundHeight(int height) { m_backgroundHeight = height; }
	void SetBackgroundColor(SString color) { m_backgroundColor = color; }
	void SetBackgroundAlpha(int alpha) { m_backgroundAlpha = alpha; }

	CSKItem* CreateLine(SPtrList<stuPoint> *points, SString color="#FFFFFF", float width=1);
	CSKItem* CreatePolygonLine(SPtrList<stuPoint> *points, SString color="#FFFFFF", float width=1);
	CSKItem* CreateArcCircle(stuPoint point, float diameter, float startAngle, float endAngle, SString color="#FFFFFF", float width=1);
	CSKItem* CreateArcEllipse(stuPoint point, float longAxis, float shortAxis, float startAngle, float endAngle, SString color="#FFFFFF", float width=1);
	CSKItem* CreateRect(stuPoint point, float width, float height);
	CSKItem* CreateRoundedRect(stuPoint point, float width, float height, float rx=5.0, float ry=5.0);
	CSKItem* CreateCircle(stuPoint point, float diameter);
	CSKItem* CreateEllipse(stuPoint point, float longAxis, float shortAxis);
	CSKItem* CreateTriangle(stuPoint point, float width, float height);
	CSKItem* CreateRhombus(stuPoint point, float width, float height);
	CSKItem* CreateParallelogram(stuPoint point, float width, float height, float crossAngle);
	CSKItem* CreatePolygon(SPtrList<stuPoint> *points);
	CSKTextItem* CreateText(stuPoint point, float width, float height, SString desc);
	CSKTextItem* CreateTextTime(stuPoint point, float width, float height, SString style="yyyy-MM-dd hh:mm:ss");
	void CreateArrowLine(SPtrList<stuPoint> *points, float arrowScale=1.0, SString color="#FFFFFF", float width=1, float z=0.0);
	void CreateDoubleArrowLine(SPtrList<stuPoint> *points, float arrowScale=1.0, SString color="#FFFFFF", float width=1, float z=0.0);

	bool SaveToXml(SString &content, SString encoding="GB2312");
	bool BaseToXml(CSKItem *item, SBaseConfig *base);
	bool FontToXml(CSKTextItem *item, SBaseConfig *base);
	bool LineToXml(CSKItem *item, SBaseConfig *canvas);
	bool PolygonLineToXml(CSKItem *item, SBaseConfig *canvas);
	bool ArcCircleToXml(CSKItem *item, SBaseConfig *canvas);
	bool ArcEllipseToXml(CSKItem *item, SBaseConfig *canvas);
	bool RectToXml(CSKItem *item, SBaseConfig *canvas);
	bool RoundedRectToXml(CSKItem *item, SBaseConfig *canvas);
	bool CircleToXml(CSKItem *item, SBaseConfig *canvas);
	bool EllipseToXml(CSKItem *item, SBaseConfig *canvas);
	bool TriangleToXml(CSKItem *item, SBaseConfig *canvas);
	bool RhombusToXml(CSKItem *item, SBaseConfig *canvas);
	bool ParallelogramToXml(CSKItem *item, SBaseConfig *canvas);
	bool PolygonToXml(CSKItem *item, SBaseConfig *canvas);
	bool PictureToXml(CSKItem *item, SBaseConfig *canvas);
	bool TextToXml(CSKItem *item, SBaseConfig *canvas);
	bool TextTimeToXml(CSKItem *item, SBaseConfig *canvas);

private:
	int m_backgroundWidth;
	int m_backgroundHeight;
	SString m_backgroundColor;
	int m_backgroundAlpha;
	SPtrList<CSKItem> m_items;
};

#endif//__SK_SVG_XML_H__