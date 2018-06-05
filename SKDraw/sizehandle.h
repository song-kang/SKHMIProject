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

class SizeHandleRect : public QGraphicsRectItem
{
public:
    SizeHandleRect(QGraphicsItem *parent, int direct, bool control = false);
	~SizeHandleRect();

    void Move(qreal x, qreal y);
    void SetState(SelectionHandleState st);
	void SetBorderColor(QColor c) { m_borderColor = c; update(); }
	int  GetDirect() const { return m_direct; }

protected:
    virtual void hoverEnterEvent(QGraphicsSceneHoverEvent *e );
    virtual void hoverLeaveEvent(QGraphicsSceneHoverEvent *e );
    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

private:
    const int m_direct;
    bool m_controlPoint;
    QColor m_borderColor;
	SelectionHandleState m_state;
};


#endif // SIZEHANDLE

