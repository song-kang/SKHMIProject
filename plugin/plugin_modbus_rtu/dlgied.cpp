#include "dlgied.h"
#include "sk_database.h"
#include <QMessageBox>

DlgIed::DlgIed(QWidget *parent)
	: QDialog(parent)
{
	ui.setupUi(this);

	Init();
	InitUi();
	InitSlot();
}

DlgIed::~DlgIed()
{

}

void DlgIed::Init()
{
	m_isFix = false;
}

void DlgIed::InitUi()
{
	ui.lineEditNo->setReadOnly(true);
}

void DlgIed::InitSlot()
{
	connect(ui.btnOk, SIGNAL(clicked()), this, SLOT(SlotOk()));
	connect(ui.btnCancel, SIGNAL(clicked()), this, SLOT(close()));
}

void DlgIed::Start(quint32 no)
{
	m_iNodeNo = no;

	SString sql;
	SRecordset rs;
	sql.sprintf("select max(ied_no) from t_oe_ied");
	int cnt = DB->Retrieve(sql,rs);
	if (cnt == 1) {
		int no = rs.GetValue(0,0).toInt();
		ui.lineEditNo->setText(QString("%1").arg(++no));
		ui.lineEditName->setText("Modbus×°ÖÃ");
		ui.lineEditModel->setText("ModbusRtu");
		ui.lineEditVersion->setText("1.0.0");
	}

	sql.sprintf("select max(device_no) from t_oe_run_device");
	cnt = DB->Retrieve(sql,rs);
	if (cnt == 1) {
		m_iDevNo = rs.GetValue(0,0).toInt();
		m_iDevNo++;
	}
}

void DlgIed::StartFix(quint32 no)
{
	SString sql;
	SRecordset rs;
	sql.sprintf("select ied_no,name,model,version "
		"from t_oe_ied where ied_no=%d",no);
	int cnt = DB->Retrieve(sql,rs);
	if (cnt == 1) {
		ui.lineEditNo->setText(rs.GetValue(0,0).data());
		ui.lineEditName->setText(rs.GetValue(0,1).data());
		ui.lineEditModel->setText(rs.GetValue(0,2).data());
		ui.lineEditVersion->setText(rs.GetValue(0,3).data());
	}

	m_isFix = true;
	this->setWindowTitle(tr("ĞŞ¸Ä×°ÖÃ"));
}

void DlgIed::SlotOk()
{
	m_iNo = ui.lineEditNo->text().toInt();
	m_sName = ui.lineEditName->text();
	m_sModel = ui.lineEditModel->text();
	m_sVersion = ui.lineEditVersion->text();

	SString sql;
	SRecordset rs;
	if (!m_isFix) {
		sql.sprintf("insert into t_oe_ied(ied_no,sub_no,name,model,version) "
			"values (%d,%d,'%s','%s','%s')",
			m_iNo,1,
			m_sName.toStdString().data(),
			m_sModel.toStdString().data(),
			m_sVersion.toStdString().data());
		if (!DB->Execute(sql)) {
			QMessageBox::critical(this,tr("´íÎó"),tr("SQLÓï¾äÖ´ĞĞ´íÎó¡£\n\n%1").arg(sql.data()));
			return;
		}
		sql.sprintf("insert into t_oe_run_device(device_no,sub_no,ied_no,node_no) "
			"values (%d,%d,%d,%d)",
			m_iDevNo,1,m_iNo,m_iNodeNo);
		if (!DB->Execute(sql)) {
			QMessageBox::critical(this,tr("´íÎó"),tr("SQLÓï¾äÖ´ĞĞ´íÎó¡£\n\n%1").arg(sql.data()));
			return;
		}
		sql.sprintf("insert into t_oe_cpu(ied_no,cpu_no,name,version) "
			"values (%d,%d,'%s','%s')",
			m_iNo,1,"cpu1","1.0.0");
		if (!DB->Execute(sql)) {
			QMessageBox::critical(this,tr("´íÎó"),tr("SQLÓï¾äÖ´ĞĞ´íÎó¡£\n\n%1").arg(sql.data()));
			return;
		}
	}
	else {
		sql.sprintf("update t_oe_ied set name='%s',model='%s',version='%s' "
			"where ied_no=%d",
			m_sName.toStdString().data(),
			m_sModel.toStdString().data(),
			m_sVersion.toStdString().data(),
			m_iNo);
		if (!DB->Execute(sql)) {
			QMessageBox::critical(this,tr("´íÎó"),tr("SQLÓï¾äÖ´ĞĞ´íÎó¡£\n\n%1").arg(sql.data()));
			return;
		}
	}

	accept();
}
