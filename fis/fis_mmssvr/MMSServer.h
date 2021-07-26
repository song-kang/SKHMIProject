#pragma once

//#include <glbtypes.h>
#include <map>
#include <vector>
#include <string>
#include "SApplication.h"
#include "ssp_base.h"
#include "ssp_database.h"
#include "SMmsServer.h"
#include "SLog.h"

#define DATASET_SETTING		1

#define SETTING_CHANGE_GROUP		1

#define CHANNEL_A_STATE				1
#define CHANNEL_B_STATE				2
#define CHANNEL_C_STATE				3
#define CHANNEL_D_STATE				4
#define CHANNEL_STATE				5
#define SETTING_CHANGE_ELEMENT		6

#define MMSCLT_TIMEOUT				30

#ifndef DWORD
# define DWORD uint32_t
#endif

#ifndef WORD
# define WORD uint16_t
#endif

#ifndef BYTE
# define BYTE uint8_t
#endif

enum
{
	OPER_NULL = 0,
	OPER_READ_CURRENT_AREA,
	OPER_READ_EDIT_AREA,
	OPER_WRITE_CURRENT_AREA,
	OPER_WRITE_EDIT_AREA,
	OPER_READ_CURRENT_AREA_SETTING,
	OPER_READ_EDIT_AREA_SETTING,
	OPER_WRITE_EDIT_AREA_SETTING,
	OPER_CURE_SETTING,
	OPER_CONTROL_SELECT,
	OPER_CONTROL_EXECUTE,
	OPER_CONTROL_CANCEL,
	OPER_CONTROL_DIRECT,
	OPER_CONTROL_RSLED,
	OPER_CONTROL_RESET
};


struct stuSMmsReportInfo;

typedef struct
{
	SString			tableName;
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
} t_reference;

struct stuReportInfo
{
	int	ied_no;
	int cpu_no;
	int group_no;
	stuSMmsReportInfo* sMmsReportInfo;
};

struct t_oe_element_general;
struct t_oe_element_state;
struct t_na_comm_device_port;
class MMSSVRApp;

class DoNode;
struct DaNode
{
	DaNode()
	{
		leaf = NULL;
		parent = NULL;
		valType = 0;
	}
	DaNode(DoNode *p)
	{
		leaf = NULL;
		parent = p;
		valType = 0;
	}

	std::string path;
	
	stuLeafMap *leaf;
	DoNode *parent;
	int valType;
	std::string value;
};

// Do包含多个Da
class DoNode
{
public:
	DoNode(std::string path);
	~DoNode();

	void clear();
	DaNode *findDa(std::string name, bool fullpath = true);
	std::string path() { return m_path; }
	void addDa(DaNode *da);

	void getDaList(std::map<std::string, DaNode *> &daList);

protected:
	std::string m_path;						  // DO节点路径
	std::map<std::string, DaNode *> m_daList; // DA节点名与节点对象关系

public:
	int cpuno;
	int group;
	int entry;
	int groupType;
	int controlType;
	std::string name;
};

class RunNode
{
public:
	bool init(int nodeNo);
	bool findDevice(int iedNo);
	std::vector<int> &getRunDevices();
	int getNodeNo();
	int getNodeId();

protected:
	int m_nodeNo;
	SString m_name;
	int m_nodeId;		// eUK9010AppNo 定义值
	std::vector<int> m_runDevices;	// 节点下运行设备IED编号
};

class Ied
{
public:
	int iedNo();

	// 根据IED编号初始化对象
	// 成功返回true，失败返回false
	bool init(int ied_no);

	DoNode *findDo(std::string ref);
	DaNode *findDa(std::string ref);
	
	void getAllDa(std::vector<DaNode *> &daList);

protected:
	//bool _init(int ied_no, std::map<std::string, DoNode *> &mapObj, SString table);
	bool initGeneral(int iedNo);
	bool initState(int iedNo);
	bool initControl(int iedNo);

protected:
	int m_iedNo;
// 	std::map<std::string, Reference> m_mapGeneral;
// 	std::map<std::string, Reference> m_mapState;
// 	std::map<std::string, Reference> m_mapControl;
// 	std::map<std::string, std::string> m_mapTime;
	std::map<std::string, DoNode *> m_mapDo;
	std::map<std::string, DaNode *> m_mapDa;	// 用于快速查找，内存申请释放不处理
};

