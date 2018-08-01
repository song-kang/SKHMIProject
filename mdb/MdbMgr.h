/**
 *
 * �� �� �� : RealData.h
 * �������� : 2014-4-22 13:34
 * ��    �� : �ۿ���(skt001@163.com)
 * �޸����� : $Date: $
 * ��ǰ�汾 : $Revision: $
 * �������� : �ڴ����ݿ����ݽṹ����
 * �޸ļ�¼ : 
 *            $Log: $
 *
 * Ver  Date        Author  Comments
 * ---  ----------  ------  -------------------------------------------
 * 001	2014-4-22	�ۿ�������ļ�
 *
 **/

#ifndef __SKT_MDB_REALDATA_H__
#define __SKT_MDB_REALDATA_H__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "SList.h"
#include "SString.h"
#include "SHash.h"
#include "SDbf.h"
#include "db/mdb/MdbClient.h"

class CMdbField;
struct stuMdbCompare;
class CMdbRecord;
class CHisDbTable;
class CMdbManager;

//���������ֶ�����
#define C_MDB_MAX_FIELD_CNT 128

//����������������
#define C_MDB_MAX_INDEX_CNT 16

//����������ɢ������
#define C_MDB_PK_INDEX_HASHSIZE 512

//��󻺴津����¼����
#define C_MAX_BUFFERED_TRIGGER 10000

//�����ʷ��ͬ����¼����
#define C_MAX_BUFFERED_DBSYNC 100000

//���һ�β����¼����
#define C_MAX_INSERT_ONCE 100000

//������˻ص�����
typedef bool (*CB_Filter)(stuMdbCompare *pCmp,CMdbRecord *pRecord);

//��������ص�����,<0��ʾ��¼1С�ڼ�¼2��>0��ʾ��¼1���ڼ�¼2��==0��ʾ���
typedef int(*CB_FieldCompareFun)(CMdbRecord *pRecord1, CMdbRecord *pRecord2, CMdbField *m_pField);


enum eMdbCompareType
{
	MDB_CMP_BIGTHAN=1,		//���� >
	MDB_CMP_LESSTHAN,		//С�� <
	MDB_CMP_EQUAL,			//���� =
	MDB_CMP_BIGTHAN_EQUAL,	//���ڵ��� >=
	MDB_CMP_LESSTHAN_EQUAL,	//С�ڵ��� <=
	MDB_CMP_NOT_EQUAL,		//������ <>
	MDB_CMP_LIKE,			//�ַ������Ӵ� like
	MDB_CMP_NOT_LIKE,		//�ַ��������Ӵ� not like
};

#define MDB_SYS_TABLE "systable"
#define MDB_SQL_SYSTABLE_CREATE_TABLE "create table systable (name char(32),rows int,rowsize int)"



//////////////////////////////////////////////////////////////////////////
// ��    ��:  stuMdbCompare
// ��    ��:  �ۿ���
// ����ʱ��:  2014-04-22 15:20
// ��    ��:  �����Ƚ϶���
//////////////////////////////////////////////////////////////////////////
struct stuMdbCompare
{
	CMdbField *m_pField;		//�����ֶ�
	eMdbCompareType m_CmpType;	//�Ƚ�����
	CB_Filter m_pCallbackFun;	//�ص�����
	bool m_bAnd;				//����һ�����Ƿ�Ϊ���ϵ��false��ʾ���ϵ�����ֶ��ڵ�һ��������������

	//����ֵ�����ֶ�����ѡ������֮һ
	SString m_sCmpVal;			//�ַ����Ƚ�ֵ
	INT64 m_i64CmpVal;			//�����ͱȽ�ֵ
	int m_iCmpVal;				//���ͱȽ�ֵ
	float m_fCmpVal;			//����Ƚ�ֵ
};

//////////////////////////////////////////////////////////////////////////
// ��    ��:  stuMdbSetValue
// ��    ��:  �ۿ���
// ����ʱ��:  2014-04-22 15:25
// ��    ��:  ��ֵ����
//////////////////////////////////////////////////////////////////////////
struct stuMdbSetValue
{
	CMdbField *m_pField;		//����ֵ���ֶ�
	SString m_sValue;			//��ֵ
};

//////////////////////////////////////////////////////////////////////////
// ��    ��:  stuMdbSort
// ��    ��:  �ۿ���
// ����ʱ��:  2014-04-22 15:30
// ��    ��:  ������
//////////////////////////////////////////////////////////////////////////
struct stuMdbSort
{
	CMdbField *m_pField;		//��������ֶ�
	bool m_bAsc;				//�Ƿ�Ϊ��������
	CB_FieldCompareFun m_pCallbackFun;	//����ȽϵĻص�����
};


//////////////////////////////////////////////////////////////////////////
// ��    ��:  CMdbField
// ��    ��:  �ۿ���
// ����ʱ��:  2014-04-22 14:22
// ��    ��:  �ڴ���ֶ���
//////////////////////////////////////////////////////////////////////////
class CMdbField
{
public:
	CMdbField();
	~CMdbField();
	
	SString m_sName;		//�ֶ�����
	eMdbFieldType m_Type;	//�ֶ�����
	int m_iStartPos;		//��һ�м�¼�е���ʼλ�ã���N���ֽڣ���0��ʼ
	int m_iBytes;			//ֵ������ռ�洢�ֽ���
	bool m_bIsPK;			//�Ƿ�Ϊ����
	bool m_bIsNull;			//�Ƿ����Ϊ��,true��ʾ����Ϊ�գ�false��ʾ����Ϊ��
	SString m_sDefaultVal;	//ȱʡֵ
};

//////////////////////////////////////////////////////////////////////////
// ��    ��:  CMdbRecord
// ��    ��:  �ۿ���
// ����ʱ��:  2014-04-22 14:47
// ��    ��:  �ڴ���¼��
//////////////////////////////////////////////////////////////////////////
class CMdbRecord
{
public:
	CMdbRecord(BYTE *pRecData,int iLen/*,bool bClone=true*/);
	CMdbRecord();
	~CMdbRecord();
	//////////////////////////////////////////////////////////////////////////
	// ��    ��:  ȡָ���ֶε�ֵ����
	// ��    ��:  �ۿ���
	// ����ʱ��:  2014-6-11 17:09
	// ����˵��:  @pFieldΪ�ֶ�ָ��
	// �� �� ֵ:  NULL��ʾʧ�ܣ�����ʹ���ֶε�ƫ��
	//////////////////////////////////////////////////////////////////////////
	inline BYTE* GetValue(CMdbField *pField)
	{
		return m_pRecordData+pField->m_iStartPos;
	}
public:
	BYTE *m_pRecordData;	//��¼����
};

class CMdbTable;
//////////////////////////////////////////////////////////////////////////
// ��    ��:  CMdbTableIndex
// ��    ��:  �ۿ���
// ����ʱ��:  2014-6-11 8:56
// ��    ��:  �ڴ��������(Ϊ���ʹ����Ӷȣ��ݲ�ʹ������)
//////////////////////////////////////////////////////////////////////////
class CMdbTableIndex
{
public:
	friend class CMdbTable;
	CMdbTableIndex(int hash_size=1024);
	~CMdbTableIndex();

	//////////////////////////////////////////////////////////////////////////
	// ��    ��:  ���������������ָ��
	// ��    ��:  �ۿ���
	// ����ʱ��:  2014-6-12 8:37
	// ����˵��:  @p�������ڴ��ʵ��
	// �� �� ֵ:  void
	//////////////////////////////////////////////////////////////////////////
	inline void SetTablePtr(CMdbTable *p){m_pMdbTable = p;};

	//////////////////////////////////////////////////////////////////////////
	// ��    ��:  ɢ�м����ɺ���
	// ��    ��:  �ۿ���
	// ����ʱ��:  2014-6-11 15:25
	// ����˵��:  @pItem�����ɵ���
	// �� �� ֵ:  ��ֵ
	//////////////////////////////////////////////////////////////////////////
	static int SHash_Key_Idx(void *cb,void *pItem);

