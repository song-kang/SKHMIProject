/**
 *
 * 文 件 名 : mdb_plugin.h
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

#ifndef __UK9001_MDB_PLUGIN_H__
#define __UK9001_MDB_PLUGIN_H__

#include "SList.h"
#include "db/mdb/MdbClient.h"
#ifdef WIN32
#define SMDB_PLUGIN_LIB_EXPORTS
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

//////////////////////////////////////////////////////////////////////////
// 描    述:  运行初始化SQL的语句
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

_SMDB_PLUGIN_LIB_EXPORT void Mdb_Plugin_Init(Mdb_Run_Sql pRunSqlFun,Mdb_Insert pInsertFun,Mdb_Retrieve pRetrieveFun,Mdb_GetTableFields pGetFieldsFun,char* sExtAttribute);

#ifdef __cplusplus
};
#endif

#endif//__UK9001_MDB_PLUGIN_H__

