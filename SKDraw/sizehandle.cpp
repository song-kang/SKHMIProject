#include "sizehandle.h"
#include <QGraphicsScene>
#include <QGraphicsSceneContextMenuEvent>
#include <QMenu>
#include <QPainter>
#include <qdebug.h>
#include <QtGui>

SizeHandleRect::SizeHandleRect(QGraphicsItem *parent, int direct, bool control)
    :QGraphicsRectItem(-eSelectionHandleSize/2,
                       -eSelectionHandleSize/2,
                       eSelectionHandleSize,
                       eSelectionHandleSize,
					   parent)
    ,m_direct(direct)
    ,m_controlPoint(control)
    ,m_state(eSelectionHandleOff)
    ,m_borderColor(Qt::black)
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
    painter->setPen(QPen(Qt::white, Qt::SolidLine));
    painter->setBrush(QBrush(m_borderColor));
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
    case eSelectionHandleOff:
        hide();
        break;
    case eSelectionHandleInactive:
    case eSelectionHandleActive:
        show();
        break;
    }

    //m_borderColor = Qt::black;
    m_state = st;
}

void SizeHandleRect::Move(qreal x, qreal y)
{   
    setPos(x,y);
}

void SizeHandleRect::hoverEnterEvent(QGraphicsSceneHoverEvent *e)
{
    //m_borderColor = Qt::black;
    //update();

    QGraphicsRectItem::hoverEnterEvent(e);
}

void SizeHandleRect::hoverLeaveEvent(QGraphicsSceneHoverEvent *e)
{
    //m_borderColor = Qt::black;
    //update();

    QGraphicsRectItem::hoverLeaveEvent(e);
}