	//////////////////////////////////////////////////////////////////////////
	// ��    ��:  ��¼���ݵľ�ȷ�ȽϺ�����������������
	// ��    ��:  �ۿ���
	// ����ʱ��:  2014-6-11 15:28
	// ����˵��:  @pItem1Ϊ�Ƚϲ��ռ�¼1
	//         :  @pItem2Ϊ�Ƚϲ��ռ�¼2
	// �� �� ֵ:  true��ʾ��ȣ�false��ʾ�����
	//////////////////////////////////////////////////////////////////////////
	static bool SHash_Compare_Idx(void *cb,void *pItem1,void *pItem2);

	//////////////////////////////////////////////////////////////////////////
	// ��    ��:  ��������
	// ��    ��:  �ۿ���
	// ����ʱ��:  2014-6-11 15:31
	// ����˵��:  @fieldsΪ�����ֶ��б�
	//            @bUnique��ʾ�Ƿ�ΪΨһ��
	//            @bPk��ʾ�Ƿ�Ϊ����
	// �� �� ֵ:  true��ʾ�����ɹ���false��ʾ����ʧ��
	//////////////////////////////////////////////////////////////////////////
	bool CreateIndex(SPtrList<CMdbField> *fields, bool bUnique = false, bool bPk = false);

	//////////////////////////////////////////////////////////////////////////
	// ��    ��:  ���һ��������¼
	// ��    ��:  �ۿ���
	// ����ʱ��:  2014-6-11 15:32
	// ����˵��:  @pRowΪ�¼�¼ָ��
	// �� �� ֵ:  true��ʾ��ӳɹ���false��ʾ���ʧ��
	//////////////////////////////////////////////////////////////////////////
	bool AddRecord(CMdbRecord *pRow);

	//////////////////////////////////////////////////////////////////////////
	// ��    ��:  ɾ��һ��������¼
	// ��    ��:  �ۿ���
	// ����ʱ��:  2014-6-11 15:32
	// ����˵��:  @pRowΪ��ɾ���ļ�¼ָ��
	// �� �� ֵ:  true��ʾɾ���ɹ���false��ʾɾ��ʧ��
	//////////////////////////////////////////////////////////////////////////
	bool RemoveRecord(CMdbRecord *pRow);

	//////////////////////////////////////////////////////////////////////////
	// ��    ��:  �������������¼
	// ��    ��:  �ۿ���
	// ����ʱ��:  2014-6-11 15:33
	// ����˵��:  void
	// �� �� ֵ:  true��ʾ��ճɹ���false��ʾ���ʧ��
	//////////////////////////////////////////////////////////////////////////
	bool ClearRecord();

	//////////////////////////////////////////////////////////////////////////
	// ��    ��:  ��ָ���Ĳο���¼�е�������ֵ����ƥ������м�¼
	// ��    ��:  �ۿ���
	// ����ʱ��:  2015-1-16 10:38
	// ����˵��:  @pObj�ο�����ָ��
	//            @listΪ���÷��ص�ƥ���¼����
	// �� �� ֵ:  >0ƥ���¼��������<0��ʾʧ��,=0��ʾû��ƥ���¼
	//////////////////////////////////////////////////////////////////////////
	int SearchByIndex(CMdbRecord *pObj, SPtrList<CMdbRecord> &list);
	CMdbRecord* SearchOnByIndex(CMdbRecord *pObj);

	//////////////////////////////////////////////////////////////////////////
	// ��    ��:  �ۼӸ��ֽ�����
	// ��    ��:  �ۿ���
	// ����ʱ��:  2014-6-11 17:14
	// ����˵��:  @pBuf�ֽڻ�����
	//         :  @iSizeΪ���峤��
	// �� �� ֵ:  int
	//////////////////////////////////////////////////////////////////////////
	inline int AddBytes(BYTE* pBuf,int iSize)
	{
		register int i,ret = 0;
		for(i=0;i<iSize;i++)
		{
			ret += pBuf[i];
		}
		return ret;
	}

	inline void SetName(SString s){ m_sIdxName = s; };
	inline SString GetName(){ return m_sIdxName; };
	inline int GetHashSize(){ return m_IdxHash.GetHashSize(); };
	SPtrList<CMdbField>* GetIndexFieldsPtr(){ return &m_IndexFiends; };
	inline bool IsPk(){ return m_bPk; };
	inline bool IsUnique(){ return m_bUnique; };

private:
	SString m_sIdxName;
	SPtrList<CMdbField> m_IndexFiends;//�����ֶΣ�һ���������԰�������ֶ�
	class CMdbTable *m_pMdbTable;//�����ڴ��
	bool m_bPk;//�Ƿ�Ϊ����
	bool m_bUnique;//�Ƿ�ΪΨһ��
	//SPtrList<CMdbRecord> m_Records;	//��¼�б�
	SGeneralHash<CMdbRecord,SHash_Key_Idx,SHash_Compare_Idx> m_IdxHash;//ɢ������
};


//////////////////////////////////////////////////////////////////////////
// ��    ��:  CTableTrigger
// ��    ��:  �ۿ���
// ����ʱ��:  2014-10-15 16:35
// ��    ��:  �����ݵĴ�����¼
//////////////////////////////////////////////////////////////////////////
class CTableTrigger
{
public:
	CTableTrigger()
	{
		m_pTrgRowsData = NULL;
	}
	~CTableTrigger()
	{
		if (m_pTrgRowsData != NULL)
			delete[] m_pTrgRowsData;
	}

	int iTriggerType;//�������ͣ�1-insert 2-update 3-delete 4-truncate(delete all rows)
	BYTE *m_pTrgRowsData;//�����ļ�¼���ݣ���4-truncateʱ�������ݣ�
	int m_iTrgRows;//�����ļ�¼��������
};

//////////////////////////////////////////////////////////////////////////
// ��    ��:  CMdbTable
// ��    ��:  �ۿ���
// ����ʱ��:  2014-04-22 14:42
// ��    ��:  �ڴ�ʽ���ݱ�
//////////////////////////////////////////////////////////////////////////
class CMdbTable
{
public:
	friend class CMdbTableIndex;
	friend class CHisDbTable;
	friend class CMdbManager;
	CMdbTable();
	~CMdbTable();

	////////////////////////////////////////////////////////////////////////
	// ��    ��:  ��ȡָ���ֶκŶ�Ӧ���ֶ�����
	// ��    ��:  �ۿ���
	// ����ʱ��:  2014-04-22 14:38
	// ����˵��:  @iFieldNoΪ�ֶ���ţ���0��ʼ���
	// �� �� ֵ:  ��Ӧ�ֶ����ƣ��մ���ʾָ����ŵ��ֶβ�����
	//////////////////////////////////////////////////////////////////////////
	SString GetFieldName(int iFieldNo);

	////////////////////////////////////////////////////////////////////////
	// ��    ��:  ����ָ���ֶ����ƶ�Ӧ���ֶ�ʵ��
	// ��    ��:  �ۿ���
	// ����ʱ��:  2014-04-23 16:18
	// ����˵��:  @sFieldNameΪ�ֶ�����
	// �� �� ֵ:  CMdbField*
	//////////////////////////////////////////////////////////////////////////
	CMdbField* SearchFieldByName(char* sFieldName);

	////////////////////////////////////////////////////////////////////////
	// ��    ��:  ��ȡָ���ֶ����ƶ�Ӧ���ֶκ�
	// ��    ��:  �ۿ���
	// ����ʱ��:  2014-04-22 14:39
	// ����˵��:  @sFieldNameΪ�ֶ����ƣ������ִ�Сд
	// �� �� ֵ:  �����ֶκţ���0��ʼ��ţ�<0��ʾ�ֶ���������
	//////////////////////////////////////////////////////////////////////////
	int GetFieldNo(char* sFieldName);

	////////////////////////////////////////////////////////////////////////
	// ��    ��:  ����1�л���м�¼
	// ��    ��:  �ۿ���
	// ����ʱ��:  2014-04-22 15:05
	// ����˵��:  @pRecData��ʾ��¼�������������ڲ��޸�ΪNULL�����ݸ����ͷ�
	//         :  @iRowSize��ʾһ�м�¼�ĳ��ȣ��ֽ�����
	//         :  @iRows��ʾ�ж�������¼�����룬Ĭ��Ϊ1
	//		   :  @bHisRestore��ʾ�Ƿ�Ϊ��ʷ���ڻָ�����
	// �� �� ֵ:  �ɹ�����ļ�¼����
	//////////////////////////////////////////////////////////////////////////
	int Insert(BYTE* &pRecData,int iRowSize,int iRows=1,bool bHisRestore=false);

