#pragma once

/*
 南方电网智能录波器模型导出
 参照文档《南方电网智能录波器技术规范（送审稿）》
*/

#include "MMSClientSubStation.h"
#include "public/UK9010App.h"


class MMSClientSubStationNW : public MMSClientSubStation
{
public:
	MMSClientSubStationNW(MMSCLTApp *mmscltApp, int nodeNo, int substationNo, SString substationName, int iedNo, SString iedDesc, SString iedName);
	virtual ~MMSClientSubStationNW();

public:
	virtual void OnConnected();
	virtual void OnDisConnected();

public:
	// Description: 触发未知的MMS报告,在收到服务端的报告但是本地未找到映射时触发
	// Author:      qianchenglin
	// Date:        2018-05-16
	// Parameter:   @pMmsRpt[in]报告对象
	//              @reference[in]
	// Return:      void
	virtual void OnMmsUnknownReport(stuSMmsReportItem *pMmsRptItem, SString reference);

	// Description: 定值巡检后触发
	// Author:      qianchenglin
	// Date:        2018-05-17
	// Parameter:   @isManual[in]true是否是手动巡检，false为自动巡检
	//              @isDiff[in]是否有定值不一致
	// Return:      void
	virtual void onParamTimerChecked(bool isManual, bool isDiff);

	// Description: 每秒钟触发一次，由子类继承完成自定义任务
	// Author:      qianchenglin
	// Date:        2018-05-17
	// Parameter:   void
	// Return:      void
	virtual void OnTimerWork();

	//////////////////////////////////////////////////////////////////////////
	// 描    述:  MMS突发报告处理的回调虚函数，使用完报告结构后需要通过delete释放报告指针
	//            可以在派生类的重载函数中同步处理，也可以将其加入报告队列进行缓存处理
	// 作    者:  邵凯田
	// 创建时间:  2015-10-9 9:14
	// 参数说明:  @pMmsRpt表示当前连接对应的突发报告内容
	// 返 回 值:  void
	//////////////////////////////////////////////////////////////////////////
	virtual void OnMmsReportProcess(stuSMmsReport * pMmsRpt);
	
	//////////////////////////////////////////////////////////////////////////
	// 描    述:  有通信回复报文后触发
	// 作    者:  邵凯田
	// 创建时间:  2017-5-26 10:17
	// 参数说明:  void
	// 返 回 值:  NULl
	virtual void onRecvNewPacket(SDateTime dt);

	//////////////////////////////////////////////////////////////////////////
	// 描    述:  通信请求否定响应触发
	// 作    者:  邵凯田
	// 创建时间:  2017-5-26 10:17
	// 参数说明:  void
	// 返 回 值:  NULl
	virtual void OnRequestDenial();

	//////////////////////////////////////////////////////////////////////////
	// 描    述:  通信请求超时触发
	// 作    者:  邵凯田
	// 创建时间:  2017-5-26 10:17
	// 参数说明:  void
	// 返 回 值:  NULl
	virtual void OnRequestTimeout();

protected:
	bool checkModelUpdateStatus();
	bool checkModelFile(ModelFileFormat fmt, bool &isDiff);
	void checkLastCommunicationAlarmBlocked();				// 检查最后通信时间闭锁告警
	bool isModelFileSame(SString file1, SString file2);
	bool createModelReport(SString doName, int value, int &evt_seq, SString details = "");	// 创建模型报告并入库
	bool createModelUpdateReport(int &evt_seq);			// 创建模型文件更新报告并入库
	bool createModelAbnormalReport(int &evt_seq);		// 创建模型异常报告并入库
	bool createParamDiffReport(int &evt_seq);			// 创建定值不一致报告并入库
	bool createIedCommStateReport(int commState, int &evt_seq);			// 创建通信状态报告并入库


	// Description: 获取装置波形文件列表路径，如果通信参数配置了comtrade_path则优先使用该路径,
	//              其次如果配置了req_wave_time_period=1则表示装置支持时间范围召唤波形列表
	// Author:      qianchenglin
	// Date:        2018-05-17
	// Parameter:   @dtFrom[in]如果根据时间取文件列表，需要传入时间范围
	//              @dtTo[in]时间截止点
	// Return:      返回装置文件列表路径
	SString getComtradeFileListPath(SDateTime dtFrom, SDateTime dtTo);

	// Description: 波形文件是否存在列表中
	// Author:      qianchenglin
	// Date:        2018-05-17
	// Parameter:   @mapWaveList[in]key=根据传入的波形文件名，value=返回是否存在
	// Return:      void
	void isWaveFileInWaveList(std::map<std::string, bool> &mapWaveList);

	// Description: 将波形列表按要求排序
	// Author:      qianchenglin
	// Date:        2018-05-17
	// Parameter:   @waveList[in]待排序的文件列表
	// Return:      void
	void sortWaveFileList(std::vector<std::string> waveList, std::vector<std::string> &waveListSorted);

	// Description: 下载未下载的历史故障录波
	// Author:      qianchenglin
	// Date:        2018-05-17
	// Parameter:   void
	// Return:      成功返回true，否则返回false
	bool downloadHisFaultWave();

	// Description: 检查下载未下载的历史故障录波,会根据下载间隔判断是否触发下载任务
	// Author:      qianchenglin
	// Date:        2018-05-17
	// Parameter:   void
	// Return:      void
	void checkAndDownloadHisFaultWave();

	// 获取波形文件本地存储路径，包含/结尾
	SString getWaveFileLocalSaveDir();

	// 从UTC字符串中获取时间
	// 如："UTC TIME day=2764467, ms=12556"
	bool getTimeFromString(SString str, int &days, int &ms);

protected:
	bool m_isNeedCheckCidModel;
	SDateTime m_lastDownloadWaveListTime;
	SDateTime m_lastCommunicationTime;
	int m_lastCommunicationCount;
	bool m_isLastCommunicationAlarmBlocked;
};
