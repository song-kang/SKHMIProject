#ifndef PUSHBUTTON_H
#define PUSHBUTTON_H

#include <QPushButton>
#include <QPainter>
#include <QMouseEvent>

class PushButton : public QPushButton
{
	Q_OBJECT

public:
	explicit PushButton(QWidget *parent = 0);
	~PushButton();
	void loadPixmap(QString pic_name);

//protected:
public:
	void enterEvent(QEvent *);
	void leaveEvent(QEvent *);
	void mousePressEvent(QMouseEvent *event);
	void mouseReleaseEvent(QMouseEvent *event);
	void paintEvent(QPaintEvent *);

private:
	enum ButtonStatus{NORMAL, ENTER, PRESS, NOSTATUS}; //ö�ٰ�ť�ļ���״̬
	ButtonStatus status;
	QPixmap pixmap; 

	int btn_width; //��ť���
	int btn_height; //��ť�߶�
	bool mouse_press; //��ť����Ƿ���
};

#endif //PUSHBUTTON_H
