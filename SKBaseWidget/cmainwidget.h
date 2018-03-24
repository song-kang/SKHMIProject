#ifndef CMAINWIDGET_H
#define CMAINWIDGET_H

#include "skhead.h"

class CMainWidget : public QWidget
{
	Q_OBJECT

public:
	CMainWidget(QWidget *parent = 0);
	~CMainWidget();

	void SetPixmap(const QPixmap &);

protected:
	void paintEvent(QPaintEvent *e);

private:
	QRect m_rect;
	QPixmap m_localPix;
	
private:
	void Init();
	void InitUi();
};

#endif // CMAINWIDGET_H
