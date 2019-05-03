/**
 *
 * �� �� �� : view_plugin_scl_import.cpp
 * �������� : 2017-08-10 09:50
 * ��    �� : SspAssist(skt001@163.com)
 * �޸����� : $Date: $
 * ��ǰ�汾 : $Revision: $
 * �������� : SCD,ICD,CID�ļ�����
 * �޸ļ�¼ : 
 *            $Log: $
 *
 * Ver  Date        Author    Comments
 * ---  ----------  --------  -------------------------------------------
 * 001	 2017-08-10	SspAssist�������ļ�
 *
 **/
 
#include "view_plugin_scl_import.h"
#include "oe_ied.h"
#include "oe_ied_model.h"
#include "oe_ied_model_version.h"
#include "oe_cpu.h"
#include "oe_group.h"
#include "dlggrouptype.h"
#include "dlgsubnetwork.h"
#include "dlgacksubstation.h"
#include "na_sub_network.h"
#include "dlgimportwarntext.h"
//#include "HMI/uk9000_def.h"
#include "skgui.h"

view_plugin_scl_import::view_plugin_scl_import(QWidget *parent)
: CBaseView(parent)
{
	ui.setupUi(this);

	init();
	initThread();
	initSlots();
}

view_plugin_scl_import::~view_plugin_scl_import()
{
	qDeleteAll(document);
	document.clear();

	delete progressDialog;
	delete progressDialogTimer;
}

int view_plugin_scl_import::OnCommand(SString sCmd,SString &sResult)
{
	SString ret = SString::GetAttributeValue(sCmd,"MDB");
	if(ret == "false")
		m_bMDB = false;

	return 0;
}

void view_plugin_scl_import::init()
{
	progressDialog = new QProgressDialog();
	progressDialog->setModal(true);
	progressDialog->setCancelButton(NULL);

	progressDialogTimer = new QTimer();
	progressDialogTimer->setInterval(100);      
	progressDialogTimer->start();

	progressValue = 0;
	progressMaxValue = 0;
	progressBarValue = 0;
	progressBarMaxValue = 0;
	bProgressDialogStart = false;

	ui.pushButton_reslove->setEnabled(true);
	ui.widget_ieds->setEnabled(false);

	ui.checkBox_s1->setChecked(true);
	ui.checkBox_g1->setChecked(true);
	ui.checkBox_m1->setChecked(true);

	ui.progressBar->setValue(0);

	ui.tableWidget_ieds->setColumnWidth(TABLE_IEDS_NAME,150);
	ui.tableWidget_ieds->setColumnWidth(TABLE_IEDS_DESC,400);
	ui.tableWidget_ieds->setColumnWidth(TABLE_IEDS_TYPE,150);
	ui.tableWidget_ieds->setColumnWidth(TABLE_IEDS_MANFACTURER,100);

	substationNo = 0;
	ui.label_desc->setText(QString::null);
	m_bMDB = DB->SelectIntoI(SString::toFormat("select set_value from t_ssp_setup where set_key='scl.mdb'")) == 1 ? false : true;

	SetBackgroundColor();
}

void view_plugin_scl_import::initThread()
{
	parseThread = new XmlParseThread(this);
	importThread = new DbImportThread(this);
	inputsThread = new InputsThread(this);
}

void view_plugin_scl_import::initSlots()
{
	connect(ui.pushButton_reslove,SIGNAL(clicked()), this, SLOT(slotParseFile()));
	connect(ui.pushButton_abort,SIGNAL(clicked()), this, SLOT(slotAbort()));
	connect(ui.pushButton_smart_import,SIGNAL(clicked()), this, SLOT(slotSmartImport()));
	connect(ui.pushButton_import,SIGNAL(clicked()), this, SLOT(slotImport()));
	connect(ui.checkBox_s1,SIGNAL(stateChanged(int)),this,SLOT(slotS1Changed(int)));
	connect(ui.checkBox_g1,SIGNAL(stateChanged(int)),this,SLOT(slotG1Changed(int)));
	connect(ui.checkBox_m1,SIGNAL(stateChanged(int)),this,SLOT(slotM1Changed(int)));
	connect(parseThread,SIGNAL(finished()),this,SLOT(slotXmlParseThreadFinished()));
	connect(importThread,SIGNAL(finished()),this,SLOT(slotDbImportThreadFinished()));
	connect(inputsThread,SIGNAL(finished()),this,SLOT(slotInputsThreadFinished()));
	connect(progressDialogTimer,SIGNAL(timeout()),this,SLOT(slotProgressDialogTimeOut()));
	connect(this,SIGNAL(sigProgressValue(int)),this,SLOT(onProgressValue(int)));
}

void view_plugin_scl_import::slotParseFile()
{
	QString filter = tr("SCL files(*.scd *.cid *.icd);;SCD files(*.scd);;CID files(*.cid);;ICD files(*.icd);;XML files(*.xml);;All files(*.*)");
	QString fileName = QFileDialog::getOpenFileName(this,tr("Open File"),QString::null,filter);
	if (!fileName.isEmpty())
	{
		xmlFile = fileName;	//���߳���
		progressMaxValue = fileLineNumber(fileName);
		ui.lineEdit_path->setText(fileName);
		if (loadDeviceType() && loadElementType() && loadGroupType())
			xmlParseStart();
	}
}

bool view_plugin_scl_import::loadDeviceType()
{
	deviceTypeList.clear();

	SRecordset	rs;
	SString sql = SString::toFormat("select type_no,dev_cls,name,wildcard FROM t_oe_device_type");
	int iRet = DB->Retrieve(sql,rs);
	if (iRet <= 0)
	{
		int ret = QMessageBox::question(this,tr("Message"),
			tr("δ�����ݿ��з���װ�����ͱ�\n\n���[ȷ��]���������룬����ȱ��װ�����͡�\n���[����]����ֹ���룬��ѡ��?"),
			tr("ȷ��"),tr("����"));
		if (ret == 0)
			return true;
		else
			return false;
	}
	else if (iRet > 0)
	{
		for (int i = 0; i < iRet; i++)
		{
			device_type_t dtt;
			dtt.type_no = rs.GetValue(i,0).toInt();
			dtt.dev_cls = rs.GetValue(i,1).toInt();
			dtt.name = rs.GetValue(i,2).data();
			dtt.wildcard = rs.GetValue(i,3).data();
			deviceTypeList.append(dtt);
		}
	}

	return true;
}

bool view_plugin_scl_import::loadElementType()
{
	elementTypeList.clear();

	SRecordset	rs;
	SString sql = SString::toFormat("select type_no,device_type_no,name,wildcard FROM t_oe_element_type");
	int iRet = DB->Retrieve(sql,rs);
	if (iRet <= 0)
	{
		int ret = QMessageBox::question(this,tr("Message"),
			tr("δ�����ݿ��з�����Ŀ���ͱ�\n\n���[ȷ��]���������룬����ȱ����Ŀ���͡�\n���[����]����ֹ���룬��ѡ��?"),
			tr("ȷ��"),tr("����"));
		if (ret == 0)
			return true;
		else
			return false;
	}
	else if (iRet > 0)
	{
		for (int i = 0; i < iRet; i++)
		{
			element_type_t ett;
			ett.type_no = rs.GetValue(i,0).toInt();
			ett.device_type_no = rs.GetValue(i,1).toInt();
			ett.name = rs.GetValue(i,2).data();
			ett.wildcard = rs.GetValue(i,3).data();
			elementTypeList.append(ett);
		}
	}

	return true;
}

