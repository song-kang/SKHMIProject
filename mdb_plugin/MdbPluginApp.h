/**
 *
 * 文 件 名 : MdbPluginApp.h
 * 创建日期 : 2015-11-7 16:50
 * 作    者 : 邵凯田(skt001@163.com)
 * 修改日期 : $Date: $
 * 当前版本 : $Revision: $
 * 功能描述 : MDB内存数据库的初始化插件APP类，用于创建内存数据库结构，并从历史库加载初始数据
 * 修改记录 : 
 *            $Log: $
 *
 * Ver  Date        Author  Comments
 * ---  ----------  ------  -------------------------------------------
 * 001	2015-11-7	邵凯田　创建文件
 *
 **/

#ifndef __UK9001_MDB_PLUGIN_APPLICATION_H__
#define __UK9001_MDB_PLUGIN_APPLICATION_H__

#include "SApplication.h"
#include "SList.h"
#include "ssp_database.h"
#include "ssp_base.h"
#include "mdb_plugin.h"

#define OMS_ORACLE_DB
class CMdbPluginApplication : public SApplication
{
public:
	CMdbPluginApplication();
	virtual ~CMdbPluginApplication();

	//////////////////////////////////////////////////////////////////////////
	// 描    述:  启动服务，开启所有应用的服务，该函数必须由派生类实现，且启动服务完成后必须返回
	// 作    者:  邵凯田
	// 创建时间:  2010-7-1 9:15
	// 参数说明:  
	// 返 回 值:  null
	//////////////////////////////////////////////////////////////////////////
	virtual bool Start();

	//////////////////////////////////////////////////////////////////////////
	// 描    述:  停止服务，关闭所有应用的服务，
	// 作    者:  邵凯田
	// 创建时间:  2010-7-1 9:18
	// 参数说明:  
	// 返 回 值:  null
	//////////////////////////////////////////////////////////////////////////
	virtual bool Stop();

	virtual bool ProcessAgentMsg(WORD wMsgType,stuSpUnitAgentMsgHead *pMsgHead,SString &sHeadStr,BYTE* pBuffer=NULL,int iLength=0){return false;};

	//////////////////////////////////////////////////////////////////////////
	// 描    述:  从.sql文件批量执行SQL
	// 作    者:  邵凯田
	// 创建时间:  2015-11-7 22:58
	// 参数说明:  @filename为文件名称
	// 返 回 值:  true/false
	//////////////////////////////////////////////////////////////////////////
	bool RunSqlFile(SString filename);

	//////////////////////////////////////////////////////////////////////////
	// 描    述:  从一个指定的字符串批量执行SQL
	// 作    者:  邵凯田
	// 创建时间:  2015-11-7 22:59
	// 参数说明:  @sqls为字符串内容
	// 返 回 值:  true/false
	//////////////////////////////////////////////////////////////////////////
	bool RunSqlStrings(char* sqls);

	//////////////////////////////////////////////////////////////////////////
	// 描    述:  内存库执行SQL
	// 作    者:  邵凯田
	// 创建时间:  2015-11-24 13:09
	// 参数说明:  @sql
	// 返 回 值:  bool
	//////////////////////////////////////////////////////////////////////////
	bool MdbExecute(SString sql);

	//////////////////////////////////////////////////////////////////////////
	// 描    述:  内存库插入函数
	// 作    者:  邵凯田
	// 创建时间:  2015-11-24 13:18
	// 参数说明:  
	// 返 回 值:  int
	//////////////////////////////////////////////////////////////////////////
	int MdbInsert(char* sTableName, void *pRowData, int iRowSize, int iRows);

	//////////////////////////////////////////////////////////////////////////
	// 描    述:  
	// 作    者:  邵凯田
	// 创建时间:  2016-3-23 21:52
	// 参数说明:  
	// 返 回 值:  
	//////////////////////////////////////////////////////////////////////////
	int MdbRetrieve(char* sql, unsigned char** ppRecordset, int *piRowSize, int *piRows, char** ppsRowResult);

	//////////////////////////////////////////////////////////////////////////
	// 描    述:  取表字段信息
	// 作    者:  邵凯田
	// 创建时间:  2016-3-23 23:19
	// 参数说明:  @plFields
	// 返 回 值:  字段数量
	//////////////////////////////////////////////////////////////////////////
	int MdbGetTableFields(char* tablename,SPtrList<stuTableField> *plFields);

	Mdb_Insert m_pMdbInsertFun;
	Mdb_Run_Sql m_pRunSqlFun;
	Mdb_Retrieve m_pMdbRetrieve;
	Mdb_GetTableFields m_pMdbGetFields;
private:
	CSsp_Database *m_pDatabase;			//数据库配置文件
// 	SMdb *m_pMdb;
// 	CMdbClient *m_pMdbClient;
};

#endif//__UK9001_MDB_PLUGIN_APPLICATION_H__
