#ifndef _MMS_CLIENT_H_
#define _MMS_CLIENT_H_

#include <map>
#include "SApplication.h"
#include "ssp_base.h"
#include "ssp_database.h"
#include "SMmsClient.h"
#include "SLog.h"

#define DATASET_SETTING		1
#define DATASET_RELAY_AIN	3		// 遥测量
#define DATASET_RELAY_DIN   4		// 开关量
#define DATASET_RELAY_ENA   5		// 压板


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
	//MEMO: 以下字段用于历史数据保存，避免针对突发数据再从数据库查询 [2017-6-1 13:45 邵凯田]
	int				fun;
	int				inf;
	int				type;
	int				level;
	int				val_type;
	int				st_sn;
	bool			data_dirty;//是否数据为脏
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

// 定值巡检结构体
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
	bool paramchk_enabled;		// 是否启用定值巡检功能
	bool analogChannelMonitorEnabled;	// 遥测通道监视是否使能
	int paramchk_type;			// 定值校核方式：0与参考值对比；1与上一次值对比
	SString paramchk_time;		// 定值巡检时间(0:60 60分钟周期请求；1:2,20 每天2点，20点召唤
	bool syncNetworkTopologyFileEnabled;	// 同步网络拓扑文件是否使能
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
	// 历史数据,保留但是不再继续添加,使用接口获取属性值
	bool m_isSupportReqWaveByTimePeriod;
	SString m_comtradePath;
	int m_RCDCpuNo;							// 获取RCD的CPU序号
	int lastCommunicationCheckInteval;		// 最后通信时间闭锁检查间隔
};

class Ied
{
public:
	// Description: Ied
	// Author:      qianchenglin
	// Date:        2016-01-01
	// Parameter:   @mmsClient[in]mms会话对象
	//              @iedNo[in]IED号
	//              @name[in]IED名称
	//              @iedName[in]IEDName
	// Return:      Return true if succeed. Otherwise return false
	Ied(MMSClient *mmsClient, int iedNo, SString name, SString iedName);
	virtual ~Ied();

	// Description: 初始化装置
	// Author:      qianchenglin
	// Date:        2016-01-01
	// Parameter:   void
	// Return:      Return true if succeed. Otherwise return false
	bool Init();

	// Description: 获取IED号
	// Author:      qianchenglin
	// Date:        2016-01-01
	// Parameter:   void
	// Return:      返回IED号
	int iedNo() { return m_iedNo; }

	// Description: 获取A网IP地址
	// Author:      qianchenglin
	// Date:        2016-01-01
	// Parameter:   void
	// Return:      返回A网IP地址
	SString ipA() { return m_ipA; }

	// Description: 获取B网IP地址
	// Author:      qianchenglin
	// Date:        2016-01-01
	// Parameter:   void
	// Return:      返回B网IP地址
	SString ipB() { return m_ipB; }

	// Description: 根据reference路径找到其对象
	// Author:      qianchenglin
	// Date:        2016-01-01
	// Parameter:   @reference[in]reference路径
	// Return:      返回reference对象，未找到返回NULL
	t_reference *findReference(std::string reference);

	// Description: 根据rptInfo对象找到ReportInfo
	// Author:      qianchenglin
	// Date:        2016-01-01
	// Parameter:   @rptInfo[in]rpt info对象
	// Return:      返回stuReportInfo对象，未找到返回NULL
	stuReportInfo *findReportInfo(stuSMmsReportInfo *rptInfo);

	//////////////////////////////////////////////////////////////////////////
	// 描    述:  取IEDNAME
	// 作    者:  邵凯田
	// 创建时间:  2017-7-11 13:41
	// 参数说明:  void
	// 返 回 值:  char* 
	//////////////////////////////////////////////////////////////////////////
	inline char* iedName(){return m_iedName.data();};

	// Description: 获取设备名称
	// Author:      qianchenglin
	// Date:        2018-04-27
	// Parameter:   void
	// Return:      返回设备名称
	SString getName() { return m_name; };

	// Description: 获取模拟量通道监视队列
	// Author:      qianchenglin
	// Date:        2018-01-22
	// Parameter:   @analogMonitorList[out]返回模拟量通道监视队列
	// Return:      返回stuReportInfo对象，未找到返回NULL
	void getAnalogMonitorList(std::vector<AnalogMonitorItem *> &analogMonitorList) { analogMonitorList = m_analogMonitorList; }

	// Description: 获取定值巡检检查队列
	// Author:      qianchenglin
	// Date:        2018-01-22
	// Parameter:   @analogMonitorList[out]返回模拟量通道监视队列
	// Return:      返回stuReportInfo对象，未找到返回NULL
	void getParamCheckList(std::vector<ParamCheckItem *> &paramCheckItem) { paramCheckItem = m_paramCheckList; }