bool view_plugin_scl_import::loadGroupType()
{
	SString sql;
	SRecordset rs;

	mapGroupType.clear();

	sql.sprintf("select dataset_name,group_type from t_oe_dataset_group_type");
	int cnt = DB->Retrieve(sql,rs);
	if (cnt < 0)
	{
		QMessageBox::critical(this,tr("����"),tr("���ݿ����ʧ�ܡ�\n\nSQL��%s").arg(sql.data()));
		return false;
	}
	else if (cnt == 0)
	{
		QMessageBox::warning(this,tr("����"),tr("δ�����ݿ��з�����Ŀ���ͱ�����t_oe_dataset_group_type�����ݡ�"));
		return false;
	}
	else if (cnt > 0)
	{
		for (int i = 0; i < cnt; i++)
		{
			QString name = rs.GetValue(i,0).data();
			int type = rs.GetValue(i,1).toInt();
			mapGroupType.insert(name,type);
		}
	}

	return true;
}

void view_plugin_scl_import::xmlParseStart()
{
	progressValue = 0;
	progressDialog->setRange(0,progressMaxValue);
	progressDialog->setWindowTitle(tr("��Ϣ"));
	progressDialog->setLabelText(tr("����SCL�ļ���, ��ȴ�......"));
	progressDialog->setValue(0);
	progressDialog->show();
	bProgressDialogStart = true;

	qDeleteAll(document);
	document.clear();

	parseThread->start();
	ui.pushButton_reslove->setEnabled(false);
}

void view_plugin_scl_import::slotXmlParseThreadFinished()
{
	progressDialog->cancel();

	if (!parseError.isEmpty())
	{
		LOGFAULT("�ļ��������󣬴������ݣ� %s .", parseError.toLatin1().data());
		QMessageBox::critical(this,tr("Critical Message"),parseError);
		slotAbort();
	}
	else
	{
		ui.tableWidget_ieds->setDocument(document);
		ui.tableWidget_ieds->setCheckStateS1(ui.checkBox_s1->checkState());
		ui.tableWidget_ieds->setCheckStateG1(ui.checkBox_g1->checkState());
		ui.tableWidget_ieds->setCheckStateM1(ui.checkBox_m1->checkState());
		ui.tableWidget_ieds->reloadIedsTableWidget();
		ui.widget_ieds->setEnabled(true);
		loadMmsNetName();	//����MMSA��MMSB��������
	}
}

void view_plugin_scl_import::dbImportStart(int sub_no,QString name,QString desc)
{
	int ret;
	int	iedNo = 0;

	if (isExistIed(sub_no,name))
	{
		ret = QMessageBox::question(this,tr("ѯ��"),tr("װ�á�%1���Ѿ�����, �Ƿ����µ���?")
			.arg(name),tr("��"),tr("��"));
		if (ret == 0)
		{
			if(!deleteIed(sub_no,name,&iedNo))
				return;
			//if(!deleteIedParam(iedNo))
			//	return;
			//if(!deleteCpu(iedNo))
			//	return;
			//if(!deleteGroup(iedNo))
			//	return;
			//if(!deleteElementGeneral(iedNo))
			//	return;
			//if(!deleteElementState(iedNo))
			//	return;
			//if(!deleteElementControl(iedNo))
			//	return;
		}
		else
		{
			slotDbImportThreadFinished();
			return;
		}
	}

	oe_ied_model ied_model(document,name);
	if (!ied_model.execute(parseError))
	{
		slotDbImportThreadFinished();
		return;
	}

	importThread->setName(name);
	importThread->setDesc(desc);
	importThread->setIedNo(iedNo);
	importThread->setSubstation(sub_no);
	importThread->checkStateS1 = ui.checkBox_s1->checkState();
	importThread->checkStateG1 = ui.checkBox_g1->checkState();
	importThread->checkStateM1 = ui.checkBox_m1->checkState();
	if(!addGroupType(name,desc))
	{
		slotDbImportThreadFinished();
		return;
	}

	importThread->start();
}

void view_plugin_scl_import::dbSmartImportStart(int sub_no,QString name,QString desc)
{
	int ret;
	int	iedNo = 0;

	if (isExistIed(sub_no,name))
	{
		if (bReImport)
		{
			if(!deleteIed(sub_no,name,&iedNo))
				return;
			//if(!deleteIedParam(iedNo))
			//	return;
			//if(!deleteCpu(iedNo))
			//	return;
			//if(!deleteGroup(iedNo))
			//	return;
			//if(!deleteElementGeneral(iedNo))
			//	return;
			//if(!deleteElementState(iedNo))
			//	return;
			//if(!deleteElementControl(iedNo))
			//	return;
		}
		else
		{
			ret = QMessageBox::question(this,tr("Message"),tr("Device already exists, whether to all re import?"),tr("Ok"),tr("Cancel"));
			if (ret == 0)
			{
				bReImport = true;
				if(!deleteIed(sub_no,name,&iedNo))
					return;
				//if(!deleteIedParam(iedNo))
				//	return;
				//if(!deleteCpu(iedNo))
				//	return;
				//if(!deleteGroup(iedNo))
				//	return;
				//if(!deleteElementGeneral(iedNo))
				//	return;
				//if(!deleteElementState(iedNo))
				//	return;
				//if(!deleteElementControl(iedNo))
				//	return;
			}
			else
			{
				ret = QMessageBox::question(this,tr("Message"),tr("Device [%1] already exists, whether to re import?")
					.arg(name),tr("Ok"),tr("Cancel"));
				if (ret == 0)
				{
					if(!deleteIed(sub_no,name,&iedNo))
						return;
					//if(!deleteIedParam(iedNo))
					//	return;
					//if(!deleteCpu(iedNo))
					//	return;
					//if(!deleteGroup(iedNo))
					//	return;
					//if(!deleteElementGeneral(iedNo))
					//	return;
					//if(!deleteElementState(iedNo))
					//	return;
					//if(!deleteElementControl(iedNo))
					//	return;
				}
				else
				{
					slotDbImportThreadFinished();
					return;
				}
			}
		}
	}

	oe_ied_model ied_model(document,name);
	if (!ied_model.execute(parseError))
	{
		slotDbImportThreadFinished();
		return;
	}

	importThread->setName(name);
	importThread->setDesc(desc);
	importThread->setIedNo(iedNo);
	importThread->setSubstation(sub_no);
	importThread->checkStateS1 = ui.checkBox_s1->checkState();
	importThread->checkStateG1 = ui.checkBox_g1->checkState();
	importThread->checkStateM1 = ui.checkBox_m1->checkState();
	if (!addGroupType(name,desc))
	{
		slotDbImportThreadFinished();
		return;
	}

	importThread->start();
}

void view_plugin_scl_import::inputsStart()
{
	SString sql;
	SRecordset rs;
	int number = 0;

	sql.sprintf("select count(*) from t_oe_element_input_extref,t_cim_substation,t_oe_ied "
		"where t_cim_substation.sub_no=%d and t_cim_substation.sub_no=t_oe_ied.sub_no and "
		"t_oe_element_input_extref.ied_no=t_oe_ied.ied_no",substationNo);
	int cnt = DB->Retrieve(sql,rs);
	if (cnt < 0)
	{
		QMessageBox::critical(this,tr("����"),QString("SQL���ִ�д���%1").arg(sql.data()));
		slotAbort();
		return;
	}
	else if (cnt > 0)
	{
		number = rs.GetValue(0,0).toInt();
	}

	if (number == 0)
	{
		QMessageBox::information(this,tr("��ʾ"),
			QString("��վ[%1]�����ݿ��t_oe_element_input_extref����Ϊ�գ�����������ӵ��롣").arg(substationNo));
		emit reportImportComleteFinish(true);
		slotAbort();
		return;
	}

	ui.progressBar->setRange(0,number);
	ui.progressBar->setValue(0);

	inputsThread->start();
}