	////////////////////////////////////////////////////////////////////////
	// ��    ��:  ���»����һ����¼��������ΪΨһ��ʶ,�������ı�����ʹ�ã������¼���ڸ��£������������
	// ��    ��:  �ۿ���
	// ����ʱ��:  2014-04-22 15:05
	// ����˵��:  @pRecData��ʾ��¼�������������ڲ��޸�ΪNULL�����ݸ����ͷ�
	//         :  @iRowSize��ʾһ�м�¼�ĳ��ȣ��ֽ�����
	//         :  @iRows��ʾ�ж�������¼�����룬Ĭ��Ϊ1
	//		   :  @bHisRestore��ʾ�Ƿ�Ϊ��ʷ���ڻָ�����
	// �� �� ֵ:  �ɹ����»����ļ�¼����
	//////////////////////////////////////////////////////////////////////////
	int UpdateWithInsert(BYTE* &pRecData, int iRowSize, int iRows = 1);

	////////////////////////////////////////////////////////////////////////
	// ��    ��:  ɾ��ָ�������ļ�¼
	// ��    ��:  �ۿ���
	// ����ʱ��:  2014-04-22 15:22
	// ����˵��:  @Condition��ʾ��������
	// �� �� ֵ:  �ɹ�ɾ���ļ�¼������<0��ʾʧ��
	//////////////////////////////////////////////////////////////////////////
	int Delete(SPtrList<stuMdbCompare> &Condition);

	////////////////////////////////////////////////////////////////////////
	// ��    ��:  ����ָ��������¼�е�ָ��ֵΪ������
	// ��    ��:  �ۿ���
	// ����ʱ��:  2014-04-22 15:26
	// ����˵��:  @SetValueΪ��ֵ�б�
	//         :  @Condition��ʾ��������
	// �� �� ֵ:  �ɹ����µļ�¼������<0��ʾʧ��
	//////////////////////////////////////////////////////////////////////////
	int Update(SPtrList<stuMdbSetValue> &SetValue, SPtrList<stuMdbCompare> &Condition);

	////////////////////////////////////////////////////////////////////////
	// ��    ��:  ����BLOB�ֶ�
	// ��    ��:  �ۿ���
	// ����ʱ��:  2014-12-22 16:20
	// ����˵��:  @pBlobBufΪ�����µĴ��ֶλ���������
	//         :  @iBlobSize��ʾ��������С
	//         :  @sFieldNameΪ���ֶ�����
	//         :  @ConditionΪָ����������ָ������Ӧ����һ��Ψһ�ļ�¼
	// �� �� ֵ:  �ɹ����µļ�¼������<0��ʾʧ�ܣ�=0��ʾδ����
	//////////////////////////////////////////////////////////////////////////
	int UpdateBlob(BYTE *pBlobBuf, int iBlobSize, SString &sFieldName, SPtrList<stuMdbCompare> &Condition);

	////////////////////////////////////////////////////////////////////////
	// ��    ��:  ��ȡBLOB�ֶ�
	// ��    ��:  �ۿ���
	// ����ʱ��:  2014-12-22 16:26
	// ����˵��:  @pBlobBufΪ����ȡ�Ĵ��ֶλ��������ݣ��ں������뻺�����������߸����ͷ��ڴ�
	//         :  @iBlobSize��ʾ��������С
	//         :  @sFieldNameΪ���ֶ�����
	//         :  @ConditionΪָ����������ָ������Ӧ����һ��Ψһ�ļ�¼
	// �� �� ֵ:  �ɹ���ȡ�ļ�¼������<0��ʾʧ�ܣ�=0��ʾδ����
	//////////////////////////////////////////////////////////////////////////
	int ReadBlob(BYTE* &pBlobBuf, int &iBlobSize, SString sFieldName, SPtrList<stuMdbCompare> &Condition);

	////////////////////////////////////////////////////////////////////////
	// ��    ��:  �ضϱ��ȫ������
	// ��    ��:  �ۿ���
	// ����ʱ��:  2014-04-22 15:27
	// ����˵��:  void
	// �� �� ֵ:  ���ضϼ�¼������
	//////////////////////////////////////////////////////////////////////////
	int Truncate();

	////////////////////////////////////////////////////////////////////////
	// ��    ��:  ��ѯ�����ļ�¼������ָ����������
	// ��    ��:  �ۿ���
	// ����ʱ��:  2014-04-22 15:31
	// ����˵��:  @Condition��ʾ��������
	//         :  @Sort��ʾ��������������ָ������ֶ�Ϊ������������ָ���������л�������
	//         :  @Result��ʾ�����¼��
	//         :  @pIndexRows��ʾ�������Ĳ��ּ�¼���ϣ�NULL��ʾû��ƥ����������ϣ�ʹ��ȫ��¼���ˣ�
	//         :  @iLimit��ʾ���Ƽ�¼������0��ʾ������
	//         :  @bAutoDel��ʾ�Ƿ��Զ�ɾ��ѡ�еļ�¼
	// �� �� ֵ:  ����ѯ���ļ�¼������<0��ʾʧ��
	//////////////////////////////////////////////////////////////////////////
	int Select(SPtrList<stuMdbCompare> &Condition, SPtrList<stuMdbSort> &Sort, SPtrList<CMdbRecord> &Result, 
			   SPtrList<CMdbRecord> *pIndexRows=NULL,int iLimit=0,bool bAutoDel=false);

	////////////////////////////////////////////////////////////////////////
	// ��    ��:  ��ѯ���������ļ�¼����
	// ��    ��:  �ۿ���
	// ����ʱ��:  2014-11-5 11:03
	// ����˵��:  @Condition��ʾ��������
	//         :  @pIndexRows��ʾ�������Ĳ��ּ�¼���ϣ�NULL��ʾû��ƥ����������ϣ�ʹ��ȫ��¼���ˣ�
	// �� �� ֵ:  ����ѯ���ļ�¼������<0��ʾʧ��
	//////////////////////////////////////////////////////////////////////////
	int Count(SPtrList<stuMdbCompare> &Condition, SPtrList<CMdbRecord> *pIndexRows = NULL);

	////////////////////////////////////////////////////////////////////////
	// ��    ��:  ȡ����
	// ��    ��:  �ۿ���
	// ����ʱ��:  2014-04-23 12:03
	// ����˵��:  void
	// �� �� ֵ:  SString
	//////////////////////////////////////////////////////////////////////////
	inline SString GetTableName(){return m_sTableName;};

	////////////////////////////////////////////////////////////////////////
	// ��    ��:  ȡһ�м�¼���ܳ���
	// ��    ��:  �ۿ���
	// ����ʱ��:  2014-04-24 11:32
	// ����˵��:  void
	// �� �� ֵ:  int
	//////////////////////////////////////////////////////////////////////////
	inline int GetRowSize(){return m_iRowSize;};

	////////////////////////////////////////////////////////////////////////
	// ��    ��:  ȡ��ǰ���ܼ�¼��
	// ��    ��:  �ۿ���
	// ����ʱ��:  2014-04-24 11:43
	// ����˵��:  void
	// �� �� ֵ:  int
	//////////////////////////////////////////////////////////////////////////
	inline int GetRows(){return m_Records.count();};

	////////////////////////////////////////////////////////////////////////
	// ��    ��:  ȡָ����ŵļ�¼
	// ��    ��:  �ۿ���
	// ����ʱ��:  2014-04-24 11:42
	// ����˵��:  @idxΪ��¼��ţ���0��ʼ
	// �� �� ֵ:  CMdbRecord*
	//////////////////////////////////////////////////////////////////////////
	inline CMdbRecord* GetRecord(int idx){return m_Records[idx];};

