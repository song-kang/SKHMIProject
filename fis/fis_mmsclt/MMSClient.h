#ifndef _MMS_CLIENT_H_
#define _MMS_CLIENT_H_

#include <map>
#include "SApplication.h"
#include "ssp_base.h"
#include "ssp_database.h"
#include "SMmsClient.h"
#include "SLog.h"

#define DATASET_SETTING		1
#define DATASET_RELAY_AIN	3		// ң����
#define DATASET_RELAY_DIN   4		// ������
#define DATASET_RELAY_ENA   5		// ѹ��


#define SETTING_CHANGE_GROUP		1

#define CHANNEL_A_STATE				1
#define CHANNEL_B_STATE				2
#define CHANNEL_C_STATE				3
#define CHANNEL_D_STATE				4
#define CHANNEL_STATE				5
#define SETTING_CHANGE_ELEMENT		6

#define MMSCLT_TIMEOUT				30

enum ModelFileFormat
{
	CID = 0,
	ICD = 1,
	SCD = 2,
	CCD = 3,
};

typedef struct
{
	SString			tableName;
	int				gen_sn;
	int				ied_no;
	int				cpu_no;
	int				group_no;
	int				entry;
	SString			desc;
	float			value;
	SString			strValue;
	float			threshold;
	int				smooth;
	SDateTime		dateTime;
	//MEMO: �����ֶ�������ʷ���ݱ��棬�������ͻ�������ٴ����ݿ��ѯ [2017-6-1 13:45 �ۿ���]
	int				fun;
	int				inf;
	int				type;
	int				level;
	int				val_type;
	int				st_sn;
	bool			data_dirty;//�Ƿ�����Ϊ��
} t_reference;

struct stuReportInfo
{
	int	ied_no;
	int cpu_no;
	int group_no;
	stuSMmsReportInfo* sMmsReportInfo;
};

struct AnalogMonitorItem
{
	int gen_sn;
	int ied_no;
	int cpu_no;
	int group_no;
	int entry;
	bool enabled;
	int inteval;
	float maxval;
	float minval;
	float mutate;
	unsigned int last_soc;
	SString mms_path;
	SString dsdataset;
	t_reference *reference;
};

// ��ֵѲ��ṹ��
struct ParamCheckItem
{
	int gen_sn;
	int ied_no;
	int cpu_no;
	int group_no;
	int entry;
	int valtype;
	SString reference_val;
	SString current_val;
	SString last_val;
	unsigned int last_soc;
	SString mms_path;
	SString dsdataset;
	t_reference *reference;
};

struct stuGlobalConfig
{
	bool paramchk_enabled;		// �Ƿ����ö�ֵѲ�칦��
	bool analogChannelMonitorEnabled;	// ң��ͨ�������Ƿ�ʹ��
	int paramchk_type;			// ��ֵУ�˷�ʽ��0��ο�ֵ�Աȣ�1����һ��ֵ�Ա�
	SString paramchk_time;		// ��ֵѲ��ʱ��(0:60 60������������1:2,20 ÿ��2�㣬20���ٻ�
	bool syncNetworkTopologyFileEnabled;	// ͬ�����������ļ��Ƿ�ʹ��
};

class MMSCLTApp;
class MMSClient;

class IedParam
{
public:
	IedParam();

	void setAttr(std::string key, std::string value);
	bool isContainAttr(std::string key);
	std::string getAttrValueString(std::string key);
	int getAttrValueInt(std::string key);
	float getAttrValueFloat(std::string key);

protected:
	std::map<std::string, std::string> m_mapAttrs;

public:
	// ��ʷ����,�������ǲ��ټ������,ʹ�ýӿڻ�ȡ����ֵ
	bool m_isSupportReqWaveByTimePeriod;
	SString m_comtradePath;
	int m_RCDCpuNo;							// ��ȡRCD��CPU���
	int lastCommunicationCheckInteval;		// ���ͨ��ʱ����������
};