void view_plugin_scl_import::slotDbImportThreadFinished()
{
	//�д�����ʾ����ֹ����
	if (!parseError.isEmpty())
	{
		if (parseError.size() > 1000)
			parseError = parseError.left(1000);

		LOGFAULT("SCL���ݿ⵼����󣬴������ݣ� %s .", parseError.toLatin1().data());
		QMessageBox::critical(this,tr("����"),parseError);
		slotAbort();
		ui.label_desc->setText(QString::null);
		return;
	}

	//�޴��󣬸��½�����
	ieds_row++;
	ui.progressBar->setValue(++progressBarValue);
	if (ieds_row >= ui.tableWidget_ieds->rowCount())
	{
		ui.label_desc->setText(tr("���ڽ�������..."));
		inputsStart();	//װ�õ�����ɺ󣬽���inputs�����߳�
		ui.label_desc->setText(QString::null);
		return;
	}

	while(ui.tableWidget_ieds->item(ieds_row,TABLE_IEDS_NAME)->checkState() != Qt::Checked)
	{
		ieds_row++;
		if (ieds_row >= ui.tableWidget_ieds->rowCount())
		{
			ui.label_desc->setText(tr("���ڽ�������..."));
			inputsStart();	//װ�õ�����ɺ󣬽���inputs�����߳�
			ui.label_desc->setText(QString::null);
			return;
		}
	}

	QTableWidgetItem * item;
	QString name;
	QString desc;
	item = ui.tableWidget_ieds->item(ieds_row,TABLE_IEDS_NAME);
	if (item)
		name = item->text().trimmed();
	item = ui.tableWidget_ieds->item(ieds_row,TABLE_IEDS_DESC);
	if (item)
		desc = item->text().trimmed();

	if (bSmartImport)	//���ܵ���
		dbSmartImportStart(substationNo,name,desc);
	else	//�����ܵ���
		dbImportStart(substationNo,name,desc);
}

void view_plugin_scl_import::slotInputsThreadFinished()
{
	//�д�����ʾ����ֹ����
	if (!parseError.isEmpty())
	{
		if (parseError.size() > 1000)
			parseError = parseError.left(1000);

		LOGFAULT("SCL���ݿ⵼����󣬴������ݣ� %s .", parseError.toLatin1().data());
		QMessageBox::critical(this,tr("����"),parseError);
	}
	else
	{
		if (!_oe_element_state_unknown_desc())
			QMessageBox::information(this,tr("�澯"),tr("��վװ����Ŀ��ʵ��װ�ù���ʧ�ܡ�"));
		else
			QMessageBox::information(this,tr("��Ϣ"),tr("���й�ѡװ�õ���ɹ���"));
		emit reportImportComleteFinish(true);

		//if (!parseWarn.isEmpty())
		//{
		//	DlgImportWarnText dlg(this);
		//	dlg.setWarnText(parseWarn);
		//	dlg.exec();
		//}
	}

	slotAbort();
}

void view_plugin_scl_import::slotProgressDialogTimeOut()
{
	if (bProgressDialogStart)
		progressDialog->setValue(progressValue);
	else
		progressDialog->cancel();
}

void view_plugin_scl_import::onProgressValue(int value)
{
	ui.progressBar->setValue(value);
}

void view_plugin_scl_import::slotS1Changed(int state)
{
	S_UNUSED(state);
	ui.tableWidget_ieds->setCheckStateS1(ui.checkBox_s1->checkState());
	ui.tableWidget_ieds->reloadIedsTableWidget();
}

void view_plugin_scl_import::slotG1Changed(int state)
{
	S_UNUSED(state);
	ui.tableWidget_ieds->setCheckStateG1(ui.checkBox_g1->checkState());
	ui.tableWidget_ieds->reloadIedsTableWidget();
}

void view_plugin_scl_import::slotM1Changed(int state)
{
	S_UNUSED(state);
	ui.tableWidget_ieds->setCheckStateM1(ui.checkBox_m1->checkState());
	ui.tableWidget_ieds->reloadIedsTableWidget();
}

void view_plugin_scl_import::slotAbort()
{
	ieds_row = 0;
	mmsNetA_name = QString::null;
	mmsNetB_name = QString::null;
	ui.pushButton_reslove->setEnabled(true);
	ui.lineEdit_path->setText(QString::null);
	ui.tableWidget_ieds->setRowCount(0);
	ui.tableWidget_ieds->clearContents();
	ui.widget_ieds->setEnabled(false);

	qDeleteAll(document);
	document.clear();
	//SString sFile = CGuiApplication::GetApp()->GetConfPath()+"1.scd";
	//SFile::copy(xmlFile.toStdString().data(),sFile);
	//view_visual_ap::LoadScdBackground(sFile);
}

void view_plugin_scl_import::slotImport()
{
	progressBarValue = 0;
	progressBarMaxValue = 0;
	bReImport = false;
	bSmartImport = false;

	//����Ƿ�ѡ��װ�ã������ý�����max
	for (int i = 0; i < ui.tableWidget_ieds->rowCount(); i++)
	{
		if (ui.tableWidget_ieds->item(i,TABLE_IEDS_NAME)->checkState() == Qt::Checked)
			progressBarMaxValue++;
	}
	if (progressBarMaxValue > 0)
	{
		ui.progressBar->setRange(0,progressBarMaxValue);
		ui.progressBar->setValue(0);
	}
	else
	{
		QMessageBox::information(this,tr("Message"),tr("Please select the device that needs to import the database."));
		return;
	}

	DlgAckSubstation dlg(this);
	if (dlg.exec() == QDialog::Accepted)
	{
		substationNo = dlg.getSubstationNo();
		if (substationNo <= 0)
		{
			QMessageBox::information(this,tr("��Ϣ"),tr("��ѡ��վ��վ���쳣��������ѡ��"));
			return;
		}
	}
	else
		return;

	if (!deleteIedByDocument(substationNo)) //ɾ�����ݿ��в������ڵ����ļ���װ��
		return;
	if (!deleteSubNetwork(substationNo)) //ɾ�����ݿ��г�վ��Ӧ������
		return;

	na_sub_network subnetwork(this,document,NULL);
	if (!subnetwork.execute(parseError))
	{
		QMessageBox::critical(this,tr("����"),parseError);
		slotAbort();
		return;
	}

	ui.pushButton_reslove->setEnabled(false);
	ui.widget_ieds->setEnabled(false);

	//��ȡ��һ��ѡ��װ�ã�����
	ieds_row = 0;
	while(ui.tableWidget_ieds->item(ieds_row,TABLE_IEDS_NAME)->checkState() != Qt::Checked)
		ieds_row++;

	QTableWidgetItem * item;
	QString name;
	QString desc;
	item = ui.tableWidget_ieds->item(ieds_row,TABLE_IEDS_NAME);
	if (item)
		name = item->text().trimmed();
	item = ui.tableWidget_ieds->item(ieds_row,TABLE_IEDS_DESC);
	if (item)
		desc = item->text().trimmed();

	int ret = QMessageBox::question(this,tr("ѯ��"),tr("��SCL�ļ��Ƿ������ļ�����"),tr("��"),tr("��"));
	if (ret == 0)
		UploadScdFile(substationNo,xmlFile.toStdString().data());	//MEMO: ���ӵ��ļ���ʷ�汾���� [2016-2-22 10:17 �ۿ���]

	ui.label_desc->setText(tr("���ڵ���ģ��..."));
	dbImportStart(substationNo,name,desc);
}