	// 初始化模拟量通道
	bool InitElementAnalogMonitor(SPtrList<CSMmsDataset> *m_Datasets);

	// 初始化定值巡检
	bool InitElementParamCheck(SPtrList<CSMmsDataset> *m_Datasets, stuGlobalConfig *config);

	// 获取
	unsigned int lastParamCheckSoc() { return m_lastParamCheckSoc; }

	// 设置定值巡检时间
	void updateParamCheckSoc(unsigned int soc);

	// 获取定值巡检检查时间
	void getParamCheckTimed(std::vector<int> &paramCheckTimed) { paramCheckTimed = m_paramCheckTimed; }

	// 获取定值巡检检查间隔
	unsigned int getParamCheckInteval() { return m_paramCheckInteval; }
	
	// 是否到了定值巡检时刻
	bool isTimeParamCheck(unsigned int currentSoc);

	// 获取定值变化告警
	t_reference *getParamChangeRef() { return m_paramChangeAlarmRef; }

	// 获取ied配置信息
	IedParam *getIedParam() { return &m_iedParam; }

	// Description: 装置是否自动下载波形
	// Author:      qianchenglin
	// Date:        2016-01-01
	// Parameter:   void
	// Return:      自动下载返回true，否则返回false
	bool isAutoDownloadWave();

private:
	// Description: 初始化IED装置参数findref ffindf
	// Author:      qianchenglin
	// Date:        2016-01-01
	// Parameter:   void
	// Return:      Return true if succeed. Otherwise return false
	bool InitIedParam();

	// Description: 初始化IED 组信息
	// Author:      qianchenglin
	// Date:        2016-01-01
	// Parameter:   void
	// Return:      Return true if succeed. Otherwise return false
	bool InitGroup();

	// Description: 添加报告控制块
	// Author:      qianchenglin
	// Date:        2016-01-01
	// Parameter:   @rs[in]数据库数据值集
	//              @i[in]数据位置
	//              @iedNo[in]IED号
	//              @cpuNo[in]CPU号
	//              @groupNo[in]组号
	//              @iedName[in]IED名
	// Return:      Return true if succeed. Otherwise return false
	bool AddReportInfo(SRecordset &rs, int i, int iedNo, int cpuNo, int groupNo, SString iedName);

	// Description: 添加报告控制块
	// Author:      qianchenglin
	// Date:        2016-01-01
	// Parameter:   @iedNo[in]IED号
	//              @cpuNo[in]CPU号
	//              @groupNo[in]组号
	//              @rptInfo[in]RPT对象
	// Return:      Return true if succeed. Otherwise return false
	void AddCliReportInfo(int iedNo, int cpuNo, int groupNo, stuSMmsReportInfo *rptInfo);

	// Description: 获取LD
	// Author:      qianchenglin
	// Date:        2016-01-01
	// Parameter:   @cpuNo[in]CPU号
	//              @ldName[in]LD名
	// Return:      Return true if succeed. Otherwise return false
	bool GetLD(int cpuNo, SString &ldName);

	/********************************************  
	* @brief 初始化装置通用信息点
	* @author 宋康
	* @date 2015/12/24
	* @param int ied_no 装置号
	* @param int cpu_no 装置CPU号
	* @param int group_no 装置组号
	* @param CSMmsDataset * mmsDataset 组指针
	* @return 初始化成功或失败 -true 成功 -false 失败
	*********************************************/ 
	bool InitElementFromGeneral(int ied_no,int cpu_no,int group_no,CSMmsDataset *mmsDataset);

