/**
 *
 * 文 件 名 : plugin.cpp
 * 创建日期 : 2015-11-6 23:00
 * 作    者 : 邵凯田(skt001@163.com)
 * 修改日期 : $Date: $
 * 当前版本 : $Revision: $
 * 功能描述 : 初始化插件接口定义
 * 修改记录 : 
 *            $Log: $
 *
 * Ver  Date        Author  Comments
 * ---  ----------  ------  -------------------------------------------
 * 001	2015-11-6	邵凯田　创建文件
 *
 **/

#include "plugin.h"

CMdbPluginMgr::CMdbPluginMgr()
{
	m_hInstance = NULL;
	m_pPluginInitFun = NULL;
}

CMdbPluginMgr::~CMdbPluginMgr()
{
#ifdef WIN32
	if(m_hInstance != NULL)
		FreeLibrary(m_hInstance);
#else
	dlclose(m_hInstance);
#endif
}

bool CMdbPluginMgr::LoadPlugin()
{
	SString sPath = SDir::currentPath();
	SString sFile;
	bool err = false;
#ifdef WIN32
	sFile = sPath+"mdb_plugin.dll";
	m_hInstance = LoadLibrary(sFile.data());
	if(m_hInstance == NULL)
	{
		LOGWARN("加载插件(%s)时失败!",sFile.data());
		return false;
	}
	m_pPluginInitFun = (Mdb_Plugin_Init)GetProcAddress(m_hInstance, "Mdb_Plugin_Init");

#else
	sFile = sPath+"libmdb_plugin.so";
	m_hInstance = dlopen(sFile.data(),RTLD_LAZY);
	char *pErr;

	if ((pErr = dlerror()) != NULL)
	{
		printf("dlopen error:%s, trying ../lib path\n",pErr);
	}
	if(m_hInstance == NULL)
	{
		sFile = sPath+"../lib/libmdb_plugin.so";
		m_hInstance = dlopen(sFile.data(),RTLD_LAZY);
		
		if ((pErr = dlerror()) != NULL)
		{
			printf("dlopen error:%s\n",pErr);
		}
		if(m_hInstance == NULL)
		{
			LOGWARN("加载插件(%s)时失败!",sFile.data());
			return false;
		}
	}
	m_pPluginInitFun = (Mdb_Plugin_Init)dlsym(m_hInstance, "Mdb_Plugin_Init");
	if ((pErr = dlerror()) != NULL)
	{
		err = true;
		printf("dlsym error:%s\n",pErr);
	}
#endif
	if(err || m_pPluginInitFun == NULL)
	{
		LOGWARN("插件(%s)接口(Mdb_Plugin_Init)无效!",sFile.data());
		return false;
	}
	return true;
}

void CMdbPluginMgr::Do_Mdb_Plugin_Init(Mdb_Run_Sql pRunSqlFun,Mdb_Insert pInsertFun,Mdb_Retrieve pRetrieveFun,Mdb_GetTableFields pGetFields,char* sExtAttribute)
{
	if(m_pPluginInitFun == NULL)
		return;
	m_pPluginInitFun(pRunSqlFun,pInsertFun,pRetrieveFun,pGetFields,sExtAttribute);
}

