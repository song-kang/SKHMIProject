/**
 *
 * 文 件 名 : WatchDogPPC.cpp
 * 创建日期 : 2011-4-13 9:26
 * 修改日期 : $Date: 2011/08/31 08:43:31 $
 * 当前版本 : $Revision: 1.1 $
 * 功能描述 : PowerPC8315平台下的硬件看门狗操作类
 * 修改记录 : 
 *
 **/

#include "WatchDog.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CWatchDogPPC::CWatchDogPPC()
{
	m_hHandle = 0;
}

CWatchDogPPC::~CWatchDogPPC()
{

}

////////////////////////////////////////////////////////////////////////
// 描    述:  打开看门狗设备
// 作    者:  SK
// 创建时间:  2011-4-13 9:26
// 参数说明:  void
// 返 回 值:  true/false
//////////////////////////////////////////////////////////////////////////
bool CWatchDogPPC::Open()
{
#ifdef WIN32
	return false;
#else
	if((m_hHandle = open(PPC_WATCHDOG_DEVICE, O_WRONLY)) <= 0) 
	{
		LOGERROR("打开看门狗设备(%s)失败",PPC_WATCHDOG_DEVICE);
		m_hHandle = 0;
		return false;
	}
	return true;
#endif
}

////////////////////////////////////////////////////////////////////////
// 描    述:  喂狗，当未打开设备时自动打开
// 作    者:  SK
// 创建时间:  2011-4-13 9:27
// 参数说明:  void
// 返 回 值:  true/false
//////////////////////////////////////////////////////////////////////////
bool CWatchDogPPC::FeedDog()
{
#ifdef WIN32
	return false;
#else
	if(m_hHandle <= 0 && Open() == false)
		return false;
	write(m_hHandle,"a",1);
#endif
	return true;
}

////////////////////////////////////////////////////////////////////////
// 描    述:  关闭看门狗关停止其硬件守护工作
// 作    者:  SK
// 创建时间:  2011-4-13 9:27
// 参数说明:  void
// 返 回 值:  true
//////////////////////////////////////////////////////////////////////////
bool CWatchDogPPC::Close()
{
#ifdef WIN32
	return false;
#else
	if(m_hHandle <= 0)
		return false;
	write(m_hHandle,"V",1);
	close(m_hHandle);
	m_hHandle = 0;
	return true;
#endif
}
