#include "dlgacksubstation.h"
#include <QMessageBox>
#include "view_plugin_scl_import.h"

DlgAckSubstation::DlgAckSubstation(QWidget *parent)
	: QDialog(parent)
{
	ui.setupUi(this);

	init();

	connect(ui.newRadioButton,SIGNAL(clicked()),this,SLOT(slotNewRadioButtonClicked()));
	connect(ui.selectRadioButton,SIGNAL(clicked()),this,SLOT(slotSelectRadioButtonClicked()));
	connect(ui.okBtn,SIGNAL(clicked()), this, SLOT(slotOk()));
	connect(ui.cancelBtn,SIGNAL(clicked()), this, SLOT(close()));
}

DlgAckSubstation::~DlgAckSubstation()
{

}

void DlgAckSubstation::init()
{
	sub_no = 0;
	ui.subLineEdit->setPlaceholderText("�������½���վ���ƣ��磺220kV������ά��վ");

	bool ret = initComboBox();
	if (ret)
	{
		ui.selectRadioButton->setChecked(true);
		ui.subLineEdit->setEnabled(false);
	}
	else
	{
		ui.selectRadioButton->setEnabled(false);
		ui.newRadioButton->setChecked(true);
		ui.subLineEdit->setEnabled(true);
	}
	
}

bool DlgAckSubstation::initComboBox()
{
	SRecordset	rs;

	SString sql = SString::toFormat("select sub_no,name from t_cim_substation");
	int iRet = DB->Retrieve(sql,rs);
	if (iRet <= 0)
	{
		return false;
	}
	else if (iRet > 0)
	{
		for (int i = 0; i < iRet; i++)
		{
			int no = rs.GetValue(i,0).toInt();
			QString name = rs.GetValue(i,1).data();
			ui.subComboBox->addItem(name.trimmed(),no);
		}
	}

	return true;
}

void DlgAckSubstation::slotOk()
{
	SRecordset	rs;
	SString		sql;

	if (ui.selectRadioButton->isChecked())
	{
		sub_no = ui.subComboBox->itemData(ui.subComboBox->currentIndex()).toInt();
	}
	else if (ui.newRadioButton->isChecked())
	{
		if (ui.subLineEdit->text().isEmpty())
		{
			QMessageBox::information(this,tr("��Ϣ"),tr("��������վ���ơ�"));
			return;
		}

		sql = "select max(sub_no) from t_cim_substation";
		int iRet = DB->Retrieve(sql,rs);
		if (iRet < 0)
		{
			QMessageBox::information(this,tr("����"),tr("SQL���ִ�д���\n\n%1").arg(sql.data()));
			return;
		}
		else if (iRet == 0)
			sub_no = 1;
		else if (iRet > 0)
		{
			sub_no = rs.GetValue(0,0).toInt();
			sub_no++;
		}

		int cmp_no = 0;
		sql = SString::toFormat("select max(cmp_no) from t_cim_company");
		DB->Retrieve(sql, rs);
		if (rs.GetRows() > 0)
			cmp_no = rs.GetValueInt(0, 0);

		SString name = ui.subLineEdit->text().trimmed().toLocal8Bit().data();
		sql.sprintf("insert into t_cim_substation (sub_no,cmp_no,name) values (%d,%d,'%s')",sub_no,cmp_no,name.data());
		if (!DB->Execute(sql))
		{
			QMessageBox::information(this,tr("����"),tr("SQL���ִ�д���\n\n%1").arg(sql.data()));
			return;
		}

		if (m_bMDB && !MDB->Execute(sql))
		{
			QMessageBox::information(this,tr("����"),tr("�ڴ��SQL���ִ�д���\n\n%1").arg(sql.data()));
			return;
		}
	}

	if (!SetNodeParam())
	{
		QMessageBox::information(this,tr("����"),tr("���нڵ�����в�������ʧ�ܣ����顣"));
		return;
	}

	accept();
}

void DlgAckSubstation::slotNewRadioButtonClicked()
{
	ui.subComboBox->setEnabled(false);
	ui.subLineEdit->setEnabled(true);
}

void DlgAckSubstation::slotSelectRadioButtonClicked()
{
	ui.subComboBox->setEnabled(true);
	ui.subLineEdit->setEnabled(false);
}