////////////////////////////////////////////////////////////////////////
// ��    ��:  ������Ϣ����ӿڣ���������ʵ�֣����������뾡����С�����ٷ��أ���̨�߳������������в��ܵ����κν���Ԫ��
// ��    ��:  �ۿ���
// ����ʱ��:  2016-2-3 15:20
// ����˵��:  @wMsgType��ʾ��Ϣ����
//         :  @pMsgHeadΪ��Ϣͷ
//         :  @sHeadStr��Ϣͷ�ַ���
//         :  @pBufferΪ��ϢЯ���Ķ������������ݣ�NULL��ʾ�޶���������
//         :  @iLengthΪ���������ݳ���
// �� �� ֵ:  true��ʾ����ɹ���false��ʾ����ʧ��
//////////////////////////////////////////////////////////////////////////
bool view_plugin_scl_import::ProcessAgentMsg(WORD wMsgType,stuSpUnitAgentMsgHead *pMsgHead,SString &sHeadStr,BYTE* pBuffer/*=NULL*/,int iLength/*=0*/)
{
	S_UNUSED(pMsgHead);
	S_UNUSED(pBuffer);
	S_UNUSED(iLength);
	//switch(wMsgType)
	//{
	//case UK9000_MSG_NEWSCD_CHECK_END:
	//	{
	//		SString new_scd = SString::GetAttributeValue(sHeadStr,"new_scd");
	//		SSP_GUI->SetStatusTip(SString::toFormat("SCD�ļ�(%s)�ļ���⼰�ȶ����!",new_scd.data()));
	//		//GAPP_ADD_TO_SPEAKER("SCD�ļ��ļ���⼰�ȶ����");
	//		return true;
	//	}
	//}
	return false;
}

//////////////////////////////////////////////////////////////////////////
// ��    ��:  �ϴ�һ���°汾��SCD�ļ���ָ����վ
// ��    ��:  �ۿ���
// ����ʱ��:  2016-2-23 8:32
// ����˵��:  @sub_no��ʾ��վ���
//         :  @sLocalScdPathFile��ʾ���ص�SCD�ļ�ȫ·��
// �� �� ֵ:  true��ʾ�ϴ��ɹ���false��ʾ�ϴ�ʧ��
//////////////////////////////////////////////////////////////////////////
bool view_plugin_scl_import::UploadScdFile(int sub_no,SString sLocalScdPathFile)
{
	//SPtrList<stuSpUnitAgentProcessId> ProcIds;
	//SBASE_SAPP->GetProcessId(UK9000_APP_OMS_MAIN,0,ProcIds);
	//if(ProcIds.count() <= 0)
	//{
	//	SQt::ShowErrorBox(tr("����"),tr("δ�ҵ���Ӧ�ó���SCD���޷�������ʷ�汾!"));
	//	return false;
	//}
	//else
	//{
	//	SString error;
	//	SString crc = SApi::GenerateCRCByFile(sLocalScdPathFile, error);

	//	stuSpUnitAgentProcessId *pOmsMainId = ProcIds[0];
	//	DWORD msg_sn = SBASE_SAPP->NewMsgSn();
	//	SString sHeadStr;
	//	sHeadStr.sprintf("sub_no=%d;crc=%s;",sub_no,crc.data());
	//	stuSTcpPackage *pRecvPackage = NULL;
	//	if(!SBASE_SAPP->SendAgentMsgAndRecv(pOmsMainId,UK9000_MSG_REQ_NEW_SCDFILE,msg_sn,0,NULL,0,&sHeadStr,UK9000_MSG_RES_NEW_SCDFILE,pRecvPackage,3))
	//	{
	//		SQt::ShowErrorBox(tr("����"),tr("��������ʷ�汾�洢ʱʧ�ܣ�SCD���޷�������ʷ�汾!"));
	//		return false;
	//	}
	//	else
	//	{
	//		//sub_no=%d;last_scd=%s;new_scd=%s;
	//		SString sLastScd = SString::GetAttributeValue(pRecvPackage->m_sHead,"last_scd");
	//		SString sNewScd = SString::GetAttributeValue(pRecvPackage->m_sHead,"new_scd");
	//		int scd_no = SString::GetAttributeValueI(pRecvPackage->m_sHead,"scd_no");
	//		delete pRecvPackage;
	//		pRecvPackage = NULL;
	//		if(SBASE_SAPP->PutUnitFile(pOmsMainId->m_iUnitId,sNewScd,sLocalScdPathFile,60) < 0)
	//		{
	//			SQt::ShowErrorBox(tr("����"),tr("�ϴ�SCD�ļ�����ʷ�汾��ʱʧ�ܣ�SCD���޷�������ʷ�汾!"));
	//			return false;
	//		}
	//		else
	//		{
	//			//�ϴ��ɹ�����ʼ��ʷ���				
	//			sHeadStr.sprintf("sub_no=%d;scd_no=%d;last_scd=%s;new_scd=%s;usr_sn=%d;",sub_no,scd_no,sLastScd.data(),sNewScd.data(),SSP_BASE->m_iOperUserSn);
	//			if(!SBASE_SAPP->SendAgentMsgAndRecv(pOmsMainId,UK9000_MSG_NEWSCD_CHECK,msg_sn,0,NULL,0,&sHeadStr,UK9000_MSG_NEWSCD_CHECK_START,pRecvPackage,3))
	//			{
	//				SQt::ShowErrorBox(tr("����"),tr("�����汾����Ƚ�����ʧ�ܣ�SCD���޷�������ʷ�汾!"));
	//				return false;
	//			}
	//			else
	//			{
	//				int result = SString::GetAttributeValueI(pRecvPackage->m_sHead,"result");
	//				if(result != 0)
	//				{
	//					SQt::ShowErrorBox(tr("����"),tr("�����汾����Ƚ��������SCD���޷�������ʷ�汾!"));
	//					return false;
	//				}
	//				else
	//				{
	//					SSP_GUI->SetStatusTip(SString::toFormat("SCD�ļ�(%s)�����,����������ȶ�!",sNewScd.data()));
	//					//GAPP_ADD_TO_SPEAKER("SCD�ļ������,����������ȶ�");
	//				}
	//			}
	//			if(pRecvPackage != NULL)
	//			{
	//				delete pRecvPackage;
	//				pRecvPackage = NULL;
	//			}
	//		}
	//	}
	//}
	return true;
}

void view_plugin_scl_import::slotSmartImport()
{
	progressBarValue = 0;
	progressBarMaxValue = 0;
	bReImport = false;
	bSmartImport = true;

	SRecordset rs;
	if (DB->Retrieve("select da_name from t_oe_element_state",rs) == -1 ||
		DB->Retrieve("select da_name from t_oe_element_general",rs) == -1)
	{
		QMessageBox::warning(this,tr("����"),tr("�˰汾��Ҫt_oe_element_state��t_oe_element_general������da_name�ֶΣ����ֶ���ӡ�"));
		return;
	}

	//����Ƿ�ѡ��װ�ã������ý�����max
	for (int i = 0; i < ui.tableWidget_ieds->rowCount(); i++)
	{
		if (ui.tableWidget_ieds->item(i,TABLE_IEDS_NAME)->checkState() == Qt::Checked)
			progressBarMaxValue++;
	}
	if (progressBarMaxValue > 0)
	{
		ui.progressBar->setRange(0,progressBarMaxValue);
		ui.progressBar->setValue(0);
	}
	else
	{
		QMessageBox::information(this,tr("��Ϣ"),tr("��ѡ����Ҫ�����װ�á�"));
		return;
	}

	DlgAckSubstation dlg(this);
	if (dlg.exec() == QDialog::Accepted)
	{
		substationNo = dlg.getSubstationNo();
		if (substationNo <= 0)
		{
			QMessageBox::information(this,tr("��Ϣ"),tr("��ѡ��վ��վ���쳣��������ѡ��"));
			return;
		}
	}
	else
		return;

	if (!deleteIedByDocument(substationNo)) //ɾ�����ݿ��в������ڵ����ļ���װ��
		return;
	if (!deleteSubNetwork(substationNo)) //ɾ�����ݿ��г�վ��Ӧ������
		return;

	na_sub_network subnetwork(this,document,NULL);
	if (!subnetwork.execute(parseError))
	{
		QMessageBox::critical(this,tr("����"),parseError);
		slotAbort();
		return;
	}

	ui.pushButton_reslove->setEnabled(false);
	ui.widget_ieds->setEnabled(false);

	//��ȡ��һ��ѡ��װ�ã�����
	ieds_row = 0;
	while(ui.tableWidget_ieds->item(ieds_row,TABLE_IEDS_NAME)->checkState() != Qt::Checked)
		ieds_row++;

	QTableWidgetItem * item;
	QString name;
	QString desc;
	item = ui.tableWidget_ieds->item(ieds_row,TABLE_IEDS_NAME);
	if (item)
		name = item->text().trimmed();
	item = ui.tableWidget_ieds->item(ieds_row,TABLE_IEDS_DESC);
	if (item)
		desc = item->text().trimmed();

	int ret = QMessageBox::question(this,tr("ѯ��"),tr("��SCL�ļ��Ƿ������ļ�����"),tr("��"),tr("��"));
	if (ret == 0)
		UploadScdFile(substationNo,xmlFile.toStdString().data());	//MEMO: ���ӵ��ļ���ʷ�汾���� [2016-2-22 10:17 �ۿ���]

	ui.label_desc->setText(tr("���ڵ���ģ��..."));
	dbImportStart(substationNo,name,desc);
}

