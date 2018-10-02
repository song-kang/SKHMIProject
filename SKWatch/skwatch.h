#ifndef SKWATCH_H
#define SKWATCH_H

#include "skhead.h"
#include "ui_skwatch.h"
#include "Watch.h"
#include "csystray.h"

class SKWatch : public QWidget
{
	Q_OBJECT

public:
	SKWatch(QWidget *parent = 0, Qt::WFlags flags = 0);
	~SKWatch();

	void Start(int argc, char *argv[]);

protected:
	void closeEvent(QCloseEvent *event);

private:
	Ui::SKWatchClass ui;

	CWatch m_watch;
	QTimer *m_timer;
	SXmlConfig m_cfgXml;
	QTableWidgetItem *m_pCurrentItem;
	CSysTray *m_pSystemTray;

private:
	void Init();
	void InitUi();
	void InitSlot();
	void Refresh();
	void WriteConfigFile();
	stuExtModule* GetExtModuleBySn(int sn);

private slots:
	void SlotAdd();
	void SlotRemove();
	void SlotStartStop();
	void SlotModify();
	void SlotTimeout();
	void SlotTableItemClicked(QTableWidgetItem *tableItem);

};

#endif // SKWATCH_H