	////////////////////////////////////////////////////////////////////////
	// ��    ��:  �����±�
	// ��    ��:  �ۿ���
	// ����ʱ��:  2014-04-23 15:17
	// ����˵��:  @sqlΪ����SQL�﷨
	//         :  @sLastErrorΪ��������������
	// �� �� ֵ:  MDB_RET_SUCCESS��ʾ�ɹ���������ʾʧ��
	//////////////////////////////////////////////////////////////////////////
	int CreateTable(SString sql,char* sLastError);

	////////////////////////////////////////////////////////////////////////
	// ��    ��:  ȡ��¼��ָ����ŵ��ֶ��ַ���ֵ����
	// ��    ��:  �ۿ���
	// ����ʱ��:  2014-11-14 10:46
	// ����˵��:  @pRowΪ��¼����
	//			  @pFldΪ�ֶ���Ϣ
	// �� �� ֵ:  �ַ�����ֵ����
	//////////////////////////////////////////////////////////////////////////
	SString GetValueStr(BYTE *pRow, CMdbField *pFld);

	////////////////////////////////////////////////////////////////////////
	// ��    ��:  ȡ��¼��ָ����ŵ��ֶ�����ֵ����
	// ��    ��:  �ۿ���
	// ����ʱ��:  2014-11-14 10:46
	// ����˵��:  @pRowΪ��¼����
	//			  @pFldΪ�ֶ���Ϣ
	// �� �� ֵ:  �ַ�����ֵ����
	//////////////////////////////////////////////////////////////////////////
	int GetValueInt(BYTE *pRow, CMdbField *pFld);

	////////////////////////////////////////////////////////////////////////
	// ��    ��:  ȡ��¼��ָ����ŵ��ֶθ�����ֵ����
	// ��    ��:  �ۿ���
	// ����ʱ��:  2014-11-14 10:46
	// ����˵��:  @pRowΪ��¼����
	//			  @pFldΪ�ֶ���Ϣ
	// �� �� ֵ:  �ַ�����ֵ����
	//////////////////////////////////////////////////////////////////////////
	float GetValueFloat(BYTE *pRow, CMdbField *pFld);

	////////////////////////////////////////////////////////////////////////
	// ��    ��:  ȡָ�������µĻ��ܺ���ִ�н��
	// ��    ��:  �ۿ���
	// ����ʱ��:  2014-12-22 17:12
	// ����˵��:  @ConditionΪ��������
	//			  @sFunNameΪ��������(count,sum,min,max,avg)
	//			  @pFieldΪ��Ӧ�ֶΣ�NULL��ʾ����Ӧ�����ֶ�
	// �� �� ֵ:  �ַ�����ֵ����
	//////////////////////////////////////////////////////////////////////////
	SString GetTotalFunctonVal(SPtrList<stuMdbCompare> &Condition, SString sFunName, CMdbField *pField);

protected:

	////////////////////////////////////////////////////////////////////////
	// ��    ��:  �����ݱ���Ӷ�Ӧ������������ֻ�Ӳ�ɾ
	// ��    ��:  �ۿ���
	// ����ʱ��:  2014-9-18 09:22
	// ����˵��:  @pIdxΪ����ָ��
	// �� �� ֵ:  void
	//////////////////////////////////////////////////////////////////////////
	void AddIndex(CMdbTableIndex *pIdx);

	////////////////////////////////////////////////////////////////////////
	// ��    ��:  ��ָ�����ݼ���������
	// ��    ��:  �ۿ���
	// ����ʱ��:  2014-9-18 09:22
	// ����˵��:  @SortΪ��������
	//			  @SrcResultΪԴ��¼��
	//			  @DstResultΪĿ���¼��
	// �� �� ֵ:  void
	//////////////////////////////////////////////////////////////////////////
	void SortResultset(SPtrList<stuMdbSort> &Sort, SPtrList<CMdbRecord> &SrcResult, SPtrList<CMdbRecord> &DstResult);

public:
	////////////////////////////////////////////////////////////////////////
	// ��    ��:  �ж�ָ����¼�ڸ��������������Ƿ�����
	// ��    ��:  �ۿ���
	// ����ʱ��:  2014-04-22 15:44
	// ����˵��:  @pRecordΪ���Ƚϵļ�¼
	//         :  @ConditionΪ��������
	// �� �� ֵ:  true��ʾ��������,false��ʾ����������
	//////////////////////////////////////////////////////////////////////////
	bool FilterRecord(CMdbRecord *pRecord, SPtrList<stuMdbCompare> &Condition);

	////////////////////////////////////////////////////////////////////////
	// ��    ��:  �趨ָ����¼���ֶ�ֵ
	// ��    ��:  �ۿ���
	// ����ʱ��:  2014-11-25 08:41
	// ����˵��:  @pRecΪ����ֵ�ļ�¼
	//         :  @SetValueΪ��ֵ�ֶμ�ֵ
	// �� �� ֵ:  true��ʾ��������,false��ʾ����������
	//////////////////////////////////////////////////////////////////////////
	bool SetRecordValue(CMdbRecord *pRec, SPtrList<stuMdbSetValue> &SetValue);

	////////////////////////////////////////////////////////////////////////
	// ��    ��:  �������˸��������µ�Ԥ������
	// ��    ��:  �ۿ���
	// ����ʱ��:  2014-04-23 10:29
	// ����˵��:  @pCmpΪ�Ƚ�����
	//         :  @pRecordΪ��¼ʵ��
	// �� �� ֵ:  true��ʾ����������false��ʾ����������
	//////////////////////////////////////////////////////////////////////////
	static bool CB_Filter_INT_BIGTHAN(stuMdbCompare *pCmp,CMdbRecord *pRecord);
	static bool CB_Filter_INT_LESSTHAN(stuMdbCompare *pCmp,CMdbRecord *pRecord);
	static bool CB_Filter_INT_EQUAL(stuMdbCompare *pCmp,CMdbRecord *pRecord);
	static bool CB_Filter_INT_BIGTHAN_EQUAL(stuMdbCompare *pCmp,CMdbRecord *pRecord);
	static bool CB_Filter_INT_LESSTHAN_EQUAL(stuMdbCompare *pCmp,CMdbRecord *pRecord);
	static bool CB_Filter_INT_NOT_EQUAL(stuMdbCompare *pCmp,CMdbRecord *pRecord);

	static bool CB_Filter_INT64_BIGTHAN(stuMdbCompare *pCmp,CMdbRecord *pRecord);
	static bool CB_Filter_INT64_LESSTHAN(stuMdbCompare *pCmp,CMdbRecord *pRecord);
	static bool CB_Filter_INT64_EQUAL(stuMdbCompare *pCmp,CMdbRecord *pRecord);
	static bool CB_Filter_INT64_BIGTHAN_EQUAL(stuMdbCompare *pCmp,CMdbRecord *pRecord);
	static bool CB_Filter_INT64_LESSTHAN_EQUAL(stuMdbCompare *pCmp,CMdbRecord *pRecord);
	static bool CB_Filter_INT64_NOT_EQUAL(stuMdbCompare *pCmp,CMdbRecord *pRecord);

	static bool CB_Filter_SHORT_BIGTHAN(stuMdbCompare *pCmp,CMdbRecord *pRecord);
	static bool CB_Filter_SHORT_LESSTHAN(stuMdbCompare *pCmp,CMdbRecord *pRecord);
	static bool CB_Filter_SHORT_EQUAL(stuMdbCompare *pCmp,CMdbRecord *pRecord);
	static bool CB_Filter_SHORT_BIGTHAN_EQUAL(stuMdbCompare *pCmp,CMdbRecord *pRecord);
	static bool CB_Filter_SHORT_LESSTHAN_EQUAL(stuMdbCompare *pCmp,CMdbRecord *pRecord);
	static bool CB_Filter_SHORT_NOT_EQUAL(stuMdbCompare *pCmp,CMdbRecord *pRecord);

