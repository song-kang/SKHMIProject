/**
 *
 * �� �� �� : plugin.cpp
 * �������� : 2015-11-6 23:00
 * ��    �� : �ۿ���(skt001@163.com)
 * �޸����� : $Date: $
 * ��ǰ�汾 : $Revision: $
 * �������� : ��ʼ������ӿڶ���
 * �޸ļ�¼ : 
 *            $Log: $
 *
 * Ver  Date        Author  Comments
 * ---  ----------  ------  -------------------------------------------
 * 001	2015-11-6	�ۿ�������ļ�
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
		LOGWARN("���ز��(%s)ʱʧ��!",sFile.data());
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
			LOGWARN("���ز��(%s)ʱʧ��!",sFile.data());
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
		LOGWARN("���(%s)�ӿ�(Mdb_Plugin_Init)��Ч!",sFile.data());
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