class Ied
{
public:
	// Description: Ied
	// Author:      qianchenglin
	// Date:        2016-01-01
	// Parameter:   @mmsClient[in]mms�Ự����
	//              @iedNo[in]IED��
	//              @name[in]IED����
	//              @iedName[in]IEDName
	// Return:      Return true if succeed. Otherwise return false
	Ied(MMSClient *mmsClient, int iedNo, SString name, SString iedName);
	virtual ~Ied();

	// Description: ��ʼ��װ��
	// Author:      qianchenglin
	// Date:        2016-01-01
	// Parameter:   void
	// Return:      Return true if succeed. Otherwise return false
	bool Init();

	// Description: ��ȡIED��
	// Author:      qianchenglin
	// Date:        2016-01-01
	// Parameter:   void
	// Return:      ����IED��
	int iedNo() { return m_iedNo; }

	// Description: ��ȡA��IP��ַ
	// Author:      qianchenglin
	// Date:        2016-01-01
	// Parameter:   void
	// Return:      ����A��IP��ַ
	SString ipA() { return m_ipA; }

	// Description: ��ȡB��IP��ַ
	// Author:      qianchenglin
	// Date:        2016-01-01
	// Parameter:   void
	// Return:      ����B��IP��ַ
	SString ipB() { return m_ipB; }

	// Description: ����reference·���ҵ������
	// Author:      qianchenglin
	// Date:        2016-01-01
	// Parameter:   @reference[in]reference·��
	// Return:      ����reference����δ�ҵ�����NULL
	t_reference *findReference(std::string reference);

	// Description: ����rptInfo�����ҵ�ReportInfo
	// Author:      qianchenglin
	// Date:        2016-01-01
	// Parameter:   @rptInfo[in]rpt info����
	// Return:      ����stuReportInfo����δ�ҵ�����NULL
	stuReportInfo *findReportInfo(stuSMmsReportInfo *rptInfo);

	//////////////////////////////////////////////////////////////////////////
	// ��    ��:  ȡIEDNAME
	// ��    ��:  �ۿ���
	// ����ʱ��:  2017-7-11 13:41
	// ����˵��:  void
	// �� �� ֵ:  char* 
	//////////////////////////////////////////////////////////////////////////
	inline char* iedName(){return m_iedName.data();};

	// Description: ��ȡ�豸����
	// Author:      qianchenglin
	// Date:        2018-04-27
	// Parameter:   void
	// Return:      �����豸����
	SString getName() { return m_name; };

	// Description: ��ȡģ����ͨ�����Ӷ���
	// Author:      qianchenglin
	// Date:        2018-01-22
	// Parameter:   @analogMonitorList[out]����ģ����ͨ�����Ӷ���
	// Return:      ����stuReportInfo����δ�ҵ�����NULL
	void getAnalogMonitorList(std::vector<AnalogMonitorItem *> &analogMonitorList) { analogMonitorList = m_analogMonitorList; }

	// Description: ��ȡ��ֵѲ�������
	// Author:      qianchenglin
	// Date:        2018-01-22
	// Parameter:   @analogMonitorList[out]����ģ����ͨ�����Ӷ���
	// Return:      ����stuReportInfo����δ�ҵ�����NULL
	void getParamCheckList(std::vector<ParamCheckItem *> &paramCheckItem) { paramCheckItem = m_paramCheckList; }

	// ��ʼ��ģ����ͨ��
	bool InitElementAnalogMonitor(SPtrList<CSMmsDataset> *m_Datasets);

	// ��ʼ����ֵѲ��
	bool InitElementParamCheck(SPtrList<CSMmsDataset> *m_Datasets, stuGlobalConfig *config);

	// ��ȡ
	unsigned int lastParamCheckSoc() { return m_lastParamCheckSoc; }

	// ���ö�ֵѲ��ʱ��
	void updateParamCheckSoc(unsigned int soc);

