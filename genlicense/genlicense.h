#ifndef GENLICENSE_H
#define GENLICENSE_H

#include <QtGui/QWidget>
#include "ui_genlicense.h"

class genlicense : public QWidget
{
	Q_OBJECT

public:
	genlicense(QWidget *parent = 0, Qt::WFlags flags = 0);
	~genlicense();

private:
	Ui::genlicenseClass ui;

	QString sHostIdFile;

private:
	void Init();
	void InitUi();
	void InitSlot();

private slots:
	void SlotHostId();
	void SlotLicence();
	void SlotSelLic();
	void SlotStateChangedTM(int state);
	void SlotP2C();
	void SlotC2P();
};

#endif // GENLICENSE_H
