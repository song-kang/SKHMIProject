/**
 *
 * 文 件 名 : main.cpp
 * 创建日期 : 2014-4-26 21:38
 * 作    者 : 邵凯田(skt001@163.com)
 * 修改日期 : $Date: $
 * 当前版本 : $Revision: $
 * 功能描述 : 单元管理主程序
 * 修改记录 : 
 *            $Log: $
 *
 * Ver  Date        Author  Comments
 * ---  ----------  ------  -------------------------------------------
 * 001	2014-4-26	邵凯田　创建文件
 *
 **/


//////////////////////////////////////////////////////////////////////////
// History
// 2.0.0    2014-04-26 邵凯田　起始版本
//////////////////////////////////////////////////////////////////////////

#define MOD_VERSION "2.0.0"

#ifndef MOD_DATE
#define MOD_DATE "2014-05-20"
#endif
#ifndef MOD_TIME
#define MOD_TIME "12:36:00"
#endif

#include "UnitMgrApplication.h"
#include "..\SKLic\slicense.h"
CUnitMgrApplication g_app;

#ifdef WIN32
#include "vld.h"
#endif

int main(int argc, char* argv[])
{
	SLicense lic;
	if(!lic.CheckLicense()) {
		printf("Licsence error\n");
		return 1;
	}

	g_app.SetModuleDesc("单元管理主程序");
	g_app.SetVersion(MOD_VERSION);
	g_app.SetApplicationId(SP_UA_APPNO_AGENT);
	g_app.Run(argc,argv,MOD_DATE,MOD_TIME);
	return 0;
}
