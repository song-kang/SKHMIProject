/**
 *
 * �� �� �� : mdb_plugin.h
 * �������� : 2015-11-7 16:43
 * ��    �� : �ۿ���(skt001@163.com)
 * �޸����� : $Date: $
 * ��ǰ�汾 : $Revision: $
 * �������� : 
 * �޸ļ�¼ : 
 *            $Log: $
 *
 * Ver  Date        Author  Comments
 * ---  ----------  ------  -------------------------------------------
 * 001	2015-11-7	�ۿ�������ļ�
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
// ��    ��:  ���г�ʼ��SQL�����
// ��    ��:  �ۿ���
// ����ʱ��:  2015-11-24 11:33
// ����˵��:  @sqlΪSQL���
// �� �� ֵ:  bool
//////////////////////////////////////////////////////////////////////////
typedef bool (*Mdb_Run_Sql)(char* sql);

//////////////////////////////////////////////////////////////////////////
// ��    ��:  �ڴ�����ص�����
// ��    ��:  �ۿ���
// ����ʱ��:  2015-11-24 13:13
// ����˵��:  
// �� �� ֵ:  int
//////////////////////////////////////////////////////////////////////////
typedef int (*Mdb_Insert)(char* sTableName, void *pRowData, int iRowSize, int iRows);


//////////////////////////////////////////////////////////////////////////
// ��    ��:  �ڴ���ѯ�ص�����
// ��    ��:  �ۿ���
// ����ʱ��:  2015-12-14 10:55
// ����˵��:  @sqlΪSQL���
//         :  @ppRecordsetΪCMdbTable*���͵�ָ��,��ΪNULLʱ������Ӧͨ��delete[]�ͷ�
//         :  @piRowSizeΪÿ�м�¼�Ĵ�С���ֽڣ�
//         :  @piRowsΪ���ص�����
//         :  @ppsRowResult��ʾ�ۺ�����µĽ���ַ�������ΪNULLʱ������Ӧͨ��delete[]�ͷ�
// �� �� ֵ:  int
//////////////////////////////////////////////////////////////////////////
typedef int (*Mdb_Retrieve)(char* sql, unsigned char** ppRecordset, int *piRowSize, int *piRows, char** ppsRowResult);

//////////////////////////////////////////////////////////////////////////
// ��    ��:  ȡ���ֶ���Ϣ
// ��    ��:  �ۿ���
// ����ʱ��:  2016-3-23 23:19
// ����˵��:  @plFields
// �� �� ֵ:  �ֶ�����
//////////////////////////////////////////////////////////////////////////
typedef int (*Mdb_GetTableFields)(char* tablename,SPtrList<stuTableField> *plFields);

_SMDB_PLUGIN_LIB_EXPORT void Mdb_Plugin_Init(Mdb_Run_Sql pRunSqlFun,Mdb_Insert pInsertFun,Mdb_Retrieve pRetrieveFun,Mdb_GetTableFields pGetFieldsFun,char* sExtAttribute);

#ifdef __cplusplus
};
#endif

#endif//__UK9001_MDB_PLUGIN_H__