int	view_plugin_scl_import::fileLineNumber(QString fileName)
{
	int count = 0;

	QFile file(fileName);
	if(!file.open(QFile::ReadOnly | QFile::Text)) 
		return 0;

	QTextStream in(&file);
	while (!in.atEnd()) 
	{
		QString line = in.readLine();
		count++;
	}

	file.close();
	return count;
}

bool view_plugin_scl_import::isExistIed(int sub_no,QString name)
{
	bool		b = false;
	SString		sql;
	SRecordset	rs;

	sql = SString::toFormat("SELECT COUNT(*) FROM t_oe_ied WHERE sub_no=%d and mms_path='%s'",sub_no,name.toLatin1().data());
	int iRet = DB->Retrieve(sql,rs);
	if (iRet <= 0)
	{
		QMessageBox::critical(this,tr("Critical Message"),tr("Database operation failed.\n\nSQL: %1").arg(sql.data()));
		slotAbort();
	}
	else if (iRet > 0)
	{
		int count = rs.GetValue(0,0).toInt();
		if (count > 0)
			b = true;
	}

	return b;
}

bool view_plugin_scl_import::deleteIed(int sub_no,QString name,int * iedNo)
{
	SString		sql;
	SRecordset	rs;

	sql = SString::toFormat("SELECT ied_no FROM t_oe_ied WHERE sub_no=%d and mms_path='%s'",sub_no,name.toLatin1().data());
	int iRet = DB->Retrieve(sql,rs);
	if (iRet <= 0)
	{
		QMessageBox::critical(this,tr("Critical Message"),tr("Database operation failed.\n\nSQL: %1").arg(sql.data()));
		slotAbort();
		return false;
	}
	else if (iRet > 0)
		*iedNo = rs.GetValue(0,0).toInt();

	if(!deleteIedParam(*iedNo))
		return false;
	if(!deleteCpu(*iedNo))
		return false;
	if(!deleteGroup(*iedNo))
		return false;
	if(!deleteElementGeneral(*iedNo))
		return false;
	if(!deleteElementState(*iedNo))
		return false;
	if(!deleteElementControl(*iedNo))
		return false;
	if(!deleteRunDevice(*iedNo))
		return false;
	if(!deleteGooseCtrlBlock(*iedNo))
		return false;
	if(!deleteSmvCtrlBlock(*iedNo))
		return false;
	if(!deleteGooseAp(*iedNo))
		return false;
	if(!deleteSmvAp(*iedNo))
		return false;
	if(!deleteMmsAp(*iedNo))
		return false;
	if(!deleteElementInputExtref(*iedNo))
		return false;
	if(!deletePhysConn(*iedNo))
		return false;

	sql = SString::toFormat("DELETE FROM t_oe_account WHERE dev_id=%d and dev_cls=1",iedNo);
	DB->Execute(sql);

	sql = SString::toFormat("DELETE FROM t_oe_ied WHERE ied_no=%d",*iedNo);
	if (!DB->Execute(sql))
	{
		QMessageBox::critical(this,tr("������Ϣ"),tr("���ݿ����ʧ��.\n\nSQL: %1").arg(sql.data()));
		slotAbort();
		return false;
	}
	if (m_bMDB && !MDB->Execute(sql))
	{
		QMessageBox::critical(this,tr("������Ϣ"),tr("�ڴ�����ݿ����ʧ��.\n\nSQL: %1").arg(sql.data()));
		slotAbort();
		return false;
	}

	// 	sql = SString::toFormat("DELETE FROM t_na_sub_network WHERE sub_no=%d",sub_no);
	// 	if (!DB->Execute(sql))
	// 	{
	// 		QMessageBox::critical(this,tr("����"),tr("���ݿ����ʧ�ܡ�\n\nSQL: %1").arg(sql.data()));
	// 		slotAbort();
	// 		return false;
	// 	}

	return true;
}

bool view_plugin_scl_import::deleteIedParam(int iedNo)
{
	SString		sql;
	SRecordset	rs;

	sql = SString::toFormat("SELECT COUNT(*) FROM t_oe_ied_param WHERE ied_no=%d",iedNo);
	int iRet = DB->Retrieve(sql,rs);
	if (iRet <= 0)
	{
		QMessageBox::critical(this,tr("Critical Message"),tr("Database operation failed.\n\nSQL: %1").arg(sql.data()));
		slotAbort();
	}
	else if (iRet > 0)
	{
		int count = rs.GetValue(0,0).toInt();
		if (count == 0)
			return true;
	}

	sql = SString::toFormat("DELETE FROM t_oe_ied_param WHERE ied_no=%d",iedNo);
	if (!DB->Execute(sql))
	{
		QMessageBox::critical(this,tr("������Ϣ"),tr("���ݿ����ʧ��.\n\nSQL: %1").arg(sql.data()));
		slotAbort();
		return false;
	}
	if (m_bMDB && !MDB->Execute(sql))
	{
		QMessageBox::critical(this,tr("������Ϣ"),tr("�ڴ�����ݿ����ʧ��.\n\nSQL: %1").arg(sql.data()));
		slotAbort();
		return false;
	}

	return true;
}

bool view_plugin_scl_import::deleteCpu(int iedNo)
{
	SString		sql;
	SRecordset	rs;

	sql = SString::toFormat("SELECT COUNT(*) FROM t_oe_cpu WHERE ied_no=%d",iedNo);
	int iRet = DB->Retrieve(sql,rs);
	if (iRet <= 0)
	{
		QMessageBox::critical(this,tr("Critical Message"),tr("Database operation failed.\n\nSQL: %1").arg(sql.data()));
		slotAbort();
	}
	else if (iRet > 0)
	{
		int count = rs.GetValue(0,0).toInt();
		if (count == 0)
			return true;
	}

	sql = SString::toFormat("DELETE FROM t_oe_cpu WHERE ied_no=%d",iedNo);
	if (!DB->Execute(sql))
	{
		QMessageBox::critical(this,tr("������Ϣ"),tr("���ݿ����ʧ��.\n\nSQL: %1").arg(sql.data()));
		slotAbort();
		return false;
	}
	if (m_bMDB && !MDB->Execute(sql))
	{
		QMessageBox::critical(this,tr("������Ϣ"),tr("�ڴ�����ݿ����ʧ��.\n\nSQL: %1").arg(sql.data()));
		slotAbort();
		return false;
	}

	return true;
}

