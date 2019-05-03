/**
 *
 * �� �� �� : view_plugin_scl_import.h
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

#ifndef __VIEW_PLUGIN_SCL_IMPORT_H__
#define __VIEW_PLUGIN_SCL_IMPORT_H__

#include "cbaseview.h"
#include "sk_database.h"
#include "ui_view_plugin_scl_import.h"
#include "xmlobject.h"
#include "xmlparsethread.h"
#include "dbimportthread.h"
#include "inputsthread.h"

#ifdef _DEBUG
#include "vld.h"
#endif

static bool	m_bMDB;

typedef struct 
{
	int		type_no;
	int		dev_cls;
	QString	name;
	QString wildcard;
} device_type_t;

typedef struct  
{
	int		type_no;
	int		device_type_no;
	QString	name;
	QString	wildcard;
} element_type_t;

class view_plugin_scl_import : public CBaseView
{
	Q_OBJECT

	friend class InputsThread;

public:
	friend class oe_group;
	view_plugin_scl_import(QWidget *parent = 0);
	~view_plugin_scl_import();

	virtual void SetBackgroundColor(int red = 240, int yellow = 240, int blue = 240, int alpha = 230)
	{
		setStyleSheet(tr("QWidget#%1{background:rgb(%2,%3,%4,%5);}").arg(objectName()).arg(red).arg(yellow).arg(blue).arg(alpha));
	}

	//////////////////////////////////////////////////////////////////////////
	// ��    ��:  һ�������麯���������������غ����������Ӧ
	// ��    ��:  �ۿ���
	// ����ʱ��:  2017-08-11 11:27
	// ����˵��:  @sCmd��ʾ�����ַ�������ʽ����Ҫ����Ӧ��Լ��
	//         :  @sResutl��ʾ����ִ�н���ַ�������ʽҲ����Ҫ��
	// �� �� ֵ:  int����Ӧ��Լ��
	//////////////////////////////////////////////////////////////////////////
	virtual int OnCommand(SString sCmd,SString &sResult);

	QMap<QString,int>		getMapGroupType() { return mapGroupType; }
	QMap<XmlObject*,int>	getIedGroupType() { return iedGroupType; }
	bool					getBSmartImport() { return bSmartImport; }
	int						getSubstationNo() { return substationNo; }

public:
	XmlParseThread			*parseThread;
	DbImportThread			*importThread;
	InputsThread			*inputsThread;
	QString					xmlFile;
	QList<XmlObject*>		document;
	bool					bProgressDialogStart;
	int						progressValue;
	int						progressMaxValue;
	int						progressBarValue;
	int						progressBarMaxValue;
	QString					parseError;
	QString					parseWarn;
	QMap<QString,int>		mapGroupType;	
	QMap<XmlObject*,int>	iedGroupType;
	QString					mmsNetA_name;
	QString					mmsNetB_name;
	QList<device_type_t>	deviceTypeList;
	QList<element_type_t>	elementTypeList;

private:
	Ui::view_plugin_scl_import ui;

	QTimer				*progressDialogTimer;
	QProgressDialog		*progressDialog;
	int					ieds_row;
	bool				bReImport;		//�ж��������Ƿ��Զ�ȫ������
	bool				bSmartImport;	//�ж�����������ܵ���
	int					substationNo;

private:
	void init();
	void initThread();
	void initSlots();
	bool openFile(QString fileName);
	int	 fileLineNumber(QString fileName);
	void xmlParseStart();
	void dbImportStart(int sub_no,QString name,QString desc);
	void dbSmartImportStart(int sub_no,QString name,QString desc);
	void inputsStart();
	bool dbImport(QString iedName,QString & error);
	bool isExistIed(int sub_no,QString name);
	bool deleteIed(int sub_no,QString name,int * iedNo);
	bool deleteIedParam(int iedNo);
	bool deleteCpu(int iedNo);
	bool deleteGroup(int iedNo);
	bool deleteElementGeneral(int iedNo);
	bool deleteElementState(int iedNo);
	bool deleteElementControl(int iedNo);
	bool deleteRunDevice(int iedNo);
	bool deleteGooseCtrlBlock(int iedNo);
	bool deleteSmvCtrlBlock(int iedNo);
	bool deleteGooseAp(int iedNo);
	bool deleteSmvAp(int iedNo);
	bool deleteMmsAp(int iedNo);
	bool deleteElementInputExtref(int iedNo);
	bool deletePhysConn(int iedNo);
	bool addGroupType(QString name,QString desc);
	void loadMmsNetName();
	bool loadDeviceType();
	bool loadElementType();
	bool loadGroupType();
	bool deleteIedByDocument(int sub_no);
	bool deleteSubNetwork(int sub_no);
	bool _oe_element_state_unknown_desc();
	bool _insertMapGroupType(QString name,int type);

	//////////////////////////////////////////////////////////////////////////
	// ��    ��:  �ϴ�һ���°汾��SCD�ļ���ָ����վ
	// ��    ��:  �ۿ���
	// ����ʱ��:  2016-2-23 8:32
	// ����˵��:  @sub_no��ʾ��վ���
	//         :  @sLocalScdPathFile��ʾ���ص�SCD�ļ�ȫ·��
	// �� �� ֵ:  true��ʾ�ϴ��ɹ���false��ʾ�ϴ�ʧ��
	//////////////////////////////////////////////////////////////////////////
	bool UploadScdFile(int sub_no,SString sLocalScdPathFile);

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
	virtual bool ProcessAgentMsg(WORD wMsgType,stuSpUnitAgentMsgHead *pMsgHead,SString &sHeadStr,BYTE* pBuffer=NULL,int iLength=0);

signals:
	void sigProgressValue(int);
	void reportImportComleteFinish(bool bFlag);
	public slots:
		void slotParseFile();
		void slotXmlParseThreadFinished();
		void slotDbImportThreadFinished();
		void slotInputsThreadFinished();
		void slotProgressDialogTimeOut();
		void slotS1Changed(int);
		void slotG1Changed(int);
		void slotM1Changed(int);
		void slotAbort();;
		void slotSmartImport();
		void slotImport();
		void onProgressValue(int value);
};

#endif // __VIEW_PLUGIN_SCL_IMPORT_H__

