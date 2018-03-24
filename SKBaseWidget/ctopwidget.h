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
	QLabel *m_labelTitle;			//����
	QPushButton *m_btnLogo;			//��ʶ
	QPushButton *m_btnFullScreen;	//ȫ��
	QPushButton *m_btnMainMenu;		//���˵�
	QPushButton *m_btnMinimize;		//��С��
	QPushButton *m_btnMaximize;		//���
	QPushButton *m_btnClose;		//�ر�

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
