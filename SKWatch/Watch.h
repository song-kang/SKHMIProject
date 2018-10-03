/**
 *
 * 文 件 名 : Watch.h
 * 创建日期 : 2015-8-1 13:35
 * 修改日期 : $Date: $
 * 当前版本 : $Revision: $
 * 功能描述 : 看门狗服务类
 * 修改记录 : 
 *
 **/

#ifndef __WATCH_DOG_H__
#define __WATCH_DOG_H__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "SApi.h"
#include "SString.h"
//#include "WatchDog.h"
#include "sk_config_mgr.h"

//////////////////////////////////////////////////////////////////////////
// 名    称:  stuExtModule
// 作    者:  SK
// 创建时间:  2015-8-1 13:39
// 描    述:  被守护进程的信息
//////////////////////////////////////////////////////////////////////////
struct stuExtModule
{
	int iSn;
	SString sModule;
	SString sCmdLine;
	SString sArg;
	SString sPath;
	int first_delay_sec;//初步启动的滞后时间（单位秒）
	bool is_first;
	int iStartErrTimes;//启动失败次数，>100次则60秒启动一次,>1000次3000秒启动一次
	time_t tLastStartErr;//最后一次启动失败的时间
	int iStartTimes;//正常启动次数，大于1代表有程序重启情况
	time_t tLastStart;//最后一次启动成功时间
	bool is_run;
	bool is_watch;
};

//////////////////////////////////////////////////////////////////////////
// 名    称:  CWatch
// 作    者:  SK
// 创建时间:  2015-8-1 13:39
// 描    述:  监视类
//////////////////////////////////////////////////////////////////////////
class CWatch : public CConfigBase
{
public:
	CWatch();
	virtual ~CWatch();

	//////////////////////////////////////////////////////////////////////////
	// 描    述:  执行一次守护动作
	// 作    者:  SK
	// 创建时间:  2015-8-1 15:16
	// 参数说明:  void
	// 返 回 值:  void
	//////////////////////////////////////////////////////////////////////////
	void Watch();

	//////////////////////////////////////////////////////////////////////////
	// 描    述:  加载配置文件
	// 作    者:  SK
	// 创建时间:  2015-8-1 13:52
	// 参数说明:  void
	// 返 回 值:  true/false
	//////////////////////////////////////////////////////////////////////////
	virtual bool Load(SString sPathFile);

	static void* ThreadWatch(void *lp);

	//////////////////////////////////////////////////////////////////////////
	// 描    述:  运行并守护所有进程
	// 作    者:  SK
	// 创建时间:  2015-8-1 13:40
	// 参数说明:  
	// 返 回 值:  true表示正常守护运行，false表示需要中止运行
	//////////////////////////////////////////////////////////////////////////
	bool Run(int argc, char* argv[]);

	//////////////////////////////////////////////////////////////////////////
	// 描    述:  中止所有被守护的进程
	// 作    者:  SK
	// 创建时间:  2015-8-1 13:40
	// 参数说明:  void
	// 返 回 值:  void
	//////////////////////////////////////////////////////////////////////////
	void HaltAll();
	
	//////////////////////////////////////////////////////////////////////////
	// 描    述:  先查找其它的守护程序，并终止
	// 作    者:  SK
	// 创建时间:  2015-8-1 14:04
	// 参数说明:  void
	// 返 回 值:  void
	//////////////////////////////////////////////////////////////////////////
	void HaltOtherSelfModule();

	void SetModuleName(SString name) { m_sModuleName = name; }

	bool m_bQuit,m_bQuited;
	bool m_bHardWatch;//是否启动硬件看门狗
	SPtrList<stuExtModule> m_ExtModuleList;//独立运行模块列表
	//CWatchDogPPC m_WatchDog;//看门狗
	SString m_sBinPath;
	bool m_bCmdNoDog;//运行参数中是否含有nodog
	SLock m_Lock;//配置锁
	SString m_sModuleName;
};

#endif //__WATCH_DOG_H__
