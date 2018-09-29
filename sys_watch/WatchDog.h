/**
 *
 * 文 件 名 : WatchDogPPC.h
 * 创建日期 : 2011-4-13 9:26
 * 修改日期 : $Date: 2011/08/31 08:43:32 $
 * 当前版本 : $Revision: 1.1 $
 * 功能描述 : PowerPC8315平台下的硬件看门狗操作类
 * 修改记录 : 
 *
 **/

#if !defined(AFX_WATCHDOGPPC_H__056E28A6_8C67_4A4C_9FF7_D1C6CB3B4E77__INCLUDED_)
#define AFX_WATCHDOGPPC_H__056E28A6_8C67_4A4C_9FF7_D1C6CB3B4E77__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define PPC_WATCHDOG_DEVICE "/dev/watchdog"

#include "SApi.h"
#ifndef WIN32
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#endif

class CWatchDogPPC  
{
public:
	CWatchDogPPC();
	virtual ~CWatchDogPPC();

	////////////////////////////////////////////////////////////////////////
	// 描    述:  打开看门狗设备
	// 作    者:  SK
	// 创建时间:  2011-4-13 9:26
	// 参数说明:  void
	// 返 回 值:  true/false
	//////////////////////////////////////////////////////////////////////////
	bool Open();

	////////////////////////////////////////////////////////////////////////
	// 描    述:  喂狗，当未打开设备时自动打开
	// 作    者:  SK
	// 创建时间:  2011-4-13 9:27
	// 参数说明:  void
	// 返 回 值:  true/false
	//////////////////////////////////////////////////////////////////////////
	bool FeedDog();

	////////////////////////////////////////////////////////////////////////
	// 描    述:  关闭看门狗关停止其硬件守护工作
	// 作    者:  SK
	// 创建时间:  2011-4-13 9:27
	// 参数说明:  void
	// 返 回 值:  true
	//////////////////////////////////////////////////////////////////////////
	bool Close();

private:
	int m_hHandle;//看门狗设置句柄
};

#endif // !defined(AFX_WATCHDOGPPC_H__056E28A6_8C67_4A4C_9FF7_D1C6CB3B4E77__INCLUDED_)