	// ��ȡ��ֵѲ����ʱ��
	void getParamCheckTimed(std::vector<int> &paramCheckTimed) { paramCheckTimed = m_paramCheckTimed; }

	// ��ȡ��ֵѲ������
	unsigned int getParamCheckInteval() { return m_paramCheckInteval; }
	
	// �Ƿ��˶�ֵѲ��ʱ��
	bool isTimeParamCheck(unsigned int currentSoc);

	// ��ȡ��ֵ�仯�澯
	t_reference *getParamChangeRef() { return m_paramChangeAlarmRef; }

	// ��ȡied������Ϣ
	IedParam *getIedParam() { return &m_iedParam; }

	// Description: װ���Ƿ��Զ����ز���
	// Author:      qianchenglin
	// Date:        2016-01-01
	// Parameter:   void
	// Return:      �Զ����ط���true�����򷵻�false
	bool isAutoDownloadWave();

private:
	// Description: ��ʼ��IEDװ�ò���findref ffindf
	// Author:      qianchenglin
	// Date:        2016-01-01
	// Parameter:   void
	// Return:      Return true if succeed. Otherwise return false
	bool InitIedParam();

	// Description: ��ʼ��IED ����Ϣ
	// Author:      qianchenglin
	// Date:        2016-01-01
	// Parameter:   void
	// Return:      Return true if succeed. Otherwise return false
	bool InitGroup();

	// Description: ��ӱ�����ƿ�
	// Author:      qianchenglin
	// Date:        2016-01-01
	// Parameter:   @rs[in]���ݿ�����ֵ��
	//              @i[in]����λ��
	//              @iedNo[in]IED��
	//              @cpuNo[in]CPU��
	//              @groupNo[in]���
	//              @iedName[in]IED��
	// Return:      Return true if succeed. Otherwise return false
	bool AddReportInfo(SRecordset &rs, int i, int iedNo, int cpuNo, int groupNo, SString iedName);

	// Description: ��ӱ�����ƿ�
	// Author:      qianchenglin
	// Date:        2016-01-01
	// Parameter:   @iedNo[in]IED��
	//              @cpuNo[in]CPU��
	//              @groupNo[in]���
	//              @rptInfo[in]RPT����
	// Return:      Return true if succeed. Otherwise return false
	void AddCliReportInfo(int iedNo, int cpuNo, int groupNo, stuSMmsReportInfo *rptInfo);

	// Description: ��ȡLD
	// Author:      qianchenglin
	// Date:        2016-01-01
	// Parameter:   @cpuNo[in]CPU��
	//              @ldName[in]LD��
	// Return:      Return true if succeed. Otherwise return false
	bool GetLD(int cpuNo, SString &ldName);

	/********************************************  
	* @brief ��ʼ��װ��ͨ����Ϣ��
	* @author �ο�
	* @date 2015/12/24
	* @param int ied_no װ�ú�
	* @param int cpu_no װ��CPU��
	* @param int group_no װ�����
	* @param CSMmsDataset * mmsDataset ��ָ��
	* @return ��ʼ���ɹ���ʧ�� -true �ɹ� -false ʧ��
	*********************************************/ 
	bool InitElementFromGeneral(int ied_no,int cpu_no,int group_no,CSMmsDataset *mmsDataset);

	/********************************************  
	* @brief ��ʼ��װ��״̬��Ϣ��
	* @author �ο�
	* @date 2015/12/24
	* @param int ied_no װ�ú�
	* @param int cpu_no װ��CPU��
	* @param int group_no װ�����
	* @param CSMmsDataset * mmsDataset ��ָ��
	* @return ��ʼ���ɹ���ʧ�� -true �ɹ� -false ʧ��
	*********************************************/ 
	bool InitElementFromState(int ied_no,int cpu_no,int group_no,CSMmsDataset *mmsDataset);

