#ifndef SIZEHANDLE
#define SIZEHANDLE

#include "skhead.h"

enum { SELECTION_HANDLE_SIZE = 6, SELECTION_MARGIN = 10 };
enum SelectionHandleState { SelectionHandleOff, SelectionHandleInactive, SelectionHandleActive };
enum { Handle_None = 0 , Handle_LeftTop , Handle_Top, Handle_RightTop, Handle_Right, Handle_RightBottom, Handle_Bottom, Handle_LeftBottom, Handle_Left };

class SizeHandleRect : public QGraphicsRectItem
{
public:
    SizeHandleRect(QGraphicsItem *parent, int direct, bool control = false);
	~SizeHandleRect();

    void Move(qreal x, qreal y);
    void SetState(SelectionHandleState st);
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

