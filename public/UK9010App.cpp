/**
 *
 * 文 件 名 : UK9010App.cpp
 * 创建日期 : 2016-2-6 9:40
 * 作    者 : 邵凯田(skt001@163.com)
 * 修改日期 : $Date: $
 * 当前版本 : $Revision: $
 * 功能描述 : UK9010运维主站应用基础类
 * 修改记录 : 
 *            $Log: $
 *
 * Ver  Date        Author  Comments
 * ---  ----------  ------  -------------------------------------------
 * 001	2016-2-6	邵凯田　创建文件
 *
 **/
#include "UK9010App.h"
#include "sp_config_mgr.h"

CUK9010Application::CUK9010Application()
{
	m_sSoftwareType = C_SOFTWARE_TYPE;

}

CUK9010Application::~CUK9010Application()
{
	SService::Stop();
	CConfigMgr::Quit();
}

//////////////////////////////////////////////////////////////////////////
// 描    述:  启动服务，开启所有应用的服务，该函数必须由派生类实现，且启动服务完成后必须返回
// 作    者:  邵凯田
// 创建时间:  2015-7-30 16:05
// 参数说明:  void
// 返 回 值:  true/false
//////////////////////////////////////////////////////////////////////////
bool CUK9010Application::Start()
{
	SString sFile = GetConfPath()+"sys_unitconfig.xml";
	if(!m_UnitConfig.Load(sFile))
	{
		LOGFAULT("加载单元配置文件(%s)时失败!",sFile.data());
		return false;
	}
	m_iUnitId = m_UnitConfig.m_iUnitId;
	m_sUnitName = m_UnitConfig.m_sUnitName;
	m_sUnitDesc = m_UnitConfig.m_sUnitDesc;
	m_sSoftwareType = m_UnitConfig.m_sSystemType;
	
	return true;
}

////////////////////////////////////////////////////////////////////////
// 描    述:  代理消息处理接口，由派生类实现，处理函数必须尽量短小，快速返回
// 作    者:  邵凯田
// 创建时间:  2016-2-3 15:20
// 参数说明:  @wMsgType表示消息类型
//         :  @pMsgHead为消息头
//         :  @sHeadStr消息头字符串
//         :  @pBuffer为消息携带的二进制数据内容，NULL表示无二进制数据
//         :  @iLength为二进制数据长度
// 返 回 值:  true表示处理成功，false表示处理失败
//////////////////////////////////////////////////////////////////////////
bool CUK9010Application::ProcessAgentMsg(unsigned short wMsgType,stuSpUnitAgentMsgHead *pMsgHead,SString &sHeadStr,unsigned char* pBuffer/*=NULL*/,int iLength/*=0*/)
{
	S_UNUSED(wMsgType);
	S_UNUSED(pMsgHead);
	S_UNUSED(sHeadStr);
	S_UNUSED(pBuffer);
	S_UNUSED(iLength);
	return false;
}

//////////////////////////////////////////////////////////////////////////
// 描    述:  将SQL语句中日期类型转为字符串类型字段
// 作    者:  邵凯田
// 创建时间:  2016-5-6 16:23
// 参数说明:  @sField为字段名称
// 返 回 值:  根据不同的数据库类型而不同
//////////////////////////////////////////////////////////////////////////
SString CUK9010Application::Date2String(SString sField)
{
	switch (GET_DB_CFG->GetMasterType())
	{
	case DB_ORACLE:
		return "to_char("+sField+",'yyyy-mm-dd hh24:mi:ss')";

	case DB_MYSQL:
		return "DATE_FORMAT("+sField+",'%Y-%m-%d %k:%i:%s')";

	default:
		return sField;
	}
}

//////////////////////////////////////////////////////////////////////////
// 描    述:  将SQL语句中字符串改为日期类型
// 作    者:  邵凯田
// 创建时间:  2016-5-6 16:25
// 参数说明:  @sDateTime格式为:yyyy-MM-dd hh:mm:ss
// 返 回 值:  根据不同的数据库类型而不同
//////////////////////////////////////////////////////////////////////////
SString CUK9010Application::String2Date(SString sDateTime)
{
	switch (GET_DB_CFG->GetMasterType())
	{
	case DB_ORACLE:
		return "to_date('"+sDateTime+"','yyyy-mm-dd hh24:mi:ss')";

	case DB_MYSQL:
		return "str_to_date('"+sDateTime+"','%Y-%m-%d %k:%i:%s')";

	default:
		return sDateTime;
	}
}


