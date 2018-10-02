#ifndef CSYSTRAY_H
#define CSYSTRAY_H

#include "skhead.h"
#include <QSystemTrayIcon>

class SKWatch;
class CSysTray : public QSystemTrayIcon
{
	Q_OBJECT

public:
	CSysTray(QWidget *parent);
	~CSysTray();

private:
	SKWatch *m_pApp;
	QMenu *m_pTrayIconMenu;
	QAction *m_pShowAction;
	QAction *m_pQuitAction;

private:
	void Init();
	void InitUi();
	void InitSlot();

private slots:
	void SlotShow();
	void SlotQuit();
	void SlotActivated(QSystemTrayIcon::ActivationReason reason);
	
};

#endif // CSYSTRAY_H
