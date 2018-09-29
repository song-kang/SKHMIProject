/**
 *
 * 文 件 名 : WatchApplication.cpp
 * 创建日期 : 2015-8-1 13:33
 * 修改日期 : $Date: $
 * 当前版本 : $Revision: $
 * 功能描述 : 看门狗程序APP类
 * 修改记录 : 
 *
 **/

#include "WatchApplication.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CWatchApplication::CWatchApplication()
{

}

CWatchApplication::~CWatchApplication()
{

}

//////////////////////////////////////////////////////////////////////////
// 描    述:  启动服务，开启所有应用的服务，该函数必须由派生类实现，且启动服务完成后必须返回
// 作    者:  SK
// 创建时间:  2015-8-1 13:34
// 参数说明:  void
// 返 回 值:  true/false
//////////////////////////////////////////////////////////////////////////
bool CWatchApplication::Start()
{
	m_Watch.m_sBinPath = GetBinPath();
	m_Watch.Load(GetConfPath()+"sys_watch.xml");

	if(!IsParam("noreload"))
	{
		LOGDEBUG("启动配置文件帧听功能，当配置文件改变时将自动应用新配置!如须关闭请添加启动参数:noreload");
		CConfigMgr::SetReloadSeconds(5);
		CConfigMgr::StartReload();
	}
	m_Watch.m_bCmdNoDog = IsParam("nodog");
	if(!m_Watch.Run(m_iArgc,m_ppArgv))
		Quit();//退出运行
	return true;
}

//////////////////////////////////////////////////////////////////////////
// 描    述:  停止服务，关闭所有应用的服务，
// 作    者:  SK
// 创建时间:  2015-8-1 13:34
// 参数说明:  void
// 返 回 值:  true/false
//////////////////////////////////////////////////////////////////////////
bool CWatchApplication::Stop()
{
	m_Watch.m_bQuit = true;
	while(!m_Watch.m_bQuited)
		SApi::UsSleep(1000);
	return true;
}

////////////////////////////////////////////////////////////////////////
// 描    述:  代理消息处理接口，由派生类实现，处理函数必须尽量短小，快速返回
// 作    者:  SK
// 创建时间:  2016-2-3 15:20
// 参数说明:  @wMsgType表示消息类型
//         :  @pMsgHead为消息头
//         :  @sHeadStr消息头字符串
//         :  @pBuffer为消息携带的二进制数据内容，NULL表示无二进制数据
//         :  @iLength为二进制数据长度
// 返 回 值:  true表示处理成功，false表示处理失败或未处理
//////////////////////////////////////////////////////////////////////////
bool CWatchApplication::ProcessAgentMsg(WORD wMsgType,stuSpUnitAgentMsgHead *pMsgHead,SString &sHeadStr,BYTE* pBuffer/*=NULL*/,int iLength/*=0*/)
{
	return false;
}