	bool InitElementFromControl(int ied_no,int cpu_no,int group_no,CSMmsDataset *mmsDataset);

private:
	MMSClient *m_mmsClient;
	int m_iedNo;
	SString m_name;
	SString m_iedName;
	SString m_ipA;
	SString m_ipB;
	std::vector<stuReportInfo *> m_reportInfo;
	std::map<std::string, t_reference *> m_mapReference;
	std::vector<AnalogMonitorItem *> m_analogMonitorList;
	std::vector<ParamCheckItem *> m_paramCheckList;
	int m_paramCheckInteval;			// Ѳ��ʱ���������ӣ�
	std::vector<int> m_paramCheckTimed;	// ��ֵѲ�춨ʱʱ�䣨Сʱ0<=x<24������2�㣬8�㣬14�㣩������Ϊ��ʱ��ʾ�Ǽ��Ѳ��					
	unsigned int m_lastParamCheckSoc;	// ���ֵѲ��ʱ��
	t_reference *m_paramChangeAlarmRef;
	IedParam m_iedParam;
};

class MMSClient : public SMmsClient
{
public:
	enum ClientMode
	{
		DefaultMode,
		MainStationMode,
		SubStationMode,
	};

public:
	// Description: MMSClient
	// Author:      qianchenglin
	// Date:        2016-01-01
	// Parameter:   @app[in]MMSCLTApp����
	//              @nodeNo[in]���нڵ��
	//              @subNo[in]��վ��
	//              @subName[in]��վ��
	// Return:      ������վ��
	MMSClient(MMSCLTApp *app, int nodeNo, int subNo, SString subName);
	virtual ~MMSClient(void);

	virtual bool Start();
	virtual bool Stop();
	virtual void OnConnected();
	virtual void OnConnected_A();
	virtual void OnConnected_B();
	virtual void OnDisConnected();
	virtual void OnDisConnected_A();
	virtual void OnDisConnected_B();

	// Description: ��ȡ��ǰ�Ự������վ��
	// Author:      qianchenglin
	// Date:        2016-01-01
	// Parameter:   void
	// Return:      ������վ��
	int getSubNo() { return m_subNo; }

	// Description: �Ƿ�����������Ч
	// Author:      qianchenglin
	// Date:        2016-01-01
	// Parameter:   void
	// Return:      Return true if succeed. Otherwise return false
	bool isPeriod() { return m_isPeriod; }

	// Description: ��ȡ������ƿ�ע��ʵ����
	// Author:      qianchenglin
	// Date:        2016-01-01
	// Parameter:   void
	// Return:      ����ʵ���� 
	unsigned char reportId() { return m_reportId; } 

	// Description: ���ñ�����ƿ�ע��ʵ����
	// Author:      qianchenglin
	// Date:        2016-01-01
	// Parameter:   @m_reportId[in]ʵ����
	// Return:      void
	void setReportID(unsigned char reportId) { m_reportId = reportId; }

	// Description: �����Ƿ�����������Ч
	// Author:      qianchenglin
	// Date:        2016-01-01
	// Parameter:   @isPeriod[in]�Ƿ�����
	// Return:      void
	void setIsPeriod(bool isPeriod) { m_isPeriod = isPeriod; }

	// Description: ����ȫ�����ò���
	// Author:      qianchenglin
	// Date:        2016-01-01
	// Parameter:   @globalConfig[in]���ö���
	// Return:      void
	void setGlobalConfig(stuGlobalConfig *globalConfig) { m_globalConfig = globalConfig; }

	// Description: ��������ģʽ����վģʽ����վģʽ��Ĭ��
	// Author:      qianchenglin
	// Date:        2017-10-25
	// Parameter:   void
	// Return:      ��������ģʽ
	ClientMode clientMode() { return m_clientMode; }

	// Description: ��ʼ�����б�Ҫ��Ϣ
	// Author:      qianchenglin
	// Date:        2016-01-01
	// Parameter:   void
	// Return:      Return true if succeed. Otherwise return false
	virtual bool Init() { return false; }

