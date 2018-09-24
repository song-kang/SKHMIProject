#ifndef SIZEHANDLE
#define SIZEHANDLE

#include "skhead.h"

enum 
{ 
	eSelectionHandleSize = 6, 
	eSelectionMargin = 10 
};

enum SelectionHandleState 
{ 
	eSelectionHandleOff, 
	eSelectionHandleInactive, 
	eSelectionHandleActive 
};

enum 
{ 
	eHandleNone = 0,
	eHandleLeftTop, 
	eHandleTop, 
	eHandleRightTop, 
	eHandleRight, 
	eHandleRightBottom, 
	eHandleBottom, 
	eHandleLeftBottom, 
	eHandleLeft 
};

class DrawView;
class SizeHandleRect : public QGraphicsRectItem
{
public:
    SizeHandleRect(QGraphicsItem *parent, DrawView *view, int direct, bool control = false);
	~SizeHandleRect();

    void Move(qreal x, qreal y);
    void SetState(SelectionHandleState st);
	void SetBorderColor(QColor c) { m_borderColor = c; update(); }
	void SetView(DrawView *view) { m_pView = view; }
	int  GetDirect() const { return m_direct; }

protected:
	virtual QPainterPath shape() const;
    virtual void hoverEnterEvent(QGraphicsSceneHoverEvent *e );
    virtual void hoverLeaveEvent(QGraphicsSceneHoverEvent *e );
    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

private:
    const int m_direct;
    bool m_controlPoint;
    QColor m_borderColor;
	SelectionHandleState m_state;
	DrawView *m_pView;
};


#endif // SIZEHANDLE