	/********************************************  
	* @brief 初始化装置状态信息点
	* @author 宋康
	* @date 2015/12/24
	* @param int ied_no 装置号
	* @param int cpu_no 装置CPU号
	* @param int group_no 装置组号
	* @param CSMmsDataset * mmsDataset 组指针
	* @return 初始化成功或失败 -true 成功 -false 失败
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
	int m_paramCheckInteval;			// 巡检时间间隔（分钟）
	std::vector<int> m_paramCheckTimed;	// 定值巡检定时时间（小时0<=x<24），如2点，8点，14点），队列为空时表示是间隔巡检					
	unsigned int m_lastParamCheckSoc;	// 最后定值巡检时间
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
	// Parameter:   @app[in]MMSCLTApp对象
	//              @nodeNo[in]运行节点号
	//              @subNo[in]子站号
	//              @subName[in]子站名
	// Return:      返回子站号
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

	// Description: 获取当前会话隶属子站号
	// Author:      qianchenglin
	// Date:        2016-01-01
	// Parameter:   void
	// Return:      返回子站号
	int getSubNo() { return m_subNo; }

	// Description: 是否周期上送生效
	// Author:      qianchenglin
	// Date:        2016-01-01
	// Parameter:   void
	// Return:      Return true if succeed. Otherwise return false
	bool isPeriod() { return m_isPeriod; }

	// Description: 获取报告控制块注册实例号
	// Author:      qianchenglin
	// Date:        2016-01-01
	// Parameter:   void
	// Return:      返回实例号 
	unsigned char reportId() { return m_reportId; } 

	// Description: 设置报告控制块注册实例号
	// Author:      qianchenglin
	// Date:        2016-01-01
	// Parameter:   @m_reportId[in]实例号
	// Return:      void
	void setReportID(unsigned char reportId) { m_reportId = reportId; }

	// Description: 设置是否周期上送生效
	// Author:      qianchenglin
	// Date:        2016-01-01
	// Parameter:   @isPeriod[in]是否周期
	// Return:      void
	void setIsPeriod(bool isPeriod) { m_isPeriod = isPeriod; }

	// Description: 设置全局配置参数
	// Author:      qianchenglin
	// Date:        2016-01-01
	// Parameter:   @globalConfig[in]配置对象
	// Return:      void
	void setGlobalConfig(stuGlobalConfig *globalConfig) { m_globalConfig = globalConfig; }

	// Description: 设置连接模式，子站模式、主站模式或默认
	// Author:      qianchenglin
	// Date:        2017-10-25
	// Parameter:   void
	// Return:      返回连接模式
	ClientMode clientMode() { return m_clientMode; }

	// Description: 初始化运行必要信息
	// Author:      qianchenglin
	// Date:        2016-01-01
	// Parameter:   void
	// Return:      Return true if succeed. Otherwise return false
	virtual bool Init() { return false; }

	// Description: 根据reference路径找到其对象
	// Author:      qianchenglin
	// Date:        2016-01-01
	// Parameter:   @reference[in]reference路径
	// Return:      返回reference对象，未找到返回NULL
	virtual t_reference *findReference(std::string reference) { return NULL; }

	// Description: 设置Entry ID
	// Author:      qianchenglin
	// Date:        2016-01-01
	// Parameter:   @rptInfo[in]报告信息结构
	//              @entryId[in]条目ID
	// Return:      void
	virtual void setEntryID(stuSMmsReportInfo *rptInfo, SString entryId) { }
	
	// Description: 检查模拟量通道，如果有监视中的模拟量通道将会下发定值召唤，然后对返回值进行对比
	// Author:      qianchenglin
	// Date:        2016-01-01
	// Parameter:   void
	// Return:      Return true if succeed. Otherwise return false
	virtual bool checkAnalogChannel();
	virtual bool checkAnalogChannel(int ied_no);

	// Description: 定值巡检
	// Author:      qianchenglin
	// Date:        2016-01-01
	// Parameter:   void
	// Return:      Return true if succeed. Otherwise return false
	virtual bool checkParam();

	// Description: 获取IED设备
	// Author:      qianchenglin
	// Date:        2016-01-01
	// Parameter:   @ied_no[in]IED设备号
	// Return:      Return true if succeed. Otherwise return false
	virtual Ied *getIed(int ied_no);

	std::vector<Ied *> getIeds();

protected:
	// Description: 设置通道通信状态
	// Author:      qianchenglin
	// Date:        2016-01-01
	// Parameter:   @state[in]0断开；1正常
	// Return:      Return true if succeed. Otherwise return false
	virtual bool SetChannelState(int state) { return false; }

	// Description: 设置通道A通信状态
	// Author:      qianchenglin
	// Date:        2016-01-01
	// Parameter:   @state[in]0断开；1正常
	// Return:      Return true if succeed. Otherwise return false
	virtual bool SetChannelStateA(int state) { return false; }

	// Description: 设置通道B通信状态
	// Author:      qianchenglin
	// Date:        2016-01-01
	// Parameter:   @state[in]0断开；1正常
	// Return:      Return true if succeed. Otherwise return false
	virtual bool SetChannelStateB(int state) { return false; }

	// Description: 系统日志中写入指定通信变位
	// Author:      qianchenglin
	// Date:        2016-01-01
	// Parameter:   @text[in]待写入文本
	//              @val[in]通信状态0断开；1正常
	// Return:      Return true if succeed. Otherwise return false
	bool dbSubStateSyslog(SString text, int val);

	// Description: 设置Entry ID
	// Author:      qianchenglin
	// Date:        2016-01-01
	// Parameter:   @reportInfo[in]报告信息结构
	//              @entryId[in]条目ID
	// Return:      void
	void SetEntryID(stuReportInfo *reportInfo, SString entryId);

	//////////////////////////////////////////////////////////////////////////
	// 描    述:  MMS突发报告处理的回调虚函数，使用完报告结构后需要通过delete释放报告指针
	//            可以在派生类的重载函数中同步处理，也可以将其加入报告队列进行缓存处理
	// 作    者:  邵凯田
	// 创建时间:  2015-10-9 9:14
	// 参数说明:  @pMmsRpt表示当前连接对应的突发报告内容
	// 返 回 值:  void
	//////////////////////////////////////////////////////////////////////////
	virtual void OnMmsReportProcess(stuSMmsReport * pMmsRpt);

	// Description: 触发未知的MMS报告,在收到服务端的报告但是本地未找到映射时触发
	// Author:      qianchenglin
	// Date:        2018-05-16
	// Parameter:   @pMmsRpt[in]报告对象
	//              @reference[in]
	// Return:      void
	virtual void OnMmsUnknownReport(stuSMmsReportItem *pMmsRptItem, SString reference) {};

	// Description: 每秒钟触发一次，由子类继承完成自定义任务
	// Author:      qianchenglin
	// Date:        2018-05-17
	// Parameter:   void
	// Return:      void
	virtual void OnTimerWork() {};

	virtual void OnProcessReport();

	//////////////////////////////////////////////////////////////////////////
	// 描    述:  使能报告回调，每一个报告无论使能成功或失败都会触发回调，子类可以继承实现报告总召唤等功能
	// 作    者:  钱程林
	// 创建时间:  2019-01-16 10:22
	// 参数说明:  rpt 当前使能的报告对象
	//           result 使能结果，true成功，false失败
	//           isLastRpt 是否是最后一个报告
	// 返 回 值:  void
	virtual void OnEnabledRpt(stuSMmsReportInfo *rpt, bool result, bool isLastRpt);

#ifdef C_SMMS_SIGNAL_THREAD
	//////////////////////////////////////////////////////////////////////////
	// 描    述:  每秒钟一次的回调，由应用层决定执行定时任务，如通过取服务端标识确定通信是否正常
	// 作    者:  邵凯田
	// 创建时间:  2017-5-26 10:30
	// 参数说明:  void
	// 返 回 值:  void
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
	bool ReadValueSetting(int ied_no,int cpu_no,int group_no,int timeOut=10);	// 读取模拟量，压板，开关量信息

	// 
	bool GetLatestModelFile(int ied_no, ModelFileFormat fmt, SString &localFile, int timeOut=10);

	// 获取数据存储目录
	SString GetIedDataDir(int ied_no);

	// Description: 定值巡检后触发
	// Author:      qianchenglin
	// Date:        2018-05-17
	// Parameter:   @isManual[in]true是否是手动巡检，false为自动巡检
	//              @isDiff[in]是否有定值不一致
	// Return:      void
	virtual void onParamTimerChecked(bool isManual, bool isDiff) {}

	// 获取模型文件类型名
	static const char *getModelFileFormatName(ModelFileFormat fmt);

protected:
	int			m_nodeNo;		// 运行节点号
	int			m_subNo;		// 子站号
	SString		m_subName;		// 子站名称
	unsigned char m_reportId;	// 报告控制块注册实例号
	ClientMode m_clientMode;	// 客户端运行模式
	std::vector<Ied *> m_ieds;	// 模块下所包含的设备，子站运行模式只有一个IED，主站模式包含多个IED
	stuGlobalConfig *m_globalConfig;

private:
	MMSCLTApp	*mms_app;	//应用指针
	bool		m_bQuit;	//是否准备退出服务
	bool		m_isPeriod;	// 周期上送是否数据生效

	int			netA_state;	//A网状态
	int			netB_state;	//B网状态
	int			m_iThreads;	//当前运行的线程数量
	int			m_settingChangeTime;//定值变化计时

// 	SPtrList<int> m_lstIedNo; //数据库中对应的装置编号队列
// 	SPtrList<SString> m_lstIedName; //MMS Ied name队列
	SPtrList<stuSMmsReport> m_reports;	//突发回调数据存储空间
	//static void* ThreadReport(void* lp);//突发回调数据处理线程
	static void* ThreadBatchSqlTimeout(void* lp);//批量SQL超时检查线程，全局运行一次
#ifndef C_SMMS_SIGNAL_THREAD
	static void* ThreadTimer(void* lp);	//定时器线程
#else
	int m_iGetIdErrorTimes;//到标识错误次数
#endif
};

#endif	//_MMS_CLIENT_H_