#ifndef SKBASEWIDGET_H
#define SKBASEWIDGET_H

#include "AbsFrameLessAutoSize.h"

#ifdef SKBASEWIDGET_LIB
# define SKBASEWIDGET_EXPORT Q_DECL_EXPORT
#else
# define SKBASEWIDGET_EXPORT Q_DECL_IMPORT
#endif

#define SKBASEWIDGET_MINIMIZE	0x01
#define SKBASEWIDGET_MAXIMIZE	0x02
#define SKBASEWIDGET_MAINMENU	0x04
#define SKBASEWIDGET_FULLSCREEN	0x08

class SKBASEWIDGET_EXPORT SKBaseWidget : public AbsFrameLessAutoSize
{
	Q_OBJECT

public:
	SKBaseWidget(QWidget *parent = 0,QWidget *w = 0);
	~SKBaseWidget();

	void Show();
	void ShowMaximized();
	void ShowFullScreen();
	void SetWindowFlags(int flags);
	void SetWindowIcon(QIcon icon);
	void SetWindowTitle(QString title);
	void SetWindowSize(int width,int height);
	void SetWindowFixSize(int width,int height);
	void SetWindowMainMenu(QMenu *menu);
	void SetWindowBackgroundImage(QPixmap pix);
	void HideTopFrame();
	void SetIsDrag(bool b) { m_bIsDrag = b; }
	void SetIsTopDrag(bool b) { m_bIsTopDrag = b; }
	void SetWindowsFlagsTool() { setWindowFlags(windowFlags() | Qt::Tool); }
	void SetWindowsFlagsTopHit() { setWindowFlags(windowFlags() | Qt::WindowStaysOnTopHint); }
	void SetWindowsModal() { setAttribute(Qt::WA_ShowModal, true); setWindowModality(Qt::ApplicationModal); }

	QWidget* GetCenterWidget() { return m_centerWidget; }

protected:
	virtual void mouseMoveEvent(QMouseEvent *e);
	virtual void mousePressEvent(QMouseEvent *e);
	virtual void mouseReleaseEvent(QMouseEvent *e);
	virtual void mouseDoubleClickEvent(QMouseEvent *e);
	virtual void keyPressEvent(QKeyEvent *e);
	virtual void closeEvent(QCloseEvent *e);
	virtual void paintEvent(QPaintEvent *e);
	virtual void changeEvent(QEvent *e);

private:
	bool m_bIsMax;
	bool m_bIsFull;
	bool m_bIsDrag;
	bool m_bIsTopDrag;
	QRect m_iLocation;
	QWidget *m_centerWidget;
	void *m_topWidget;
	void *m_mainWidget;
	QGridLayout *m_mainGridLayout;
	QVBoxLayout *m_vBoxLyout;

private:
	void Init();
	void InitUi();
	void InitSlot();

signals:
	void SigMax();
	void SigMin();
	void SigMove();
	void SigClose();

public slots:
	void SlotClose();
	void SlotShowMax();
	void SlotShowMin();
	void SlotShowNormal();
	void SlotFullScreen();

};

#endif // SKBASEWIDGET_H
