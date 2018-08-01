/**
 *
 * 文 件 名 : plugin.h
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

#ifndef __SMDB_PLUGIN_DEF_H__
#define __SMDB_PLUGIN_DEF_H__

#include "SInclude.h"
#ifndef WIN32
#include<dlfcn.h>
#endif
#include "db/mdb/MdbClient.h"

#ifdef WIN32
#ifdef SMDB_PLUGIN_LIB_EXPORTS
#define _SMDB_PLUGIN_LIB_EXPORT _declspec(dllexport)
#else
#define _SMDB_PLUGIN_LIB_EXPORT _declspec(dllimport)
#endif
#else
#define _SMDB_PLUGIN_LIB_EXPORT
#endif

#ifdef __cplusplus
extern "C" {
#endif

//插件文件名为mdb_plugin.dll/.so，存放路径为sys_mdb.exe所在路径
//插件不存在则相关逻辑忽略


//////////////////////////////////////////////////////////////////////////
// 描    述:  运行初始化SQL的回调函数
// 作    者:  邵凯田
// 创建时间:  2015-11-24 11:33
// 参数说明:  @sql为SQL语句
// 返 回 值:  bool
//////////////////////////////////////////////////////////////////////////
typedef bool (*Mdb_Run_Sql)(char* sql);

//////////////////////////////////////////////////////////////////////////
// 描    述:  内存库插入回调函数
// 作    者:  邵凯田
// 创建时间:  2015-11-24 13:13
// 参数说明:  
// 返 回 值:  int
//////////////////////////////////////////////////////////////////////////
typedef int (*Mdb_Insert)(char* sTableName, void *pRowData, int iRowSize, int iRows);

//////////////////////////////////////////////////////////////////////////
// 描    述:  内存库查询回调函数
// 作    者:  邵凯田
// 创建时间:  2015-12-14 10:55
// 参数说明:  @sql为SQL语句
//         :  @ppRecordset为CMdbTable*类型的指针,不为NULL时调用者应通过delete[]释放
//         :  @piRowSize为每行记录的大小（字节）
//         :  @piRows为返回的行数
//         :  @ppsRowResult表示聚合情况下的结果字符串，不为NULL时调用者应通过delete[]释放
// 返 回 值:  int
//////////////////////////////////////////////////////////////////////////
typedef int (*Mdb_Retrieve)(char* sql, unsigned char** ppRecordset, int *piRowSize, int *piRows, char** ppsRowResult);

//////////////////////////////////////////////////////////////////////////
// 描    述:  取表字段信息
// 作    者:  邵凯田
// 创建时间:  2016-3-23 23:19
// 参数说明:  @plFields
// 返 回 值:  字段数量
//////////////////////////////////////////////////////////////////////////
typedef int (*Mdb_GetTableFields)(char* tablename,SPtrList<stuTableField> *plFields);

//////////////////////////////////////////////////////////////////////////
// 描    述:  当数据库服务启动完毕后调用的初始化插件入口函数
// 作    者:  邵凯田
// 创建时间:  2015-11-7 14:39
// 参数说明:  @pRunSqlFun表示Mdb_Run_Sql回调函数，用于执行SQL
//         :  @pInsertFun表示插入回调函数
//         :  @sExtAttribute表示扩展属性字符串
// 返 回 值:  void
//////////////////////////////////////////////////////////////////////////
typedef void (*Mdb_Plugin_Init)(Mdb_Run_Sql pRunSqlFun,Mdb_Insert pInsertFun,Mdb_Retrieve pRetrieveFun,Mdb_GetTableFields pGetFields,char* sExtAttribute);


#ifdef __cplusplus
};
#endif

class CMdbPluginMgr
{
public:
	CMdbPluginMgr();
	~CMdbPluginMgr();

	bool LoadPlugin();

	void Do_Mdb_Plugin_Init(Mdb_Run_Sql pRunSqlFun,Mdb_Insert pInsertFun,Mdb_Retrieve pRetrieveFun,Mdb_GetTableFields pGetFields,char* sExtAttribute);

	Mdb_Plugin_Init m_pPluginInitFun;
#ifdef WIN32
	HMODULE m_hInstance;
#else
	void* m_hInstance;
#endif

};

#endif