	static bool CB_Filter_BYTE_BIGTHAN(stuMdbCompare *pCmp,CMdbRecord *pRecord);
	static bool CB_Filter_BYTE_LESSTHAN(stuMdbCompare *pCmp,CMdbRecord *pRecord);
	static bool CB_Filter_BYTE_EQUAL(stuMdbCompare *pCmp,CMdbRecord *pRecord);
	static bool CB_Filter_BYTE_BIGTHAN_EQUAL(stuMdbCompare *pCmp,CMdbRecord *pRecord);
	static bool CB_Filter_BYTE_LESSTHAN_EQUAL(stuMdbCompare *pCmp,CMdbRecord *pRecord);
	static bool CB_Filter_BYTE_NOT_EQUAL(stuMdbCompare *pCmp,CMdbRecord *pRecord);

	static bool CB_Filter_FLOAT_BIGTHAN(stuMdbCompare *pCmp,CMdbRecord *pRecord);
	static bool CB_Filter_FLOAT_LESSTHAN(stuMdbCompare *pCmp,CMdbRecord *pRecord);
	static bool CB_Filter_FLOAT_EQUAL(stuMdbCompare *pCmp,CMdbRecord *pRecord);
	static bool CB_Filter_FLOAT_BIGTHAN_EQUAL(stuMdbCompare *pCmp,CMdbRecord *pRecord);
	static bool CB_Filter_FLOAT_LESSTHAN_EQUAL(stuMdbCompare *pCmp,CMdbRecord *pRecord);
	static bool CB_Filter_FLOAT_NOT_EQUAL(stuMdbCompare *pCmp,CMdbRecord *pRecord);

	static bool CB_Filter_STR_BIGTHAN(stuMdbCompare *pCmp,CMdbRecord *pRecord);
	static bool CB_Filter_STR_LESSTHAN(stuMdbCompare *pCmp,CMdbRecord *pRecord);
	static bool CB_Filter_STR_EQUAL(stuMdbCompare *pCmp,CMdbRecord *pRecord);
	static bool CB_Filter_STR_BIGTHAN_EQUAL(stuMdbCompare *pCmp,CMdbRecord *pRecord);
	static bool CB_Filter_STR_LESSTHAN_EQUAL(stuMdbCompare *pCmp,CMdbRecord *pRecord);
	static bool CB_Filter_STR_NOT_EQUAL(stuMdbCompare *pCmp,CMdbRecord *pRecord);
	static bool CB_Filter_STR_LIKE(stuMdbCompare *pCmp,CMdbRecord *pRecord);
	static bool CB_Filter_STR_NOT_LIKE(stuMdbCompare *pCmp,CMdbRecord *pRecord);

	static int CB_FieldCompareInt(CMdbRecord *pRecord1, CMdbRecord *pRecord2, CMdbField *pField);
	static int CB_FieldCompareInt64(CMdbRecord *pRecord1, CMdbRecord *pRecord2, CMdbField *pField);
	static int CB_FieldCompareShort(CMdbRecord *pRecord1, CMdbRecord *pRecord2, CMdbField *pField);
	static int CB_FieldCompareByte(CMdbRecord *pRecord1, CMdbRecord *pRecord2, CMdbField *pField);
	static int CB_FieldCompareFloat(CMdbRecord *pRecord1, CMdbRecord *pRecord2, CMdbField *pField);
	static int CB_FieldCompareStr(CMdbRecord *pRecord1, CMdbRecord *pRecord2, CMdbField *pField);


	////////////////////////////////////////////////////////////////////////
	// ��    ��:  ���ö�systab���¼ָ��
	// ��    ��:  �ۿ���
	// ����ʱ��:  2014-04-24 15:01
	// ����˵��:  stuSysTable*
	// �� �� ֵ:  void
	//////////////////////////////////////////////////////////////////////////
	inline void SetSysTableRecord(stuSysTable* p){m_pSysTableRecord=p;};

	////////////////////////////////////////////////////////////////////////
	// ��    ��:  ȡsystab��¼ָ��
	// ��    ��:  �ۿ���
	// ����ʱ��:  2014-04-24 15:01
	// ����˵��:  void
	// �� �� ֵ:  stuSysTable*
	//////////////////////////////////////////////////////////////////////////
	inline stuSysTable* GetSysTableRecord(){return m_pSysTableRecord;};

	//////////////////////////////////////////////////////////////////////////
	// ��    ��:  ȡ�м�¼ָ��
	// ��    ��:  �ۿ���
	// ����ʱ��:  2014-6-12 8:49
	// ����˵��:  void
	// �� �� ֵ:  SPtrList<CMdbRecord>*
	//////////////////////////////////////////////////////////////////////////
	inline SPtrList<CMdbRecord>* GetRecordPtr(){return &m_Records;};

	//////////////////////////////////////////////////////////////////////////
	// ��    ��:  ȡ�����ֶε�ָ��
	// ��    ��:  �ۿ���
	// ����ʱ��:  2014-6-12 8:49
	// ����˵��:  void
	// �� �� ֵ:  SPtrList<CMdbField>*
	//////////////////////////////////////////////////////////////////////////
	inline SPtrList<CMdbField>* GetPkFieldPtr(){ return &m_PkFields; };

	//////////////////////////////////////////////////////////////////////////
	// ��    ��:  ȡ�ֶε�ָ��
	// ��    ��:  �ۿ���
	// ����ʱ��:  2014-11-12 18:19
	// ����˵��:  void
	// �� �� ֵ:  SPtrList<CMdbField>*
	//////////////////////////////////////////////////////////////////////////
	inline SPtrList<CMdbField>* GetFieldPtr(){ return &m_Fields; };

	//////////////////////////////////////////////////////////////////////////
	// ��    ��:  �����������ļ�¼����
	// ��    ��:  �ۿ���
	// ����ʱ��:  2014-11-20 09:27
	// ����˵��:  @rΪ�������
	// �� �� ֵ:  void
	//////////////////////////////////////////////////////////////////////////
	inline void SetMemMaxRows(int r){ m_iMemMaxRows = r; };
	inline void SetHisMaxRows(int r){ m_iHisMaxRows = r; };

	//////////////////////////////////////////////////////////////////////////
	// ��    ��:  ȡ�������ļ�¼����
	// ��    ��:  �ۿ���
	// ����ʱ��:  2014-11-20 09:27
	// ����˵��:  @rΪ�������
	// �� �� ֵ:  void
	//////////////////////////////////////////////////////////////////////////
	inline int GetMemMaxRows(){ return m_iMemMaxRows; };
	inline int GetHisMaxRows(){ return m_iHisMaxRows; };

	//////////////////////////////////////////////////////////////////////////
	// ��    ��:  ȡ��������ָ��
	// ��    ��:  �ۿ���
	// ����ʱ��:  2014-11-20 09:27
	// ����˵��:  void
	// �� �� ֵ:  SPtrList<CMdbTableIndex>*
	//////////////////////////////////////////////////////////////////////////
	inline SPtrList<CMdbTableIndex>* GetIndexPtr(){ return &m_MyIndex; };

	//////////////////////////////////////////////////////////////////////////
	// ��    ��:  �������ݿ����ָ��
	// ��    ��:  �ۿ���
	// ����ʱ��:  2014-11-21 13:46
	// ����˵��:  @p
	// �� �� ֵ:  void
	//////////////////////////////////////////////////////////////////////////
	inline void SetMdbMgr(CMdbManager *p){ m_pMgr = p; };

	inline void lock(){m_Lock.lock();};
	inline void unlock(){m_Lock.unlock();};

private:
	SPtrList<CMdbField> m_Fields;	//�ֶ��б�
	SPtrList<CMdbRecord> m_Records;	//��¼�б�
	int m_iRowSize;					//һ�м�¼�Ĺ̶�����
	SPtrList<CMdbField> m_PkFields;	//�����ֶ��б�������ʱ���뼰���²�����Ҫ���������Ψһ��
	SString m_sTableName;			//������ǿ��Сд
	stuSysTable *m_pSysTableRecord;	//��Ӧϵͳ���еļ�¼ָ��
	SPtrList<CMdbTableIndex> m_MyIndex;//��ǰ���Ӧ������ʵ��
	CMdbTableIndex *m_pPkIndex;		//��������
	BYTE m_TriggerFlag;//����ѡ��,(bit0:insert,bit1:update,bit2:delete,bit3:truncate)
	int m_iMemMaxRows;	//�ڴ������������¼����
	int m_iHisMaxRows;	//��ʷ�������������¼����
	CMdbManager *m_pMgr;//������ָ��
	CHisDbTable *m_pHisDbTable;//��ʷ�����
	SLock m_Lock;
public:
	//1-insert 2-update 3-delete 4-truncate(delete all rows)
	SPtrList<CTableTrigger> m_InsertTrigger;//���봥����Ϣ�б�
	SPtrList<CTableTrigger> m_UpdateTrigger;//���´�����Ϣ�б�
	SPtrList<CTableTrigger> m_DeleteTrigger;//ɾ��������Ϣ�б�
	SPtrList<CTableTrigger> m_TruncateTrigger;//��մ�����Ϣ�б�

};


