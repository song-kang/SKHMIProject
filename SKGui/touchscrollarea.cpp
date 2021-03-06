#include <QMouseEvent>
#include <QScrollBar>
#include "touchscrollarea.h"

TouchScrollArea::TouchScrollArea(QWidget *parent)
    :QScrollArea(parent)
    ,mMoveStart(false)
    ,mContinuousMove(false)
    ,mMousePoint(QPoint(0,0))
{
    installEventFilter(this);
}

TouchScrollArea::~TouchScrollArea()
{
}

bool TouchScrollArea::eventFilter(QObject *obj, QEvent *evt)
{
    if (evt->type() == QEvent::MouseMove)
    {
        QMouseEvent *me = (QMouseEvent*) evt;
        if (me->buttons() & Qt::LeftButton)
        {
            if (!mMoveStart)
            {
                mMoveStart = true;
                mContinuousMove = false;
                mMousePoint = me->globalPos();
            }
            else
            {
                QScrollBar *scrollBarx = horizontalScrollBar();
                QScrollBar *scrollBary = verticalScrollBar();

                QPoint p = me->globalPos();
                int offsetx = p.x() - mMousePoint.x();
                int offsety = p.y() - mMousePoint.y();
                if (!mContinuousMove && (offsetx > -10 && offsetx < 10) && (offsety > -10 && offsety < 10))
                    return false;

                mContinuousMove = true;

                scrollBarx->setValue(scrollBarx->value() - offsetx);
                scrollBary->setValue(scrollBary->value() - offsety);
                mMousePoint = p;
            }

            return true;
        }
    }
    else if (evt->type() == QEvent::MouseButtonRelease)
    {
        mMoveStart = false;
    }

    return QObject::eventFilter(obj,evt);
}