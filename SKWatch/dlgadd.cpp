#include "dlgadd.h"

DlgAdd::DlgAdd(QWidget *parent)
	: QDialog(parent)
{
	ui.setupUi(this);

	Init();
	InitUi();
	InitSlot();
}

DlgAdd::~DlgAdd()
{

}

void DlgAdd::Init()
{
	ui.lineEditName->setReadOnly(true);
	ui.lineEditPath->setReadOnly(true);
}

void DlgAdd::InitUi()
{

}

void DlgAdd::InitSlot()
{
	connect(ui.btnApp, SIGNAL(clicked()), this, SLOT(SlotApp()));
	connect(ui.btnOk, SIGNAL(clicked()), this, SLOT(SlotOk()));
	connect(ui.btnCancel, SIGNAL(clicked()), this, SLOT(close()));
}

void DlgAdd::Start()
{
	if (m_type == TYPE_ADD)
	{
		setWindowTitle("���");
		setWindowIcon(QIcon(":/images/add"));
	}
	else if (m_type == TYPE_MODIFY)
	{
		setWindowTitle("�޸�");
		setWindowIcon(QIcon(":/images/edit"));
		ui.btnApp->setVisible(false);
	}
}

void DlgAdd::SlotApp()
{
	QString filter = tr("app file(*.exe *.*)");
	QString fileName = QFileDialog::getOpenFileName(this,tr("ѡ��Ӧ�ó���"),QString::null,filter);
	if (!fileName.isEmpty())
	{
		QFileInfo info(fileName);
		ui.lineEditName->setText(info.fileName());
		ui.lineEditPath->setText(info.absolutePath());
	}
}

void DlgAdd::SlotOk()
{
	if (ui.lineEditName->text().trimmed().isEmpty() ||
		ui.lineEditPath->text().trimmed().isEmpty())
	{
		QMessageBox::warning(this, "�澯", "��ѡ��һ��Ӧ��");
		return;
	}

	m_sAppName = ui.lineEditName->text().trimmed();
	m_sAppArg = ui.lineEditArg->text().trimmed();
	m_sAppPath = ui.lineEditPath->text().trimmed();
	m_iDelayTime = ui.spinBoxDelay->text().toInt();

	accept();
}