//////////////////////////////////////////////////////////////////////////
// ��    ��:  CMdbManager
// ��    ��:  �ۿ���
// ����ʱ��:  2014-04-23 11:56
// ��    ��:  �ڴ����ݿ������
//////////////////////////////////////////////////////////////////////////
class CMdbManager
{
public:
	struct stuSessionConnectTrigger
	{
		void *m_pSession;
		CMdbTable *m_pTable;
		BYTE m_TriggerFlag;//bit0:insert; 1:update; 2:delete; 3:truncate
	};


	//////////////////////////////////////////////////////////////////////////
	// ��    ��:  stuMdbMgrHisDbSyncOper
	// ��    ��:  �ۿ���
	// ����ʱ��:  2014-11:24 19:40
	// ��    ��:  ��ʷ��ͬ����������
	//////////////////////////////////////////////////////////////////////////
	struct stuMdbMgrHisDbSyncOper
	{
		stuMdbMgrHisDbSyncOper(eMdbTriggerType TrgType, CHisDbTable *p, BYTE *pInsert, int iRows)
		{
			m_pHisTable = p;
			m_OperFlag = MDB_TRG_INSERT;
			m_pInsertRowData = pInsert;
			m_iInsertRows = iRows;
			Condition.setAutoDelete(true);
			SetValue.setAutoDelete(true);
		};
		stuMdbMgrHisDbSyncOper(eMdbTriggerType TrgType, CHisDbTable *p, SPtrList<stuMdbCompare> &con, SPtrList<stuMdbSetValue> &set)
		{
			m_pHisTable = p;
			m_OperFlag = MDB_TRG_UPDATE;
			m_pInsertRowData = NULL;
			m_iInsertRows = 0;
			Condition.setAutoDelete(true);
			SetValue.setAutoDelete(true);
			con.setAutoDelete(false);
			con.copyto(Condition);
			set.setAutoDelete(false);
			set.copyto(SetValue);
		};
		stuMdbMgrHisDbSyncOper(eMdbTriggerType TrgType, CHisDbTable *p, SPtrList<stuMdbCompare> &con)
		{
			m_pHisTable = p;
			m_OperFlag = MDB_TRG_DELETE;
			m_pInsertRowData = NULL;
			m_iInsertRows = 0;
			con.setAutoDelete(false);
			con.copyto(Condition);
			Condition.setAutoDelete(true);
			SetValue.setAutoDelete(true);
		};
		stuMdbMgrHisDbSyncOper(eMdbTriggerType TrgType, CHisDbTable *p)
		{
			m_pHisTable = p;
			m_OperFlag = MDB_TRG_TRUNCATE;
			m_pInsertRowData = NULL;
			m_iInsertRows = 0;
		};

		~stuMdbMgrHisDbSyncOper()
		{
			if (m_pInsertRowData != NULL)
				delete[] m_pInsertRowData;
		}
		CHisDbTable *m_pHisTable;
		eMdbTriggerType m_OperFlag;//1:insert; 2:update; 3:delete; 4:truncate
		BYTE *m_pInsertRowData;
		int m_iInsertRows;
		SPtrList<stuMdbCompare> Condition;
		SPtrList<stuMdbSetValue> SetValue;
	};

	CMdbManager();
	virtual ~CMdbManager();

	////////////////////////////////////////////////////////////////////////
	// ��    ��:  ���ݱ����Ʋ�ѯ��ʵ��ָ��
	// ��    ��:  �ۿ���
	// ����ʱ��:  2014-04-23 12:01
	// ����˵��:  sTableNameΪ������
	// �� �� ֵ:  ��ʵ��ָ�룬NULL��ʾδ��ָ����
	//////////////////////////////////////////////////////////////////////////
	CMdbTable* SearchTableByName(char* sTableName);

	////////////////////////////////////////////////////////////////////////
	// ��    ��:  ���ݱ����Ʋ�ѯ����
	// ��    ��:  �ۿ���
	// ����ʱ��:  2014-04-23 12:50
	// ����˵��:  @sTableNameΪ������
	// �� �� ֵ:  ��ľ����0��ʾδ��ָ����
	//////////////////////////////////////////////////////////////////////////
	inline long GetTableHandle(char* sTableName){return (long)SearchTableByName(sTableName);};

	////////////////////////////////////////////////////////////////////////
	// ��    ��:  ��ʼ���ڴ����ݿ������
	// ��    ��:  �ۿ���
	// ����ʱ��:  2014-04-24 11:25
	// ����˵��:  @sParamsΪ����
	// �� �� ֵ:  0��ʾ�ɹ���������ʾʧ��
	//////////////////////////////////////////////////////////////////////////
	int Init(SString sParams);

	////////////////////////////////////////////////////////////////////////
	// ��    ��:  ������ʷ��
	// ��    ��:  �ۿ���
	// ����ʱ��:  2014-11-17 09:25
	// ����˵��:  void
	// �� �� ֵ:  0��ʾ�ɹ���������ʾʧ��
	//////////////////////////////////////////////////////////////////////////
	int LoadHisDb();

	////////////////////////////////////////////////////////////////////////
	// ��    ��:  �����µı�
	// ��    ��:  �ۿ���
	// ����ʱ��:  2014-04-23 15:06
	// ����˵��:  @sqlΪ����SQL�﷨
	// �� �� ֵ:  MDB_RET_SUCCESS��ʾ�ɹ���������ʾʧ��
	//////////////////////////////////////////////////////////////////////////
	int CreateTable(SString sql);

	////////////////////////////////////////////////////////////////////////
	// ��    ��:  �����µı�����
	// ��    ��:  �ۿ���
	// ����ʱ��:  2014-9-17 18:53
	// ����˵��:  @sqlΪ������SQL�﷨
	// �� �� ֵ:  MDB_RET_SUCCESS��ʾ�ɹ���������ʾʧ��
	//////////////////////////////////////////////////////////////////////////
	int CreateIndex(SString sql);

	////////////////////////////////////////////////////////////////////////
	// ��    ��:  �����µ�һ����¼
	// ��    ��:  �ۿ���
	// ����ʱ��:  2014-04-23 12:52
	// ����˵��:  @sTableNameΪ����
	//         :  @pRowData��ʾ��¼���ݣ����뽨��ʱ����һ�µ����м�¼,�����ڲ��޸�ΪNULL���ڲ������ͷ�
	//         :  @iRowSize��ʾһ����¼�ĳ���
	//         :  @iRows��ʾ��ǰһ���ж�������¼�����룬������¼�����洢��pRowData��
	// �� �� ֵ:  �ɹ�����ļ�¼����
	//////////////////////////////////////////////////////////////////////////
	int Insert(char* sTableName,BYTE* &pRowData,int iRowSize,int iRows,CMdbTable* &pTable);

	////////////////////////////////////////////////////////////////////////
	// ��    ��:  ���»����һ����¼��������ΪΨһ��ʶ�������¼���ڸ��£������������
	// ��    ��:  �ۿ���
	// ����ʱ��:  2014-04-23 12:52
	// ����˵��:  @sTableNameΪ����
	//         :  @pRowData��ʾ��¼���ݣ����뽨��ʱ����һ�µ����м�¼,�����ڲ��޸�ΪNULL���ڲ������ͷ�
	//         :  @iRowSize��ʾһ����¼�ĳ���
	//         :  @iRows��ʾ��ǰһ���ж�������¼�����룬������¼�����洢��pRowData��
	// �� �� ֵ:  �ɹ�����ļ�¼����
	//////////////////////////////////////////////////////////////////////////
	int UpdateWithInsert(char* sTableName,BYTE* &pRowData,int iRowSize,int iRows,CMdbTable* &pTable);