bool view_plugin_scl_import::deleteGroup(int iedNo)
{
	SString		sql;
	SRecordset	rs;

	sql = SString::toFormat("SELECT COUNT(*) FROM t_oe_group WHERE ied_no=%d",iedNo);
	int iRet = DB->Retrieve(sql,rs);
	if (iRet <= 0)
	{
		QMessageBox::critical(this,tr("Critical Message"),tr("Database operation failed.\n\nSQL: %1").arg(sql.data()));
		slotAbort();
	}
	else if (iRet > 0)
	{
		int count = rs.GetValue(0,0).toInt();
		if (count == 0)
			return true;
	}

	sql = SString::toFormat("DELETE FROM t_oe_group WHERE ied_no=%d",iedNo);
	if (!DB->Execute(sql))
	{
		QMessageBox::critical(this,tr("������Ϣ"),tr("���ݿ����ʧ��.\n\nSQL: %1").arg(sql.data()));
		slotAbort();
		return false;
	}
	if (m_bMDB && !MDB->Execute(sql))
	{
		QMessageBox::critical(this,tr("������Ϣ"),tr("�ڴ�����ݿ����ʧ��.\n\nSQL: %1").arg(sql.data()));
		slotAbort();
		return false;
	}

	return true;
}

bool view_plugin_scl_import::deleteElementGeneral(int iedNo)
{
	SString		sql;
	SRecordset	rs;

	sql = SString::toFormat("select count(*) from t_oe_element_general where ied_no=%d",iedNo);
	int iRet = DB->Retrieve(sql,rs);
	if (iRet <= 0)
	{
		QMessageBox::critical(this,tr("����"),tr("���ݿ����ʧ�ܡ�\n\nSQL: %1").arg(sql.data()));
		slotAbort();
	}
	else if (iRet > 0)
	{
		int count = rs.GetValue(0,0).toInt();
		if (count == 0)
			return true;
	}

	sql = SString::toFormat("delete from t_oe_element_general where ied_no=%d",iedNo);
	if (!DB->Execute(sql))
	{
		QMessageBox::critical(this,tr("����"),tr("���ݿ����ʧ�ܡ�\n\nSQL: %1").arg(sql.data()));
		slotAbort();
		return false;
	}

	if (m_bMDB && !MDB->Execute(sql))
	{
		QMessageBox::critical(this,tr("����"),tr("�ڴ�����ʧ�ܡ�\n\nSQL: %1").arg(sql.data()));
		slotAbort();
		return false;
	}

	return true;
}

bool view_plugin_scl_import::deleteElementState(int iedNo)
{
	SString		sql;
	SRecordset	rs;

	sql = SString::toFormat("select count(*) from t_oe_element_state where ied_no=%d",iedNo);
	int iRet = DB->Retrieve(sql,rs);
	if (iRet <= 0)
	{
		QMessageBox::critical(this,tr("����"),tr("���ݿ����ʧ�ܡ�\n\nSQL: %1").arg(sql.data()));
		slotAbort();
	}
	else if (iRet > 0)
	{
		int count = rs.GetValue(0,0).toInt();
		if (count == 0)
			return true;
	}

	sql = SString::toFormat("delete from t_oe_element_state where ied_no=%d",iedNo);
	if (!DB->Execute(sql))
	{
		QMessageBox::critical(this,tr("����"),tr("���ݿ����ʧ�ܡ�\n\nSQL: %1").arg(sql.data()));
		slotAbort();
		return false;
	}

	if (m_bMDB && !MDB->Execute(sql))
	{
		QMessageBox::critical(this,tr("����"),tr("�ڴ�����ʧ�ܡ�\n\nSQL: %1").arg(sql.data()));
		slotAbort();
		return false;
	}

	return true;
}

bool view_plugin_scl_import::deleteElementControl(int iedNo)
{
	SString		sql;
	SRecordset	rs;

	sql = SString::toFormat("SELECT COUNT(*) FROM t_oe_element_control WHERE ied_no=%d",iedNo);
	int iRet = DB->Retrieve(sql,rs);
	if (iRet <= 0)
	{
		QMessageBox::critical(this,tr("Critical Message"),tr("Database operation failed.\n\nSQL: %1").arg(sql.data()));
		slotAbort();
	}
	else if (iRet > 0)
	{
		int count = rs.GetValue(0,0).toInt();
		if (count == 0)
			return true;
	}

	sql = SString::toFormat("DELETE FROM t_oe_element_control WHERE ied_no=%d",iedNo);
	if (!DB->Execute(sql))
	{
		QMessageBox::critical(this,tr("������Ϣ"),tr("���ݿ����ʧ��.\n\nSQL: %1").arg(sql.data()));
		slotAbort();
		return false;
	}
	if (m_bMDB && !MDB->Execute(sql))
	{
		QMessageBox::critical(this,tr("������Ϣ"),tr("�ڴ�����ݿ����ʧ��.\n\nSQL: %1").arg(sql.data()));
		slotAbort();
		return false;
	}

	return true;
}

bool view_plugin_scl_import::deleteRunDevice(int iedNo)
{
	SString		sql;
	SRecordset	rs;

	sql = SString::toFormat("select count(*) from t_oe_run_device where ied_no=%d",iedNo);
	int iRet = DB->Retrieve(sql,rs);
	if (iRet <= 0)
	{
		QMessageBox::critical(this,tr("����"),tr("���ݿ����ʧ�ܡ�\n\nSQL: %1").arg(sql.data()));
		slotAbort();
	}
	else if (iRet > 0)
	{
		int count = rs.GetValue(0,0).toInt();
		if (count == 0)
			return true;
	}

	sql = SString::toFormat("delete from t_oe_run_device where ied_no=%d",iedNo);
	if (!DB->Execute(sql))
	{
		QMessageBox::critical(this,tr("����"),tr("���ݿ����ʧ�ܡ�\n\nSQL: %1").arg(sql.data()));
		slotAbort();
		return false;
	}
	if (m_bMDB && !MDB->Execute(sql))
	{
		QMessageBox::critical(this,tr("������Ϣ"),tr("�ڴ�����ݿ����ʧ��.\n\nSQL: %1").arg(sql.data()));
		slotAbort();
		return false;
	}

	return true;
}

bool view_plugin_scl_import::deleteGooseCtrlBlock(int iedNo)
{
	SString sql = SString::toFormat("delete from t_oe_goose_ctrl_block where ied_no=%d",iedNo);
	if (!DB->Execute(sql))
	{
		QMessageBox::critical(this,tr("����"),tr("���ݿ����ʧ�ܡ�\n\nSQL: %1").arg(sql.data()));
		slotAbort();
		return false;
	}
	if (m_bMDB && !MDB->Execute(sql))
	{
		QMessageBox::critical(this,tr("������Ϣ"),tr("�ڴ�����ݿ����ʧ��.\n\nSQL: %1").arg(sql.data()));
		slotAbort();
		return false;
	}

	return true;
}

bool view_plugin_scl_import::deleteSmvCtrlBlock(int iedNo)
{
	SString sql = SString::toFormat("delete from t_oe_smv_ctrl_block where ied_no=%d",iedNo);
	if (!DB->Execute(sql))
	{
		QMessageBox::critical(this,tr("����"),tr("���ݿ����ʧ�ܡ�\n\nSQL: %1").arg(sql.data()));
		slotAbort();
		return false;
	}
	if (m_bMDB && !MDB->Execute(sql))
	{
		QMessageBox::critical(this,tr("������Ϣ"),tr("�ڴ�����ݿ����ʧ��.\n\nSQL: %1").arg(sql.data()));
		slotAbort();
		return false;
	}

	return true;
}

