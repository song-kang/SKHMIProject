/**
 *
 * 文 件 名 : plugin_drawer.cpp
 * 创建日期 : 2018-08-11 09:03
 * 修改日期 : $Date: $
 * 功能描述 : 自画图形展示插件
 * 修改记录 : 
 *
 **/
#include "plugin_drawer.h"
#include "view_plugin_drawer.h"

//////////////////////////////////////////////////////////////////////////
// 描    述:  初始化插件
// 作    者:  SspAssist
// 创建时间:  2018-08-11 09:03
// 参数说明:  @sExtAttr表示扩展属性字符串，作保留参数，暂时为空串
// 返 回 值:  int, 0表示成功，其他表示失败
//////////////////////////////////////////////////////////////////////////
PLUGIN_EXPORT int SSP_Init(const char* sExtAttr)
{
	_PLUGIN_INIT;
	//TODO: 在这里添加初始化代码，如通用动作注册、SVGTip回调注册等
	return 0;
}

//////////////////////////////////////////////////////////////////////////
// 描    述:  退出插件
// 作    者:  SspAssist
// 创建时间:  2018-08-11 09:03
// 参数说明:  void
// 返 回 值:  int, 0表示成功，其他表示失败
//////////////////////////////////////////////////////////////////////////
PLUGIN_EXPORT int SSP_Exit()
{
	//TODO: 在这里添加退出代码
	return 0;
}

//////////////////////////////////////////////////////////////////////////
// 描    述:  取当前插件的名称
// 作    者:  SspAssist
// 创建时间:  2018-08-11 09:03
// 参数说明:  void
// 返 回 值:  char*, 插件名称
//////////////////////////////////////////////////////////////////////////
PLUGIN_EXPORT const char* SSP_GetPluginName()
{
	return "自画图形展示插件";
}

//////////////////////////////////////////////////////////////////////////
// 描    述:  取当前插件的版本号
// 作    者:  SspAssist
// 创建时间:  2018-08-11 09:03
// 参数说明:  void
// 返 回 值:  char*, 插件版本号
//////////////////////////////////////////////////////////////////////////
PLUGIN_EXPORT const char* SSP_GetPluginVer()
{
	return "1.0.0";
}

//////////////////////////////////////////////////////////////////////////
// 描    述:  返回当前插件支持的所有功能点名称与描述
// 作    者:  SspAssist
// 创建时间:  2018-08-11 09:03
// 参数说明:  void
// 返 回 值:  char*, 如： ia_homepage=智能告警主界面;ia_report=告警告警简报;
//////////////////////////////////////////////////////////////////////////
PLUGIN_EXPORT const char* SSP_FunPointSupported()
{
	return "plugin_drawer=自画图形展示插件;";
}
//////////////////////////////////////////////////////////////////////////
// 描    述:  返回当前插件支持的所有通用动作名称与描述
// 作    者:  SspAssist
// 创建时间:  2018-08-11 09:03
// 参数说明:  void
// 返 回 值:  char*, 如：gact.nam.station.start=启动厂站网分通信;gact.nam.station.stop=停止厂站网分通信;
//////////////////////////////////////////////////////////////////////////
PLUGIN_EXPORT const char* SSP_GActSupported()
{
	return "";
}

//////////////////////////////////////////////////////////////////////////
// 描    述:  创建新功能点窗口
// 作    者:  SspAssist
// 创建时间:  2018-08-11 09:03
// 参数说明:  @sFunName为待打开功能点名称
//         :  @parentWidget为父窗口指针，真实类型为QWidget*
// 返 回 值:  返回新打开的窗口指针，真实类型为CBaseView*，NULL表示非本插件对应的功能点
//////////////////////////////////////////////////////////////////////////
PLUGIN_EXPORT void* SSP_NewView(const char* sFunName,const void* parentWidget)
{
	SString sName = sFunName;
	if (sName.left((int)strlen("plugin_drawer")) == "plugin_drawer")
	{
		view_plugin_drawer *v = new view_plugin_drawer((QWidget*)parentWidget);
		if (v)
		{
			int sn = DB->SelectIntoI(SString::toFormat("select ref_sn from t_ssp_fun_point where fun_key='%s'",sFunName));
			SString sCmd(SString::toFormat("wnd_sn=%d",sn));
			SString sResult;
			v->OnCommand(sCmd, sResult);
		}

		return v;
	}

	return NULL;
}
