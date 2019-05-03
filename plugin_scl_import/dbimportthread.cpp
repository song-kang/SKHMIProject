#include "dbimportthread.h"
#include "view_plugin_scl_import.h"
#include "oe_ied.h"
#include "oe_cpu.h"
#include "oe_ied_model.h"
#include "oe_ied_model_version.h"
#include "oe_ied_param.h"
#include "na_sub_network.h"
#include "cim_substation.h"
#include "oe_run_device.h"
#include "oe_mms_ap.h"
#include "oe_element_input_extref.h"
#include "oe_ied_physconn.h"

DbImportThread::DbImportThread(QObject *parent)
	: QThread(parent)
{
	sclImport = (view_plugin_scl_import *)parent;
}

DbImportThread::~DbImportThread()
{

}

void DbImportThread::run()
{
	document = sclImport->document;
	error = QString::null;
	warnText = QString::null;

	bool b = dbImport(iedName,error,warnText);
	if (!b)
		error = QObject::tr("Import [%1-%2] error.\n\n%3").arg(iedName).arg(iedDesc).arg(error);

	sclImport->parseError = error;
	sclImport->parseWarn += warnText;
}

bool DbImportThread::dbImport(QString iedName,QString & error,QString &warnText)
{
	bool b;

	//cim_substation substaion(document);
	//b = substaion.execute(error);
	//if (!b)
	//	return b;

	oe_ied ied(sclImport,document,iedName,iedNo,substationNo);
	b = ied.execute(error);
	if (!b)
		return b;

	if (iedName.compare("OMS") == 0 || iedName.indexOf("UK90") >= 0)
	{
		// 不将子站自身节点加入到运行节点下
	}
	else
	{
		oe_run_device run_device(sclImport,document,&ied,substationNo);
		b = run_device.execute(error);
		if (!b)
			return b;
	}

	//oe_ied_model ied_model(document,iedName);
	//b = ied_model.execute(error);
	//if (!b)
	//	return b;

	oe_ied_model_version ied_model_version(document,iedName);
	b = ied_model_version.execute(error);
	if (!b)
		return b;

	oe_ied_param ied_param(document,&ied);
	ied_param.setMmsAName(sclImport->mmsNetA_name);
	ied_param.setMmsBName(sclImport->mmsNetB_name);
	b = ied_param.execute(error);
	if (!b)
		return b;

	oe_cpu cpu(sclImport,document,&ied);
	cpu.setCheckStateS1(checkStateS1);
	cpu.setCheckStateG1(checkStateG1);
	cpu.setCheckStateM1(checkStateM1);
	b = cpu.execute(error,warnText);
	if (!b)
		return b;

	if (DB_MASTER_TYPE == DB_MYSQL)
	{
		na_sub_network subnetwork(sclImport,document,&ied);
		b = subnetwork.execute(error);
		if (!b)
			return b;
	}

	oe_mms_ap mmsap(sclImport,document,&ied);
	b = mmsap.execute(error,warnText);
	if (!b)
		return b;

	oe_element_input_extref extref(sclImport,document,&ied);
	b = extref.execute(error,warnText);
	if (!b)
		return b;

	oe_ied_physconn physconn(sclImport,document,&ied);
	b = physconn.execute(error,warnText);
	if (!b)
		return b;

	return b;
}