	// Description: ����reference·���ҵ������
	// Author:      qianchenglin
	// Date:        2016-01-01
	// Parameter:   @reference[in]reference·��
	// Return:      ����reference����δ�ҵ�����NULL
	virtual t_reference *findReference(std::string reference) { return NULL; }

	// Description: ����Entry ID
	// Author:      qianchenglin
	// Date:        2016-01-01
	// Parameter:   @rptInfo[in]������Ϣ�ṹ
	//              @entryId[in]��ĿID
	// Return:      void
	virtual void setEntryID(stuSMmsReportInfo *rptInfo, SString entryId) { }
	
	// Description: ���ģ����ͨ��������м����е�ģ����ͨ�������·���ֵ�ٻ���Ȼ��Է���ֵ���жԱ�
	// Author:      qianchenglin
	// Date:        2016-01-01
	// Parameter:   void
	// Return:      Return true if succeed. Otherwise return false
	virtual bool checkAnalogChannel();
	virtual bool checkAnalogChannel(int ied_no);

	// Description: ��ֵѲ��
	// Author:      qianchenglin
	// Date:        2016-01-01
	// Parameter:   void
	// Return:      Return true if succeed. Otherwise return false
	virtual bool checkParam();

	// Description: ��ȡIED�豸
	// Author:      qianchenglin
	// Date:        2016-01-01
	// Parameter:   @ied_no[in]IED�豸��
	// Return:      Return true if succeed. Otherwise return false
	virtual Ied *getIed(int ied_no);

	std::vector<Ied *> getIeds();

protected:
	// Description: ����ͨ��ͨ��״̬
	// Author:      qianchenglin
	// Date:        2016-01-01
	// Parameter:   @state[in]0�Ͽ���1����
	// Return:      Return true if succeed. Otherwise return false
	virtual bool SetChannelState(int state) { return false; }

	// Description: ����ͨ��Aͨ��״̬
	// Author:      qianchenglin
	// Date:        2016-01-01
	// Parameter:   @state[in]0�Ͽ���1����
	// Return:      Return true if succeed. Otherwise return false
	virtual bool SetChannelStateA(int state) { return false; }

	// Description: ����ͨ��Bͨ��״̬
	// Author:      qianchenglin
	// Date:        2016-01-01
	// Parameter:   @state[in]0�Ͽ���1����
	// Return:      Return true if succeed. Otherwise return false
	virtual bool SetChannelStateB(int state) { return false; }

	// Description: ϵͳ��־��д��ָ��ͨ�ű�λ
	// Author:      qianchenglin
	// Date:        2016-01-01
	// Parameter:   @text[in]��д���ı�
	//              @val[in]ͨ��״̬0�Ͽ���1����
	// Return:      Return true if succeed. Otherwise return false
	bool dbSubStateSyslog(SString text, int val);

	// Description: ����Entry ID
	// Author:      qianchenglin
	// Date:        2016-01-01
	// Parameter:   @reportInfo[in]������Ϣ�ṹ
	//              @entryId[in]��ĿID
	// Return:      void
	void SetEntryID(stuReportInfo *reportInfo, SString entryId);

	//////////////////////////////////////////////////////////////////////////
	// ��    ��:  MMSͻ�����洦��Ļص��麯����ʹ���걨��ṹ����Ҫͨ��delete�ͷű���ָ��
	//            ����������������غ�����ͬ������Ҳ���Խ�����뱨����н��л��洦��
	// ��    ��:  �ۿ���
	// ����ʱ��:  2015-10-9 9:14
	// ����˵��:  @pMmsRpt��ʾ��ǰ���Ӷ�Ӧ��ͻ����������
	// �� �� ֵ:  void
	//////////////////////////////////////////////////////////////////////////
	virtual void OnMmsReportProcess(stuSMmsReport * pMmsRpt);

