#include "genlicense.h"
#include <QFileDialog>
#include <QMessageBox>
#include "..\SKLic\clicense.h"

genlicense::genlicense(QWidget *parent, Qt::WFlags flags)
	: QWidget(parent, flags)
{
	ui.setupUi(this);

	Init();
	InitUi();
	InitSlot();
}

genlicense::~genlicense()
{

}

void genlicense::Init()
{

}

void genlicense::InitUi()
{
	ui.dateEditStart->setEnabled(false);
	ui.dateEditEnd->setEnabled(false);
	ui.lineEdit->setReadOnly(true);
}

void genlicense::InitSlot()
{
	connect(ui.btnHostId, SIGNAL(clicked()), this, SLOT(SlotHostId()));
	connect(ui.btnLicense, SIGNAL(clicked()), this, SLOT(SlotLicence()));
	connect(ui.btnSelLic, SIGNAL(clicked()), this, SLOT(SlotSelLic()));
	connect(ui.checkBoxTM, SIGNAL(stateChanged(int)), this, SLOT(SlotStateChangedTM(int)));
	connect(ui.btn_plaintext_ciphertext, SIGNAL(clicked()), this, SLOT(SlotP2C()));
	connect(ui.btn_ciphertext_plaintext, SIGNAL(clicked()), this, SLOT(SlotC2P()));
}

void genlicense::SlotHostId()
{
	sHostIdFile = QFileDialog::getOpenFileName(this,tr("打开..."),QString::null,tr("ini file(*.ini)"));
	ui.lineEdit->setText(sHostIdFile);
}

void genlicense::SlotSelLic()
{
	QString sLic = QFileDialog::getOpenFileName(this,tr("打开..."),QString::null,tr("ini file(*.ini)"));
	CLicense lic;
	ui.textBrowser->setText(lic.HostId(sLic));
}

void genlicense::SlotLicence()
{
	CLicense lic;
	if (ui.checkBoxTM->checkState() != Qt::Checked)
	{
		if (!lic.CreateLicenseFile(sHostIdFile))
			QMessageBox::warning(this, "warning", "License create failed.");
	}
	else
	{
		QDate ds = ui.dateEditStart->date();
		QDate de = ui.dateEditEnd->date();
		if (!lic.CreateLicenseFileWithTime(sHostIdFile,ds,de))
			QMessageBox::warning(this, "warning", "License create failed.");
	}

	QMessageBox::information(this, "tip", "License create success.");
}

void genlicense::SlotStateChangedTM(int state)
{
	if (state == Qt::Unchecked)
	{
		ui.dateEditStart->setEnabled(false);
		ui.dateEditEnd->setEnabled(false);
	}
	else if (state == Qt::Checked)
	{
		ui.dateEditStart->setEnabled(true);
		ui.dateEditEnd->setEnabled(true);
	}
}

void genlicense::SlotP2C()
{
	SString s = SApi::Encrypt_String(ui.lineEdit_plaintext->text().toStdString().data(),"yqqlm^gsycl.1978");
	ui.lineEdit_ciphertext->setText(s.data());
}

void genlicense::SlotC2P()
{
	SString s = SApi::Decrypt_String(ui.lineEdit_ciphertext->text().toStdString().data(),"yqqlm^gsycl.1978");
	ui.lineEdit_plaintext->setText(s.data());
}
