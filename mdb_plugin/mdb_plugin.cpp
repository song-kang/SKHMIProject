/**
 *
 * 文 件 名 : mdb_plugin.cpp
 * 创建日期 : 2015-11-7 16:43
 * 作    者 : 邵凯田(skt001@163.com)
 * 修改日期 : $Date: $
 * 当前版本 : $Revision: $
 * 功能描述 : 
 * 修改记录 : 
 *            $Log: $
 *
 * Ver  Date        Author  Comments
 * ---  ----------  ------  -------------------------------------------
 * 001	2015-11-7	邵凯田　创建文件
 *
 **/

#include "mdb_plugin.h"
#include "MdbPluginApp.h"

#ifdef _DEBUG
#include "vld.h"
#endif

/**
 * Ver    Date        Author  Comments
 * -----  ----------  ------  -------------------------------------------
 * 1.0.0  2015-11-07  邵凯田  初始版本
 *
 **/

#define MOD_VERSION "1.0.2"

#ifndef MOD_DATE
#define MOD_DATE "2015-11-07"
#endif
#ifndef MOD_TIME
#define MOD_TIME "15:55:00"
#endif

_SMDB_PLUGIN_LIB_EXPORT void Mdb_Plugin_Init(Mdb_Run_Sql pRunSqlFun,Mdb_Insert pInsertFun,Mdb_Retrieve pRetrieveFun,Mdb_GetTableFields pGetFieldsFun,char* sExtAttribute)
{
 	CMdbPluginApplication app;
	app.SetFork(false);
	app.SetModuleDesc("UK9001内存库初始化插件(配置版)");
	app.SetVersion(MOD_VERSION);
	app.m_iUnitId = 1;
	char* argv[1];
	argv[0] = "libmdb_plugin";
	app.m_pRunSqlFun = pRunSqlFun;
	app.m_pMdbInsertFun = pInsertFun;
	app.m_pMdbRetrieve = pRetrieveFun;
	app.m_pMdbGetFields = pGetFieldsFun;
	app.Run(1,argv,MOD_DATE,MOD_TIME,false);

// 	app.Stop();
}