	// Description: ����δ֪��MMS����,���յ�����˵ı��浫�Ǳ���δ�ҵ�ӳ��ʱ����
	// Author:      qianchenglin
	// Date:        2018-05-16
	// Parameter:   @pMmsRpt[in]�������
	//              @reference[in]
	// Return:      void
	virtual void OnMmsUnknownReport(stuSMmsReportItem *pMmsRptItem, SString reference) {};

	// Description: ÿ���Ӵ���һ�Σ�������̳�����Զ�������
	// Author:      qianchenglin
	// Date:        2018-05-17
	// Parameter:   void
	// Return:      void
	virtual void OnTimerWork() {};

	virtual void OnProcessReport();

	//////////////////////////////////////////////////////////////////////////
	// ��    ��:  ʹ�ܱ���ص���ÿһ����������ʹ�ܳɹ���ʧ�ܶ��ᴥ���ص���������Լ̳�ʵ�ֱ������ٻ��ȹ���
	// ��    ��:  Ǯ����
	// ����ʱ��:  2019-01-16 10:22
	// ����˵��:  rpt ��ǰʹ�ܵı������
	//           result ʹ�ܽ����true�ɹ���falseʧ��
	//           isLastRpt �Ƿ������һ������
	// �� �� ֵ:  void
	virtual void OnEnabledRpt(stuSMmsReportInfo *rpt, bool result, bool isLastRpt);

#ifdef C_SMMS_SIGNAL_THREAD
	//////////////////////////////////////////////////////////////////////////
	// ��    ��:  ÿ����һ�εĻص�����Ӧ�ò����ִ�ж�ʱ������ͨ��ȡ����˱�ʶȷ��ͨ���Ƿ�����
	// ��    ��:  �ۿ���
	// ����ʱ��:  2017-5-26 10:30
	// ����˵��:  void
	// �� �� ֵ:  void
	//////////////////////////////////////////////////////////////////////////
	virtual void OnSecondTimer();
#endif
private:
	void OnMmsReportItemProcess(stuSMmsReportItem * pMmsRptItem);
	bool dbElementGeneral(t_reference *reference,SPtrList<MMSValue> &vList,SString reason);
	bool dbElementState(t_reference *reference,SPtrList<MMSValue> &vList,SString reason);
	bool dbElementStateDigital(t_reference *reference,int type,SPtrList<MMSValue> &vList,SString reason);
	bool dbElementStateDigitalHistory(t_reference *reference,int val,int soc,int usec,unsigned char tq,
		unsigned short q,SString reason);
	bool dbElementStateDigitalHistory(int st_sn,int ied_no,int cpu_no,int group_no,int entry,int val);
	bool dbSyslog(SString text,int log_type);
	bool dbElementStateMeasure(t_reference *reference,SPtrList<MMSValue> &vList,SString reason);
	bool _getElementStateValueType(t_reference *reference,int & val_type);
	void loopSetting();
	bool dbSettingValue(SPtrList<t_reference> &lstFc,SPtrList< SPtrList<MMSValue> > &lstValue,bool isEditArea=false);
	bool dbSettingChange(t_reference *reference,SString new_val);
	bool dbSettingChangeReset();
	bool dbCommStateChange(int ied_no, int state);

public:
	bool ResetLed(int iedNo);
	bool ResetIed(int iedNo);
	bool ControlDirect(int ied_no,int cpu_no,int group_no,int entry,int value,int timeOut=10);
	bool ControlSelect(int ied_no,int cpu_no,int group_no,int entry,int value,int timeOut=10);
	bool ControlExecute(int ied_no,int cpu_no,int group_no,int entry,int value,int timeOut=10);
	bool ControlCancel(int ied_no,int cpu_no,int group_no,int entry,int value,int timeOut=10);
	bool GetCurrentArea(int iedNo,int &area,int timeOut=10);
	bool GetEditArea(int iedNo,int &area,int timeOut=10);
	bool SetCurrentArea(int iedNo,int area,int timeOut=10);
	bool SetEditArea(int iedNo,int area,int timeOut=10);
	bool ReadCurrentAreaSetting(int ied_no,int cpu_no,int group_no,int timeOut=10);
	bool ReadEditAreaSetting(int ied_no,int cpu_no,int group_no,int timeOut=10);
	bool ReadAppointAreaSetting(int ied_no,int cpu_no,int group_no,int area,int timeOut=10);
	bool WriteSettingValue(int ied_no,int cpu_no,int group_no,int entry,int area,SString value,int timeOut=10);
	bool CureSettingValue(int ied_no,int cpu_no,int group_no,int timeOut=10);
	bool ReadLeafValue(SString dom, SString var, int &valType, SString &value, int timeOut=10);
	bool dbAnalogValue(SPtrList<t_reference> &lstFc,SPtrList< SPtrList<MMSValue> > &lstValue, int soc);
	bool dbParamCheckValue(std::vector<ParamCheckItem *> reqList, SPtrList<t_reference> &lstFc,SPtrList< SPtrList<MMSValue> > &lstValue, bool isManualRequest, bool &isParamDiff);
	bool ReadParamCheckValue(int ied_no, int cpu_no, int group_no, bool isManual = false, int timeOut=10);
	bool ReadValueSetting(int ied_no,int cpu_no,int group_no,int timeOut=10);	// ��ȡģ������ѹ�壬��������Ϣ