	////////////////////////////////////////////////////////////////////////
	// ��    ��:  ִ��SQL���
	// ��    ��:  �ۿ���
	// ����ʱ��:  2014-04-23 12:05
	// ����˵��:  @sqlΪ��ִ�е�SQL��䣬֧�ּ��﷨
	// �� �� ֵ:  int
	//////////////////////////////////////////////////////////////////////////
	int Execute(SString &sql, CMdbTable* &pTable);

	////////////////////////////////////////////////////////////////////////
	// ��    ��:  ��ѯָ��SQL�������ݼ�
	// ��    ��:  �ۿ���
	// ����ʱ��:  2014-04-23 12:07
	// ����˵��:  @sqlΪ��ִ�е�SQL��䣬֧�ּ��﷨
	//         :  @pTableΪ��Ӧ�ı�ʵ������pTable��Ϊ��ʱ�����ú�������pTable->unlock()�ͷű���
	//         :  @ResultΪ��Ӧ�ļ�¼���б�
	//         :  @sRowResult��ʾ��һ��Ϊ�����ͳ����Ϣ��ÿ��Ϊһ���ַ���ֵ(count(*),min,max��,��֧��group by��
	// �� �� ֵ:  ����ѯ���ļ�¼������<0��ʾʧ��
	//////////////////////////////////////////////////////////////////////////
	int Retrieve(SString &sql,CMdbTable* &pTable,SPtrList<CMdbRecord> &Result,SString *sRowResult=NULL);

	////////////////////////////////////////////////////////////////////////
	// ��    ��:  ȡ���Ĵ�������
	// ��    ��:  �ۿ���
	// ����ʱ��:  2014-04-23 14:41
	// ����˵��:  void
	// �� �� ֵ:  SString
	//////////////////////////////////////////////////////////////////////////
	inline SString GetLastError(){return m_sLastError;};

	////////////////////////////////////////////////////////////////////////
	// ��    ��:  ����ָ���Ự���Ӵ�����
	// ��    ��:  �ۿ���
	// ����ʱ��:  2014-10-16 10��56
	// ����˵��:  @pSessionΪ�Ựʵ��ָ��
	//			  @tableΪ������
	//			  @actionΪ����������bit0~bit3��ʾ:insert/update/delete/truncate
	// �� �� ֵ:  true/false
	//////////////////////////////////////////////////////////////////////////
	bool ConnectTrigger(void *pSession, SString table, SString action);

	////////////////////////////////////////////////////////////////////////
	// ��    ��:  ɾ��ָ���Ự���Ӵ�����
	// ��    ��:  �ۿ���
	// ����ʱ��:  2014-10-16 10��56
	// ����˵��:  @pSessionΪ�Ựʵ��ָ��
	//			  @tableΪ������
	//			  @actionΪ����������bit0~bit3��ʾ:insert/update/delete/truncate
	// �� �� ֵ:  true/false
	//////////////////////////////////////////////////////////////////////////
	bool RemoveTrigger(void *pSession, SString table, SString action);

	////////////////////////////////////////////////////////////////////////
	// ��    ��:  ָ���Ự���Ӵ�����
	// ��    ��:  �ۿ���
	// ����ʱ��:  2014-10-16 10��56
	// ����˵��:  @pSessionΪ�Ựʵ��ָ��
	//			  @pTableΪ������
	// �� �� ֵ:  stuSessionConnectTrigger* , NULL��ʾδ�ҵ�
	//////////////////////////////////////////////////////////////////////////
	stuSessionConnectTrigger* SearchConnTrg(void *pSession, CMdbTable *pTable);

	////////////////////////////////////////////////////////////////////////
	// ��    ��:  �ͷ�ָ���Ự���ӵ����д�����
	// ��    ��:  �ۿ���
	// ����ʱ��:  2014-10-16 10:58
	// ����˵��:  @pSessionΪ�Ựʵ��ָ��
	// �� �� ֵ:  true/false
	//////////////////////////////////////////////////////////////////////////
	bool ClearSessionTrigger(void *pSession);

	////////////////////////////////////////////////////////////////////////
	// ��    ��:  �ж�ָ���ĻỰ�Ƿ�������ָ�����ָ�����͵Ĵ�����
	// ��    ��:  �ۿ���
	// ����ʱ��:  2014-10-16 10:58
	// ����˵��:  @pSessionΪ�Ựʵ��ָ��
	//			  @pTableΪָ�����ʵ��ָ��
	//			  @iTrgType��ʾ�������ͣ�1:insert; 2:update; 3:delete; 4:truncate
	// �� �� ֵ:  true/false
	//////////////////////////////////////////////////////////////////////////
	bool IsSessionTrgConnected(void *pSession, CMdbTable *pTable, int iTrgType);

	//////////////////////////////////////////////////////////////////////////
	// ��    ��:  ������ʷ���Ƿ�ʹ��
	// ��    ��:  �ۿ���
	// ����ʱ��:  2015-7-14 14:33
	// ����˵��:  @enΪtrue��ʾʹ�ܳ־û���ʷ�洢��false��ʾ�رճ־û�
	// �� �� ֵ:  void
	//////////////////////////////////////////////////////////////////////////
	void SetHisDbEnable(bool en){m_bHisDb=en;};

	////////////////////////////////////////////////////////////////////////
	// ��    ��:  ����MDB���ش洢·��
	// ��    ��:  �ۿ���
	// ����ʱ��:  2014-11-17 09:24
	// ����˵��:  @pathΪ����·��
	// �� �� ֵ:  void
	//////////////////////////////////////////////////////////////////////////
	void SetMdbHomePath(SString path){ m_sMdbHomePath = path; };

	////////////////////////////////////////////////////////////////////////
	// ��    ��:  ȡMDB���ش洢·��
	// ��    ��:  �ۿ���
	// ����ʱ��:  2014-11-18 15:34
	// ����˵��:  void
	// �� �� ֵ:  ����·��
	//////////////////////////////////////////////////////////////////////////
	SString GetMdbHomePath(){ return m_sMdbHomePath; };

	////////////////////////////////////////////////////////////////////////
	// ��    ��:  �����ʷ��ͬ������
	// ��    ��:  �ۿ���
	// ����ʱ��:  2014-11-24 19:36
	// ����˵��:  stuMdbMgrHisDbSyncOper*
	// �� �� ֵ:  void
	//////////////////////////////////////////////////////////////////////////
	inline void AddHisSyncOper(stuMdbMgrHisDbSyncOper *p)
	{
		if (m_HisSyncOper.count() > C_MAX_BUFFERED_DBSYNC)
		{
			static int last_soc = 0;
			if((int)::time(NULL) != last_soc)
			{
				last_soc = (int)::time(NULL);
				LOGWARN("��ʷ��ͬ���ж���(>%d)����������������!",C_MAX_BUFFERED_DBSYNC);
			}
			delete p;
		}
		else
			m_HisSyncOper.append(p); 
	};

	////////////////////////////////////////////////////////////////////////
	// ��    ��:  ȡϵͳ��ָ��
	// ��    ��:  �ۿ���
	// ����ʱ��:  2014-12-17 15:19
	// ����˵��:  void
	// �� �� ֵ:  CMdbTable*
	//////////////////////////////////////////////////////////////////////////
	inline CMdbTable* GetSysTable(){ return m_pSysTable; };

	////////////////////////////////////////////////////////////////////////
	// ��    ��:  ����BLOB�ֶ�
	// ��    ��:  �ۿ���
	// ����ʱ��:  2014-12-22 18:20
	// ����˵��:  @pBlobBufΪ�����µĴ��ֶλ���������
	//         :  @iBlobSize��ʾ��������С
	//         :  @sTableNameΪ����
	//         :  @sFieldNameΪ���ֶ�����
	//         :  @ConditionΪָ����������ָ������Ӧ����һ��Ψһ�ļ�¼
	// �� �� ֵ:  �ɹ����µļ�¼������<0��ʾʧ�ܣ�=0��ʾδ����
	//////////////////////////////////////////////////////////////////////////
	int UpdateBlob(BYTE *pBlobBuf, int iBlobSize, SString sTableName, SString sFieldName, SString sWhere);

