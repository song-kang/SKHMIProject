#ifndef SKWIDGET_H
#define SKWIDGET_H

#include "skhead.h"

#ifdef SKBASEWIDGET_LIB
# define SKBASEWIDGET_EXPORT Q_DECL_EXPORT
#else
# define SKBASEWIDGET_EXPORT Q_DECL_IMPORT
#endif

class SKBASEWIDGET_EXPORT SKWidget : public QWidget
{
	Q_OBJECT

public:
	SKWidget(QWidget *parent = 0);
	~SKWidget();

protected:
	virtual void paintEvent(QPaintEvent *);
	//virtual void mouseMoveEvent(QMouseEvent *);

private:
	
};

#endif // SKWIDGET_H
