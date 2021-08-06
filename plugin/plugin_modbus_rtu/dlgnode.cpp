#include "dlgnode.h"
#include "sk_database.h"
#include <QMessageBox>

DlgNode::DlgNode(QWidget *parent)
	: QDialog(parent)
{
	ui.setupUi(this);

	Init();
	InitUi();
	InitSlot();
}

DlgNode::~DlgNode()
{

}

void DlgNode::Init()
{
	m_isFix = false;
}

void DlgNode::InitUi()
{
	ui.lineEditNo->setReadOnly(true);
	ui.lineEditName->setReadOnly(true);
}

void DlgNode::InitSlot()
{
	connect(ui.btnOk, SIGNAL(clicked()), this, SLOT(SlotOk()));
	connect(ui.btnCancel, SIGNAL(clicked()), this, SLOT(close()));
}

void DlgNode::Start()
{
	SString sql;
	SRecordset rs;
	sql.sprintf("select max(node_no) from t_oe_run_node");
	int cnt = DB->Retrieve(sql,rs);
	if (cnt == 1) {
		int no = rs.GetValue(0,0).toInt();
		ui.lineEditNo->setText(QString("%1").arg(++no));
		ui.lineEditName->setText("fis_modbus_rtu");
		ui.lineEditDesc->setText("Modbus采集模块");
		ui.lineEditParam->setText("COM1,9600,N,8,1");
	}
}

void DlgNode::StartFix(quint32 no)
{
	SString sql;
	SRecordset rs;
	sql.sprintf("select node_no,name,description,module_name,reserve,appid "
		"from t_oe_run_node where node_no=%d",no);
	int cnt = DB->Retrieve(sql,rs);
	if (cnt == 1) {
		ui.lineEditNo->setText(rs.GetValue(0,0).data());
		ui.lineEditName->setText(rs.GetValue(0,1).data());
		ui.lineEditDesc->setText(rs.GetValue(0,2).data());
		ui.lineEditParam->setText(rs.GetValue(0,4).data());
	}

	m_isFix = true;
	this->setWindowTitle(tr("修改节点"));
}

void DlgNode::SlotOk()
{
	m_iNo = ui.lineEditNo->text().toInt();
	m_sName = ui.lineEditName->text();
	m_sDesc = ui.lineEditDesc->text();
	m_sParam = ui.lineEditParam->text();

	SString sql;
	SRecordset rs;
	if (!m_isFix) {
		sql.sprintf("insert into t_oe_run_node(node_no,name,description,module_name,reserve,appid) "
			"values (%d,'%s','%s','%s','%s',%d)",
			m_iNo,
			m_sName.toStdString().data(),
			m_sDesc.toStdString().data(),
			m_sName.toStdString().data(),
			m_sParam.toStdString().data(),
			1);
		if (!DB->Execute(sql)) {
			QMessageBox::critical(this,tr("错误"),tr("SQL语句执行错误。\n\n%1").arg(sql.data()));
			return;
		}
	}
	else {
		sql.sprintf("update t_oe_run_node set name='%s',description='%s',module_name='%s',reserve='%s' "
			"where node_no=%d",
			m_sName.toStdString().data(),
			m_sDesc.toStdString().data(),
			NODE_NAME,
			m_sParam.toStdString().data(),
			m_iNo);
		if (!DB->Execute(sql)) {
			QMessageBox::critical(this,tr("错误"),tr("SQL语句执行错误。\n\n%1").arg(sql.data()));
			return;
		}
	}
	
	accept();
}