	////////////////////////////////////////////////////////////////////////
	// ��    ��:  ��ȡBLOB�ֶ�
	// ��    ��:  �ۿ���
	// ����ʱ��:  2014-12-22 18:26
	// ����˵��:  @pBlobBufΪ����ȡ�Ĵ��ֶλ��������ݣ��ں������뻺�����������߸����ͷ��ڴ�
	//         :  @iBlobSize��ʾ��������С
	//         :  @sTableNameΪ����
	//         :  @sFieldNameΪ���ֶ�����
	//         :  @ConditionΪָ����������ָ������Ӧ����һ��Ψһ�ļ�¼
	// �� �� ֵ:  �ɹ���ȡ�ļ�¼������<0��ʾʧ�ܣ�=0��ʾδ����
	//////////////////////////////////////////////////////////////////////////
	int ReadBlob(BYTE* &pBlobBuf, int &iBlobSize, SString sTableName, SString sFieldName, SString sWhere);

	////////////////////////////////////////////////////////////////////////
	// ��    ��:  ȡģ�����Ϣ����
	// ��    ��:  �ۿ���
	// ����ʱ��:  2015-01-09 16:07
	// ����˵��:  void
	// �� �� ֵ:  SString
	//////////////////////////////////////////////////////////////////////////
	SString GetMdbMgrInfo();

	inline void ClearDeletedRecords() { m_DeletedRecords.clear(); };
	inline void AddDelRecord(CMdbRecord *p){ m_DeletedRecords.append(p); };
	inline SPtrList<CMdbRecord>* GetDelRecordPtr(){ return &m_DeletedRecords; };
	inline bool IsHisDb(){return m_bHisDb;};
	void Quit();
protected:
	////////////////////////////////////////////////////////////////////////
	// ��    ��:  ����SQL�е�where����Ԥ��������
	// ��    ��:  �ۿ���
	// ����ʱ��:  2014-04-23 14:44
	// ����˵��:  @pTableΪ��Ӧ�ı�ʵ��
	//         :  @sWhereΪ�����ַ�����SQL�е�where����
	//         :  @ConditionΪ���������÷���
	// �� �� ֵ:  true��ʾ�ɹ���false��ʾʧ��
	//////////////////////////////////////////////////////////////////////////
	bool PrepareWhere(CMdbTable *pTable,SString &sWhere,SPtrList<stuMdbCompare> &Condition);

	////////////////////////////////////////////////////////////////////////
	// ��    ��:  �޸�ϵͳ���Ӧ����ʷ���е�ָ�����ݱ�ļ�¼����
	// ��    ��:  �ۿ���
	// ����ʱ��:  2014-12-17 15:27
	// ����˵��:  @pTableΪ��ʵ��ָ��
	// �� �� ֵ:  void
	//////////////////////////////////////////////////////////////////////////
	void PrepareHisSysTableRows(CMdbTable *pTable);

	////////////////////////////////////////////////////////////////////////
	// ��    ��:  ��ʷ���ͬ�������̣߳��������б�Ĵ��и���
	// ��    ��:  �ۿ���
	// ����ʱ��:  2014-11-24 18:38
	// ����˵��:  @lpΪthisָ��
	// �� �� ֵ:  NULL
	//////////////////////////////////////////////////////////////////////////
	static void* ThreadHisDbSync(void *lp);

	////////////////////////////////////////////////////////////////////////
	// ��    ��:  �ͷŹ��ڵ���Ч��¼
	// ��    ��:  �ۿ���
	// ����ʱ��:  2014-11-24 18:38
	// ����˵��:  @lpΪthisָ��
	// �� �� ֵ:  NULL
	//////////////////////////////////////////////////////////////////////////
	static void* ThreadFreeOldRecord(void *lp);

private:

	SPtrList<CMdbTable> m_Tables;//���е����ݱ�ʵ��
	SPtrList<CMdbRecord> m_DeletedRecords;	//���б���Ҫ��ɾ���ļ�¼�б�
	char m_sLastError[1024];
	CMdbTable *m_pSysTable;		//ϵͳ��
	SPtrList<CMdbTableIndex> m_Index;//���е�����
	SPtrList<stuSessionConnectTrigger> m_ConnTrg;//�������ӹ�ϵ
	SString m_sMdbHomePath;//·����Ŀ¼
	bool m_bHisDb;//�Ƿ񺬳־û��洢��(�����ļ��洢)
	bool m_bQuit;
	int m_iThreads;
	SPtrList<stuMdbMgrHisDbSyncOper> m_HisSyncOper;
};



#pragma pack(push)  
#pragma pack(1)

struct stuHisIndexInfo
{
	int hash_size;	  //HASH����
	unsigned int flags;//bit0��ʾ����״̬��bit1��ʾΨһ��״̬
	char idx_name[32];//��������
	BYTE field[32];//�����ֶ���ţ���1��ʼ��ţ�0��ʾ��Ч
};
//////////////////////////////////////////////////////////////////////////
// ��    ��:  stuHisTableInfo
// ��    ��:  �ۿ���
// ����ʱ��:  2014-11-18 15:10
// ��    ��:  ��ʷ���ݱ��ļ���Ϣ
//////////////////////////////////////////////////////////////////////////
struct stuHisTableInfo
{
	int m_iMemMaxRows;	//�ڴ������������¼����
	int m_iHisMaxRows;	//��ʷ�������������¼����
	int m_iFieldCnt;	//�ֶ�����
	int m_iIndexCnt;	//��������
	stuTableField fields[C_MDB_MAX_FIELD_CNT];	//���е��ֶ����ݣ����C_MDB_MAX_FIELD_CNT��
	stuHisIndexInfo indexs[C_MDB_MAX_INDEX_CNT];//���е���������
};

#pragma pack()
#pragma pack(pop)  

//////////////////////////////////////////////////////////////////////////
// ��    ��:  CHisDbTable
// ��    ��:  �ۿ���
// ����ʱ��:  2014-11-18 15:10
// ��    ��:  ��ʷ���ݱ��ļ���
//////////////////////////////////////////////////////////////////////////
class CHisDbTable : public SVarDbf<stuHisTableInfo>
{
public:
	CHisDbTable(CMdbManager *pMgr,CMdbTable *pTable);
	CHisDbTable(CMdbManager *pMgr, SString tabname, int iRowSize);
	virtual ~CHisDbTable();

	////////////////////////////////////////////////////////////////////////
	// ��    ��:  ���һ����������ʷ��
	// ��    ��:  �ۿ���
	// ����ʱ��:  2014-04-23 14:44
	// ����˵��:  @pIdxΪ����ָ��
	// �� �� ֵ:  true��ʾ�ɹ���false��ʾʧ��
	//////////////////////////////////////////////////////////////////////////
	bool AddIndex(CMdbTableIndex *pIdx);

	////////////////////////////////////////////////////////////////////////
	// ��    ��:  ����ʷ��ṹ�����ݻ�ԭ���ڴ��
	// ��    ��:  �ۿ���
	// ����ʱ��:  2014-11-24 11:00
	// ����˵��:  void
	// �� �� ֵ:  true��ʾ�ɹ���false��ʾʧ��
	//////////////////////////////////////////////////////////////////////////
	bool RestoreTable();
	
	////////////////////////////////////////////////////////////////////////
	// ��    ��:  ȡ��Ӧ�ı�ָ��
	// ��    ��:  �ۿ���
	// ����ʱ��:  2014-11-24 14:47
	// ����˵��:  void
	// �� �� ֵ:  CMdbTable*
	//////////////////////////////////////////////////////////////////////////
	CMdbTable* GetTablePtr(){ return m_pTable; };

private:
	CMdbManager *m_pMdbMgr;
	CMdbTable *m_pTable;
};

#endif // !defined(AFX_REALDATA_H__426DEEA4_DEF8_4F0F_A173_C1F7461AF378__INCLUDED_)
