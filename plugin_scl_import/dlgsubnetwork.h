#ifndef DLGSUBNETWORK_H
#define DLGSUBNETWORK_H

#include <QDialog>
#include "ui_dlgsubnetwork.h"
#include "xmlobject.h"

#define TABLE_SUBNETWORK_NAME			0
#define TABLE_SUBNETWORK_DESC			1
#define TABLE_SUBNETWORK_TYPE			2
#define TABLE_SUBNETWORK_NETNAME		3

class DlgSubNetwork : public QDialog
{
	Q_OBJECT

public:
	DlgSubNetwork(QWidget *parent = 0);
	~DlgSubNetwork();

	void	setSubNetworks(QList<XmlObject*> l) { subNetworks = l;}

	QString	getMmsAName() { return mmsAName; }
	QString	getMmsBName() { return mmsBName; }

	void	start();

private:
	Ui::DlgSubNetwork ui;

	int			row;
	QString		mmsAName;
	QString		mmsBName;

	QList<XmlObject*> subNetworks;

private:
	void initTable();
	void closeEvent(QCloseEvent * e);

public slots:
	void slotOk();

};

#endif // DLGSUBNETWORK_H
