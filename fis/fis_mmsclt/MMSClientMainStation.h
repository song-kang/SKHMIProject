#pragma once

#include "MMSClient.h"


// 一个模块具有多个连接，一个连接对应一台装置。
class MMSClientMainStation : public MMSClient
{
public:
	// Description: MMSClientMainStation
	// Author:      qianchenglin
	// Date:        2016-01-01
	// Parameter:   @mmscltApp[in]父类指针
	//              @nodeNo[in]运行节点
	//              @substationNo[in]站号
	//              @substationName[in]站名
	// Return:      Return true if succeed. Otherwise return false
	MMSClientMainStation(MMSCLTApp *mmscltApp, int nodeNo, int substationNo, SString substationName);
	virtual ~MMSClientMainStation();

public:
	// Description: 初始化运行必要信息
	// Author:      qianchenglin
	// Date:        2016-01-01
	// Parameter:   void
	// Return:      Return true if succeed. Otherwise return false
	virtual bool Init();


	// Description: 根据reference路径找到其对象
	// Author:      邵凯田
	// Date:        2016-01-01
	// Parameter:   @reference[in]reference路径
	// Return:      返回reference对象，未找到返回NULL	
	virtual t_reference * findReference(std::string reference);

protected:
	// Description: 设置通道通信状态
	// Author:      qianchenglin
	// Date:        2016-01-01
	// Parameter:   @state[in]0断开；1正常
	// Return:      Return true if succeed. Otherwise return false
	virtual bool SetChannelState(int state);

	// Description: 设置Entry ID
	// Author:      qianchenglin
	// Date:        2016-01-01
	// Parameter:   @rptInfo[in]报告信息结构
	//              @entryId[in]条目ID
	// Return:      void
	virtual void setEntryID(stuSMmsReportInfo *rptInfo,SString entryId);

private:
	// Description: 初始化运行参数
	// Author:      qianchenglin
	// Date:        2016-01-01
	// Parameter:   void
	// Return:      Return true if succeed. Otherwise return false
	bool InitParam();

	// Description: 初始化节点下所有IED
	// Author:      qianchenglin
	// Date:        2016-01-01
	// Parameter:   void
	// Return:      Return true if succeed. Otherwise return false
	bool InitIeds();

private:
	unsigned char m_reportID;		// 报告控制块ID
	SString m_iedIPA;				// 主站模式使用t_oe_module_param中IP
	SString m_iedIPB;
};