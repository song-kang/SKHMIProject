/**
 *
 * 文 件 名 : view_plugin_scl_import.h
 * 创建日期 : 2017-08-10 09:50
 * 作    者 : SspAssist(skt001@163.com)
 * 修改日期 : $Date: $
 * 当前版本 : $Revision: $
 * 功能描述 : SCD,ICD,CID文件导入
 * 修改记录 : 
 *            $Log: $
 *
 * Ver  Date        Author    Comments
 * ---  ----------  --------  -------------------------------------------
 * 001	 2017-08-10	SspAssist　创建文件
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
	// 描    述:  一般命令虚函数，由派生类重载后进行命令响应
	// 作    者:  邵凯田
	// 创建时间:  2017-08-11 11:27
	// 参数说明:  @sCmd表示命令字符串，格式不作要求，由应用约定
	//         :  @sResutl表示命令执行结果字符串，格式也不作要求
	// 返 回 值:  int，由应用约定
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
	bool				bReImport;		//判断智能中是否自动全部导入
	bool				bSmartImport;	//判断智能与非智能导入
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
	// 描    述:  上传一个新版本的SCD文件到指定厂站
	// 作    者:  邵凯田
	// 创建时间:  2016-2-23 8:32
	// 参数说明:  @sub_no表示厂站编号
	//         :  @sLocalScdPathFile表示本地的SCD文件全路径
	// 返 回 值:  true表示上传成功，false表示上传失败
	//////////////////////////////////////////////////////////////////////////
	bool UploadScdFile(int sub_no,SString sLocalScdPathFile);

	////////////////////////////////////////////////////////////////////////
	// 描    述:  代理消息处理接口，由派生类实现，处理函数必须尽量短小，快速返回，后台线程驱动，函数中不能调用任何界面元素
	// 作    者:  邵凯田
	// 创建时间:  2016-2-3 15:20
	// 参数说明:  @wMsgType表示消息类型
	//         :  @pMsgHead为消息头
	//         :  @sHeadStr消息头字符串
	//         :  @pBuffer为消息携带的二进制数据内容，NULL表示无二进制数据
	//         :  @iLength为二进制数据长度
	// 返 回 值:  true表示处理成功，false表示处理失败
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

