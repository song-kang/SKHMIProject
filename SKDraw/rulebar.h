#ifndef RULEBAR
#define RULEBAR

#include "skhead.h"

#define RULER_SIZE    16

class QtCornerBox : public QWidget
{
    Q_OBJECT
public:
    explicit QtCornerBox(QWidget * parent );
protected:
    void paintEvent(QPaintEvent *);
};

class QtRuleBar : public QWidget
{
    Q_OBJECT

public:
    explicit QtRuleBar(Qt::Orientation direction, QGraphicsView *view, QWidget *parent = 0);

    void setRange(double lower, double upper, double max_size);
    void updatePosition(const QPoint &pos);

protected:
	double m_lower;
	double m_upper;
	double m_maxsize;
	QPoint m_lastPos;
	QColor m_faceColor;
    Qt::Orientation m_direction;
    QGraphicsView *m_view;

protected:
	virtual void paintEvent(QPaintEvent *event);

	void DrawTicker(QPainter *painter);
	void DrawPos(QPainter *painter);

};

#endif // RULEBAR