bool DlgAckSubstation::SetNodeParam()
{
	SRecordset	rs;
	SString		sql;

	sql.sprintf("select t_oe_run_node.node_no from t_oe_run_node,t_oe_module_param where "
		"t_oe_run_node.node_no=t_oe_module_param.node_no and "
		"t_oe_module_param.param_name='substation_id' and "
		"t_oe_module_param.current_val='%d'",sub_no);
	int cnt = DB->Retrieve(sql,rs);
	if (cnt > 0)	//���д���վ�ŵ����нڵ�
		return true;

	int node_no = 0;
	sql = "select max(node_no) from t_oe_run_node";
	cnt = DB->Retrieve(sql,rs);
	if (cnt < 0)
	{
		QMessageBox::information(this,tr("����"),tr("SQL���ִ�д���\n\n%1").arg(sql.data()));
		return false;
	}
	else if (cnt == 0)
	{
		node_no = 1;
	}
	else if (cnt > 0)
	{
		node_no = rs.GetValue(0,0).toInt();
		node_no++;
	}

	sql.sprintf("delete from t_oe_run_node where node_no=%d",node_no);
	DB->Execute(sql);
	if (m_bMDB)
		MDB->Execute(sql);
	sql.sprintf("delete from t_oe_module_param where node_no=%d",node_no);
	DB->Execute(sql);
	if (m_bMDB)
		MDB->Execute(sql);
	sql.sprintf("insert into t_oe_run_node (node_no,name,description,module_name) values (%d,'%s','%s','%s')",
		node_no,"��վ61850�ɼ�",ui.subLineEdit->text().trimmed().toLocal8Bit().data(),"mst_mmsclt");
	if (!DB->Execute(sql))
		return false;

	if (m_bMDB && !MDB->Execute(sql))
		return false;

	switch (DB_MASTER_TYPE)
	{
	case DB_MYSQL:
		sql.sprintf("insert into t_oe_module_param (node_no,module_name,param_name,val_type,current_val,val_fmt,def_val,dsc) values "
			"(%d,'%s','%s',%d,'%s','%s','%s','%s'),"
			"(%d,'%s','%s',%d,'%s','%s','%s','%s'),"
			"(%d,'%s','%s',%d,'%s','%s','%s','%s'),"
			"(%d,'%s','%s',%d,'%s','%s','%s','%s'),"
			"(%d,'%s','%s',%d,'%s','%s','%s','%s'),"
			"(%d,'%s','%s',%d,'%s','%s','%s','%s'),"
			"(%d,'%s','%s',%d,'%s','%s','%s','%s')",
			node_no,"mst_mmsclt","ip_addr_a",0,"","","","A��IP��ַ",
			node_no,"mst_mmsclt","ip_addr_b",0,"","","","B��IP��ַ",
			node_no,"mst_mmsclt","is_period",0,"","","","���������Ƿ���Ч",
			node_no,"mst_mmsclt","report_id",0,"","","","ע�ᱨ���",
			node_no,"mst_mmsclt","substation_id",0,SString::toFormat("%d",sub_no).data(),"","","��ά��վ��",
			node_no,"mst_mmsclt","run_mode",0,SString::toFormat("%d",1).data(),"","","����ģʽ��1��վģʽ��2��վģʽ",
			node_no,"mst_mmsclt","region_specification",0,SString::toFormat("%d",1).data(),"","","�����淶��0Ĭ�ϣ�1��������¼����");
		break;

	case DB_ORACLE:
		sql.sprintf("insert all into t_oe_module_param (node_no,module_name,param_name,val_type,current_val,val_fmt,def_val,dsc) values "
			"(%d,'%s','%s',%d,'%s','%s','%s','%s') "
			"into t_oe_module_param (node_no,module_name,param_name,val_type,current_val,val_fmt,def_val,dsc) values "
			"(%d,'%s','%s',%d,'%s','%s','%s','%s') "
			"into t_oe_module_param (node_no,module_name,param_name,val_type,current_val,val_fmt,def_val,dsc) values "
			"(%d,'%s','%s',%d,'%s','%s','%s','%s') "
			"into t_oe_module_param (node_no,module_name,param_name,val_type,current_val,val_fmt,def_val,dsc) values "
			"(%d,'%s','%s',%d,'%s','%s','%s','%s') "
			"into t_oe_module_param (node_no,module_name,param_name,val_type,current_val,val_fmt,def_val,dsc) values "
			"(%d,'%s','%s',%d,'%s','%s','%s','%s') "
			"into t_oe_module_param (node_no,module_name,param_name,val_type,current_val,val_fmt,def_val,dsc) values "
			"(%d,'%s','%s',%d,'%s','%s','%s','%s') "
			"into t_oe_module_param (node_no,module_name,param_name,val_type,current_val,val_fmt,def_val,dsc) values "
			"(%d,'%s','%s',%d,'%s','%s','%s','%s') SELECT 1 FROM DUAL",
			node_no,"mst_mmsclt","ip_addr_a",0,"","","","A��IP��ַ",
			node_no,"mst_mmsclt","ip_addr_b",0,"","","","B��IP��ַ",
			node_no,"mst_mmsclt","is_period",0,"","","","���������Ƿ���Ч",
			node_no,"mst_mmsclt","report_id",0,"","","","ע�ᱨ���",
			node_no,"mst_mmsclt","substation_id",0,SString::toFormat("%d",sub_no).data(),"","","��ά��վ��",
			node_no,"mst_mmsclt","run_mode",0,SString::toFormat("%d",1).data(),"","","����ģʽ��1��վģʽ��2��վģʽ",
			node_no,"mst_mmsclt","region_specification",0,SString::toFormat("%d",1).data(),"","","�����淶��0Ĭ�ϣ�1��������¼����");
		break;

	default:
		return false;
	}
	
	if (!DB->Execute(sql))
		return false;

	if (m_bMDB && !MDB->Execute(sql))
		return false;

	return true;
}
