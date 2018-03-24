#ifndef CTOPWIDGET_H
#define CTOPWIDGET_H

#include "skhead.h"
#include "PushButton.h"

#define TOPWIDGET_HEIGHT	27

class SKBaseWidget;
class CTopWidget : public QWidget
{
	Q_OBJECT

public:
	CTopWidget(QWidget *parent = 0);
	~CTopWidget();

	void SetWindowIcon(QIcon icon);
	void SetWindowTitle(QString title);
	void SetWindowFlags(int flags = 0);
	void SetWindowMainMenu(QMenu *menu) { m_iMainMenu = menu; }
	void SetWindowFixed(bool b) { m_bFixed = b; }
	void SetWindowBackgroundImaged(bool b);

	QPushButton *GetBtnMaximize() { return m_btnMaximize; }
	QPushButton *GetBtnMinimize() { return m_btnMinimize; }

protected:
	bool eventFilter(QObject *, QEvent *);
	virtual void mousePressEvent(QMouseEvent *);
	virtual void mouseReleaseEvent(QMouseEvent *);
	virtual void mouseMoveEvent(QMouseEvent *);
	virtual void mouseDoubleClickEvent(QMouseEvent *);
	virtual void paintEvent(QPaintEvent *);

private:
	QLabel *m_labelTitle;			//标题
	QPushButton *m_btnLogo;			//标识
	QPushButton *m_btnFullScreen;	//全屏
	QPushButton *m_btnMainMenu;		//主菜单
	QPushButton *m_btnMinimize;		//最小化
	QPushButton *m_btnMaximize;		//最大化
	QPushButton *m_btnClose;		//关闭

	QHBoxLayout *m_mainLayout;
	QMenu *m_iMainMenu;
	bool m_bFixed;
	bool m_bBackgroundImaged;

private:
	void Init();
	void InitUi();

signals:
	void SigMin();
	void SigMax();
	void SigClose();
	void SigFullScreen();

private slots:
	void SlotMainMenu();

private:
	SKBaseWidget *m_pBaseWidget;

};

#endif // CTOPWIDGET_H