class MMSServer : public SMmsServer
{
public:
	enum FC
	{
		MX,
		ST,
		SG,
		SP,
	};

	typedef struct
	{
		int		command;
		int		sub_no;
		int		ied_no;
		int		cpu_no;
		int		group_no;
		int		entry;
		int		sector;
		int		valType;
		std::string domName;
		std::string varName;
		std::string value;
		bool result;
		time_t time;
		int command_no;
	} oper_command_t;

	typedef struct  
	{
		unsigned short	msgType;
		unsigned char	buffer[1024];
		int		length;
		stuSpUnitAgentMsgHead head;
	} agent_msg_t;

	static const int kCtrlTimeout = 5000;	// unit ms

public:
	MMSServer(MMSSVRApp *, int stationNo, SString name, SString iedName = "");
	virtual ~MMSServer(void);

	virtual bool Start();
	virtual bool Stop();

	void setStationInfo(int stationNo, SString stationName);

	// 设置模型文件
	void setSclFile(SString file);

	// Description: 根据MMS路径快速查找stuLeafMap对象
	// Author:      qianchenglin
	// Date:        2016-01-01
	// Parameter:   @mmspath[in]mmspath(IED+LD/path)
	// Return:      找到返回对象，否则返回NULL
	stuLeafMap *FindLeafMap(SString mmspath);

	// Description: 是否启用模拟量更新Report
	// Author:      qianchenglin
	// Date:        2016-01-01
	// Parameter:   void
	// Return:      启用返回true，否则返回false
	bool IsEnableAnalogReport() { return m_enableAnalogReport; }

	//////////////////////////////////////////////////////////////////////////
	// 描    述:  取服务端文件目录内容
	// 作    者:  邵凯田
	// 创建时间:  2016-09-02 10:04
	// 参数说明:  @sPath为目录名称
	//         :  @slFiles这目录内容
	//         :  @bNeedToSort表示是否需要排序，true排序，false不排序
	// 返 回 值:  >0表示内容数量，<0表示失败, =0表示内容为空
	//////////////////////////////////////////////////////////////////////////
	virtual int OnServerDirectory(SString sPath,SPtrList<stuSMmsFileInfo> &slFiles,bool bNeedToSort=false);

	//////////////////////////////////////////////////////////////////////////
	// 描    述:  读取服务端文件
	// 作    者:  邵凯田
	// 创建时间:  2016-09-02 10:10
	// 参数说明:  @sFileName表示服务端文件名称
	// 返 回 值:  CMmsMemFile*, 表示加载到内存的文件内容，NULL表示文件打开或读取失败
	//////////////////////////////////////////////////////////////////////////
	virtual CMmsMemFile* OnReadServerFile(SString sFileName);

	//////////////////////////////////////////////////////////////////////////
	// 描    述:  写入服务端文件成功后的回调
	// 作    者:  邵凯田
	// 创建时间:  2016-09-02 10:12
	// 参数说明:  @sFileName表示服务端文件名称
	//         :  @pBuf表示文件内容缓冲区
	//         :  @iLen表示内容长度（字节）
	// 返 回 值:  >0表示写入成功，=0表示写入0，<0表示失败
	//////////////////////////////////////////////////////////////////////////
	virtual int OnWriteServerFile(SString sFileName);

	//////////////////////////////////////////////////////////////////////////
	// 描    述:  写叶子节点的值
	// 作    者:  邵凯田
	// 创建时间:  2016-8-29 18:36
	// 参数说明:  @sIedLdName表示IED+LD名称
	//         :  @sLeafPath表示叶子节点路径
	//         :  @pLeafMap表示叶子节点的用户映射指针,NULL表示未映射
	//         :  @mvluWrVaCtrl表示读取变量控制指针，实际类型为MVLU_WR_VA_CTRL*， 但该类型不需要用户层解析，用于存储数据使用
	// 返 回 值:  true/false, true表示写成功，false表示写失败
	//////////////////////////////////////////////////////////////////////////
	virtual bool OnWriteLeafValue(const char* sIedLdName, const char* sLeafPath, stuLeafMap *pLeafMap, void *mvluWrVaCtrl);