bool view_plugin_scl_import::deleteGooseAp(int iedNo)
{
	SString sql = SString::toFormat("delete from t_oe_goose_ap where ied_no=%d",iedNo);
	if (!DB->Execute(sql))
	{
		QMessageBox::critical(this,tr("����"),tr("���ݿ����ʧ�ܡ�\n\nSQL: %1").arg(sql.data()));
		slotAbort();
		return false;
	}
	if (m_bMDB && !MDB->Execute(sql))
	{
		QMessageBox::critical(this,tr("������Ϣ"),tr("�ڴ�����ݿ����ʧ��.\n\nSQL: %1").arg(sql.data()));
		slotAbort();
		return false;
	}

	return true;
}

bool view_plugin_scl_import::deleteSmvAp(int iedNo)
{
	SString sql = SString::toFormat("delete from t_oe_smv_ap where ied_no=%d",iedNo);
	if (!DB->Execute(sql))
	{
		QMessageBox::critical(this,tr("����"),tr("���ݿ����ʧ�ܡ�\n\nSQL: %1").arg(sql.data()));
		slotAbort();
		return false;
	}
	if (m_bMDB && !MDB->Execute(sql))
	{
		QMessageBox::critical(this,tr("������Ϣ"),tr("�ڴ�����ݿ����ʧ��.\n\nSQL: %1").arg(sql.data()));
		slotAbort();
		return false;
	}

	return true;
}

bool view_plugin_scl_import::deleteMmsAp(int iedNo)
{
	SString sql = SString::toFormat("delete from t_oe_mms_ap where ied_no=%d",iedNo);
	if (!DB->Execute(sql))
	{
		QMessageBox::critical(this,tr("����"),tr("���ݿ����ʧ�ܡ�\n\nSQL: %1").arg(sql.data()));
		slotAbort();
		return false;
	}
	if (m_bMDB && !MDB->Execute(sql))
	{
		QMessageBox::critical(this,tr("������Ϣ"),tr("�ڴ�����ݿ����ʧ��.\n\nSQL: %1").arg(sql.data()));
		slotAbort();
		return false;
	}

	return true;
}

bool view_plugin_scl_import::deleteElementInputExtref(int iedNo)
{
	SString sql = SString::toFormat("delete from t_oe_element_input_extref where ied_no=%d",iedNo);
	if (!DB->Execute(sql))
	{
		QMessageBox::critical(this,tr("����"),tr("���ݿ����ʧ�ܡ�\n\nSQL: %1").arg(sql.data()));
		slotAbort();
		return false;
	}
	if (m_bMDB && !MDB->Execute(sql))
	{
		QMessageBox::critical(this,tr("������Ϣ"),tr("�ڴ�����ݿ����ʧ��.\n\nSQL: %1").arg(sql.data()));
		slotAbort();
		return false;
	}

	return true;
}

bool view_plugin_scl_import::deletePhysConn(int iedNo)
{
	SString sql = SString::toFormat("delete from t_oe_ied_physconn where ied_id=%d",iedNo);
	if (!DB->Execute(sql))
	{
		QMessageBox::critical(this,tr("����"),tr("���ݿ����ʧ�ܡ�\n\nSQL: %1").arg(sql.data()));
		slotAbort();
		return false;
	}
	if (m_bMDB && !MDB->Execute(sql))
	{
		QMessageBox::critical(this,tr("������Ϣ"),tr("�ڴ�����ݿ����ʧ��.\n\nSQL: %1").arg(sql.data()));
		slotAbort();
		return false;
	}

	return true;
}

bool view_plugin_scl_import::deleteSubNetwork(int sub_no)
{
	SString sql = SString::toFormat("delete from t_na_sub_network where sub_no=%d",sub_no);
	if (!DB->Execute(sql))
	{
		QMessageBox::critical(this,tr("����"),tr("���ݿ����ʧ�ܡ�\n\nSQL: %1").arg(sql.data()));
		slotAbort();
		return false;
	}
	if (m_bMDB && !MDB->Execute(sql))
	{
		QMessageBox::critical(this,tr("������Ϣ"),tr("�ڴ�����ݿ����ʧ��.\n\nSQL: %1").arg(sql.data()));
		slotAbort();
		return false;
	}

	return true;
}

bool view_plugin_scl_import::addGroupType(QString name,QString desc)
{
	XmlObject * object = document.at(0)->findChildDeep("IED","name",name);
	if (!object)
		return false;

	QList<XmlObject*> dataSets;
	dataSets = object->findChildrenDeep("DataSet");

	iedGroupType.clear();	//�����ͨ����ʱ�������DataSet��type
	QMap<QString,int> map;
	foreach (object, dataSets)
	{
		QString name = object->attrib("name");
		if (bSmartImport)	//���ܵ�������£��ȶ�DATASET
		{
			if (name.contains("dsAin") ||
				name.contains("dsDin") ||
				name.contains("dsAlarm") ||
				name.contains("dsWarning") ||
				name.contains("dsCommState") ||
				name.contains("dsParameter") ||
				name.contains("dsTripInfo") ||
				name.contains("dsRelayDin") ||
				name.contains("dsRelayEna") ||
				name.contains("dsRelayRec") ||
				name.contains("dsRelayAin") ||
				name.contains("dsSetting") ||
				name.contains("dsGOOSE") ||
				name.contains("dsSV") ||
				name.contains("dsLog"))
			{
				continue;
			}

			QMap<QString,int>::iterator iter = mapGroupType.find(name);
			if (iter != mapGroupType.end()) 
				continue;

			map.insert(name,GROUP_UNKNOWN);
		}
		else	//�����ܵ�������£�ȫ����map
		{
			if (name.contains("dsAin"))
				iedGroupType.insert(object,GROUP_ANALOGUE);
			else if (name.contains("dsDin"))
				iedGroupType.insert(object,GROUP_SWITCH);
			else if (name.contains("dsAlarm"))
				iedGroupType.insert(object,GROUP_EVENT);
			else if (name.contains("dsWarning"))
				iedGroupType.insert(object,GROUP_ALARM);
			else if (name.contains("dsCommState"))
				iedGroupType.insert(object,GROUP_SWITCH);
			else if (name.contains("dsParameter"))
				iedGroupType.insert(object,GROUP_SETTING);
			else if (name.contains("dsTripInfo"))
				iedGroupType.insert(object,GROUP_EVENT);
			else if (name.contains("dsRelayDin"))
				iedGroupType.insert(object,GROUP_SWITCH);
			else if (name.contains("dsRelayEna"))
				iedGroupType.insert(object,GROUP_SOFT_STRAP);
			else if (name.contains("dsRelayRec"))
				iedGroupType.insert(object,GROUP_RELAY_REC);
			else if (name.contains("dsRelayAin"))
				iedGroupType.insert(object,GROUP_ANALOGUE);
			else if (name.contains("dsSetting"))
				iedGroupType.insert(object,GROUP_SETTING);
			else if (name.contains("dsGOOSE"))
				iedGroupType.insert(object,GROUP_GOOSE);
			else if (name.contains("dsSV"))
				iedGroupType.insert(object,GROUP_SV);
			else if (name.contains("dsLog"))
				iedGroupType.insert(object,GROUP_LOG);
			else
				iedGroupType.insert(object,GROUP_UNKNOWN);	
		}
	}

	if (bSmartImport && !map.count())	//���ܵ���
		return true;

	DlgGroupType dlg(this);
	dlg.setDocument(document);
	dlg.setName(name);
	dlg.setDesc(desc);
	dlg.setMap(map);
	dlg.setSmartImport(bSmartImport);
	dlg.start();
	if (dlg.exec() == QDialog::Accepted)
	{
		if (bSmartImport)	//���ܵ���
		{
			map = dlg.getMap();
			QMap<QString,int>::const_iterator iter = map.begin();
			for (;iter != map.end(); iter++)
			{
				mapGroupType.insert(iter.key(),iter.value());	//����TYPE���������û�д�DataSet���ͣ�������
				if (!_insertMapGroupType(iter.key(),iter.value()))
					return false;
			}
		}
		else	//�����ܵ���
			iedGroupType = dlg.getIedMap();
	}
	else
		return false;

	return true;
}