	// 
	bool GetLatestModelFile(int ied_no, ModelFileFormat fmt, SString &localFile, int timeOut=10);

	// ��ȡ���ݴ洢Ŀ¼
	SString GetIedDataDir(int ied_no);

	// Description: ��ֵѲ��󴥷�
	// Author:      qianchenglin
	// Date:        2018-05-17
	// Parameter:   @isManual[in]true�Ƿ����ֶ�Ѳ�죬falseΪ�Զ�Ѳ��
	//              @isDiff[in]�Ƿ��ж�ֵ��һ��
	// Return:      void
	virtual void onParamTimerChecked(bool isManual, bool isDiff) {}

	// ��ȡģ���ļ�������
	static const char *getModelFileFormatName(ModelFileFormat fmt);

protected:
	int			m_nodeNo;		// ���нڵ��
	int			m_subNo;		// ��վ��
	SString		m_subName;		// ��վ����
	unsigned char m_reportId;	// ������ƿ�ע��ʵ����
	ClientMode m_clientMode;	// �ͻ�������ģʽ
	std::vector<Ied *> m_ieds;	// ģ�������������豸����վ����ģʽֻ��һ��IED����վģʽ�������IED
	stuGlobalConfig *m_globalConfig;

private:
	MMSCLTApp	*mms_app;	//Ӧ��ָ��
	bool		m_bQuit;	//�Ƿ�׼���˳�����
	bool		m_isPeriod;	// ���������Ƿ�������Ч

	int			netA_state;	//A��״̬
	int			netB_state;	//B��״̬
	int			m_iThreads;	//��ǰ���е��߳�����
	int			m_settingChangeTime;//��ֵ�仯��ʱ

// 	SPtrList<int> m_lstIedNo; //���ݿ��ж�Ӧ��װ�ñ�Ŷ���
// 	SPtrList<SString> m_lstIedName; //MMS Ied name����
	SPtrList<stuSMmsReport> m_reports;	//ͻ���ص����ݴ洢�ռ�
	//static void* ThreadReport(void* lp);//ͻ���ص����ݴ����߳�
	static void* ThreadBatchSqlTimeout(void* lp);//����SQL��ʱ����̣߳�ȫ������һ��
#ifndef C_SMMS_SIGNAL_THREAD
	static void* ThreadTimer(void* lp);	//��ʱ���߳�
#else
	int m_iGetIdErrorTimes;//����ʶ�������
#endif
};

#endif	//_MMS_CLIENT_H_