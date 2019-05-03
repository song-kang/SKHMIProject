/**
 *
 * 文 件 名 : plugin_scl_import.cpp
 * 创建日期 : 2017-08-10 09:50
 * 作    者 : SspAssist(skt001@163.com)
 * 修改日期 : $Date: $
 * 当前版本 : $Revision: $
 * 功能描述 : SCD,ICD,CID文件导入
 * 修改记录 : 
 *            $Log: $
 *
 * Ver  Date        Author    Comments
 * ---  ----------  --------  -------------------------------------------
 * 001	 2017-08-10	SspAssist　创建文件
 *
 **/
#include "plugin_scl_import.h"
#include "view_plugin_scl_import.h"


//////////////////////////////////////////////////////////////////////////
// 描    述:  初始化插件
// 作    者:  SspAssist
// 创建时间:  2017-08-10 09:50
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
// 描    述:  取当前插件的名称
// 作    者:  SspAssist
// 创建时间:  2017-08-10 09:50
// 参数说明:  void
// 返 回 值:  char*, 插件名称
//////////////////////////////////////////////////////////////////////////
PLUGIN_EXPORT const char* SSP_GetPluginName()
{
	return "SCD,ICD,CID文件导入";
}

//////////////////////////////////////////////////////////////////////////
// 描    述:  取当前插件的版本号
// 作    者:  SspAssist
// 创建时间:  2017-08-10 09:50
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
// 创建时间:  2017-08-10 09:50
// 参数说明:  void
// 返 回 值:  char*, 如： ia_homepage=智能告警主界面;ia_report=告警告警简报;
//////////////////////////////////////////////////////////////////////////
PLUGIN_EXPORT const char* SSP_FunPointSupported()
{
	return "plugin_scl_import=SCD,ICD,CID文件导入;";
}

//////////////////////////////////////////////////////////////////////////
// 描    述:  创建新功能点窗口
// 作    者:  SspAssist
// 创建时间:  2017-08-10 09:50
// 参数说明:  @sFunName为待打开功能点名称
//         :  @parentWidget为父窗口指针，真实类型为QWidget*
// 返 回 值:  返回新打开的窗口指针，真实类型为CBaseView*，NULL表示非本插件对应的功能点
//////////////////////////////////////////////////////////////////////////
PLUGIN_EXPORT void* SSP_NewView(const char* sFunName,const void* parentWidget)
{
	SString sName = sFunName;
	if (sName == "plugin_scl_import")
		return new view_plugin_scl_import((QWidget*)parentWidget);

	return NULL;
}