void view_plugin_scl_import::loadMmsNetName()
{
	QList<XmlObject*> list = document.at(0)->findChildrenDeep("SubNetwork");
	foreach (XmlObject * object,list)
	{
		if (object->attrib("type") == "8-MMS" && object->attrib("name") == "MMS-A")
			mmsNetA_name = object->attrib("name");
		else if (object->attrib("type") == "8-MMS" && object->attrib("name") == "MMS-B")
			mmsNetB_name = object->attrib("name");
	}

	if (mmsNetA_name.isEmpty() || mmsNetB_name.isEmpty())
	{
		DlgSubNetwork dlg;
		dlg.setSubNetworks(list);
		dlg.start();
		if (dlg.exec() == QDialog::Accepted)
		{
			mmsNetA_name = dlg.getMmsAName();
			mmsNetB_name = dlg.getMmsBName();
		}
		else
			slotAbort();
	}
}

bool view_plugin_scl_import::deleteIedByDocument(int sub_no)
{
	int			i;
	SString		sql;
	SRecordset	rs;
	bool		bFind = false;
	QString		text;
	QList<int>	lstIedNo;
	QList<QString> lstIedMms;

	QList<XmlObject*> lstIed = document.at(0)->findChildren("IED");

	sql = SString::toFormat("select ied_no,name,mms_path from t_oe_ied WHERE sub_no=%d",sub_no);
	int iRet = DB->Retrieve(sql,rs);
	if (iRet < 0)
	{
		QMessageBox::critical(this,tr("Critical Message"),tr("Database operation failed.\n\nSQL: %1").arg(sql.data()));
		slotAbort();
	}
	else if (iRet > 0)
	{
		for (i = 0; i < iRet; i++)
		{
			bFind = false;
			QString ied_no = rs.GetValue(i,0).data();
			QString name = rs.GetValue(i,1).data();
			QString mms = rs.GetValue(i,2).data();
			foreach (XmlObject *obj, lstIed)
			{
				if(mms == obj->attrib("name"))
				{
					bFind = true;
					break;
				}
			}
			if (!bFind)
			{
				lstIedNo.append(ied_no.toInt());
				lstIedMms.append(mms);
				if (lstIedMms.count() < 10)
					text += ied_no+":"+name+"("+mms+")"+"\n";
				else if (lstIedMms.count() == 10)
					text += "...............\n";

			}
		}
	}

	if (lstIedMms.count() > 0)
	{
		int ret = QMessageBox::question(this,tr("��Ϣ"),tr("���ݿ�����վ��[%1]��װ�ò��ڵ����ļ��д��ڡ�\n\n%2\n�ܹ�[%3]̨װ�ã��Ƿ�ɾ��?")
			.arg(sub_no).arg(text).arg(lstIedMms.count()),tr("ɾ��"),tr("����"));
		if (ret == 0)
		{
			int count = lstIedMms.count();
			for (i = 0; i < count; i++)
			{
				int iedNo;
				QString name = lstIedMms.at(i);
				if(!deleteIed(sub_no,name,&iedNo))
					return false;
				//if(!deleteIedParam(iedNo))
				//	return false;
				//if(!deleteCpu(iedNo))
				//	return false;
				//if(!deleteGroup(iedNo))
				//	return false;
				//if(!deleteElementGeneral(iedNo))
				//	return false;
				//if(!deleteElementState(iedNo))
				//	return false;
				//if(!deleteElementControl(iedNo))
				//	return false;
			}
		}
	}

	return true;
}

bool view_plugin_scl_import::_oe_element_state_unknown_desc()
{
	SString		sql;
	SRecordset	rs_state;
	SRecordset	rs_ied;

	sql.sprintf("select ied_no,name,mms_path from t_oe_ied "
		"where sub_no=%d and model!='AGENT_MONI' and mms_path!='AGENT_MONI'",substationNo);
	int cnt_ied = DB->Retrieve(sql,rs_ied);
	if (cnt_ied < 0)
	{
		QMessageBox::critical(this,tr("����"),tr("���ݿ����ʧ��.\n\nSQL: %1").arg(sql.data()));
		return false;
	}
	else if (cnt_ied == 0)
		return true;

	sql.sprintf("select t_oe_element_state.ied_no,t_oe_element_state.cpu_no,"
		"t_oe_element_state.group_no,t_oe_element_state.entry,t_oe_element_state.name from t_oe_element_state,t_oe_ied "
		"where t_oe_ied.sub_no=%d and t_oe_ied.mms_path='AGENT_MONI' and t_oe_ied.ied_no=t_oe_element_state.ied_no",substationNo);
	int cnt_state = DB->Retrieve(sql,rs_state);
	if (cnt_state < 0)
	{
		QMessageBox::critical(this,tr("����"),tr("���ݿ����ʧ��.\n\nSQL: %1").arg(sql.data()));
		return false;
	}
	else if (cnt_state > 0)
	{
		for (int i = 0; i < cnt_state; i++)
		{
			int state_ied_no = rs_state.GetValue(i,0).toInt();
			int state_cpu_no = rs_state.GetValue(i,1).toInt();
			int state_group_no = rs_state.GetValue(i,2).toInt();
			int state_entry = rs_state.GetValue(i,3).toInt();
			QString state_name = rs_state.GetValue(i,4).data();
			for (int j = 0; j < cnt_ied; j++)
			{
				SString ied_no = rs_ied.GetValue(j,0);
				QString ied_name = rs_ied.GetValue(j,1).data();
				QString ied_mms = rs_ied.GetValue(j,2).data();
				if (state_name.contains(ied_name) || state_name.contains(ied_mms))
				{
					sql.sprintf("update t_oe_element_state set unknown_desc='%s' "
						"where ied_no=%d and cpu_no=%d and group_no=%d and entry=%d",
						ied_no.data(),state_ied_no,state_cpu_no,state_group_no,state_entry);
					bool ret = DB->Execute(sql);
					if (!ret)
					{
						QMessageBox::critical(this,tr("����"),tr("���ݿ����ʧ��.\n\nSQL: %1").arg(sql.data()));
						return false;
					}
					if (m_bMDB)
					{
						ret = MDB->Execute(sql);
						if (!ret)
						{
							QMessageBox::critical(this,tr("����"),tr("�ڴ����ݿ����ʧ��.\n\nSQL: %1").arg(sql.data()));
							return false;
						}
					}

					continue;
				}
			}
		}
	}

	return true;
}

bool view_plugin_scl_import::_insertMapGroupType(QString name,int type)
{
	SString		sql;
	SRecordset	rs;

	sql.sprintf("insert into t_oe_dataset_group_type (dataset_name,group_type) values ('%s',%d)",name.toLocal8Bit().data(),type);
	bool ret = DB->Execute(sql);
	if (!ret)
		QMessageBox::critical(this,tr("����"),tr("���ݿ����ʧ��.\n\nSQL: %1").arg(sql.data()));

	if (m_bMDB)
	{
		ret = MDB->Execute(sql);
		if (!ret)
			QMessageBox::critical(this,tr("����"),tr("�ڴ����ݿ����ʧ��.\n\nSQL: %1").arg(sql.data()));
	}

	return ret;
}
