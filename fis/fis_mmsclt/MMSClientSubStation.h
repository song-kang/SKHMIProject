#pragma once

#include "MMSClient.h"


// 一个模块具有多个连接，一个连接对应一台装置。
class MMSClientSubStation : public MMSClient
{
public:
	MMSClientSubStation(MMSCLTApp *mmscltApp, int nodeNo, int substationNo, SString substationName, int iedNo, SString iedDesc, SString iedName);
	virtual ~MMSClientSubStation();

public:
	virtual bool Start();

	// Description: 初始化运行必要信息
	// Author:      qianchenglin
	// Date:        2016-01-01
	// Parameter:   void
	// Return:      Return true if succeed. Otherwise return false
	virtual bool Init();

	// Description: 根据reference路径找到其对象
	// Author:      qianchenglin
	// Date:        2016-01-01
	// Parameter:   @reference[in]reference路径
	// Return:      返回reference对象，未找到返回NULL
	virtual t_reference *findReference(std::string reference);

	// Description: 检查模拟量通道，如果有监视中的模拟量通道将会下发定值召唤，然后对返回值进行对比
	// Author:      qianchenglin
	// Date:        2016-01-01
	// Parameter:   void
	// Return:      Return true if succeed. Otherwise return false
	//virtual bool checkAnalogChannel();

	Ied *getIed();

protected:
	// Description: 设置Entry ID
	// Author:      qianchenglin
	// Date:        2016-01-01
	// Parameter:   @rptInfo[in]报告信息结构
	//              @entryId[in]条目ID
	// Return:      void
	virtual void setEntryID(stuSMmsReportInfo *rptInfo,SString entryId);

	// Description: 设置通道A通信状态
	// Author:      qianchenglin
	// Date:        2016-01-01
	// Parameter:   @state[in]0断开；1正常
	// Return:      Return true if succeed. Otherwise return false
	virtual bool SetChannelStateA(int state);

	// Description: 设置通道B通信状态
	// Author:      qianchenglin
	// Date:        2016-01-01
	// Parameter:   @state[in]0断开；1正常
	// Return:      Return true if succeed. Otherwise return false
	virtual bool SetChannelStateB(int state);

private:
	unsigned char m_reportID;		// 报告控制块ID
};