	virtual bool OnReturnSelect(char *ref, char *st, bool ret);
	virtual bool OnReturnOper(char *ref, char *st, bool ret);
	virtual bool OnReturnCancel(char *ref, char *st, bool ret);
	bool OnReturnBase(char *ref, char *st, bool ret, int type, int &controlValue);

	// 处理代理消息
	virtual bool ProcessAgentMsg(unsigned short wMsgType,stuSpUnitAgentMsgHead *pMsgHead,SString &sHeadStr,unsigned char* pBuffer=NULL,int iLength=0);

	virtual bool OnReadLeafValue(const char* sIedLdName, const char* sLeafPath, stuLeafMap *pLeafMap, void *mvluRdVaCtrl, int reason = 0);

	bool getReqCommand(int type, oper_command_t &cmd, int command_no = -1);		// 获取命令，并从队列中移除
	void addRespCommand(oper_command_t cmd);

	// gengxinIED通信状态
	void updateIedComState(int iedNo, int comState);

	// Description: 状态表信息改变触发，可继承实现状态变化后的动作
	// Author:      qianchenglin
	// Date:        2018-04-20
	// Parameter:   @element_state[in]数据库中对应状态记录
	//              @soc[in]本地处理的时间
	//              @usec[in]本地处理的时间
	// Return:      成功返回true，否则返回false
	virtual bool ProcessStateChanged(t_oe_element_state *element_state, int soc, int usec);

	bool updateElementGeneralValue(t_oe_element_general *value, int soc, int usec);

	// Description: 定时处理任务，每秒触发
	// Author:      qianchenglin
	// Date:        2018-05-21
	// Parameter:   void
	// Return:      void
	virtual void OnTimerWork();

protected:
	// Description: 初始化运行参数
	// Author:      qianchenglin
	// Date:        2016-01-01
	// Parameter:   void
	// Return:      void
	void InitRunParam();

	// Description: 初始化节点值
	// Author:      qianchenglin
	// Date:        2016-01-01
	// Parameter:   void
	// Return:      void
	void InitLeafValue();

	// Description: 初始化IED通信状态
	// Author:      qianchenglin
	// Date:        2016-01-01
	// Parameter:   void
	// Return:      成功赋值返回true，否则返回false
	bool InitIedCommState();

	bool dbSubStateSyslog(SString text,int val);

	// Description: 映射节点
	// Author:      qianchenglin
	// Date:        2016-01-01
	// Parameter:   @sIedLdName[in]IED+LD名称
	//              @sLeafPath[in]mms路径
	//              @ppLeafMap[in]LeafMap对象
	// Return:      void
	virtual bool OnMapLeafToUser(char* sIedLdName, char* sLeafPath, stuLeafMap **ppLeafMap);

	//////////////////////////////////////////////////////////////////////////
	// 描    述:  程序启动读取CFG文件时触发调用，
	// 作    者:  钱程林
	// 创建时间:  2016-9-5 10:30
	// 参数说明:  @key属性
	//         : @value属性值
	// 返 回 值:  void
	//////////////////////////////////////////////////////////////////////////
	virtual void OnStartupCfgRead(SString key, SString value);

	//////////////////////////////////////////////////////////////////////////
	// 描    述:  解析SCD文件，未删除scl_info前会遍历DAI节点
	// 作    者:  钱程林
	// 创建时间:  2016-9-5 10:30
	// 参数说明:  @iedName IED名称
	// 返 回 值:  void
	//////////////////////////////////////////////////////////////////////////
	virtual void OnMapDaiToUser(const char *iedName, const char *inst, const char *varName, const char *flattened, const char *val, const char *sAddr);;

	// 获取程序运行所在单元号
	bool GetUnitAgentProcessId(int sub_no,int ied_no,stuSpUnitAgentProcessId *id);
	void startCommand();		// 发送前调用，会初始化变量
	void stopCommand();
	bool isProcessingCommand();
	void checkCommandStatus();

