#include "sizehandle.h"
#include <QGraphicsScene>
#include <QGraphicsSceneContextMenuEvent>
#include <QMenu>
#include <QPainter>
#include <qdebug.h>
#include <QtGui>
#include "drawview.h"

SizeHandleRect::SizeHandleRect(QGraphicsItem *parent, DrawView *view, int direct, bool control)
    :QGraphicsRectItem(-eSelectionHandleSize/2,
                       -eSelectionHandleSize/2,
                       eSelectionHandleSize,
                       eSelectionHandleSize,
					   parent)
	,m_pView(view)
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

QPainterPath SizeHandleRect::shape() const
{
	QPainterPath path;

	qreal scale = m_pView->GetScale();
	if (scale > 1.0)
		path.addRect(-eSelectionHandleSize/(2*scale), -eSelectionHandleSize/(2*scale),
					 eSelectionHandleSize/scale, eSelectionHandleSize/scale);
	else
		path.addRect(-eSelectionHandleSize/2, -eSelectionHandleSize/2,
					 eSelectionHandleSize, eSelectionHandleSize);

	return path;
}

void SizeHandleRect::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    painter->save();
    painter->setPen(QPen(Qt::white, 0, Qt::SolidLine));
    painter->setBrush(QBrush(m_borderColor));
    painter->setRenderHint(QPainter::Antialiasing,false);

    if (m_controlPoint)
    {
        painter->setPen(QPen(Qt::red, 0, Qt::SolidLine));
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
