#ifndef TOUCHSCROLLAREA_H
#define TOUCHSCROLLAREA_H

#include <QObject>
#include <QScrollArea>
#include <QPoint>

class TouchScrollArea : public QScrollArea
{
    Q_OBJECT

public:
    TouchScrollArea(QWidget* parent = NULL);
    ~TouchScrollArea();

protected:
    bool eventFilter(QObject *obj,QEvent *evt);

private:
    bool mMoveStart;
    bool mContinuousMove;
    QPoint mMousePoint;
};

#endif // TOUCHSCROLLAREA_H