/**
 *
 * �� �� �� : MdbPluginApp.h
 * �������� : 2015-11-7 16:50
 * ��    �� : �ۿ���(skt001@163.com)
 * �޸����� : $Date: $
 * ��ǰ�汾 : $Revision: $
 * �������� : MDB�ڴ����ݿ�ĳ�ʼ�����APP�࣬���ڴ����ڴ����ݿ�ṹ��������ʷ����س�ʼ����
 * �޸ļ�¼ : 
 *            $Log: $
 *
 * Ver  Date        Author  Comments
 * ---  ----------  ------  -------------------------------------------
 * 001	2015-11-7	�ۿ�������ļ�
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
	// ��    ��:  �������񣬿�������Ӧ�õķ��񣬸ú���������������ʵ�֣�������������ɺ���뷵��
	// ��    ��:  �ۿ���
	// ����ʱ��:  2010-7-1 9:15
	// ����˵��:  
	// �� �� ֵ:  null
	//////////////////////////////////////////////////////////////////////////
	virtual bool Start();

	//////////////////////////////////////////////////////////////////////////
	// ��    ��:  ֹͣ���񣬹ر�����Ӧ�õķ���
	// ��    ��:  �ۿ���
	// ����ʱ��:  2010-7-1 9:18
	// ����˵��:  
	// �� �� ֵ:  null
	//////////////////////////////////////////////////////////////////////////
	virtual bool Stop();

	virtual bool ProcessAgentMsg(WORD wMsgType,stuSpUnitAgentMsgHead *pMsgHead,SString &sHeadStr,BYTE* pBuffer=NULL,int iLength=0){return false;};

	//////////////////////////////////////////////////////////////////////////
	// ��    ��:  ��.sql�ļ�����ִ��SQL
	// ��    ��:  �ۿ���
	// ����ʱ��:  2015-11-7 22:58
	// ����˵��:  @filenameΪ�ļ�����
	// �� �� ֵ:  true/false
	//////////////////////////////////////////////////////////////////////////
	bool RunSqlFile(SString filename);

	//////////////////////////////////////////////////////////////////////////
	// ��    ��:  ��һ��ָ�����ַ�������ִ��SQL
	// ��    ��:  �ۿ���
	// ����ʱ��:  2015-11-7 22:59
	// ����˵��:  @sqlsΪ�ַ�������
	// �� �� ֵ:  true/false
	//////////////////////////////////////////////////////////////////////////
	bool RunSqlStrings(char* sqls);

	//////////////////////////////////////////////////////////////////////////
	// ��    ��:  �ڴ��ִ��SQL
	// ��    ��:  �ۿ���
	// ����ʱ��:  2015-11-24 13:09
	// ����˵��:  @sql
	// �� �� ֵ:  bool
	//////////////////////////////////////////////////////////////////////////
	bool MdbExecute(SString sql);

	//////////////////////////////////////////////////////////////////////////
	// ��    ��:  �ڴ����뺯��
	// ��    ��:  �ۿ���
	// ����ʱ��:  2015-11-24 13:18
	// ����˵��:  
	// �� �� ֵ:  int
	//////////////////////////////////////////////////////////////////////////
	int MdbInsert(char* sTableName, void *pRowData, int iRowSize, int iRows);

	//////////////////////////////////////////////////////////////////////////
	// ��    ��:  
	// ��    ��:  �ۿ���
	// ����ʱ��:  2016-3-23 21:52
	// ����˵��:  
	// �� �� ֵ:  
	//////////////////////////////////////////////////////////////////////////
	int MdbRetrieve(char* sql, unsigned char** ppRecordset, int *piRowSize, int *piRows, char** ppsRowResult);

	//////////////////////////////////////////////////////////////////////////
	// ��    ��:  ȡ���ֶ���Ϣ
	// ��    ��:  �ۿ���
	// ����ʱ��:  2016-3-23 23:19
	// ����˵��:  @plFields
	// �� �� ֵ:  �ֶ�����
	//////////////////////////////////////////////////////////////////////////
	int MdbGetTableFields(char* tablename,SPtrList<stuTableField> *plFields);

	Mdb_Insert m_pMdbInsertFun;
	Mdb_Run_Sql m_pRunSqlFun;
	Mdb_Retrieve m_pMdbRetrieve;
	Mdb_GetTableFields m_pMdbGetFields;
private:
	CSsp_Database *m_pDatabase;			//���ݿ������ļ�
// 	SMdb *m_pMdb;
// 	CMdbClient *m_pMdbClient;
};

#endif//__UK9001_MDB_PLUGIN_APPLICATION_H__
