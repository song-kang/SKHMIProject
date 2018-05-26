#include "sizehandle.h"
#include <QGraphicsScene>
#include <QGraphicsSceneContextMenuEvent>
#include <QMenu>
#include <QPainter>
#include <qdebug.h>
#include <QtGui>

SizeHandleRect::SizeHandleRect(QGraphicsItem *parent, int direct, bool control)
    :QGraphicsRectItem(-SELECTION_HANDLE_SIZE/2,
                       -SELECTION_HANDLE_SIZE/2,
                       SELECTION_HANDLE_SIZE,
                       SELECTION_HANDLE_SIZE,
					   parent)
    ,m_direct(direct)
    ,m_controlPoint(control)
    ,m_state(SelectionHandleOff)
    ,m_borderColor(Qt::white)
{
    this->setAcceptHoverEvents(true);
    setFlag(QGraphicsItem::ItemIgnoresTransformations,true);

    hide();
}

SizeHandleRect::~SizeHandleRect()
{

}

void SizeHandleRect::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    painter->save();
    painter->setPen(Qt::SolidLine);
    painter->setBrush(QBrush(m_borderColor));
	painter->setBrush(Qt::NoBrush);
    painter->setRenderHint(QPainter::Antialiasing,false);

    if (m_controlPoint)
    {
        painter->setPen(QPen(Qt::red,Qt::SolidLine));
        painter->setBrush(Qt::green);
        painter->drawEllipse(rect().center(),3,3);
    }
	else
	{
        painter->drawRect(rect());
	}

    painter->restore();
}

void SizeHandleRect::SetState(SelectionHandleState st)
{
    if (st == m_state)
        return;

    switch (st) 
	{
    case SelectionHandleOff:
        hide();
        break;
    case SelectionHandleInactive:
    case SelectionHandleActive:
        show();
        break;
    }

    m_borderColor = Qt::white;
    m_state = st;
}

void SizeHandleRect::Move(qreal x, qreal y)
{   
    setPos(x,y);
}

void SizeHandleRect::hoverEnterEvent(QGraphicsSceneHoverEvent *e)
{
    m_borderColor = Qt::blue;
    update();

    QGraphicsRectItem::hoverEnterEvent(e);
}

void SizeHandleRect::hoverLeaveEvent(QGraphicsSceneHoverEvent *e)
{
    m_borderColor = Qt::white;
    update();

    QGraphicsRectItem::hoverLeaveEvent(e);
}