	// 读取当前定值区
	void ReadCurrentArea(int iedNo, int cpuNo, int groupNo, int entryNo);
	void ReadEditArea(int iedNo, int cpuNo, int groupNo, int entryNo);
	void WriteCurrentArea(int area);
	void WriteEditArea(int area);
	void ReadCurrentAreaSetting();		// 读取当前定值区定值
	void ReadEditAreaSetting();
	void WriteEditAreaSetting(int entry, const std::string &value);
	void CuteSetting();

	bool ReadCurrentArea(oper_command_t *command);
	bool ReadEditArea(oper_command_t *command);
	bool WriteCurrentArea(oper_command_t *command);
	bool WriteEditArea(oper_command_t *command);
	bool ReadCurrentAreaSetting(oper_command_t *command);
	bool ReadEditAreaSetting(oper_command_t *command);
	bool WriteEditAreaSetting(oper_command_t *command);
	bool CuteSetting(oper_command_t *command);

	//int GetReqTypeByGroupType(int groupType);

	bool waitForCommand(int cmdType, int commandNo, oper_command_t &resp, int timeout = 5000, bool peek = false);
	bool waitForCommand(int cmdType, int timeout = 5000);

	stuLeafMap *findValueLeafMapByRef(std::string ident);
	stuLeafMap *findTimeLeafMapByIdent(std::string ident);

	// Description: 装置是否处在检修盖
	// Author:      qianchenglin
	// Date:        2016-01-01
	// Parameter:   void
	// Return:      检修状态返回true，否则返回false
	bool IsDeviceOverhaul(int iedNo);

	// Description: 根据IEDLD名称获取装置编号
	// Author:      qianchenglin
	// Date:        2019-11-25
	// Parameter:   void
	// Return:      成功查找到装置返回>=1，-1表示失败
	int FindIedNoByIedLd(SString iedLd);

	// Description: 根据设备编号名称获取装置对象
	// Author:      qianchenglin
	// Date:        2019-11-25
	// Parameter:   void
	// Return:      成功装置对象，失败返回NULL
	Ied *FindIedByIedNo(int iedNo);

	// 创建请求命令序号
	int createReqCommandNo();

	bool isSupportCPlus11();

	bool controlSelect(oper_command_t *command, int controlValue);
	bool controlExecute(oper_command_t *command, int controlValue);
	bool controlCancel(oper_command_t *command, int controlValue);
	bool controlDirect(oper_command_t *command, int controlValue);
	bool controlBase(oper_command_t *command, int type, int controlValue);
	bool updateControlValue(int iedNo, SString doPath, int type, int st);

protected:
	MMSSVRApp	*m_mmsApp;	// 应用指针
	bool		m_bQuit;	// 是否准备退出服务
	int			m_subNo;	// 子站号
	SString		m_subName;	// 子站名称
	bool m_enableAnalogReport;

	SString m_fileServicePath;  // 文件服务路径
	std::map<std::string, SString> m_mapAddr;	// mmspath

	unsigned int gOperatorSn;
	int m_currentReqCommand;
	unsigned int m_commandStartTime;
	bool m_isCommandRespFinished;

	std::list<oper_command_t> m_reqCmdList;
	std::list<oper_command_t> m_respCmdList;
	SLock m_reqCmdListLock;
	SLock m_respCmdListLock;

	// IED、设备号等信息查找索引
	std::map<std::string, std::string> m_mapIdentReference;	// <ident, reference> 提供信息回调时，快速查找到对应的reference
	bool m_isSupportcplusplus11;		// 是否支持C++11标准
	int m_reqCommandNo;

	SString m_sclFile;
	bool m_isScdMode;			// true使用SCD模型，多IED方式；false使用cid，多LD方式
	SString m_iedName;			// 使用CID方式时，记录唯一的IEDName
	int m_lastControlType;		// 最后一次遥控类别，如OPER_CONTROL_SELECT
	std::map<std::string, int> m_mapIedLDName; // <IEDLDName, iedno>
	std::vector<RunNode *> m_nodeList;
	std::vector<Ied *> m_iedList;
	std::map<int, int> m_iedComState;			// <IEDNo, comm state>
	std::map<std::string, stuLeafMap *> m_mapMmspathLeafMap;    // 提供stuLeafMap的快速检索，mmspath(IED+LD/path) <-> stuLeafMap *
	std::map<int, int> m_readIedLeafStatus;
	std::map<int, long> m_readIedLeafTime;
};