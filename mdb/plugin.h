/**
 *
 * �� �� �� : plugin.h
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

//����ļ���Ϊmdb_plugin.dll/.so�����·��Ϊsys_mdb.exe����·��
//���������������߼�����


//////////////////////////////////////////////////////////////////////////
// ��    ��:  ���г�ʼ��SQL�Ļص�����
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

//////////////////////////////////////////////////////////////////////////
// ��    ��:  �����ݿ����������Ϻ���õĳ�ʼ�������ں���
// ��    ��:  �ۿ���
// ����ʱ��:  2015-11-7 14:39
// ����˵��:  @pRunSqlFun��ʾMdb_Run_Sql�ص�����������ִ��SQL
//         :  @pInsertFun��ʾ����ص�����
//         :  @sExtAttribute��ʾ��չ�����ַ���
// �� �� ֵ:  void
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