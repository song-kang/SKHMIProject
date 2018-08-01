/**
 *
 * 文 件 名 : RealData.cpp
 * 创建日期 : 2014-4-22 13:36
 * 作    者 : 邵凯田(skt001@163.com)
 * 修改日期 : $Date: $
 * 当前版本 : $Revision: $
 * 功能描述 : 内存数据库数据结构定义
 * 修改记录 : 
 *            $Log: $
 *
 * Ver  Date        Author  Comments
 * ---  ----------  ------  -------------------------------------------
 * 001	2014-4-22	邵凯田　创建文件
 *
 **/

#include "MdbMgr.h"
#include "db/mdb/MdbClient.h"

/************************************************************************/
/*                           字段类                                     */
/************************************************************************/

//////////////////////////////////////////////////////////////////////////
// 名    称:  CMdbField
// 作    者:  邵凯田
// 创建时间:  2014:4:22 14:22
// 描    述:  内存库字段类
//////////////////////////////////////////////////////////////////////////
CMdbField::CMdbField()
{
	m_bIsNull = true;
	m_bIsPK = false;
}

CMdbField::~CMdbField()
{

}

/************************************************************************/
/*                           记录类                                     */
/************************************************************************/

//////////////////////////////////////////////////////////////////////////
// 名    称:  CMdbRecord
// 作    者:  邵凯田
// 创建时间:  2014:4:22 14:47
// 描    述:  内存库记录类
//////////////////////////////////////////////////////////////////////////
CMdbRecord::CMdbRecord(BYTE *pRecData,int iLen/*,bool bClone*/)
{
	//if(bClone)
	{
		//clone memory
		m_pRecordData = new BYTE[iLen];
		memcpy(m_pRecordData,pRecData,iLen);
	}
	//else
	//{
	//	//link memory
	//	m_pRecordData = pRecData;
	//}
}
CMdbRecord::CMdbRecord()
{
	m_pRecordData = NULL;
}

CMdbRecord::~CMdbRecord()
{
	//destory memory
	if (m_pRecordData != NULL)
		delete[] m_pRecordData;
}


/************************************************************************/
/*                             索引类                                   */
/************************************************************************/
//////////////////////////////////////////////////////////////////////////
// 名    称:  CMdbTableIndex
// 作    者:  邵凯田
// 创建时间:  2014-6-11 8:56
// 描    述:  内存表索引类
//////////////////////////////////////////////////////////////////////////

CMdbTableIndex::CMdbTableIndex(int hash_size)
{
	m_IdxHash.SetCallbackParam(this);
	m_IdxHash.SetHashSize(hash_size);
	m_IdxHash.setAutoDelete(false);
	m_IdxHash.setShared(true);
	m_bUnique = false;
}

CMdbTableIndex::~CMdbTableIndex()
{
	m_IdxHash.clear();
}

//////////////////////////////////////////////////////////////////////////
// 描    述:  散列键生成函数
// 作    者:  邵凯田
// 创建时间:  2014-6-11 15:25
// 参数说明:  @pItem待生成的行
// 返 回 值:  键值
//////////////////////////////////////////////////////////////////////////
int CMdbTableIndex::SHash_Key_Idx(void *cb,void *pItem)
{
	int key = 0;
	unsigned long pos=0;
	CMdbTableIndex *pThis = (CMdbTableIndex*)cb;
	CMdbRecord *pRow = (CMdbRecord *)pItem;
	CMdbField *pField = pThis->m_IndexFiends.FetchFirst(pos);
	while(pField)
	{
		key += pThis->AddBytes(pRow->GetValue(pField),pField->m_iBytes);
		pField = pThis->m_IndexFiends.FetchNext(pos);
	}
	return key;
}

//////////////////////////////////////////////////////////////////////////
// 描    述:  记录内容的精确比较函数（根据索引键）
// 作    者:  邵凯田
// 创建时间:  2014-6-11 15:28
// 参数说明:  @pItem1为比较参照记录1
//         :  @pItem2为比较参照记录2
// 返 回 值:  true表示相等，false表示不相等
//////////////////////////////////////////////////////////////////////////
bool CMdbTableIndex::SHash_Compare_Idx(void *cb,void *pItem1,void *pItem2)
{
	CMdbTableIndex *pThis = (CMdbTableIndex*)cb;
	CMdbRecord *pRow1 = (CMdbRecord *)pItem1;
	unsigned long pos=0;
	CMdbRecord *pRow2 = (CMdbRecord *)pItem2;

	//根据索引键判断是否两条记录完全相等
	bool bEqual=true;
	CMdbField *pField = pThis->m_IndexFiends.FetchFirst(pos);
	while(pField)
	{
		if(memcmp(pRow1->m_pRecordData+pField->m_iStartPos,pRow2->m_pRecordData+pField->m_iStartPos,pField->m_iBytes) != 0)
		{
			bEqual = false;
			break;
		}
		pField = pThis->m_IndexFiends.FetchNext(pos);
	}
	return bEqual;
}

//////////////////////////////////////////////////////////////////////////
// 描    述:  创建索引
// 作    者:  邵凯田
// 创建时间:  2014-6-11 15:31
// 参数说明:  @fields为索引字段列表
//            @bUnique表示是否为唯一键
//            @bPk表示是否为主键
// 返 回 值:  true表示索引成功，false表示索引失败
//////////////////////////////////////////////////////////////////////////
bool CMdbTableIndex::CreateIndex(SPtrList<CMdbField> *fields, bool bUnique, bool bPk)
{
	if(m_pMdbTable == NULL)
		return false;
	fields->copyto(m_IndexFiends);
	unsigned long pos=0;
	CMdbRecord *pRow;
	m_bPk = bPk;
	m_bUnique = bUnique;

	//清除原有的索引记录
	ClearRecord();
	SPtrList<CMdbRecord>* pRecord = m_pMdbTable->GetRecordPtr();
	pRecord->lock();
	pRow = pRecord->FetchFirst(pos);
	while(pRow)
	{
		AddRecord(pRow);
		pRow = pRecord->FetchNext(pos);
	}
	pRecord->unlock();
	//将索引添加到表中，不再删除
	m_pMdbTable->AddIndex(this);
	return true;
}

//////////////////////////////////////////////////////////////////////////
// 描    述:  添加一条索引记录
// 作    者:  邵凯田
// 创建时间:  2014-6-11 15:32
// 参数说明:  @pRow为新记录指针
// 返 回 值:  true表示添加成功，false表示添加失败
//////////////////////////////////////////////////////////////////////////
bool CMdbTableIndex::AddRecord(CMdbRecord *pRow)
{
	if (m_bUnique)
	{
		//进行唯一性检查
		if (m_IdxHash.at(*pRow) != NULL)
			return false;//唯一键已存在
	}
	return m_IdxHash.append(pRow);
}

//////////////////////////////////////////////////////////////////////////
// 描    述:  删除一条索引记录
// 作    者:  邵凯田
// 创建时间:  2014-6-11 15:32
// 参数说明:  @pRow为待删除的记录指针
// 返 回 值:  true表示删除成功，false表示删除失败
//////////////////////////////////////////////////////////////////////////
bool CMdbTableIndex::RemoveRecord(CMdbRecord *pRow)
{
	return m_IdxHash.remove(pRow);
}

//////////////////////////////////////////////////////////////////////////
// 描    述:  清除所有索引记录
// 作    者:  邵凯田
// 创建时间:  2014-6-11 15:33
// 参数说明:  void
// 返 回 值:  true表示清空成功，false表示清空失败
//////////////////////////////////////////////////////////////////////////
bool CMdbTableIndex::ClearRecord()
{
	m_IdxHash.clear();
	return true;
}

//////////////////////////////////////////////////////////////////////////
// 描    述:  按指定的参考记录中的索引项值搜索匹配的所有记录
// 作    者:  邵凯田
// 创建时间:  2015-1-16 10:38
// 参数说明:  @pObj参考对象指针
//            @list为引用返回的匹配记录集合
// 返 回 值:  >0匹配记录的数量，<0表示失败,=0表示没有匹配记录
//////////////////////////////////////////////////////////////////////////
int CMdbTableIndex::SearchByIndex(CMdbRecord *pObj, SPtrList<CMdbRecord> &list)
{
	return m_IdxHash.search(*pObj, list);
}

CMdbRecord* CMdbTableIndex::SearchOnByIndex(CMdbRecord *pObj)
{
	return m_IdxHash.at(*pObj);
}

/************************************************************************/
/*                             表类                                     */
/************************************************************************/


//////////////////////////////////////////////////////////////////////////
// 名    称:  CMdbTable
// 作    者:  邵凯田
// 创建时间:  2014:4:22 14:42
// 描    述:  内存式数据表
//////////////////////////////////////////////////////////////////////////
CMdbTable::CMdbTable()
{
	m_Fields.setAutoDelete(true);
	m_PkFields.setAutoDelete(false);
	m_Records.setAutoDelete(false);
	m_Records.setShared(true);
	m_MyIndex.setAutoDelete(true);
	m_MyIndex.setShared(false);
	m_pSysTableRecord = NULL;
	m_InsertTrigger.setAutoDelete(true);
	m_UpdateTrigger.setAutoDelete(true);
	m_DeleteTrigger.setAutoDelete(true);
	m_TruncateTrigger.setAutoDelete(true);
	m_InsertTrigger.setShared(true);
	m_UpdateTrigger.setShared(true);
	m_DeleteTrigger.setShared(true);
	m_TruncateTrigger.setShared(true);
	m_TriggerFlag = 0;
	m_pHisDbTable = NULL;
	m_pMgr = NULL;
	m_iMemMaxRows = 50000000;//1000000;
	m_iHisMaxRows = 5000000;
	m_pPkIndex = NULL;
}
CMdbTable::~CMdbTable()
{
	m_Records.setAutoDelete(true);
	m_MyIndex.setAutoDelete(true);
	m_Records.clear();
	m_MyIndex.clear();
	if(m_pHisDbTable != NULL)
	{
		delete m_pHisDbTable;
		m_pHisDbTable = NULL;
	}
}

////////////////////////////////////////////////////////////////////////
// 描    述:  读取指定字段号对应的字段名称
// 作    者:  邵凯田
// 创建时间:  2014:4:22 14:38
// 参数说明:  @iFieldNo为字段序号，从0开始编号
// 返 回 值:  对应字段名称，空串表示指定序号的字段不存在
//////////////////////////////////////////////////////////////////////////
SString CMdbTable::GetFieldName(int iFieldNo)
{
	CMdbField *p = m_Fields[iFieldNo];
	if(p == NULL)
		return "";
	return p->m_sName;
}

////////////////////////////////////////////////////////////////////////
// 描    述:  检索指定字段名称对应的字段实例
// 作    者:  邵凯田
// 创建时间:  2014:4:23 16:18
// 参数说明:  @sFieldName为字段名称
// 返 回 值:  CMdbField*
//////////////////////////////////////////////////////////////////////////
CMdbField* CMdbTable::SearchFieldByName(char* sFieldName)
{
	int idx=0;
	unsigned long pos=0;
	CMdbField *p = m_Fields.FetchFirst(pos);
	while(p)
	{
		if(p->m_sName == sFieldName)
			return p;//catch it
		idx++;
		p = m_Fields.FetchNext(pos);
	}
	return NULL;//not found

}

////////////////////////////////////////////////////////////////////////
// 描    述:  读取指定字段名称对应的字段号
// 作    者:  邵凯田
// 创建时间:  2014:4:22 14:39
// 参数说明:  @sFieldName为字段名称，不区分大小写
// 返 回 值:  返回字段号，从0开始编号，<0表示字段名不存在
//////////////////////////////////////////////////////////////////////////
int CMdbTable::GetFieldNo(char* sFieldName)
{
	int idx=0;
	unsigned long pos=0;
	CMdbField *p = m_Fields.FetchFirst(pos);
	while(p)
	{
		if(p->m_sName == sFieldName)
			return idx;//catch it
		idx++;
		p = m_Fields.FetchNext(pos);
	}
	return -1;//not found
}

////////////////////////////////////////////////////////////////////////
// 描    述:  条件过滤各组合情况下的预处理函数
// 作    者:  邵凯田
// 创建时间:  2014:4:23 10:29
// 参数说明:  @pCmp为比较条件
//         :  @pRecord为记录实例
// 返 回 值:  true表示满足条件，false表示不满足条件
//////////////////////////////////////////////////////////////////////////
bool CMdbTable::CB_Filter_INT_BIGTHAN(stuMdbCompare *pCmp,CMdbRecord *pRecord)
{
#ifndef _ARM
	return  *((int*)(pRecord->m_pRecordData+pCmp->m_pField->m_iStartPos)) > pCmp->m_iCmpVal;

#else
	register int val=0;
	memcpy(&val,pRecord->m_pRecordData+pCmp->m_pField->m_iStartPos,sizeof(val));
	return val > pCmp->m_iCmpVal;
#endif
}

bool CMdbTable::CB_Filter_INT_LESSTHAN(stuMdbCompare *pCmp,CMdbRecord *pRecord)
{
#ifndef _ARM
	return  *((int*)(pRecord->m_pRecordData+pCmp->m_pField->m_iStartPos)) < pCmp->m_iCmpVal;

#else
	register int val=0;
	memcpy(&val,pRecord->m_pRecordData+pCmp->m_pField->m_iStartPos,sizeof(val));
	return val < pCmp->m_iCmpVal;
#endif
}

bool CMdbTable::CB_Filter_INT_EQUAL(stuMdbCompare *pCmp,CMdbRecord *pRecord)
{
#ifndef _ARM
	return  *((int*)(pRecord->m_pRecordData+pCmp->m_pField->m_iStartPos)) == pCmp->m_iCmpVal;

#else
	register int val=0;
	memcpy(&val,pRecord->m_pRecordData+pCmp->m_pField->m_iStartPos,sizeof(val));
	return val == pCmp->m_iCmpVal;
#endif
}

bool CMdbTable::CB_Filter_INT_BIGTHAN_EQUAL(stuMdbCompare *pCmp,CMdbRecord *pRecord)
{
#ifndef _ARM
	return  *((int*)(pRecord->m_pRecordData+pCmp->m_pField->m_iStartPos)) >= pCmp->m_iCmpVal;

#else
	register int val=0;
	memcpy(&val,pRecord->m_pRecordData+pCmp->m_pField->m_iStartPos,sizeof(val));
	return val >= pCmp->m_iCmpVal;
#endif
}

bool CMdbTable::CB_Filter_INT_LESSTHAN_EQUAL(stuMdbCompare *pCmp,CMdbRecord *pRecord)
{
#ifndef _ARM
	return  *((int*)(pRecord->m_pRecordData+pCmp->m_pField->m_iStartPos)) <= pCmp->m_iCmpVal;

#else
	register int val=0;
	memcpy(&val,pRecord->m_pRecordData+pCmp->m_pField->m_iStartPos,sizeof(val));
	return val <= pCmp->m_iCmpVal;
#endif
}

bool CMdbTable::CB_Filter_INT_NOT_EQUAL(stuMdbCompare *pCmp,CMdbRecord *pRecord)
{
#ifndef _ARM
	return  *((int*)(pRecord->m_pRecordData+pCmp->m_pField->m_iStartPos)) != pCmp->m_iCmpVal;

#else
	register int val=0;
	memcpy(&val,pRecord->m_pRecordData+pCmp->m_pField->m_iStartPos,sizeof(val));
	return val != pCmp->m_iCmpVal;
#endif
}


bool CMdbTable::CB_Filter_INT64_BIGTHAN(stuMdbCompare *pCmp,CMdbRecord *pRecord)
{
#ifndef _ARM
	return  *((INT64*)(pRecord->m_pRecordData+pCmp->m_pField->m_iStartPos)) > pCmp->m_i64CmpVal;

#else
	register INT64 val=0;
	memcpy(&val,pRecord->m_pRecordData+pCmp->m_pField->m_iStartPos,sizeof(val));
	return val > pCmp->m_i64CmpVal;
#endif
}

bool CMdbTable::CB_Filter_INT64_LESSTHAN(stuMdbCompare *pCmp,CMdbRecord *pRecord)
{
#ifndef _ARM
	return  *((INT64*)(pRecord->m_pRecordData+pCmp->m_pField->m_iStartPos)) < pCmp->m_i64CmpVal;

#else
	register INT64 val=0;
	memcpy(&val,pRecord->m_pRecordData+pCmp->m_pField->m_iStartPos,sizeof(val));
	return val < pCmp->m_i64CmpVal;
#endif
}

bool CMdbTable::CB_Filter_INT64_EQUAL(stuMdbCompare *pCmp,CMdbRecord *pRecord)
{
#ifndef _ARM
	return  *((INT64*)(pRecord->m_pRecordData+pCmp->m_pField->m_iStartPos)) == pCmp->m_i64CmpVal;

#else
	register INT64 val=0;
	memcpy(&val,pRecord->m_pRecordData+pCmp->m_pField->m_iStartPos,sizeof(val));
	return val == pCmp->m_i64CmpVal;
#endif
}

bool CMdbTable::CB_Filter_INT64_BIGTHAN_EQUAL(stuMdbCompare *pCmp,CMdbRecord *pRecord)
{
#ifndef _ARM
	return  *((INT64*)(pRecord->m_pRecordData+pCmp->m_pField->m_iStartPos)) >= pCmp->m_i64CmpVal;

#else
	register INT64 val=0;
	memcpy(&val,pRecord->m_pRecordData+pCmp->m_pField->m_iStartPos,sizeof(val));
	return val >= pCmp->m_i64CmpVal;
#endif
}

bool CMdbTable::CB_Filter_INT64_LESSTHAN_EQUAL(stuMdbCompare *pCmp,CMdbRecord *pRecord)
{
#ifndef _ARM
	return  *((INT64*)(pRecord->m_pRecordData+pCmp->m_pField->m_iStartPos)) <= pCmp->m_i64CmpVal;

#else
	register INT64 val=0;
	memcpy(&val,pRecord->m_pRecordData+pCmp->m_pField->m_iStartPos,sizeof(val));
	return val <= pCmp->m_i64CmpVal;
#endif
}

bool CMdbTable::CB_Filter_INT64_NOT_EQUAL(stuMdbCompare *pCmp,CMdbRecord *pRecord)
{
#ifndef _ARM
	return  *((INT64*)(pRecord->m_pRecordData+pCmp->m_pField->m_iStartPos)) != pCmp->m_i64CmpVal;

#else
	register INT64 val=0;
	memcpy(&val,pRecord->m_pRecordData+pCmp->m_pField->m_iStartPos,sizeof(val));
	return val != pCmp->m_i64CmpVal;
#endif
}


bool CMdbTable::CB_Filter_SHORT_BIGTHAN(stuMdbCompare *pCmp,CMdbRecord *pRecord)
{
#ifndef _ARM
	return  *((short*)(pRecord->m_pRecordData+pCmp->m_pField->m_iStartPos)) > pCmp->m_iCmpVal;

#else
	register short val=0;
	memcpy(&val,pRecord->m_pRecordData+pCmp->m_pField->m_iStartPos,sizeof(val));
	return val > pCmp->m_iCmpVal;
#endif
}

bool CMdbTable::CB_Filter_SHORT_LESSTHAN(stuMdbCompare *pCmp,CMdbRecord *pRecord)
{
#ifndef _ARM
	return  *((short*)(pRecord->m_pRecordData+pCmp->m_pField->m_iStartPos)) < pCmp->m_iCmpVal;

#else
	register short val=0;
	memcpy(&val,pRecord->m_pRecordData+pCmp->m_pField->m_iStartPos,sizeof(val));
	return val < pCmp->m_iCmpVal;
#endif
}

bool CMdbTable::CB_Filter_SHORT_EQUAL(stuMdbCompare *pCmp,CMdbRecord *pRecord)
{
#ifndef _ARM
	return  *((short*)(pRecord->m_pRecordData+pCmp->m_pField->m_iStartPos)) == pCmp->m_iCmpVal;

#else
	register short val=0;
	memcpy(&val,pRecord->m_pRecordData+pCmp->m_pField->m_iStartPos,sizeof(val));
	return val == pCmp->m_iCmpVal;
#endif
}

bool CMdbTable::CB_Filter_SHORT_BIGTHAN_EQUAL(stuMdbCompare *pCmp,CMdbRecord *pRecord)
{
#ifndef _ARM
	return  *((short*)(pRecord->m_pRecordData+pCmp->m_pField->m_iStartPos)) >= pCmp->m_iCmpVal;

#else
	register short val=0;
	memcpy(&val,pRecord->m_pRecordData+pCmp->m_pField->m_iStartPos,sizeof(val));
	return val >= pCmp->m_iCmpVal;
#endif
}

bool CMdbTable::CB_Filter_SHORT_LESSTHAN_EQUAL(stuMdbCompare *pCmp,CMdbRecord *pRecord)
{
#ifndef _ARM
	return  *((short*)(pRecord->m_pRecordData+pCmp->m_pField->m_iStartPos)) <= pCmp->m_iCmpVal;

#else
	register short val=0;
	memcpy(&val,pRecord->m_pRecordData+pCmp->m_pField->m_iStartPos,sizeof(val));
	return val <= pCmp->m_iCmpVal;
#endif
}

bool CMdbTable::CB_Filter_SHORT_NOT_EQUAL(stuMdbCompare *pCmp,CMdbRecord *pRecord)
{
#ifndef _ARM
	return  *((short*)(pRecord->m_pRecordData+pCmp->m_pField->m_iStartPos)) != pCmp->m_iCmpVal;

#else
	register short val=0;
	memcpy(&val,pRecord->m_pRecordData+pCmp->m_pField->m_iStartPos,sizeof(val));
	return val != pCmp->m_iCmpVal;
#endif
}


bool CMdbTable::CB_Filter_BYTE_BIGTHAN(stuMdbCompare *pCmp,CMdbRecord *pRecord)
{
	return pRecord->m_pRecordData[pCmp->m_pField->m_iStartPos] > pCmp->m_iCmpVal;
}

bool CMdbTable::CB_Filter_BYTE_LESSTHAN(stuMdbCompare *pCmp,CMdbRecord *pRecord)
{
	return pRecord->m_pRecordData[pCmp->m_pField->m_iStartPos] < pCmp->m_iCmpVal;
}

bool CMdbTable::CB_Filter_BYTE_EQUAL(stuMdbCompare *pCmp,CMdbRecord *pRecord)
{
	return pRecord->m_pRecordData[pCmp->m_pField->m_iStartPos] == pCmp->m_iCmpVal;
}

bool CMdbTable::CB_Filter_BYTE_BIGTHAN_EQUAL(stuMdbCompare *pCmp,CMdbRecord *pRecord)
{
	return pRecord->m_pRecordData[pCmp->m_pField->m_iStartPos] >= pCmp->m_iCmpVal;
}

bool CMdbTable::CB_Filter_BYTE_LESSTHAN_EQUAL(stuMdbCompare *pCmp,CMdbRecord *pRecord)
{
	return pRecord->m_pRecordData[pCmp->m_pField->m_iStartPos] <= pCmp->m_iCmpVal;
}

bool CMdbTable::CB_Filter_BYTE_NOT_EQUAL(stuMdbCompare *pCmp,CMdbRecord *pRecord)
{
	return pRecord->m_pRecordData[pCmp->m_pField->m_iStartPos] != pCmp->m_iCmpVal;
}


bool CMdbTable::CB_Filter_FLOAT_BIGTHAN(stuMdbCompare *pCmp,CMdbRecord *pRecord)
{
#ifndef _ARM
	return  *((float*)(pRecord->m_pRecordData+pCmp->m_pField->m_iStartPos)) > pCmp->m_fCmpVal;

#else
	register float val=0;
	memcpy(&val,pRecord->m_pRecordData+pCmp->m_pField->m_iStartPos,sizeof(val));
	return val > pCmp->m_fCmpVal;
#endif
}

bool CMdbTable::CB_Filter_FLOAT_LESSTHAN(stuMdbCompare *pCmp,CMdbRecord *pRecord)
{
#ifndef _ARM
	return  *((float*)(pRecord->m_pRecordData+pCmp->m_pField->m_iStartPos)) < pCmp->m_fCmpVal;

#else
	register float val=0;
	memcpy(&val,pRecord->m_pRecordData+pCmp->m_pField->m_iStartPos,sizeof(val));
	return val < pCmp->m_fCmpVal;
#endif
}

bool CMdbTable::CB_Filter_FLOAT_EQUAL(stuMdbCompare *pCmp,CMdbRecord *pRecord)
{
#ifndef _ARM
	return  *((float*)(pRecord->m_pRecordData+pCmp->m_pField->m_iStartPos)) == pCmp->m_fCmpVal;

#else
	register float val=0;
	memcpy(&val,pRecord->m_pRecordData+pCmp->m_pField->m_iStartPos,sizeof(val));
	return val == pCmp->m_fCmpVal;
#endif
}

bool CMdbTable::CB_Filter_FLOAT_BIGTHAN_EQUAL(stuMdbCompare *pCmp,CMdbRecord *pRecord)
{
#ifndef _ARM
	return  *((float*)(pRecord->m_pRecordData+pCmp->m_pField->m_iStartPos)) >= pCmp->m_fCmpVal;

#else
	register float val=0;
	memcpy(&val,pRecord->m_pRecordData+pCmp->m_pField->m_iStartPos,sizeof(val));
	return val >= pCmp->m_fCmpVal;
#endif
}

bool CMdbTable::CB_Filter_FLOAT_LESSTHAN_EQUAL(stuMdbCompare *pCmp,CMdbRecord *pRecord)
{
#ifndef _ARM
	return  *((float*)(pRecord->m_pRecordData+pCmp->m_pField->m_iStartPos)) <= pCmp->m_fCmpVal;

#else
	register float val=0;
	memcpy(&val,pRecord->m_pRecordData+pCmp->m_pField->m_iStartPos,sizeof(val));
	return val <= pCmp->m_fCmpVal;
#endif
}

bool CMdbTable::CB_Filter_FLOAT_NOT_EQUAL(stuMdbCompare *pCmp,CMdbRecord *pRecord)
{
#ifndef _ARM
	return  *((float*)(pRecord->m_pRecordData+pCmp->m_pField->m_iStartPos)) != pCmp->m_fCmpVal;

#else
	register float val=0;
	memcpy(&val,pRecord->m_pRecordData+pCmp->m_pField->m_iStartPos,sizeof(val));
	return val != pCmp->m_fCmpVal;
#endif
}


bool CMdbTable::CB_Filter_STR_BIGTHAN(stuMdbCompare *pCmp,CMdbRecord *pRecord)
{
	return strcmp((char*)(pRecord->m_pRecordData+pCmp->m_pField->m_iStartPos),pCmp->m_sCmpVal.data()) > 0;
}

bool CMdbTable::CB_Filter_STR_LESSTHAN(stuMdbCompare *pCmp,CMdbRecord *pRecord)
{
	return strcmp((char*)(pRecord->m_pRecordData+pCmp->m_pField->m_iStartPos),pCmp->m_sCmpVal.data()) < 0;
}

bool CMdbTable::CB_Filter_STR_EQUAL(stuMdbCompare *pCmp,CMdbRecord *pRecord)
{
	return strcmp((char*)(pRecord->m_pRecordData+pCmp->m_pField->m_iStartPos),pCmp->m_sCmpVal.data()) == 0;
}

bool CMdbTable::CB_Filter_STR_BIGTHAN_EQUAL(stuMdbCompare *pCmp,CMdbRecord *pRecord)
{
	return strcmp((char*)(pRecord->m_pRecordData+pCmp->m_pField->m_iStartPos),pCmp->m_sCmpVal.data()) >= 0;
}

bool CMdbTable::CB_Filter_STR_LESSTHAN_EQUAL(stuMdbCompare *pCmp,CMdbRecord *pRecord)
{
	return strcmp((char*)(pRecord->m_pRecordData+pCmp->m_pField->m_iStartPos),pCmp->m_sCmpVal.data()) <= 0;
}

bool CMdbTable::CB_Filter_STR_NOT_EQUAL(stuMdbCompare *pCmp,CMdbRecord *pRecord)
{
	return strcmp((char*)(pRecord->m_pRecordData+pCmp->m_pField->m_iStartPos),pCmp->m_sCmpVal.data()) != 0;
}

bool CMdbTable::CB_Filter_STR_LIKE(stuMdbCompare *pCmp,CMdbRecord *pRecord)
{
	return strstr((char*)(pRecord->m_pRecordData+pCmp->m_pField->m_iStartPos),pCmp->m_sCmpVal.data()) != 0;
}

bool CMdbTable::CB_Filter_STR_NOT_LIKE(stuMdbCompare *pCmp,CMdbRecord *pRecord)
{
	return strstr((char*)(pRecord->m_pRecordData+pCmp->m_pField->m_iStartPos),pCmp->m_sCmpVal.data()) == 0;
}

int CMdbTable::CB_FieldCompareInt(CMdbRecord *pRecord1, CMdbRecord *pRecord2, CMdbField *pField)
{
#ifndef _ARM
	return  *((int*)(pRecord1->m_pRecordData + pField->m_iStartPos)) - *((int*)(pRecord2->m_pRecordData + pField->m_iStartPos));

#else
	register int val1 = 0,val2 = 0;
	memcpy(&val1, pRecord1->m_pRecordData + pField->m_iStartPos, sizeof(val1));
	memcpy(&val2, pRecord2->m_pRecordData + pField->m_iStartPos, sizeof(val2));
	return val1 - val2;
#endif
}
int CMdbTable::CB_FieldCompareInt64(CMdbRecord *pRecord1, CMdbRecord *pRecord2, CMdbField *pField)
{
#ifndef _ARM
	INT64 v = (*((INT64*)(pRecord1->m_pRecordData + pField->m_iStartPos)) - *((INT64*)(pRecord2->m_pRecordData + pField->m_iStartPos)));
	if (v > 0)
		return 1;
	if(v < 0)
		return -1;
	return 0;
#else
	register INT64 val1 = 0, val2 = 0;
	memcpy(&val1, pRecord1->m_pRecordData + pField->m_iStartPos, sizeof(val1));
	memcpy(&val2, pRecord2->m_pRecordData + pField->m_iStartPos, sizeof(val2));
	v = val1 - val2;
	if (v > 0)
		return 1;
	if (v < 0)
		return -1;
	return 0;
#endif
}
int CMdbTable::CB_FieldCompareShort(CMdbRecord *pRecord1, CMdbRecord *pRecord2, CMdbField *pField)
{
#ifndef _ARM
	return  *((short*)(pRecord1->m_pRecordData + pField->m_iStartPos)) - *((short*)(pRecord2->m_pRecordData + pField->m_iStartPos));

#else
	register short val1 = 0, val2 = 0;
	memcpy(&val1, pRecord1->m_pRecordData + pField->m_iStartPos, sizeof(val1));
	memcpy(&val2, pRecord2->m_pRecordData + pField->m_iStartPos, sizeof(val2));
	return val1 - val2;
#endif
}
int CMdbTable::CB_FieldCompareByte(CMdbRecord *pRecord1, CMdbRecord *pRecord2, CMdbField *pField)
{
	return  *((BYTE*)(pRecord1->m_pRecordData + pField->m_iStartPos)) - *((BYTE*)(pRecord2->m_pRecordData + pField->m_iStartPos));
}
int CMdbTable::CB_FieldCompareFloat(CMdbRecord *pRecord1, CMdbRecord *pRecord2, CMdbField *pField)
{
#ifndef _ARM
	float fv = *((float*)(pRecord1->m_pRecordData + pField->m_iStartPos)) - *((float*)(pRecord2->m_pRecordData + pField->m_iStartPos));
	if (fv > 0)
		return 1;
	else if(fv < 0)
		return -1;
	else
		return 0;
#else
	register float val1 = 0, val2 = 0;
	memcpy(&val1, pRecord1->m_pRecordData + pField->m_iStartPos, sizeof(val1));
	memcpy(&val2, pRecord2->m_pRecordData + pField->m_iStartPos, sizeof(val2));
	val1 = val1 - val2;
	if (val1 > 0)
		return 1;
	else if (val1 < 0)
		return -1;
	else
		return 0;
#endif
}
int CMdbTable::CB_FieldCompareStr(CMdbRecord *pRecord1, CMdbRecord *pRecord2, CMdbField *pField)
{
	return  strcmp((char*)(pRecord1->m_pRecordData + pField->m_iStartPos),(char*)(pRecord2->m_pRecordData + pField->m_iStartPos));
}

////////////////////////////////////////////////////////////////////////
// 描    述:  对指定数据集进行排序
// 作    者:  邵凯田
// 创建时间:  2014-9-18 09:22
// 参数说明:  @Sort为排序条件
//			  @Result为记录集，引用返回
// 返 回 值:  void
//////////////////////////////////////////////////////////////////////////
void CMdbTable::SortResultset
(SPtrList<stuMdbSort> &Sort, SPtrList<CMdbRecord> &SrcResult, SPtrList<CMdbRecord> &DstResult)
{
	unsigned long pos=0, pos2=0;
	CMdbRecord *pFirst,*p;
	stuMdbSort *pSort;
	int cmp;//负数表示前数小于后数，正数表示大于，=0表示相等
	while (SrcResult.count() > 0)
	{
		pFirst = SrcResult.FetchFirst(pos);
		while ((p = SrcResult.FetchNext(pos)) != NULL)
		{
			pSort = Sort.FetchFirst(pos2);
			cmp = pSort->m_pCallbackFun(pFirst, p, pSort->m_pField);
			if (!pSort->m_bAsc)
				cmp = 0 - cmp;
			//相等则使用后续排序条件
			while (cmp == 0 && (pSort = Sort.FetchNext(pos2)) != NULL)
			{
				cmp = pSort->m_pCallbackFun(pFirst, p, pSort->m_pField);
				if (!pSort->m_bAsc)
					cmp = 0 - cmp;
				if (cmp > 0)
					break;
			}
			if (cmp > 0)
			{
				pFirst = p;
			}
		}
		DstResult.append(pFirst);
		SrcResult.remove(pFirst);
	}
}


////////////////////////////////////////////////////////////////////////
// 描    述:  判断指定记录在给定过滤条件下是否满足
// 作    者:  邵凯田
// 创建时间:  2014:4:22 15:44
// 参数说明:  @pRecord为待比较的记录
//         :  @Condition为过滤条件
// 返 回 值:  true表示满足条件,false表示不满足条件
//////////////////////////////////////////////////////////////////////////
bool CMdbTable::FilterRecord(CMdbRecord *pRecord,SPtrList<stuMdbCompare> &Condition)
{
	if(Condition.count() == 0)
		return true;
	unsigned long pos=0;
	bool ret;
	stuMdbCompare *pCmp = Condition.FetchFirst(pos);
	while(pCmp)
	{
		ret = pCmp->m_pCallbackFun(pCmp,pRecord);

		pCmp = Condition.FetchNext(pos);
		if(pCmp == NULL)
		{
			return ret;
		}
		else
		{
			if(pCmp->m_bAnd)
			{
				//and
				if(ret == false)
					return false;
			}
			else
			{
				//or
				if(ret == true)
					return true;
			}
		}
	}
	return true;
}

////////////////////////////////////////////////////////////////////////
// 描    述:  设定指定记录的字段值
// 作    者:  邵凯田
// 创建时间:  2014-11-25 08:41
// 参数说明:  @pRecord为待赋值的记录
//         :  @SetValue为赋值字段及值
// 返 回 值:  true表示满足条件,false表示不满足条件
//////////////////////////////////////////////////////////////////////////
bool CMdbTable::SetRecordValue(CMdbRecord *pRec, SPtrList<stuMdbSetValue> &SetValue)
{
	unsigned long pos2;
	stuMdbSetValue *pSet = SetValue.FetchFirst(pos2);
	while (pSet)
	{
		switch (pSet->m_pField->m_Type)
		{
		case MDB_INT:	//4 bytes
		case MDB_BLOB:	//4 bytes
		{
#ifndef _ARM
							*((int*)(pRec->m_pRecordData + pSet->m_pField->m_iStartPos)) = pSet->m_sValue.toInt();

#else
							int val = pSet->m_sValue.toInt();
							memcpy(pRec->m_pRecordData + pSet->m_pField->m_iStartPos, &val, sizeof(val));
#endif
		}
			break;
		case MDB_SHORT:	//2 bytes
		{
#ifndef _ARM
							*((short*)(pRec->m_pRecordData + pSet->m_pField->m_iStartPos)) = pSet->m_sValue.toShort();

#else
							short val = pSet->m_sValue.toShort();
							memcpy(pRec->m_pRecordData + pSet->m_pField->m_iStartPos, &val, sizeof(val));
#endif
		}
			break;
		case MDB_BYTE:	//1 bytes
			pRec->m_pRecordData[pSet->m_pField->m_iStartPos] = (BYTE)pSet->m_sValue.toInt();
			break;
		case MDB_INT64:	//8 bytes
		{
#ifndef _ARM
							*((INT64*)(pRec->m_pRecordData + pSet->m_pField->m_iStartPos)) = pSet->m_sValue.toInt();

#else
							INT64 val = pSet->m_sValue.toInt();
							memcpy(pRec->m_pRecordData + pSet->m_pField->m_iStartPos, &val, sizeof(val));
#endif
		}
			break;
		case MDB_FLOAT:	//4 bytes
		{
#ifndef _ARM
							*((float*)(pRec->m_pRecordData + pSet->m_pField->m_iStartPos)) = pSet->m_sValue.toFloat();

#else
							float val = pSet->m_sValue.toFloat();
							memcpy(pRec->m_pRecordData + pSet->m_pField->m_iStartPos, &val, sizeof(val));
#endif
		}
			break;
		case MDB_STR:	//N bytes（由建表语法决定）
		{
							int len = pSet->m_sValue.length();
							if (len >= pSet->m_pField->m_iBytes)
								len = pSet->m_pField->m_iBytes - 1;
							memcpy(pRec->m_pRecordData + pSet->m_pField->m_iStartPos, pSet->m_sValue.data(), len);
							pRec->m_pRecordData[pSet->m_pField->m_iStartPos + len] = '\0';
		}
			break;
		}

		pSet = SetValue.FetchNext(pos2);
	}
	return true;
}

////////////////////////////////////////////////////////////////////////
// 描    述:  插入1行或多行记录
// 作    者:  邵凯田
// 创建时间:  2014:4:22 15:05
// 参数说明:  @pRecData表示记录缓冲区，可在内部修改为NULL，内容负责释放
//         :  @iRowSize表示一行记录的长度（字节数）
//         :  @iRows表示有多少条记录待插入，默认为1
//		   :  @bHisRestore表示是否为历史表在恢复数据
// 返 回 值:  成功插件的记录数量
//////////////////////////////////////////////////////////////////////////
int CMdbTable::Insert(BYTE* &pRecData, int iRowSize, int iRows, bool bHisRestore)
{
	if(iRowSize != m_iRowSize || iRows < 0 || iRows > C_MAX_INSERT_ONCE)
		return -1;
	CMdbRecord *pNew,*pOld;
	int i,ret=0;
	//unsigned long pos=0;
//	CMdbTableIndex *pIdx;
	bool bUniqueErr = false;
	for (i = 0; i < iRows; i++)
	{
		pNew = new CMdbRecord(pRecData + i*iRowSize, iRowSize/*, true*/);
		//(为降低处理复杂度，暂不使用索引)
// 		pIdx = m_MyIndex.FetchFirst(pos);
// 		while (pIdx)
// 		{
// 			if (!pIdx->AddRecord(pNew))
// 			{
// 				bUniqueErr = true;
// 				break;
// 			}
// 			pIdx = m_MyIndex.FetchNext(pos);
// 		}
// 		if (bUniqueErr)
// 		{
// 			delete pNew;
// 			break;
// 		}
		m_Records.append(pNew);
		if (m_Records.count() > m_iMemMaxRows)
		{
			pOld = m_Records[0];
			m_Records.remove(0);
			m_pMgr->AddDelRecord(pOld);
		}
		ret++;
	}
	if (m_pSysTableRecord != NULL)
	{
		m_pSysTableRecord->rows = m_Records.count();
	}
	if (!bHisRestore && (m_TriggerFlag & 0x01) != 0 && ret > 0)
	{
		//生成触发记录
		if(m_InsertTrigger.count() > C_MAX_BUFFERED_TRIGGER)
		{
			static int last_soc = 0;
			if((int)::time(NULL) != last_soc)
			{
				last_soc = (int)::time(NULL);
				LOGWARN("插入触发列队满(>%d)，将丢弃部分数据!",C_MAX_BUFFERED_TRIGGER);
			}
		}
		else
		{
			CTableTrigger *pTrg = new CTableTrigger();
			pTrg->iTriggerType = 1;
			pTrg->m_iTrgRows = iRows;
			pTrg->m_pTrgRowsData = new BYTE[iRows*iRowSize];
			memcpy(pTrg->m_pTrgRowsData, pRecData, iRows*iRowSize);
			m_InsertTrigger.append(pTrg);
		}
	}
	if (!bHisRestore && m_pHisDbTable != NULL && ret > 0)
	{
		m_pMgr->AddHisSyncOper(new CMdbManager::stuMdbMgrHisDbSyncOper(MDB_TRG_INSERT,m_pHisDbTable, pRecData, iRows));
		pRecData = NULL;
	}
	if (bUniqueErr)
		return MDB_RET_UNIQUE_VIOLATION;
	return ret;
}

////////////////////////////////////////////////////////////////////////
// 描    述:  更新或插入一批记录，以主键为唯一标识，无主键的表不允许使用，如果记录存在更新，不存在则插入
// 作    者:  邵凯田
// 创建时间:  2014-04-22 15:05
// 参数说明:  @pRecData表示记录缓冲区，可在内部修改为NULL，内容负责释放
//         :  @iRowSize表示一行记录的长度（字节数）
//         :  @iRows表示有多少条记录待插入，默认为1
//		   :  @bHisRestore表示是否为历史表在恢复数据
// 返 回 值:  成功更新或插入的记录数量
//////////////////////////////////////////////////////////////////////////
int CMdbTable::UpdateWithInsert(BYTE* &pRecData, int iRowSize, int iRows)
{
	if (iRowSize != m_iRowSize || iRows < 0 || iRows > C_MAX_INSERT_ONCE)
		return -1;
	if (m_pPkIndex == NULL)
		return -2;//无主键不允许使用
	CMdbRecord *pNew, rec;
	int i,ret = 0;
	unsigned long pos=0;
	CMdbTableIndex *pIdx = NULL;
	//bool bUniqueErr = false;
	SPtrList<CMdbRecord> uplist, inslist;
	uplist.setAutoDelete(false);
	inslist.setAutoDelete(false);
	for (i = 0; i < iRows; i++)
	{
		rec.m_pRecordData = pRecData + i*iRowSize;
		pNew = m_pPkIndex->SearchOnByIndex(&rec);
		if (pNew == NULL)
		{
			//insert 
			pNew = new CMdbRecord(pRecData + i*iRowSize, iRowSize/*, true*/);
			inslist.append(pNew);
			//插入内存数据
			m_Records.append(pNew);

			//(为降低处理复杂度，暂不使用索引)
			pIdx = m_MyIndex.FetchFirst(pos);
			while (pIdx)
			{
				if (!pIdx->AddRecord(pNew))
				{
					//bUniqueErr = true;
					break;
				}
				pIdx = m_MyIndex.FetchNext(pos);
			}

		}
		else
		{
			//update
			uplist.append(pNew);
			//更新内存数据
			memcpy(pNew->m_pRecordData, pRecData + i*iRowSize, iRowSize);
		}
	}
	rec.m_pRecordData = NULL;

	if (((m_TriggerFlag & 0x01) != 0 || m_pHisDbTable != NULL) && inslist.count() > 0)
	//if (inslist.count() > 0)
	{
		if (m_pSysTableRecord != NULL)
			m_pSysTableRecord->rows = m_Records.count();
		BYTE *pBuffer = new BYTE[inslist.count()*iRowSize];
		if (inslist.count() == iRows)
			memcpy(pBuffer, pRecData, iRows*iRowSize);
		else
		{
			i = 0;
			pNew = inslist.FetchFirst(pos);
			while (pNew)
			{
				memcpy(pBuffer + i, pNew->m_pRecordData, iRowSize);
				i += m_iRowSize;
				pNew = inslist.FetchNext(pos);
			}
		}
		
		if ((m_TriggerFlag & 0x01) != 0)
		{
			if(m_InsertTrigger.count() > C_MAX_BUFFERED_TRIGGER)
			{
				static int last_soc = 0;
				if((int)::time(NULL) != last_soc)
				{
					last_soc = (int)::time(NULL);
					LOGWARN("插入触发列队满(>%d)，将丢弃部分数据!",C_MAX_BUFFERED_TRIGGER);
				}
			}
			else
			{
				//生成插入触发记录
				CTableTrigger *pTrg = new CTableTrigger();
				pTrg->iTriggerType = 1;
				pTrg->m_iTrgRows = inslist.count();
				pTrg->m_pTrgRowsData = new BYTE[inslist.count()*iRowSize];
				memcpy(pTrg->m_pTrgRowsData, pBuffer, inslist.count()*iRowSize);
			
				m_InsertTrigger.append(pTrg);
			}
		}
		if (m_pHisDbTable != NULL)
		{
			m_pMgr->AddHisSyncOper(new CMdbManager::stuMdbMgrHisDbSyncOper(MDB_TRG_INSERT, m_pHisDbTable, pBuffer, inslist.count()));
			pBuffer = NULL;
		}
		if (pBuffer != NULL)
			delete[] pBuffer;
	}
	if (((m_TriggerFlag & 0x02) != 0 || m_pHisDbTable != NULL) && uplist.count() > 0)
	//if (uplist.count() > 0)
	{
		BYTE *pBuffer = new BYTE[uplist.count()*iRowSize];
		if (uplist.count() == iRows)
			memcpy(pBuffer, pRecData, iRows*iRowSize);
		else
		{
			i = 0;
			pNew = uplist.FetchFirst(pos);
			while (pNew)
			{
				memcpy(pBuffer + i, pNew->m_pRecordData, iRowSize);
				i += m_iRowSize;
				pNew = uplist.FetchNext(pos);
			}
		}
		if ((m_TriggerFlag & 0x02) != 0)
		{
			if(m_UpdateTrigger.count() > C_MAX_BUFFERED_TRIGGER)
			{
				static int last_soc = 0;
				if((int)::time(NULL) != last_soc)
				{
					last_soc = (int)::time(NULL);
					LOGWARN("更新触发列队满(>%d)，将丢弃部分数据!",C_MAX_BUFFERED_TRIGGER);
				}
			}
			else
			{
				//生成触发记录
				CTableTrigger *pTrg = new CTableTrigger();
				pTrg->iTriggerType = 2;
				pTrg->m_iTrgRows = uplist.count();
				pTrg->m_pTrgRowsData = new BYTE[uplist.count()*m_iRowSize];
				memcpy(pTrg->m_pTrgRowsData, pBuffer, uplist.count()*iRowSize);
				m_UpdateTrigger.append(pTrg);
			}

		}
		if (m_pHisDbTable != NULL)
		{
			m_pMgr->AddHisSyncOper(new CMdbManager::stuMdbMgrHisDbSyncOper(MDB_TRG_UPDATE_WITHPK, m_pHisDbTable, pBuffer, uplist.count()));
			pBuffer = NULL;
		}
		if (pBuffer != NULL)
			delete[] pBuffer;
	}
	return ret;
}

////////////////////////////////////////////////////////////////////////
// 描    述:  删除指定条件的记录
// 作    者:  邵凯田
// 创建时间:  2014:4:22 15:22
// 参数说明:  @Condition表示过滤条件
// 返 回 值:  成功删除的记录数量，<0表示失败
//////////////////////////////////////////////////////////////////////////
int CMdbTable::Delete(SPtrList<stuMdbCompare> &Condition)
{
	SPtrList<CMdbRecord> Result,Del;
	Result.setAutoDelete(false);
	Del.setAutoDelete(false);
	SPtrList<stuMdbSort> Sort;
	Select(Condition,Sort,Result,NULL,0,true);
	unsigned long pos=0, pos2=0;
	CMdbTableIndex *pIdx;
	bool bBlob = false;
	CMdbField *pField = m_Fields.FetchFirst(pos2);
	while (pField)
	{
		if (pField->m_Type == MDB_BLOB)
		{
			bBlob = true;
			break;
		}
		pField = m_Fields.FetchNext(pos2);
	}
	CMdbRecord *pRec = Result.FetchFirst(pos);
	while(pRec)
	{
		//(为降低处理复杂度，暂不使用索引)
		pIdx = m_MyIndex.FetchFirst(pos2);
		while (pIdx)
		{
			pIdx->RemoveRecord(pRec);
			pIdx = m_MyIndex.FetchNext(pos2);
		}
		//清除可能存在的大字段文件
		if (bBlob)
		{
			pField = m_Fields.FetchFirst(pos2);
			while (pField)
			{
				if (pField->m_Type == MDB_BLOB)
				{
					int val = GetValueInt(pRec->m_pRecordData, pField);
					if (val > 0)
					{
						SString file;
#ifdef WIN32
						file.sprintf("%s\\db\\%s\\%d.blob", m_pMgr->GetMdbHomePath().data(), m_sTableName.data(), val);
#else
						file.sprintf("%s/db/%s/%d.blob", m_pMgr->GetMdbHomePath().data(), m_sTableName.data(), val);
#endif
						SFile f;
						f.remove(file);
					}
				}
				pField = m_Fields.FetchNext(pos2);
			}
		}
		Del.append(pRec);
		//m_Records.remove(pRec);
		pRec = Result.FetchNext(pos);
	}
	if(m_pSysTableRecord != NULL)
		m_pSysTableRecord->rows = m_Records.count();

	if (Del.count() > 0)
	{
		if ((m_TriggerFlag & 0x04) != 0)
		{
			if(m_DeleteTrigger.count() > C_MAX_BUFFERED_TRIGGER)
			{
				static int last_soc = 0;
				if((int)::time(NULL) != last_soc)
				{
					last_soc = (int)::time(NULL);
					LOGWARN("删除触发列队满(>%d)，将丢弃部分数据!",C_MAX_BUFFERED_TRIGGER);
				}
			}
			else
			{
				//生成触发记录
				CTableTrigger *pTrg = new CTableTrigger();
				pTrg->iTriggerType = 3;
				pTrg->m_iTrgRows = Del.count();
				pTrg->m_pTrgRowsData = new BYTE[Del.count()*m_iRowSize];
				pRec = Del.FetchFirst(pos);
				int p = 0;
				while (pRec)
				{
					memcpy(pTrg->m_pTrgRowsData + p, pRec->m_pRecordData, m_iRowSize);
					p += m_iRowSize;
					pRec = Del.FetchNext(pos);
				}
				m_DeleteTrigger.append(pTrg);
			}
		}
		Del.copyto(*m_pMgr->GetDelRecordPtr());
		if (m_pHisDbTable != NULL)
		{
			m_pMgr->AddHisSyncOper(new CMdbManager::stuMdbMgrHisDbSyncOper(MDB_TRG_DELETE,m_pHisDbTable, Condition));
		}
	}
	return Result.count();
}

////////////////////////////////////////////////////////////////////////
// 描    述:  更新指定条件记录中的指定值为新内容
// 作    者:  邵凯田
// 创建时间:  2014:4:22 15:26
// 参数说明:  @SetValue为赋值列表
//         :  @Condition表示过滤条件
// 返 回 值:  成功更新的记录数量，<0表示失败
//////////////////////////////////////////////////////////////////////////
int CMdbTable::Update(SPtrList<stuMdbSetValue> &SetValue, SPtrList<stuMdbCompare> &Condition)
{
	SPtrList<CMdbRecord> Result;
	Result.setAutoDelete(false);
	SPtrList<stuMdbSort> Sort;
	Select(Condition,Sort,Result);
	unsigned long pos=0, pos_idx=0;
	CMdbTableIndex *pIdx;

	//int cnt=0;
	CMdbRecord *pRec = Result.FetchFirst(pos);
	while(pRec)
	{
		//(为降低处理复杂度，暂不使用索引)
		//删除原有索引
		pIdx = m_MyIndex.FetchFirst(pos_idx);
		while (pIdx)
		{
			pIdx->RemoveRecord(pRec);
			pIdx = m_MyIndex.FetchNext(pos_idx);
		}

		//修改指定字段的值内容
		SetRecordValue(pRec, SetValue);

		//(为降低处理复杂度，暂不使用索引)
		//重新添加索引
		pIdx = m_MyIndex.FetchFirst(pos_idx);
		while (pIdx)
		{
			pIdx->AddRecord(pRec);
			pIdx = m_MyIndex.FetchNext(pos_idx);
		}

		pRec = Result.FetchNext(pos);
	}
	if (Result.count() > 0 && (m_TriggerFlag & 0x02) != 0)
	{
		if(m_UpdateTrigger.count() > C_MAX_BUFFERED_TRIGGER)
		{
			static int last_soc = 0;
			if((int)::time(NULL) != last_soc)
			{
				last_soc = (int)::time(NULL);
				LOGWARN("更新触发列队满(>%d)，将丢弃部分数据!",C_MAX_BUFFERED_TRIGGER);
			}
		}
		else
		{
			//生成触发记录
			CTableTrigger *pTrg = new CTableTrigger();
			pTrg->iTriggerType = 2;
			pTrg->m_iTrgRows = Result.count();
			pTrg->m_pTrgRowsData = new BYTE[Result.count()*m_iRowSize];
			pRec = Result.FetchFirst(pos);
			int p = 0;
			while (pRec)
			{
				memcpy(pTrg->m_pTrgRowsData + p, pRec->m_pRecordData, m_iRowSize);
				p += m_iRowSize;
				pRec = Result.FetchNext(pos);
			}
			m_UpdateTrigger.append(pTrg);
		}
	}
	if (Result.count() > 0 && m_pHisDbTable != NULL)
	{
		m_pMgr->AddHisSyncOper(new CMdbManager::stuMdbMgrHisDbSyncOper(MDB_TRG_UPDATE, m_pHisDbTable, Condition, SetValue));
	}
	return Result.count();
}

////////////////////////////////////////////////////////////////////////
// 描    述:  更新BLOB字段
// 作    者:  邵凯田
// 创建时间:  2014-12-22 16:20
// 参数说明:  @pBlobBuf为待更新的大字段缓冲区内容
//         :  @iBlobSize表示缓冲区大小
//         :  @sFieldName为大字段名称
//         :  @Condition为指定的条件，指定条件应命中一条唯一的记录
// 返 回 值:  成功更新的记录数量，<0表示失败，=0表示未命中
//////////////////////////////////////////////////////////////////////////
int CMdbTable::UpdateBlob(BYTE *pBlobBuf, int iBlobSize, SString &sFieldName, SPtrList<stuMdbCompare> &Condition)
{
	SPtrList<stuMdbSort> Sort;
	SPtrList<CMdbRecord> Record;
	int cnt = Select(Condition, Sort, Record);
	if (cnt != 1 || Record.count() != 1)
		return 0;
	CMdbField *pField = SearchFieldByName(sFieldName.data());
	if (pField == NULL)
		return MDB_RET_FIELD_NOTEXIST;
	if (pField->m_Type != MDB_BLOB)
		return MDB_RET_TYPE_ERROR;
	CMdbRecord *pRec = Record[0];
	int val = GetValueStr(pRec->m_pRecordData, pField).toInt();
	bool bNewVal = false;
	if (val == 0)
	{
		//创建一个新的文件
		SPtrList<stuMdbCompare> ConEmpty;
		val = GetTotalFunctonVal(ConEmpty, "max", pField).toInt() + 1;
		bNewVal = true;
	}
	SString blob = m_pMgr->GetMdbHomePath();
	SDir::createDir(blob);
#ifdef WIN32
	blob += "\\db\\";
#else
	blob += "/db/";
#endif
	SDir::createDir(blob);
#ifdef WIN32
	blob += m_sTableName + "\\";
#else
	blob += m_sTableName + "/";
#endif
	SDir::createDir(blob);
	blob += SString::toFormat("%d.blob", val);
	SFile f(blob);
	if (!f.open(IO_Truncate))
	{
		LOGERROR("创建表(%s)的大字段文件(%s)时失败!", m_sTableName.data(), blob.data());
		return MDB_RET_DISK_ERROR;
	}
	if (f.writeBlock(pBlobBuf, iBlobSize) != iBlobSize)
	{
		f.close();
		f.remove();
		LOGERROR("写表(%s)的大字段文件(%s)时失败!", m_sTableName.data(), blob.data());
		return MDB_RET_DISK_ERROR;
	}
	f.close();
	if (bNewVal)
	{
		SPtrList<stuMdbSetValue> SetValue;
		stuMdbSetValue *p = new stuMdbSetValue();
		p->m_pField = pField;
		p->m_sValue.sprintf("%d", val);
		SetValue.setAutoDelete(true);
		SetValue.append(p);
		Update(SetValue, Condition);
	}
	return 1;
}

////////////////////////////////////////////////////////////////////////
// 描    述:  读取BLOB字段
// 作    者:  邵凯田
// 创建时间:  2014-12-22 16:26
// 参数说明:  @pBlobBuf为待读取的大字段缓冲区内容，在函数申请缓冲区，调用者负责释放内存
//         :  @iBlobSize表示缓冲区大小
//         :  @sFieldName为大字段名称
//         :  @Condition为指定的条件，指定条件应命中一条唯一的记录
// 返 回 值:  成功读取的记录数量，<0表示失败，=0表示未命中
//////////////////////////////////////////////////////////////////////////
int CMdbTable::ReadBlob(BYTE* &pBlobBuf, int &iBlobSize, SString sFieldName, SPtrList<stuMdbCompare> &Condition)
{
	SPtrList<stuMdbSort> Sort;
	SPtrList<CMdbRecord> Record;
	int cnt = Select(Condition, Sort, Record);
	if (cnt != 1 || Record.count() != 1)
		return 0;
	CMdbField *pField = SearchFieldByName(sFieldName.data());
	if (pField == NULL)
		return MDB_RET_FIELD_NOTEXIST;
	if (pField->m_Type != MDB_BLOB)
		return MDB_RET_TYPE_ERROR;
	CMdbRecord *pRec = Record[0];
	int val = GetValueStr(pRec->m_pRecordData, pField).toInt();
	if (val == 0)
	{
		return MDB_RET_CONTEXT_NOTEXIST;
	}
	SString blob = m_pMgr->GetMdbHomePath();
	SDir::createDir(blob);
#ifdef WIN32
	blob += "\\db\\";
#else
	blob += "/db/";
#endif
	SDir::createDir(blob);
#ifdef WIN32
	blob += m_sTableName + "\\";
#else
	blob += m_sTableName + "/";
#endif
	SDir::createDir(blob);
	blob += SString::toFormat("%d.blob", val);
	SFile f(blob);
	if (!f.open(IO_ReadOnly))
	{
		LOGERROR("打开表(%s)的大字段文件(%s)时失败!", m_sTableName.data(), blob.data());
		return MDB_RET_DISK_ERROR;
	}
	iBlobSize = f.size();
	if (iBlobSize == 0)
	{
		pBlobBuf = NULL;
		return 1;
	}
	pBlobBuf = new BYTE[iBlobSize];
	if (f.readBlock(pBlobBuf, iBlobSize) != iBlobSize)
	{
		f.close();
		delete[] pBlobBuf;
		pBlobBuf = NULL;
		iBlobSize = 0;
		LOGERROR("读表(%s)的大字段文件(%s)时失败!", m_sTableName.data(), blob.data());
		return MDB_RET_DISK_ERROR;
	}
	f.close();
	return 1;
}


////////////////////////////////////////////////////////////////////////
// 描    述:  取记录中指定序号的字段字符串值内容
// 作    者:  邵凯田
// 创建时间:  2014-11-14 10:46
// 参数说明:  @pRow为记录内容
//			  @pFld为字段信息
// 返 回 值:  字符串型值内容
//////////////////////////////////////////////////////////////////////////
SString CMdbTable::GetValueStr(BYTE *pRow, CMdbField *pFld)
{
	SString str;
	switch (pFld->m_Type)
	{
		case MDB_INT:	//4 bytes
		case MDB_BLOB:
		{
	#ifndef _ARM
			str = SString::toString(*((int*)(pRow + pFld->m_iStartPos)));

	#else
			int val;
			memcpy(&val,pRow + pFld->m_iStartPos,4);
			str = SString::toString(val);
	#endif
		}
			break;
		case MDB_SHORT:	//2 bytes
		{
	#ifndef _ARM
			str = SString::toString(*((short*)(pRow + pFld->m_iStartPos)));
	#else
			short val;
			memcpy(&val, pRow + pFld->m_iStartPos, 2);
			str = SString::toString(val);
	#endif
		}
			break;
		case MDB_BYTE:	//1 bytes
			str = SString::toString(pRow[pFld->m_iStartPos]);
			break;
		case MDB_INT64:	//8 bytes
		{
	#ifndef _ARM
			str = SString::toFormat("%ld", *((INT64*)(pRow + pFld->m_iStartPos)));
	#else
			INT64 val;
			memcpy(&val, pRow + pFld->m_iStartPos, 8);
			str = SString::toFormat("%ld", val);
	#endif
		}
			break;
		case MDB_FLOAT:	//4 bytes
		{
	#ifndef _ARM
			str = SString::toFormat("%f", *((float*)(pRow + pFld->m_iStartPos)));
	#else
			float val;
			memcpy(&val, pRow + pFld->m_iStartPos, 4);
			str = SString::toFormat("%f", val);
	#endif
		}
			break;
		case MDB_STR:	//N bytes（由建表语法决定）
		{
			str = SString::toString((char*)(pRow + pFld->m_iStartPos), pFld->m_iBytes);
		}
			break;
		default:
		{
			LOGDEBUG("字段%s有无效的数据类型:%d", pFld->m_sName.data(), pFld->m_Type);
		}
		break;
	}
	return str;
}

////////////////////////////////////////////////////////////////////////
// 描    述:  取记录中指定序号的字段整数值内容
// 作    者:  邵凯田
// 创建时间:  2014-11-14 10:46
// 参数说明:  @pRow为记录内容
//			  @pFld为字段信息
// 返 回 值:  字符串型值内容
//////////////////////////////////////////////////////////////////////////
int CMdbTable::GetValueInt(BYTE *pRow, CMdbField *pFld)
{
	switch (pFld->m_Type)
	{
		case MDB_INT:	//4 bytes
		case MDB_BLOB:
		{
	#ifndef _ARM
			return *((int*)(pRow + pFld->m_iStartPos));
	#else
			int val;
			memcpy(&val, pRow + pFld->m_iStartPos, 4);
			return val;
	#endif
		}
			break;
		case MDB_SHORT:	//2 bytes
		{
	#ifndef _ARM
			return *((short*)(pRow + pFld->m_iStartPos));
	#else
			short val;
			memcpy(&val, pRow + pFld->m_iStartPos, 2);
			return val;
	#endif
		}
			break;
		case MDB_BYTE:	//1 bytes
			return pRow[pFld->m_iStartPos];
			break;
		default:
			break;
	}
	return 0;
}

////////////////////////////////////////////////////////////////////////
// 描    述:  取指定条件下的汇总函数执行结果
// 作    者:  邵凯田
// 创建时间:  2014-12-22 17:12
// 参数说明:  @Condition为过滤条件
//			  @sFunName为函数名称(count,sum,min,max,avg)
//			  @pField为对应字段，NULL表示不对应具体字段
// 返 回 值:  字符串型值内容
//////////////////////////////////////////////////////////////////////////
SString CMdbTable::GetTotalFunctonVal(SPtrList<stuMdbCompare> &Condition, SString sFunName, CMdbField *pField)
{
	SPtrList<CMdbRecord> *pRecs = &m_Records;
	SPtrList<stuMdbSort> Sort;
	SPtrList<CMdbRecord> recs;
	recs.setAutoDelete(false);
	if (Condition.count() > 0)
	{
		Select(Condition, Sort, recs);
		pRecs = &recs;
	}
	unsigned long pos=0;
	CMdbRecord *pRec;
	SString sVal;
	if (sFunName == "count")
		return SString::toString(pRecs->count());
	if (pField == NULL)
		return "";
	if (sFunName == "sum")
	{
		if (pField->m_Type == MDB_FLOAT)
		{
			float val = 0;
			pRec = pRecs->FetchFirst(pos);
			while (pRec)
			{
				val += GetValueFloat(pRec->m_pRecordData, pField);
				pRec = pRecs->FetchNext(pos);
			}
			sVal.sprintf("%f", val);
		}
		else if (pField->m_Type == MDB_INT || pField->m_Type == MDB_SHORT || pField->m_Type == MDB_BYTE)
		{
			int val = 0;
			pRec = pRecs->FetchFirst(pos);
			while (pRec)
			{
				val += GetValueInt(pRec->m_pRecordData, pField);
				pRec = pRecs->FetchNext(pos);
			}
			sVal.sprintf("%d", val);
		}
	}
	else if (sFunName == "min")
	{
		if (pField->m_Type == MDB_FLOAT)
		{
			float val=0,v;
			pRec = pRecs->FetchFirst(pos);
			if (pRec)
			{
				val = GetValueFloat(pRec->m_pRecordData, pField);
				pRec = pRecs->FetchNext(pos);
			}
			while (pRec)
			{
				v = GetValueFloat(pRec->m_pRecordData, pField);
				if (v < val)
					val = v;
				pRec = pRecs->FetchNext(pos);
			}
			sVal.sprintf("%f", val);
		}
		else if (pField->m_Type == MDB_INT || pField->m_Type == MDB_SHORT || pField->m_Type == MDB_BYTE)
		{
			int val=0,v;
			pRec = pRecs->FetchFirst(pos);
			if (pRec)
			{
				val = GetValueInt(pRec->m_pRecordData, pField);
				pRec = pRecs->FetchNext(pos);
			}
			while (pRec)
			{
				v = GetValueInt(pRec->m_pRecordData, pField);
				if (v < val)
					val = v;
				pRec = pRecs->FetchNext(pos);
			}
			sVal.sprintf("%d", val);
		}
	}
	else if (sFunName == "max")
	{
		if (pField->m_Type == MDB_FLOAT)
		{
			float val=0, v;
			pRec = pRecs->FetchFirst(pos);
			if (pRec)
			{
				val = GetValueFloat(pRec->m_pRecordData, pField);
				pRec = pRecs->FetchNext(pos);
			}
			while (pRec)
			{
				v = GetValueFloat(pRec->m_pRecordData, pField);
				if (v > val)
					val = v;
				pRec = pRecs->FetchNext(pos);
			}
			sVal.sprintf("%f", val);
		}
		else if (pField->m_Type == MDB_BLOB || pField->m_Type == MDB_INT || pField->m_Type == MDB_SHORT || pField->m_Type == MDB_BYTE)
		{
			int val=0, v;
			pRec = pRecs->FetchFirst(pos);
			if (pRec)
			{
				val = GetValueInt(pRec->m_pRecordData, pField);
				pRec = pRecs->FetchNext(pos);
			}
			while (pRec)
			{
				v = GetValueInt(pRec->m_pRecordData, pField);
				if (v > val)
					val = v;
				pRec = pRecs->FetchNext(pos);
			}
			sVal.sprintf("%d", val);
		}
	}
	else if (sFunName == "avg")
	{
		if (pField->m_Type == MDB_FLOAT)
		{
			double val = 0;
			pRec = pRecs->FetchFirst(pos);
			while (pRec)
			{
				val += GetValueFloat(pRec->m_pRecordData, pField);
				pRec = pRecs->FetchNext(pos);
			}
			if (pRecs->count() > 0)
				val /= pRecs->count();
			sVal.sprintf("%f", (float)val);
		}
		else if (pField->m_Type == MDB_INT || pField->m_Type == MDB_SHORT || pField->m_Type == MDB_BYTE)
		{
			INT64 val = 0;
			pRec = pRecs->FetchFirst(pos);
			while (pRec)
			{
				val += GetValueInt(pRec->m_pRecordData, pField);
				pRec = pRecs->FetchNext(pos);
			}
			if (pRecs->count() > 0)
				val /= pRecs->count();
			int ival = (int)val;
			sVal.sprintf("%d", ival);
		}
	}
	return sVal;
}

////////////////////////////////////////////////////////////////////////
// 描    述:  取记录中指定序号的字段浮点数值内容
// 作    者:  邵凯田
// 创建时间:  2014-11-14 10:46
// 参数说明:  @pRow为记录内容
//			  @pFld为字段信息
// 返 回 值:  字符串型值内容
//////////////////////////////////////////////////////////////////////////
float CMdbTable::GetValueFloat(BYTE *pRow, CMdbField *pFld)
{
	if (pFld->m_Type != MDB_FLOAT)
		return 0;
#ifndef _ARM
	return *((float*)(pRow + pFld->m_iStartPos));
#else
	float val;
	memcpy(&val, pRow + pFld->m_iStartPos, 4);
	return val;
#endif
}

////////////////////////////////////////////////////////////////////////
// 描    述:  向数据表添加对应的索引，索引只加不删
// 作    者:  邵凯田
// 创建时间:  2014-9-18 09:22
// 参数说明:  @pIdx为索引指针
// 返 回 值:  void
//////////////////////////////////////////////////////////////////////////
void CMdbTable::AddIndex(CMdbTableIndex *pIdx)
{
	if (m_pHisDbTable != NULL)
		m_pHisDbTable->AddIndex(pIdx);
	m_MyIndex.append(pIdx);
}

////////////////////////////////////////////////////////////////////////
// 描    述:  截断表的全部内容
// 作    者:  邵凯田
// 创建时间:  2014:4:22 15:27
// 参数说明:  void
// 返 回 值:  被截断记录的总数
//////////////////////////////////////////////////////////////////////////
int CMdbTable::Truncate()
{
	int cnt = m_Records.count();
	if (cnt <= 0)
		return 0;
	
	unsigned long pos=0,pos2=0;
	CMdbTableIndex *pIdx = m_MyIndex.FetchFirst(pos);
	while (pIdx)
	{
		pIdx->ClearRecord();
		pIdx = m_MyIndex.FetchNext(pos);
	}
	bool bBlob = false;
	CMdbField *pField = m_Fields.FetchFirst(pos2);
	while (pField)
	{
		if (pField->m_Type == MDB_BLOB)
		{
			bBlob = true;
			break;
		}
		pField = m_Fields.FetchNext(pos2);
	}
	if (bBlob)
	{
		CMdbRecord *pRec = m_Records.FetchFirst(pos);
		while (pRec)
		{
			pField = m_Fields.FetchFirst(pos2);
			while (pField)
			{
				if (pField->m_Type == MDB_BLOB)
				{
					int val = GetValueInt(pRec->m_pRecordData, pField);
					if (val > 0)
					{
						SString file;
#ifdef WIN32
						file.sprintf("%s\\db\\%s\\%d.blob", m_pMgr->GetMdbHomePath().data(), m_sTableName.data(), val);
#else
						file.sprintf("%s/db/%s/%d.blob", m_pMgr->GetMdbHomePath().data(), m_sTableName.data(), val);
#endif
						SFile f;
						f.remove(file);
					}
				}
				pField = m_Fields.FetchNext(pos2);
			}
			pRec = m_Records.FetchNext(pos);
		}
	}
	m_Records.copyto(*m_pMgr->GetDelRecordPtr());
	m_Records.clear();
	if(m_pSysTableRecord != NULL)
		m_pSysTableRecord->rows = 0;

	if (cnt > 0 && (m_TriggerFlag & 0x08) != 0)
	{
		//生成触发记录
		if(m_TruncateTrigger.count() > C_MAX_BUFFERED_TRIGGER)
		{
			static int last_soc = 0;
			if((int)::time(NULL) != last_soc)
			{
				last_soc = (int)::time(NULL);
				LOGWARN("清空触发列队满(>%d)，将丢弃部分数据!",C_MAX_BUFFERED_TRIGGER);
			}
		}
		else
		{
			CTableTrigger *pTrg = new CTableTrigger();
			pTrg->iTriggerType = 4;
			pTrg->m_iTrgRows = cnt;
			pTrg->m_pTrgRowsData = NULL;
			m_TruncateTrigger.append(pTrg);
		}
	}

	if (m_pHisDbTable != NULL)
	{
		m_pMgr->AddHisSyncOper(new CMdbManager::stuMdbMgrHisDbSyncOper(MDB_TRG_TRUNCATE, m_pHisDbTable));
	}
	return cnt;
}

////////////////////////////////////////////////////////////////////////
// 描    述:  查询条件的记录，并按指定条件排序
// 作    者:  邵凯田
// 创建时间:  2014:4:22 15:31
// 参数说明:  @Condition表示过滤条件
//         :  @Sort表示排序条件，可以指定多个字段为排序条件，并指定升序排列或降序排列
//         :  @Result表示结果记录集
//         :  @pIndexRows表示被索引的部分记录集合，NULL表示没有匹配的索引集合（使用全记录过滤）
//         :  @iLimit表示限制记录行数，0表示不限制
//         :  @bAutoDel表示是否自动删除选中的记录
// 返 回 值:  被查询出的记录数量，<0表示失败
//////////////////////////////////////////////////////////////////////////
int CMdbTable::Select(SPtrList<stuMdbCompare> &Condition, SPtrList<stuMdbSort> &Sort, SPtrList<CMdbRecord> &Result, 
					  SPtrList<CMdbRecord> *pIndexRows,int iLimit,bool bAutoDel)
{
	unsigned long pos=0;
	Result.clear();
	Result.setAutoDelete(false);
	if (pIndexRows == NULL)
		pIndexRows = &m_Records;
	SPtrList<CMdbRecord> Result0;
	SPtrList<CMdbRecord> *pResult;
	if (Sort.count() == 0)
		pResult = &Result;
	else
		pResult = &Result0;//待排序
	CMdbRecord *pRec = pIndexRows->FetchFirst(pos);
	while(pRec)
	{
		if (FilterRecord(pRec, Condition))
		{
			if (pResult->count() >= C_MDB_MAX_RETRIEVE_ROWS)
			{
				LOGWARN("结果集数量超出了最大限制数(%d)条!将截断结果集!", C_MDB_MAX_RETRIEVE_ROWS);
				break;
				// return MDB_RET_RETRIEVE_OVERFLOW;
			}
			pResult->append(pRec);
			if(bAutoDel)
			{
				pRec = pIndexRows->FetchNextAndRemoveThis(pos);
				if(iLimit > 0 && Sort.count() == 0 && pResult->count() >= iLimit)
					break;
				continue;
			}
			if(iLimit > 0 && Sort.count() == 0 && pResult->count() >= iLimit)
				break;
		}
		pRec = pIndexRows->FetchNext(pos);
	}
	if(Sort.count() == 0)
		return pResult->count();
	else
	{
		SortResultset(Sort, Result0, Result);
		pResult = &Result;
		while(iLimit > 0 && pResult->count() > iLimit)
		{
			pResult->remove(pResult->getTail());
		}
	}
	return pResult->count();
}

////////////////////////////////////////////////////////////////////////
// 描    述:  查询满足条件的记录总数
// 作    者:  邵凯田
// 创建时间:  2014-11-5 11:03
// 参数说明:  @Condition表示过滤条件
//         :  @pIndexRows表示被索引的部分记录集合，NULL表示没有匹配的索引集合（使用全记录过滤）
// 返 回 值:  被查询出的记录数量，<0表示失败
//////////////////////////////////////////////////////////////////////////
int CMdbTable::Count(SPtrList<stuMdbCompare> &Condition, SPtrList<CMdbRecord> *pIndexRows/* = NULL*/)
{
	unsigned long pos=0;
	int cnt = 0;
	if (pIndexRows == NULL)
		pIndexRows = &m_Records;
	CMdbRecord *pRec = pIndexRows->FetchFirst(pos);
	while (pRec)
	{
		if (FilterRecord(pRec, Condition))
		{
			cnt++;
		}
		pRec = pIndexRows->FetchNext(pos);
	}
	return cnt;
}

////////////////////////////////////////////////////////////////////////
// 描    述:  创建新表
// 作    者:  邵凯田
// 创建时间:  2014:4:23 15:17
// 参数说明:  @sql为建表SQL语法
//         :  @sLastError为错误描述缓冲区
// 返 回 值:  MDB_RET_SUCCESS表示成功，其他表示失败
//////////////////////////////////////////////////////////////////////////
int CMdbTable::CreateTable(SString sql,char* sLastError)
{
	/*
	create table table1(
	col1 int,
	col2 short,
	col3 byte,
	col4 float,
	col5 int64,
	col6 char(32),
	constraint table1_pk primary key(col1)
	);	
	*/
	sql = sql.toLower();
	SString str = sql.mid(13).trim();
	SString table = SString::GetIdAttribute(1,str," ").trim();
	SString extstr;
	table = SString::GetIdAttribute(1,table,"(").trim();
	m_sTableName = table;
	str = str.mid(table.length()).trim();
	if(str.left(1) != "(")
	{
		sprintf(sLastError,"create table语法错误!sql=%s",sql.data());
		return MDB_RET_SYNTAX_ERROR;
	}
	str = str.mid(1).trim();
	//遍历所有字段
	int pos;
	int iRowSize=0;
	SString colname,coltype,sub;
	CMdbField *pField;
	while(1)
	{
		colname = SString::GetIdAttribute(1,str," ").trim();
		if(colname == "constraint")
		{
			//处理主键
			pos = str.find("primary key");
			if(pos < 0)
				break;
			sub = str.mid(pos+11).trim();
			if(sub.left(1) != "(")
			{
				sprintf(sLastError,"create table 主键定义语法错误!sql=%s",sql.data());
				return MDB_RET_SYNTAX_ERROR;
			}
			sub = sub.mid(1);
			pos = sub.find(")");
			if(pos <= 0)
			{
				sprintf(sLastError,"create table 主键定义语法错误!sql=%s",sql.data());
				return MDB_RET_SYNTAX_ERROR;
			}
			sub = sub.left(pos).trim();
			int i,cnt = SString::GetAttributeCount(sub,",");
			bool have_one_pk=false;
			for(i=1;i<=cnt;i++)
			{
				colname = SString::GetIdAttribute(i,sub,",").trim();
				pField = SearchFieldByName(colname.data());
				if(pField == NULL)
				{
					sprintf(sLastError,"create table 主键定义语法错误!无效的主键字段'%s'",colname.data());
					return MDB_RET_SYNTAX_ERROR;
				}
				pField->m_bIsPK = true;
				m_PkFields.append(pField);
				have_one_pk = true;
			}
			if(!have_one_pk)
			{
				sprintf(sLastError,"create table 主键定义语法错误!sql=%s",sql.data());
				return MDB_RET_SYNTAX_ERROR;
			}
			break;
		}
		else if(colname == ")")
			break;//无主键结束
		
		pos = str.find(",");
		if(pos == 0)
		{
			sprintf(sLastError,"create table 语法错误!sql=%s",sql.data());
			return MDB_RET_SYNTAX_ERROR;
		}
		else if(pos < 0)
		{
			pos = str.findRev(")");
		}
		if(pos < 0)
		{
			sprintf(sLastError,"create table 语法错误!sql=%s",sql.data());
			return MDB_RET_SYNTAX_ERROR;
		}
		sub = str.left(pos);
		str = str.mid(pos).trim();
		if(str.left(1) == ",")
			str = str.mid(1).trim();
		extstr = sub;
		coltype = SString::GetIdAttribute(2,sub," ").trim();
		if(coltype.length() == 0)
		{
			sprintf(sLastError,"create table 语法错误!sql=%s",sql.data());
			return MDB_RET_SYNTAX_ERROR;
		}
		extstr.replace(coltype, "").trim();
		if(coltype == "int")
		{
			pField = new CMdbField();
			pField->m_iBytes = sizeof(int);
			pField->m_iStartPos = iRowSize;
			pField->m_sName = colname;
			pField->m_Type = MDB_INT;
			iRowSize += pField->m_iBytes;
		}
		else if(coltype == "int64")
		{
			pField = new CMdbField();
			pField->m_iBytes = sizeof(INT64);
			pField->m_iStartPos = iRowSize;
			pField->m_sName = colname;
			pField->m_Type = MDB_INT64;
			iRowSize += pField->m_iBytes;
		}
		else if(coltype == "short")
		{
			pField = new CMdbField();
			pField->m_iBytes = sizeof(short);
			pField->m_iStartPos = iRowSize;
			pField->m_sName = colname;
			pField->m_Type = MDB_SHORT;
			iRowSize += pField->m_iBytes;
		}
		else if(coltype == "byte")
		{
			pField = new CMdbField();
			pField->m_iBytes = sizeof(BYTE);
			pField->m_iStartPos = iRowSize;
			pField->m_sName = colname;
			pField->m_Type = MDB_BYTE;
			iRowSize += pField->m_iBytes;
		}
		else if(coltype == "float")
		{
			pField = new CMdbField();
			pField->m_iBytes = sizeof(float);
			pField->m_iStartPos = iRowSize;
			pField->m_sName = colname;
			pField->m_Type = MDB_FLOAT;
			iRowSize += pField->m_iBytes;
		}
		else if(coltype.find("char(") >= 0)
		{
			pField = new CMdbField();
			pField->m_iBytes = coltype.mid(5).toInt();
			if(pField->m_iBytes <= 2)
			{
				delete pField;
				sprintf(sLastError,"create table 语法错误!无效的字段类型'%s'",coltype.data());
				return MDB_RET_SYNTAX_ERROR;
			}
			pField->m_iStartPos = iRowSize;
			pField->m_sName = colname;
			pField->m_Type = MDB_STR;
			iRowSize += pField->m_iBytes;
		}
		else if (coltype == "blob")
		{
			pField = new CMdbField();
			pField->m_iBytes = sizeof(int);
			pField->m_iStartPos = iRowSize;
			pField->m_sName = colname;
			pField->m_Type = MDB_BLOB;
			iRowSize += pField->m_iBytes;
		}
		else
		{
			sprintf(sLastError,"create table 语法错误!无效的字段类型'%s'",coltype.data());
			return MDB_RET_SYNTAX_ERROR;
		}
		if (extstr.length() > 0)
		{
			extstr = extstr.toLower();
			if (extstr.find((char*)("not null")) >= 0)
			{
				pField->m_bIsNull = false;
				extstr.replace((char*)"not null", (char*)"").trim();
			}
			if (extstr.find((char*)("default ")) >= 0)
			{
				extstr.replace((char*)"default ", (char*)"").trim();
				pField->m_sDefaultVal = extstr;
			}
		}
		m_Fields.append(pField);
	}
	m_iRowSize = iRowSize;
	if (m_pHisDbTable != NULL)
	{
		delete m_pHisDbTable;
		m_pHisDbTable = NULL;
	}
	//创建主键索引
	if (m_PkFields.count())
	{
		CMdbTableIndex *pNewIndex = new CMdbTableIndex(C_MDB_PK_INDEX_HASHSIZE);
		pNewIndex->SetTablePtr(this);
		pNewIndex->SetName("pk_" + GetTableName());
		if (!pNewIndex->CreateIndex(&m_PkFields,true,true))
		{
			return MDB_RET_UNKNOWN;
		}
		m_pPkIndex = pNewIndex;
	}
	if(m_pMgr->IsHisDb())
		m_pHisDbTable = new CHisDbTable(m_pMgr, this);
	return MDB_RET_SUCCESS;
}


/************************************************************************/
/*                         内存数据库管理类                             */
/************************************************************************/

//////////////////////////////////////////////////////////////////////////
// 名    称:  CMdbManager
// 作    者:  邵凯田
// 创建时间:  2014:4:23 11:56
// 描    述:  内存数据库管理类
//////////////////////////////////////////////////////////////////////////
CMdbManager::CMdbManager()
{
	memset(m_sLastError,0,sizeof(m_sLastError));
	m_Tables.setAutoDelete(false);
	m_Tables.setShared(true);
	m_Index.setAutoDelete(false);
	m_Index.setShared(true);
	m_pSysTable = NULL;
	m_ConnTrg.setAutoDelete(true);
	m_ConnTrg.setShared(true);
	m_iThreads = 0;
	m_bQuit = false;
	m_HisSyncOper.setAutoDelete(true);
	m_HisSyncOper.setShared(true);
	m_DeletedRecords.setShared(true);
	m_DeletedRecords.setAutoDelete(true);
	m_bHisDb = true;
}

CMdbManager::~CMdbManager()
{
	Quit();
}

void CMdbManager::Quit()
{
	while(m_HisSyncOper.count() > 0)
		SApi::UsSleep(10000);
	m_bQuit = true;
	while (m_iThreads > 0)
		SApi::UsSleep(10000);
	m_HisSyncOper.clear();
	m_DeletedRecords.clear();
	m_Tables.setAutoDelete(true);
	m_Index.setAutoDelete(true);
	m_Tables.clear();
	m_Index.clear();
	m_ConnTrg.clear();
}

////////////////////////////////////////////////////////////////////////
// 描    述:  根据表名称查询表实例指针
// 作    者:  邵凯田
// 创建时间:  2014:4:23 12:01
// 参数说明:  sTableName为表名称
// 返 回 值:  表实例指针，NULL表示未打到指定表
//////////////////////////////////////////////////////////////////////////
CMdbTable* CMdbManager::SearchTableByName(char* sTableName)
{
	unsigned long pos=0;
	CMdbTable *pTab = m_Tables.FetchFirst(pos);
	SString tabname=sTableName;
	tabname = tabname.toLower();
	while(pTab)
	{
		if(pTab->GetTableName() == tabname)
			return pTab;
		pTab = m_Tables.FetchNext(pos);
	}
	return NULL;
}

////////////////////////////////////////////////////////////////////////
// 描    述:  插入新的一批记录
// 作    者:  邵凯田
// 创建时间:  2014:4:23 12:52
// 参数说明:  @sTableName为表名
//         :  @pRowData表示记录数据，含与建表时定义一致的所有记录,可在内部修改为NULL，内部负责释放
//         :  @iRowSize表示一条记录的长度
//         :  @iRows表示当前一共有多少条记录待插入，多条记录连续存储在pRowData中
// 返 回 值:  成功插件的记录数量
//////////////////////////////////////////////////////////////////////////
int CMdbManager::Insert(char* sTableName, BYTE* &pRowData, int iRowSize, int iRows, CMdbTable* &pTable)
{
	pTable = SearchTableByName(sTableName);
	if(pTable == NULL)
		return MDB_RET_TABLE_NOTEXIST;
	pTable->lock();
	int ret = pTable->Insert(pRowData,iRowSize,iRows);
	pTable->unlock();
	if (ret == -1)
	{
		sprintf(m_sLastError, "表[%s]记录大小不匹配!", sTableName);
	}
	return ret;
}

////////////////////////////////////////////////////////////////////////
// 描    述:  更新或插入一批记录，以主键为唯一标识，如果记录存在更新，不存在则插入
// 作    者:  邵凯田
// 创建时间:  2014-04-23 12:52
// 参数说明:  @sTableName为表名
//         :  @pRowData表示记录数据，含与建表时定义一致的所有记录,可在内部修改为NULL，内部负责释放
//         :  @iRowSize表示一条记录的长度
//         :  @iRows表示当前一共有多少条记录待插入，多条记录连续存储在pRowData中
// 返 回 值:  成功插件的记录数量
//////////////////////////////////////////////////////////////////////////
int CMdbManager::UpdateWithInsert(char* sTableName,BYTE* &pRowData,int iRowSize,int iRows,CMdbTable* &pTable)
{
	pTable = SearchTableByName(sTableName);
	if(pTable == NULL)
		return MDB_RET_TABLE_NOTEXIST;
	pTable->lock();
	int ret = pTable->UpdateWithInsert(pRowData,iRowSize,iRows);
	pTable->unlock();
	if (ret == -1)
	{
		sprintf(m_sLastError, "表[%s]记录大小不匹配!", sTableName);
	}
	return ret;
}

void skip_space(char* str)
{
	while(*str == ' ' || *str == '\t')
		str++;
}

////////////////////////////////////////////////////////////////////////
// 描    述:  执行SQL语句
// 作    者:  邵凯田
// 创建时间:  2014:4:23 12:05
// 参数说明:  @sql为可执行的SQL语句，支持简化语法
// 返 回 值:  int
//////////////////////////////////////////////////////////////////////////
int CMdbManager::Execute(SString &sql,CMdbTable* &pTable)
{
	m_sLastError[0] = '\0';
	sql.trim();
	SString sql_lower = sql.toLower();
	SString table;
	pTable = NULL;
	if(sql_lower.find("insert all into") == 0)
	{
		//insert all into t_ied (ied_sn,sub_sn,deviceno) values (1,2,3) into t_ied (ied_sn,sub_sn,deviceno) values (5,6,7) into t_group (ied_sn,sub_sn,deviceno) values (4,5,6) select 1 from dual
		char *pSql = sql_lower.data();
		char *pInto;
		char *pEnd;
		SString one_sql;
		int cnt = 0;
		while(*pSql != '\0')
		{
			pInto = strstr(pSql,"into ");
			if(pInto == NULL)
			{
				break;
			}
			pEnd = strstr(pInto," into ");
			if(pEnd == NULL)
				pEnd = strstr(pInto," select ");
			if(pEnd == NULL)
			{
				sprintf(m_sLastError, "Insert all语法错误1!");
				return MDB_RET_SYNTAX_ERROR;
			}
			one_sql = "insert " + sql.mid(pInto-sql_lower.data(),pEnd-pInto);
			if(Execute(one_sql,pTable) != 1)
			{
				sprintf(m_sLastError, "Insert all插入错误!");
				return MDB_RET_SYNTAX_ERROR;
			}
			cnt ++;
			pSql = pEnd;
		}
		return cnt;
	}
	else if (sql_lower.find("insert into") == 0/* || sql_lower.find("insert all into") == 0*/)
	{
		//insert into t_ied (ied_sn,sub_sn,deviceno) values(5,6,7),(1,2,3),(4,5,6)
		//insert all into t_ied (ied_sn,sub_sn,deviceno) values (1,2,3) into t_ied (ied_sn,sub_sn,deviceno) values (5,6,7) into t_ied (ied_sn,sub_sn,deviceno) values (4,5,6) select 1 from dual
		//insert into t_ied values(5,6,7,8)
#if 0
		if(sql_lower.find("insert all into") == 0)
		{
			//处理ORACLE语法兼容的批量插入SQL
			//bool bHaveField;//是否指定字段名称
			SString new_sql = "insert into"; 
			char *pLowerSql = sql_lower.data();
			table = sql_lower.left(64).mid(16).trim();
			table = SString::GetIdAttribute(1, table, " ").trim();
			SString sInto = " into "+table;
			
			int p1=sql_lower.find(sInto);
			if(p1 < 0)
			{
				sprintf(m_sLastError, "ORACLE兼容语法insert all into错误!");
				return MDB_RET_TABLE_NOTEXIST;
			}
			p1 += sInto.length();
			int p2 = sql_lower.find(sInto,p1+6);
			if(p2 < 0)
			{
				//第二次into 找不到，认为只有一条记录
				if(sql_lower.find("from dual") > 0)
				{
					p2 = sql_lower.findRev(" select ");
				}
				else
					p2 = sql_lower.length()-1;
				sql = "insert into "+table+sql.mid(p1,p2-p1);
				sql_lower = sql.toLower();
			}
			else
			{
				new_sql = "insert into "+table+sql.mid(p1,p2-p1);
				bool bEnd=false;
				while(1)
				{
					p1 = p2;
					p2 = sql_lower.find(sInto,p1+6);
					if(p2 < 0)
					{
						if(sql_lower.find("from dual") > 0)
						{
							p2 = sql_lower.findRev(" select ");
						}
						else
							p2 = sql_lower.length();
						bEnd = true;
					}

					//将p1从第一个into 移到values
					//从p1开始找到下一个into位置或末尾
					bool in1 = false;
					for(int i=p1;i<sql_lower.length();i++)
					{
						if(in1)
						{
							if(pLowerSql[i] == ')')
								in1 = false;
						}
						else
						{
							if(pLowerSql[i] == '(')
								in1 = true;
						}
						if(!in1 && memcmp(pLowerSql+i,"values",6)==0 && (pLowerSql[i-1]==' ' || pLowerSql[i-1]=='\r' || pLowerSql[i-1]=='\n' || pLowerSql[i-1]=='\t'))
						{
							//找到values位置
							p1 = i+6;
							while(pLowerSql[p1]==' ')
								p1++;
							break;
						}
					}					
					new_sql += ","+sql.mid(p1,p2-p1).trim();
					if(bEnd == true)
						break;
				}
				sql = new_sql;
				sql_lower = sql.toLower();
			}

		}
#endif
		table = sql_lower.mid(12);
		table = SString::GetIdAttribute(1, table, " ").trim();
		int p = table.find("(");
		if (p > 0)
			table = table.left(p);
		
		pTable = SearchTableByName(table.data());
		if (pTable == NULL)
		{
			sprintf(m_sLastError, "表'%s'不存在于当前库实例!", table.data());
			return MDB_RET_TABLE_NOTEXIST;
		}

		//解析字段名
		int pos_values = sql_lower.find("values");
		SString cols;
		p = sql_lower.find('(');
		int p2 = sql_lower.find(')');
		if (p > pos_values)
		{
			cols = "";
		}
		else if (p < 0 || p2 < 0 || p > p2)
		{
			sprintf(m_sLastError, "Insert语法中字段列表填写有误!");
			return MDB_RET_SYNTAX_ERROR;
		}
		else
		{
			cols = sql_lower.mid(p + 1, p2 - p - 1);
			cols.replace((char*)" ", (char*)"");
			cols.replace((char*)"\t", (char*)"");
			cols.replace((char*)"\r", (char*)"");
			cols.replace((char*)"\n", (char*)"");
		}
		SPtrList<CMdbField> fields,*pFields;
		CMdbField *pF;
		if(cols.length() == 0)
			pFields = pTable->GetFieldPtr();
		else
		{
			fields.setAutoDelete(false);
			int cnt = SString::GetAttributeCount(cols, ",");
			for (p = 1; p <= cnt; p++)
			{
				pF = pTable->SearchFieldByName(SString::GetIdAttribute(p, cols, ",").trim().data());
				if(pF == NULL)
				{
					sprintf(m_sLastError, "Insert中有未知的字段'%s'!",SString::GetIdAttribute(p, cols, ",").data());
					return MDB_RET_SYNTAX_ERROR;
				}
				fields.append(pF);
			}
			pFields = &fields;
		}

		//解析values，可能有多行插入
		p = sql_lower.find("values");
		if (p < 0)
		{
			sprintf(m_sLastError, "Insert语法中缺少必需的values表述!");
			return MDB_RET_SYNTAX_ERROR;
		}
		char *pVal;
		char *pValues = (char*)(sql.data()+p+6);
		unsigned long pos=0;
		int iAllocRows = 1;
		int iRows = 0;
		BYTE *pRow = new BYTE[pTable->GetRowSize()];
		BYTE *pAllRow = pRow;
		bool bRowEnd;
		while(*pValues != '\0')
		{
			memset(pRow, 0, pTable->GetRowSize());
			//循环多个values
			while(*pValues == ' ')
				pValues++;
			if(*pValues != '(')
			{
				sprintf(m_sLastError,"Insert语法错误，values后跟随的信息不合法!");
				delete[] pAllRow;
				return MDB_RET_SYNTAX_ERROR;
			}
			bRowEnd = false;
			pValues++;//跳过(
			pF = pFields->FetchFirst(pos);
			while(pF)
			{
				bool bIn=false;//是单引号内
				if(*pValues == '\'')
				{
					bIn = true;
					pValues++;
					if(*pValues == '\0')
					{
						sprintf(m_sLastError,"Insert语法错误，values未正常结束!");
						delete[] pAllRow;
						return MDB_RET_SYNTAX_ERROR;
					}
				}
				//MEMO: 增加VALUE值的去除空格功能 [2017-4-5 11:01 邵凯田]
				while(*pValues == ' ' || *pValues == '\t' || *pValues == '\r' || *pValues == '\n')
				{
					pValues++;
					continue;
				}
				pVal = pValues;
				while(1)
				{
					if(!bIn && *pValues == ',')
					{
						break;
					}
					if(bIn && *pValues == '\'')
					{
						if(pValues[1] == '\'')
						{
							//连着两个引号
							pValues+=2;
						}
						else
						{
							bIn = false;
							*pValues = '\0';
							pValues++;
							break;
						}
					}
					if(!bIn && *pValues == ')')
					{
						if(pFields->FetchNext(pos) != NULL)
						{
							sprintf(m_sLastError,"Insert语法错误，values后跟随值数量不正确!");
							delete[] pAllRow;
							return MDB_RET_SYNTAX_ERROR;
						}
						bRowEnd = true;
						break;
					}
					if(*pValues == '\0')
					{
						sprintf(m_sLastError,"Insert语法错误，values未正常结束!");
						delete[] pAllRow;
						return MDB_RET_SYNTAX_ERROR;
					}
					pValues++;
				}
				*pValues = '\0';
				pValues++;

				//pVal
				switch (pF->m_Type)
				{
				case MDB_INT:	//4 bytes
					{
#ifndef _ARM
						*((int*)(pRow + pF->m_iStartPos)) = atoi(pVal);

#else
						int v=atoi(pVal);
						memcpy(pRow + pF->m_iStartPos, &v, sizeof(v));
#endif
					}
					break;
				case MDB_SHORT:	//2 bytes
					{
#ifndef _ARM
						*((short*)(pRow + pF->m_iStartPos)) = (short)atoi(pVal);

#else
						short v=(short)atoi(pVal);
						memcpy(pRow + pF->m_iStartPos, &v, sizeof(v));
#endif
					}
					break;
				case MDB_BYTE:	//1 bytes
					pRow[pF->m_iStartPos] = (BYTE)atoi(pVal);
					break;
				case MDB_INT64:	//8 bytes
					{
#ifndef _ARM
						*((INT64*)(pRow + pF->m_iStartPos)) = atol(pVal);

#else
						INT64 v=atol(pVal);
						memcpy(pRow + pF->m_iStartPos, &v, sizeof(v));
#endif
					}
					break;
				case MDB_FLOAT:	//4 bytes
					{
#ifndef _ARM
						*((float*)(pRow + pF->m_iStartPos)) = (float)atof(pVal);

#else
						float v = (float)atof(pVal);
						memcpy(pRow + pF->m_iStartPos, &v, sizeof(v));
#endif
					}
					break;
				case MDB_STR:	//N bytes（由建表语法决定）
					{
						int len = strlen(pVal);
						if (len >= pF->m_iBytes)
							len = pF->m_iBytes - 1;
						if(strstr(pVal,"''") != NULL)
						{
							SString str = pVal;
							str.replace("''","'");
							memcpy(pRow + pF->m_iStartPos, str.data(), str.length());
						}
						else
							memcpy(pRow + pF->m_iStartPos, pVal, len);
						pRow[pF->m_iStartPos + len] = '\0';						
					}
					break;
				default:
					break;
				}
				pF = pFields->FetchNext(pos);
				if(bRowEnd == true && pF != NULL)
				{
					sprintf(m_sLastError,"Insert语法错误，values后跟随值数量不足!");
					delete[] pAllRow;
					return MDB_RET_SYNTAX_ERROR;
				}
			}
			while(*pValues == ' ' || *pValues == '\t' || *pValues == '\r' || *pValues == '\n')
				pValues++;
			iRows ++;
			if(*pValues == '\0')
				break;
			if(*pValues != ',')
			{
				sprintf(m_sLastError,"Insert语法错误，values携带多行时应使用逗号分隔!");
				delete[] pAllRow;
				return MDB_RET_SYNTAX_ERROR;
			}
			pValues++;
			if(iRows >= iAllocRows)
			{
				BYTE *pNewAllRow = new BYTE[pTable->GetRowSize()*(iAllocRows+10)];
				memcpy(pNewAllRow,pAllRow,pTable->GetRowSize()*iAllocRows);
				delete[] pAllRow;
				pAllRow = pNewAllRow;
				pRow = pAllRow+pTable->GetRowSize()*iRows;
				iAllocRows += 10;
			}
			else
				pRow += pTable->GetRowSize();			
		}

		pTable->lock();
		p = pTable->Insert(pAllRow, pTable->GetRowSize(), iRows);
		pTable->unlock();
		delete[] pAllRow;
		return p;
	}
	else if(sql_lower.find("delete from") == 0)
	{
		table = sql_lower.mid(12);
		table = SString::GetIdAttribute(1,table," ").trim();
		pTable = SearchTableByName(table.data());
		if(pTable == NULL)
		{
			sprintf(m_sLastError,"表'%s'不存在于当前库实例!",table.data());
			return MDB_RET_TABLE_NOTEXIST;
		}

		SPtrList<stuMdbCompare> Condition;
		Condition.setAutoDelete(true);
		SString where;
		int p;
		if((p=sql_lower.find(" where ")) > 0)
		{
			where = sql.mid(p+7);
			if (PrepareWhere(pTable, where, Condition) == false)
			{
				sprintf(m_sLastError, "语法错误!条件字符串处理失败:%s", where.data());
				return MDB_RET_SYNTAX_ERROR;
			}
		}
		pTable->lock();
		p = pTable->Delete(Condition);
		pTable->unlock();
		return p;
	}
	else if(sql_lower.find("truncate table") == 0)
	{
		table = sql_lower.mid(15);
		table = SString::GetIdAttribute(1,table," ").trim();
		pTable = SearchTableByName(table.data());
		if(pTable == NULL)
		{
			sprintf(m_sLastError,"表'%s'不存在于当前库实例!",table.data());
			return MDB_RET_TABLE_NOTEXIST;
		}
		pTable->lock();
		int ret = pTable->Truncate();
		pTable->unlock();
		return ret;
	}
	else if(sql_lower.find("create table") == 0)
	{
		table = sql_lower.mid(13);
		table = SString::GetIdAttribute(1,table," ").trim();
		pTable = SearchTableByName(table.data());
		if(pTable != NULL)
		{
			sprintf(m_sLastError,"表'%s'已经存在于当前库实例!",table.data());
			return MDB_RET_OBJ_EXIST;
		}
		int ret = CreateTable(sql);
		if (ret < MDB_RET_SUCCESS)
			return ret;
		//如果含主键，则创建主键索引
		//pTable = SearchTableByName(table.data());
		//if (pTable != NULL)
		//{
		//	if (pTable->GetPkFieldPtr()->count() > 0)
		//	{
		//		sql.sprintf("create index idxpk_%s on %s using hash(", table.data(), table.data());
		//		unsigned long pos=0;
		//		CMdbField *pF = pTable->GetPkFieldPtr()->FetchFirst(pos);
		//		while (pF)
		//		{
		//			sql += pF->m_sName;
		//			pF = pTable->GetPkFieldPtr()->FetchNext(pos);
		//			if (pF != NULL)
		//				sql += ",";
		//		}
		//		sql += ")";
		//		ret = CreateIndex(sql);
		//	}
		//}
		return ret;
	}
	else if (sql_lower.find("create index") == 0)
	{
		table = SString::GetIdAttribute(5, sql_lower, " ").trim();
		pTable = SearchTableByName(table.data());
		if (pTable == NULL)
		{
			sprintf(m_sLastError, "表'%s'不存在于当前库实例!", table.data());
			return MDB_RET_TABLE_NOTEXIST;
		}
		return CreateIndex(sql);
	}
	else if(sql_lower.find("drop table") == 0)
	{
		table = sql_lower.mid(11);
		table = SString::GetIdAttribute(1,table," ").trim();
		pTable = SearchTableByName(table.data());
		if(pTable == NULL)
		{
			sprintf(m_sLastError,"表'%s'不存在于当前库实例!",table.data());
			return MDB_RET_TABLE_NOTEXIST;
		}
		m_Tables.remove(pTable);
		pTable->lock();
		pTable->Truncate();
		pTable->unlock();
		if(pTable->m_pHisDbTable != NULL)
		{
			pTable->m_pHisDbTable->close();
		}
		if(pTable->m_pHisDbTable != NULL)
			SFile::remove(pTable->m_pHisDbTable->getFileName());
		SApi::UsSleep(100000);
		if(pTable->m_pHisDbTable != NULL)
		{
			delete pTable->m_pHisDbTable;
			pTable->m_pHisDbTable = NULL;
		}
		delete pTable;
		pTable = NULL;
		sql.sprintf("delete from %s where name='%s'", MDB_SYS_TABLE,table.data());
		Execute(sql,pTable);
		return MDB_RET_SUCCESS;
	}
	else if(sql_lower.find("update") == 0)
	{
		table = sql_lower.mid(7);
		table = SString::GetIdAttribute(1,table," ").trim();
		pTable = SearchTableByName(table.data());
		if(pTable == NULL)
		{
			sprintf(m_sLastError,"表'%s'不存在于当前库实例!",table.data());
			return MDB_RET_TABLE_NOTEXIST;
		}
		SPtrList<stuMdbSetValue> SetValue;
		SPtrList<stuMdbCompare> Condition;
		SetValue.setAutoDelete(true);
		Condition.setAutoDelete(true);
		SString where;
		int p_set = sql_lower.find(" set ");
		int p_where;
		if(p_set <= 0)
		{
			sprintf(m_sLastError, "语法错误!update语句的set未写明:%s", sql.left(1000).data());
			return MDB_RET_SYNTAX_ERROR;
		}
		p_set += 5;
		if ((p_where = sql_lower.find(" where ")) > 0)
		{
			where = sql.mid(p_where + 7);
		}
		else
			p_where = sql.length();
		SString sSet;
		char ch;
 		stuMdbSetValue *pSet = new stuMdbSetValue();
		pSet->m_pField = NULL;
		int step=0;//0:等待字段名,1:字段名进行中，2:等待=，3:等待值,4:值进行中
		bool bInInChar=false;
		for(;p_set!=p_where;p_set++)
		{
			ch = sql.at(p_set);
			if(step == 0)
			{
				if(sSet.length() == 0)
				{
					if(ch == ' ' || ch == '\t')
						continue;
					sSet += ch;
					step = 1;
				}			
			}
			else if(step == 1)
			{
				if(ch == ' ' || ch == '\t' || ch == '=')
				{
					pSet->m_pField = pTable->SearchFieldByName(sSet.data());
					if(pSet->m_pField == NULL)
					{
						delete pSet;
						sprintf(m_sLastError, "语法错误!无效的字符名:%s", sSet.data());
						return MDB_RET_FIELD_NOTEXIST;
					}
					sSet = "";
					if(ch == '=')
						step = 3;
					else
						step = 2;
					continue;
				}
				else
					sSet += ch;
			}
			else if(step == 2)
			{
				if(ch == ' ' || ch == '\t')
					continue;
				if(ch == '=')
				{
					step = 3;
					continue;
				}
				else
				{
					delete pSet;
					sprintf(m_sLastError, "语法错误!update语句语法不正确:%s", sql.left(1000).data());
					return MDB_RET_SYNTAX_ERROR;
				}
			}
			else if(step == 3)
			{
				if(ch == ' ' || ch == '\t')
					continue;
				if(ch == '\'')
				{
					bInInChar = true;					
				}
				else
				{
					bInInChar = false;
					pSet->m_sValue += ch;
				}
				step=4;
			}
			else if(step == 4)
			{
				if((bInInChar && ch == '\'') || (!bInInChar && (ch == ' ' || ch == '\t' || ch == ',')))
				{
					//值结束
					SetValue.append(pSet);
					//找到逗号表示有下一个
					bool bNext=false;
					if(ch == ',')
						bNext = true;
					else
					{
						p_set++;
						while(p_set<p_where)
						{
							ch = sql.at(p_set);
							if(ch == ',')
							{
								bNext = true;
								break;
							}
							else if(ch != ' ' && ch != '\t')
							{
								sprintf(m_sLastError, "update语法set错误!%s", sql.left(1000).data());
								return MDB_RET_SYNTAX_ERROR;
							}
							p_set++;
						}
					}
					if(bNext == false)
					{
						pSet = NULL;
						break;
					}
					pSet = new stuMdbSetValue();
					pSet->m_pField = NULL;
					step = 0;
					continue;
				}
				pSet->m_sValue += ch;
			}
		}
		if(pSet != NULL)
		{
			if(step == 4)
				SetValue.append(pSet);
			else
				delete pSet;
		}
		if(SetValue.count() == 0)
		{
			sprintf(m_sLastError, "语法错误!%s", sql.left(1000).data());
			return MDB_RET_SYNTAX_ERROR;
		}



		if (PrepareWhere(pTable, where, Condition) == false)
		{
			sprintf(m_sLastError, "语法错误!条件字符串处理失败:%s", where.left(1000).data());
			return MDB_RET_SYNTAX_ERROR;
		}
		pTable->lock();
		int ret = pTable->Update(SetValue, Condition);
		pTable->unlock();
		SetValue.clear();
		Condition.clear();
		return ret;
	}
	sprintf(m_sLastError, "语法错误!无效的SQL:%s", sql.left(1000).data());
	return MDB_RET_SYNTAX_ERROR;
}

////////////////////////////////////////////////////////////////////////
// 描    述:  查询指定SQL语句的数据集
// 作    者:  邵凯田
// 创建时间:  2014:4:23 12:07
// 参数说明:  @sql为可执行的SQL语句，支持简化语法
//         :  @pTable为对应的表实例
//         :  @Result为对应的记录集列表
//         :  @sRowResult表示以一行为结果的统计信息，每列为一个字符串值(count(*),min,max等,不支持group by）
// 返 回 值:  被查询出的记录数量，<0表示失败
//////////////////////////////////////////////////////////////////////////
int CMdbManager::Retrieve(SString &sql, CMdbTable* &pTable, SPtrList<CMdbRecord> &Result, SString *sRowResult)
{
	SPtrList<stuMdbCompare> Condition;
	Condition.setAutoDelete(true);
	SPtrList<stuMdbSort> Sort;
	Sort.setAutoDelete(true);

	SString table,where;

	//select col1,col2 from tab1 where col1=1
	//select * from tab1 where col2=1
	int p;
	SString sql_lower = sql.toLower();
	if((p=sql_lower.find(" from ")) > 0)
	{
		table = sql_lower.mid(p+6);
		table = SString::GetIdAttribute(1,table," ");
	}
	else
	{
		sprintf(m_sLastError,"SQL语法错误!sql=%s",sql.data());
		return MDB_RET_SYNTAX_ERROR;
	}
	pTable = SearchTableByName(table.data());
	if(pTable == NULL)
	{
		sprintf(m_sLastError,"表'%s'不存在!",table.data());
		return MDB_RET_TABLE_NOTEXIST;
	}
	pTable->lock();
	int iLimit;//限制记录行数条件，0表示不限
	if((iLimit = sql_lower.find(" limit ")) > 0)
	{
		bool bOk = true;
		int iLimit0 = iLimit;
		iLimit += 7;
		char ch;
		while(iLimit < sql_lower.length())
		{
			ch = sql_lower.at(iLimit++);
			if(ch != ' ' && (ch <'0' || ch > '9'))
			{
				bOk = false;
				break;
			}
		}
		if(bOk)
		{
			iLimit = atoi(sql_lower.data()+iLimit0+7);
			sql_lower = sql_lower.left(iLimit0);
		}
		else
			iLimit = 0;
	}
	else
		iLimit = 0;
	if((p=sql_lower.find(" where ")) > 0)
	{
		where = sql.mid(p+7);
		if (PrepareWhere(pTable, where, Condition) == false)
		{
			sprintf(m_sLastError, "语法错误!条件字符串处理失败:%s", where.data());
			return MDB_RET_SYNTAX_ERROR;
		}
	}
	if (sRowResult != NULL && 
		(sql_lower.find("count(*)") > 0 || sql_lower.find("min(") > 0 || sql_lower.find("max(") > 0
		  || sql_lower.find("sum(") > 0 || sql_lower.find("avg(") > 0))
	{
		p = sql_lower.find(" from ");
		SString val, colname, colnames = sql_lower.mid(7, p - 7).stripWhiteSpace();
		int i, cnt = SString::GetAttributeCount(colnames, ",");
		for (i = 1; i <= cnt; i++)
		{
			colname = SString::GetIdAttribute(i, colnames, ",").stripWhiteSpace();
			if (colname.find("count(") == 0)
			{
				val.sprintf("%d", pTable->Count(Condition));
			}
			else if (colname.find("min(") == 0)
			{
				colname = colname.mid(4, colname.length() - 5);
				CMdbField *pField = pTable->SearchFieldByName(colname.data());
				if (pField == NULL)
					return MDB_RET_FIELD_NOTEXIST;
				val = pTable->GetTotalFunctonVal(Condition, "min", pField);
			}
			else if (colname.find("max(") == 0)
			{
				colname = colname.mid(4, colname.length() - 5);
				CMdbField *pField = pTable->SearchFieldByName(colname.data());
				if (pField == NULL)
					return MDB_RET_FIELD_NOTEXIST;
				val = pTable->GetTotalFunctonVal(Condition, "max", pField);
			}
			else if (colname.find("sum(") == 0)
			{
				colname = colname.mid(4, colname.length() - 5);
				CMdbField *pField = pTable->SearchFieldByName(colname.data());
				if (pField == NULL)
					return MDB_RET_FIELD_NOTEXIST;
				val = pTable->GetTotalFunctonVal(Condition, "sum", pField);
			}
			else if (colname.find("avg(") == 0)
			{
				colname = colname.mid(4, colname.length() - 5);
				CMdbField *pField = pTable->SearchFieldByName(colname.data());
				if (pField == NULL)
					return MDB_RET_FIELD_NOTEXIST;
				val = pTable->GetTotalFunctonVal(Condition, "avg", pField);
			}
			else
				val = colname;
			if (sRowResult->length() > 0)
				*sRowResult += "\t";
			*sRowResult += val;
		}
		return 0;
	}
	if((p=sql_lower.find(" order by ")) >= 0)
	{
		where = sql_lower.mid(p + 10);
		SString col,asc;
		CMdbField *pFld;
		int i, cnt = SString::GetAttributeCount(where, ",");
		for (i = 1; i <= cnt; i++)
		{
			col = SString::GetIdAttribute(i, where, ",").stripWhiteSpace();
			if (col.find(" ") > 0)
			{
				asc = SString::GetIdAttribute(2, col, " ").stripWhiteSpace();
				col = SString::GetIdAttribute(1, col, " ").stripWhiteSpace();
			}
			else
				asc = "";
			pFld = pTable->SearchFieldByName(col.data());
			if (pFld == NULL)
			{
				sprintf(m_sLastError, "字段名[%s]不可识别!", col.data());
				return MDB_RET_FIELD_NOTEXIST;
			}
			stuMdbSort *pSort = new stuMdbSort();
			pSort->m_pField = pFld;
			pSort->m_bAsc = (asc != "desc");
			
			switch (pSort->m_pField->m_Type)
			{
			case MDB_INT:
			case MDB_BLOB:	//4 bytes

				pSort->m_pCallbackFun = CMdbTable::CB_FieldCompareInt;
				break;
			case MDB_INT64:
				pSort->m_pCallbackFun = CMdbTable::CB_FieldCompareInt64;
				break;
			case MDB_FLOAT:
				pSort->m_pCallbackFun = CMdbTable::CB_FieldCompareFloat;
				break;
			case MDB_SHORT:
				pSort->m_pCallbackFun = CMdbTable::CB_FieldCompareShort;
				break;
			case MDB_BYTE:
				pSort->m_pCallbackFun = CMdbTable::CB_FieldCompareByte;
				break;
			case MDB_STR:
				pSort->m_pCallbackFun = CMdbTable::CB_FieldCompareStr;
				break;
			default:
				pSort->m_pCallbackFun = CMdbTable::CB_FieldCompareInt;
				break;
			}
			
			Sort.append(pSort);
		}
	}
	return pTable->Select(Condition,Sort,Result,NULL,iLimit);
}

////////////////////////////////////////////////////////////////////////
// 描    述:  更新BLOB字段
// 作    者:  邵凯田
// 创建时间:  2014-12-22 18:20
// 参数说明:  @pBlobBuf为待更新的大字段缓冲区内容
//         :  @iBlobSize表示缓冲区大小
//         :  @sTableName为表名
//         :  @sFieldName为大字段名称
//         :  @Condition为指定的条件，指定条件应命中一条唯一的记录
// 返 回 值:  成功更新的记录数量，<0表示失败，=0表示未命中
//////////////////////////////////////////////////////////////////////////
int CMdbManager::UpdateBlob(BYTE *pBlobBuf, int iBlobSize, SString sTableName, SString sFieldName, SString sWhere)
{
	CMdbTable *pTable = SearchTableByName(sTableName.data());
	if (pTable == NULL)
	{
		return MDB_RET_TABLE_NOTEXIST;
	}
	SPtrList<stuMdbCompare> Condition;
	Condition.setAutoDelete(true);
	if (!PrepareWhere(pTable, sWhere, Condition))
	{
		sprintf(m_sLastError, "语法错误!条件字符串处理失败:%s", sWhere.data());
		return MDB_RET_SYNTAX_ERROR;
	}
	return pTable->UpdateBlob(pBlobBuf, iBlobSize, sFieldName, Condition);
}

////////////////////////////////////////////////////////////////////////
// 描    述:  读取BLOB字段
// 作    者:  邵凯田
// 创建时间:  2014-12-22 18:26
// 参数说明:  @pBlobBuf为待读取的大字段缓冲区内容，在函数申请缓冲区，调用者负责释放内存
//         :  @iBlobSize表示缓冲区大小
//         :  @sTableName为表名
//         :  @sFieldName为大字段名称
//         :  @Condition为指定的条件，指定条件应命中一条唯一的记录
// 返 回 值:  成功读取的记录数量，<0表示失败，=0表示未命中
//////////////////////////////////////////////////////////////////////////
int CMdbManager::ReadBlob(BYTE* &pBlobBuf, int &iBlobSize, SString sTableName, SString sFieldName, SString sWhere)
{
	CMdbTable *pTable = SearchTableByName(sTableName.data());
	if (pTable == NULL)
	{
		return MDB_RET_TABLE_NOTEXIST;
	}
	SPtrList<stuMdbCompare> Condition;
	Condition.setAutoDelete(true);
	if (!PrepareWhere(pTable, sWhere, Condition))
	{
		sprintf(m_sLastError, "语法错误!条件字符串处理失败:%s", sWhere.data());
		return MDB_RET_SYNTAX_ERROR;
	}
	return pTable->ReadBlob(pBlobBuf, iBlobSize, sFieldName, Condition);
}

////////////////////////////////////////////////////////////////////////
// 描    述:  取模块的信息描述
// 作    者:  邵凯田
// 创建时间:  2015-01-09 16:07
// 参数说明:  void
// 返 回 值:  SString
//////////////////////////////////////////////////////////////////////////
SString CMdbManager::GetMdbMgrInfo()
{
	SString sText = "";
	sText += SString::toFormat("共%d张数据表.\r\n", m_Tables.count());
	sText += SString::toFormat("共%d个索引.\r\n", m_Index.count());
	sText += SString::toFormat("共%d个触发订阅.\r\n", m_ConnTrg.count());
	sText += SString::toFormat("共%d个历史库同步记录.\r\n", m_HisSyncOper.count());
	sText += SString::toFormat("共%d个待释放记录.\r\n", m_DeletedRecords.count());

	return sText;
}

////////////////////////////////////////////////////////////////////////
// 描    述:  根据SQL中的where条件预处理条件
// 作    者:  邵凯田
// 创建时间:  2014:4:23 14:44
// 参数说明:  @pTable为对应的表实例
//         :  @sWhere为条件字符串，SQL中的where部分
//         :  @Condition为条件，引用返回
// 返 回 值:  true表示成功，false表示失败
//////////////////////////////////////////////////////////////////////////
bool CMdbManager::PrepareWhere(CMdbTable *pTable,SString &sWhere,SPtrList<stuMdbCompare> &Condition)
{
	int p1,p2,pos;
	bool bAnd=true;
	stuMdbCompare *pCmp;
	char oper[16];
	SString str = sWhere;
	SString sub;
	SString colname,val;
	char ch;

	while(str.length() > 0)
	{
		if (str.left(4) == "and ")
		{
			bAnd = true;
			str = str.mid(4);
		}
		else if (str.left(3) == "or ")
		{
			bAnd = false;
			str = str.mid(3);
		}

		pos = str.find(" and ");
		p2 = str.find(" or ");
		if (pos < 0)
			pos = p2;
		else if(p2 > 0 && p2 < pos)
		{
			pos = p2;
		}
		if (pos > 0)
		{
			sub = str.left(pos);
			str = str.mid(pos).trim();
		}
		else
		{
			sub = str;
			str = "";
		}
		while(sub.length()>0 && (sub.at(0) == ' ' || sub.at(0) == '\t' || sub.at(0) == '\r' || sub.at(0) == '\n'))
			sub = sub.mid(1);

		//p1为比较开始位置，p2为比较结束位置
		p1 = 0;
		while(1)
		{
			if((ch=sub.at(p1)) == ' ' || ch == '>' || ch == '<' || ch == '=')
				break;
			p1++;
			if(p1 >= sub.length())
			{
				sprintf(m_sLastError,"SQL语法错误！条件无效:%s",sWhere.data());
				return false;
			}
		}

		while(sub.at(p1) == ' ')
		{
			p1++;
			if(p1 >= sub.length())
			{
				sprintf(m_sLastError,"SQL语法错误！条件无效:%s",sWhere.data());
				return false;
			}
		}
		//p1位置确定

		p2 = p1;
		if((ch=sub.at(p2)) == '>' || ch == '<' || ch == '=')
		{
			memset(oper, 0, sizeof(oper));
			oper[0] = ch;
			p2++;
			while(1)
			{
				if ((ch = sub.at(p2)) == '>' || ch == '<' || ch == '=')
				{
					if (p2 - p1 > 4)
					{
						sprintf(m_sLastError, "SQL语法错误！条件无效:%s", sWhere.data());
						return false;
					}
					oper[p2-p1] = ch;
					p2++;
				}
				else
				{
					p2--;
					break;
				}
				if(p2 >= sub.length())
				{
					sprintf(m_sLastError,"SQL语法错误！条件无效:%s",sWhere.data());
					return false;
				}
			}
		}
		else
		{
			//like , not like
			if(sub.find("like ",p2) == p2)
			{
				strcpy(oper,"like");
				p2 += 4;
			}
			else if(sub.find("not ",p2) == p2)
			{
				p2 += 4;
				while(sub.at(p2) == ' ')
				{
					p2++;
					if(p2 >= sub.length())
					{
						sprintf(m_sLastError,"SQL语法错误！条件无效:%s",sWhere.data());
						return false;
					}
				}

				if(sub.find("like ",p2) != p2)
				{
					sprintf(m_sLastError,"SQL语法错误！条件无效:%s",sWhere.data());
					return false;
				}
				p2 += 4;
				strcpy(oper,"not like");
			}
			else
			{
				sprintf(m_sLastError,"SQL语法错误！条件无效:%s",sWhere.data());
				return false;
			}
		}
		colname = sub.left(p1).trim().toLower();
		val = sub.mid(p2+1).trim();
		if(val.left(1) == '\'')
			val = val.mid(1);
		if(val.right(1) == '\'')
			val = val.left(val.length()-1);
		
		pCmp = new stuMdbCompare();
		pCmp->m_pField = pTable->SearchFieldByName(colname.data());
		if(pCmp->m_pField == NULL)
		{
			delete pCmp;
			sprintf(m_sLastError,"SQL语法错误！条件无效:%s，不认识的字段'%s'",sWhere.data(),colname.data());
			return false;
		}
		switch(pCmp->m_pField->m_Type)
		{
		case MDB_INT:	//4 bytes
		case MDB_BLOB:
		case MDB_SHORT:	//2 bytes
		case MDB_BYTE:	//1 bytes
			{
				pCmp->m_iCmpVal = val.toInt();
			}
			break;
		case MDB_INT64:	//8 bytes
			{
				pCmp->m_i64CmpVal = val.toInt();
			}
			break;
		case MDB_FLOAT:	//4 bytes
			pCmp->m_fCmpVal = val.toFloat();
			break;
		case MDB_STR:	//N bytes（由建表语法决定）
			if (val.left(1) == "'" && val.right(1) == "'")
				val = val.mid(1, val.length() - 2);
			if (val.left(1) == "%")
				val = val.mid(1);
			if (val.right(1) == "%")
				val = val.left(val.length() - 1);
			pCmp->m_sCmpVal = val;
			break;
		}
		if(strcmp(oper,">") == 0)
			pCmp->m_CmpType = MDB_CMP_BIGTHAN;
		else if(strcmp(oper,">=") == 0)
			pCmp->m_CmpType = MDB_CMP_BIGTHAN_EQUAL;
		else if(strcmp(oper,"<") == 0)
			pCmp->m_CmpType = MDB_CMP_LESSTHAN;
		else if(strcmp(oper,"<=") == 0)
			pCmp->m_CmpType = MDB_CMP_LESSTHAN_EQUAL;
		else if(strcmp(oper,"=") == 0)
			pCmp->m_CmpType = MDB_CMP_EQUAL;
		else if(strcmp(oper,"<>") == 0 || strcmp(oper,"!=") == 0)
			pCmp->m_CmpType = MDB_CMP_NOT_EQUAL;
		else if(strcmp(oper,"like") == 0)
			pCmp->m_CmpType = MDB_CMP_LIKE;
		else if(strcmp(oper,"not like") == 0)
			pCmp->m_CmpType = MDB_CMP_NOT_LIKE;

		pCmp->m_pCallbackFun = NULL;
		switch(pCmp->m_pField->m_Type)
		{
		case MDB_INT:	//4 bytes
		case MDB_BLOB:	//4 bytes
			switch(pCmp->m_CmpType)
			{
			case MDB_CMP_BIGTHAN:		//大于 >
				pCmp->m_pCallbackFun = CMdbTable::CB_Filter_INT_BIGTHAN;
				break;
			case MDB_CMP_LESSTHAN:		//小于 <
				pCmp->m_pCallbackFun = CMdbTable::CB_Filter_INT_LESSTHAN;
				break;
			case MDB_CMP_EQUAL:			//等于 =
				pCmp->m_pCallbackFun = CMdbTable::CB_Filter_INT_EQUAL;
				break;
			case MDB_CMP_BIGTHAN_EQUAL:	//大于等于 >=
				pCmp->m_pCallbackFun = CMdbTable::CB_Filter_INT_BIGTHAN_EQUAL;
				break;
			case MDB_CMP_LESSTHAN_EQUAL:	//小于等于 <=
				pCmp->m_pCallbackFun = CMdbTable::CB_Filter_INT_LESSTHAN_EQUAL;
				break;
			case MDB_CMP_NOT_EQUAL:		//不等于 <>
				pCmp->m_pCallbackFun = CMdbTable::CB_Filter_INT_NOT_EQUAL;
				break;
			case MDB_CMP_LIKE:			//字符串含子串 like
				break;
			case MDB_CMP_NOT_LIKE:		//字符串不含子串 not like
				break;
			}
			break;
		case MDB_SHORT:	//2 bytes
			switch(pCmp->m_CmpType)
			{
			case MDB_CMP_BIGTHAN:		//大于 >
				pCmp->m_pCallbackFun = CMdbTable::CB_Filter_SHORT_BIGTHAN;
				break;
			case MDB_CMP_LESSTHAN:		//小于 <
				pCmp->m_pCallbackFun = CMdbTable::CB_Filter_SHORT_LESSTHAN;
				break;
			case MDB_CMP_EQUAL:			//等于 =
				pCmp->m_pCallbackFun = CMdbTable::CB_Filter_SHORT_EQUAL;
				break;
			case MDB_CMP_BIGTHAN_EQUAL:	//大于等于 >=
				pCmp->m_pCallbackFun = CMdbTable::CB_Filter_SHORT_BIGTHAN_EQUAL;
				break;
			case MDB_CMP_LESSTHAN_EQUAL:	//小于等于 <=
				pCmp->m_pCallbackFun = CMdbTable::CB_Filter_SHORT_LESSTHAN_EQUAL;
				break;
			case MDB_CMP_NOT_EQUAL:		//不等于 <>
				pCmp->m_pCallbackFun = CMdbTable::CB_Filter_SHORT_NOT_EQUAL;
				break;
			case MDB_CMP_LIKE:			//字符串含子串 like
				break;
			case MDB_CMP_NOT_LIKE:		//字符串不含子串 not like
				break;
			}
			break;
		case MDB_BYTE:	//1 bytes
			switch(pCmp->m_CmpType)
			{
			case MDB_CMP_BIGTHAN:		//大于 >
				pCmp->m_pCallbackFun = CMdbTable::CB_Filter_BYTE_BIGTHAN;
				break;
			case MDB_CMP_LESSTHAN:		//小于 <
				pCmp->m_pCallbackFun = CMdbTable::CB_Filter_BYTE_LESSTHAN;
				break;
			case MDB_CMP_EQUAL:			//等于 =
				pCmp->m_pCallbackFun = CMdbTable::CB_Filter_BYTE_EQUAL;
				break;
			case MDB_CMP_BIGTHAN_EQUAL:	//大于等于 >=
				pCmp->m_pCallbackFun = CMdbTable::CB_Filter_BYTE_BIGTHAN_EQUAL;
				break;
			case MDB_CMP_LESSTHAN_EQUAL:	//小于等于 <=
				pCmp->m_pCallbackFun = CMdbTable::CB_Filter_BYTE_LESSTHAN_EQUAL;
				break;
			case MDB_CMP_NOT_EQUAL:		//不等于 <>
				pCmp->m_pCallbackFun = CMdbTable::CB_Filter_BYTE_NOT_EQUAL;
				break;
			case MDB_CMP_LIKE:			//字符串含子串 like
				break;
			case MDB_CMP_NOT_LIKE:		//字符串不含子串 not like
				break;
			}
			break;
		case MDB_INT64:	//8 bytes
			switch(pCmp->m_CmpType)
			{
			case MDB_CMP_BIGTHAN:		//大于 >
				pCmp->m_pCallbackFun = CMdbTable::CB_Filter_INT64_BIGTHAN;
				break;
			case MDB_CMP_LESSTHAN:		//小于 <
				pCmp->m_pCallbackFun = CMdbTable::CB_Filter_INT64_LESSTHAN;
				break;
			case MDB_CMP_EQUAL:			//等于 =
				pCmp->m_pCallbackFun = CMdbTable::CB_Filter_INT64_NOT_EQUAL;
				break;
			case MDB_CMP_BIGTHAN_EQUAL:	//大于等于 >=
				pCmp->m_pCallbackFun = CMdbTable::CB_Filter_INT64_BIGTHAN_EQUAL;
				break;
			case MDB_CMP_LESSTHAN_EQUAL:	//小于等于 <=
				pCmp->m_pCallbackFun = CMdbTable::CB_Filter_INT64_LESSTHAN_EQUAL;
				break;
			case MDB_CMP_NOT_EQUAL:		//不等于 <>
				pCmp->m_pCallbackFun = CMdbTable::CB_Filter_INT64_EQUAL;
				break;
			case MDB_CMP_LIKE:			//字符串含子串 like
				break;
			case MDB_CMP_NOT_LIKE:		//字符串不含子串 not like
				break;
			}
			break;
		case MDB_FLOAT:	//4 bytes
			switch(pCmp->m_CmpType)
			{
			case MDB_CMP_BIGTHAN:		//大于 >
				pCmp->m_pCallbackFun = CMdbTable::CB_Filter_FLOAT_BIGTHAN;
				break;
			case MDB_CMP_LESSTHAN:		//小于 <
				pCmp->m_pCallbackFun = CMdbTable::CB_Filter_FLOAT_LESSTHAN;
				break;
			case MDB_CMP_EQUAL:			//等于 =
				pCmp->m_pCallbackFun = CMdbTable::CB_Filter_FLOAT_EQUAL;
				break;
			case MDB_CMP_BIGTHAN_EQUAL:	//大于等于 >=
				pCmp->m_pCallbackFun = CMdbTable::CB_Filter_FLOAT_BIGTHAN_EQUAL;
				break;
			case MDB_CMP_LESSTHAN_EQUAL:	//小于等于 <=
				pCmp->m_pCallbackFun = CMdbTable::CB_Filter_FLOAT_LESSTHAN_EQUAL;
				break;
			case MDB_CMP_NOT_EQUAL:		//不等于 <>
				pCmp->m_pCallbackFun = CMdbTable::CB_Filter_FLOAT_NOT_EQUAL;
				break;
			case MDB_CMP_LIKE:			//字符串含子串 like
				break;
			case MDB_CMP_NOT_LIKE:		//字符串不含子串 not like
				break;
			}
			break;
		case MDB_STR:	//N bytes（由建表语法决定）
			switch(pCmp->m_CmpType)
			{
			case MDB_CMP_BIGTHAN:		//大于 >
				pCmp->m_pCallbackFun = CMdbTable::CB_Filter_STR_BIGTHAN;
				break;
			case MDB_CMP_LESSTHAN:		//小于 <
				pCmp->m_pCallbackFun = CMdbTable::CB_Filter_STR_LESSTHAN;
				break;
			case MDB_CMP_EQUAL:			//等于 =
				pCmp->m_pCallbackFun = CMdbTable::CB_Filter_STR_EQUAL;
				break;
			case MDB_CMP_BIGTHAN_EQUAL:	//大于等于 >=
				pCmp->m_pCallbackFun = CMdbTable::CB_Filter_STR_BIGTHAN_EQUAL;
				break;
			case MDB_CMP_LESSTHAN_EQUAL:	//小于等于 <=
				pCmp->m_pCallbackFun = CMdbTable::CB_Filter_STR_LESSTHAN_EQUAL;
				break;
			case MDB_CMP_NOT_EQUAL:		//不等于 <>
				pCmp->m_pCallbackFun = CMdbTable::CB_Filter_STR_NOT_EQUAL;
				break;
			case MDB_CMP_LIKE:			//字符串含子串 like
				pCmp->m_pCallbackFun = CMdbTable::CB_Filter_STR_LIKE;
				break;
			case MDB_CMP_NOT_LIKE:		//字符串不含子串 not like
				pCmp->m_pCallbackFun = CMdbTable::CB_Filter_STR_NOT_LIKE;
				break;
			}
			break;
		}
		if(pCmp->m_pCallbackFun == NULL)
		{
			delete pCmp;
			sprintf(m_sLastError,"SQL语法错误！条件无效:%s，过滤回调函数未匹配!",sWhere.data());
			return false;
		}

		pCmp->m_bAnd = bAnd;
		Condition.append(pCmp);
	}
	return true;
}

////////////////////////////////////////////////////////////////////////
// 描    述:  初始化内存数据库管理器
// 作    者:  邵凯田
// 创建时间:  2014:4:24 11:25
// 参数说明:  @sParams为参数
// 返 回 值:  0表示成功，其它表示失败
//////////////////////////////////////////////////////////////////////////
int CMdbManager::Init(SString sParams)
{
	//从本地加载历史库
	if(m_bHisDb == false)
	{
		LOGWARN("内存数据库的持久化功能已关闭!");
	}
	else
	{
		SKT_CREATE_THREAD(ThreadHisDbSync, this);
		LoadHisDb();
	}
	SKT_CREATE_THREAD(ThreadFreeOldRecord, this);

	int ret;
	m_pSysTable = SearchTableByName((char*)MDB_SYS_TABLE);
	if (m_pSysTable == NULL)
	{
		ret = CreateTable(MDB_SQL_SYSTABLE_CREATE_TABLE);
		if(ret != MDB_RET_SUCCESS)
		{
			return ret;
		}
	}
	m_pSysTable = SearchTableByName((char*)MDB_SYS_TABLE);
	if(m_pSysTable == NULL)
	{
		sprintf(m_sLastError, "%s表创建失败!", MDB_SYS_TABLE);
		return MDB_RET_SYSTAB_ERR;
	}
	return MDB_RET_SUCCESS;
}

////////////////////////////////////////////////////////////////////////
// 描    述:  加载历史库
// 作    者:  邵凯田
// 创建时间:  2014-11-17 09:25
// 参数说明:  void
// 返 回 值:  0表示成功，其它表示失败
//////////////////////////////////////////////////////////////////////////
int CMdbManager::LoadHisDb()
{
	if (m_sMdbHomePath.length() == 0)
	{
		LOGERROR("未指定内存数据库的历史存储路径!");
		return -1;
	}
	CHisDbTable *pHis = new CHisDbTable(this, MDB_SYS_TABLE, sizeof(stuSysTable));
	if (pHis->GetTablePtr() == NULL)
	{
		delete pHis;
		LOGERROR("加载systable表时失败!");
		return -1;
	}
	m_Tables.append(pHis->GetTablePtr());
	m_pSysTable = pHis->GetTablePtr();
	m_pSysTable->SetMdbMgr(this);
	int i,cnt = pHis->getRowCount();
	stuSysTable systab;
	CMdbRecord *pRec;
	for (i = 0; i < cnt; i++)
	{
		if(!pHis->readRecord(i, (BYTE*)&systab))
			break;
		CHisDbTable *pHisTab = new CHisDbTable(this, systab.name, systab.rowsize);
		if (pHisTab->GetTablePtr() == NULL)
		{
			LOGERROR("加载%s表时失败!", systab.name);
			return -2;
		}
		if ((pRec = m_pSysTable->m_Records[i]) != NULL)
			pHisTab->GetTablePtr()->SetSysTableRecord((stuSysTable*)pRec->m_pRecordData);
		pHisTab->GetTablePtr()->SetMdbMgr(this);
		m_Tables.append(pHisTab->GetTablePtr());
	}
	return 0;
}

////////////////////////////////////////////////////////////////////////
// 描    述:  修改系统表对应的历史表中的指定数据表的记录数量
// 作    者:  邵凯田
// 创建时间:  2014-12-17 15:27
// 参数说明:  @pTable为表实例指针
// 返 回 值:  void
//////////////////////////////////////////////////////////////////////////
void CMdbManager::PrepareHisSysTableRows(CMdbTable *pTable)
{
	unsigned long pos=0;
	int i = 0;
	stuSysTable *pSysTab;
	CMdbRecord *pRecord = m_pSysTable->m_Records.FetchFirst(pos);
	while (pRecord)
	{
		pSysTab = (stuSysTable*)pRecord->m_pRecordData;
		if (strcmp(pSysTab->name, pTable->m_sTableName.data()) == 0)
		{
			m_pSysTable->m_pHisDbTable->writeRecord(i, pRecord->m_pRecordData);
			m_pSysTable->m_pHisDbTable->flush();
			break;
		}
		i++;
		pRecord = m_pSysTable->m_Records.FetchNext(pos);
	}
}

////////////////////////////////////////////////////////////////////////
// 描    述:  历史库的同步更新线程，负责所有表的串行更新
// 作    者:  邵凯田
// 创建时间:  2014-11-24 18:38
// 参数说明:  @lp为this指针
// 返 回 值:  NULL
//////////////////////////////////////////////////////////////////////////
void* CMdbManager::ThreadHisDbSync(void *lp)
{
#ifndef WIN32
	pthread_detach(pthread_self());
	signal(SIGPIPE, SIG_IGN);
	sigset_t signal_mask;
	sigemptyset(&signal_mask);
	sigaddset(&signal_mask, SIGPIPE);
#endif

	CMdbManager *pThis = (CMdbManager*)lp;
	pThis->m_iThreads++;
	stuMdbMgrHisDbSyncOper *pOper;
	int iCnt = 0;

	while (pThis->m_bQuit == false)
	{
		pOper = pThis->m_HisSyncOper[0];
		if (pOper == NULL)
		{
			if (iCnt > 0)
			{
				//同步所有的文件
				unsigned long pos=0;
				CMdbTable *pTable = pThis->m_Tables.FetchFirst(pos);
				while (pTable)
				{
					if(pTable->m_pHisDbTable != NULL)
						pTable->m_pHisDbTable->flush();
					pTable = pThis->m_Tables.FetchNext(pos);
				}
			}
			iCnt = 0;
			SApi::UsSleep(10000);
			continue;
		}
		iCnt++;
		switch (pOper->m_OperFlag)
		{
		case MDB_TRG_INSERT:
			//汇集同一个表中所有的insert语句，批量执行
		{
			unsigned long pos=0;
			SPtrList<stuMdbMgrHisDbSyncOper> lstNextOpers;
			lstNextOpers.setAutoDelete(false);
			stuMdbMgrHisDbSyncOper *pOperNext = pThis->m_HisSyncOper.FetchFirst(pos);
			if (pOperNext != NULL)
				pOperNext = pThis->m_HisSyncOper.FetchNext(pos);
			while (pOperNext)
			{
				if (pOperNext->m_pHisTable != pOper->m_pHisTable || pOperNext->m_OperFlag != MDB_TRG_INSERT)
				{
					break;
				}
				lstNextOpers.append(pOperNext);
				pOperNext = pThis->m_HisSyncOper.FetchNext(pos);
			}
			if (lstNextOpers.count() == 0)
			{
				if (pOper->m_pHisTable->append(pOper->m_pInsertRowData, pOper->m_iInsertRows))
				{
					pThis->PrepareHisSysTableRows(pOper->m_pHisTable->GetTablePtr());
				}
				else
				{
					LOGERROR("插入历史表[%s]时失败!", pOper->m_pHisTable->GetTablePtr()->GetTableName().data());
				}
			}
			else
			{
				int rowsize = pOper->m_pHisTable->GetTablePtr()->GetRowSize();
				int rows = pOper->m_iInsertRows;
				pOperNext = lstNextOpers.FetchFirst(pos);
				while (pOperNext)
				{
					rows += pOperNext->m_iInsertRows;
					pOperNext = lstNextOpers.FetchNext(pos);
				}
				BYTE *pRowData = new BYTE[rowsize * rows];
				memcpy(pRowData, pOper->m_pInsertRowData, rowsize*pOper->m_iInsertRows);
				BYTE *p = pRowData + rowsize*pOper->m_iInsertRows;
				pOperNext = lstNextOpers.FetchFirst(pos);
				while (pOperNext)
				{
					memcpy(p, pOperNext->m_pInsertRowData, rowsize*pOperNext->m_iInsertRows);
					p += rowsize*pOperNext->m_iInsertRows;
					pOperNext = lstNextOpers.FetchNext(pos);
				}
				if (pOper->m_pHisTable->append(pRowData, rows))
				{
					pThis->PrepareHisSysTableRows(pOper->m_pHisTable->GetTablePtr());
				}
				else
				{
					LOGERROR("插入历史表[%s]时失败!", pOper->m_pHisTable->GetTablePtr()->GetTableName().data());
				}
				delete[] pRowData;
				pOperNext = lstNextOpers.FetchFirst(pos);
				while (pOperNext)
				{
					pThis->m_HisSyncOper.remove(pOperNext);
					pOperNext = lstNextOpers.FetchNext(pos);
				}
			}
		}
			break;
		case MDB_TRG_UPDATE:
		{
		   int i, cnt = pOper->m_pHisTable->getRowCount();
		   CMdbTable *pTable = pOper->m_pHisTable->GetTablePtr();
		   CHisDbTable *pHis = pOper->m_pHisTable;
		   CMdbRecord rec;
		   BYTE *pRow = rec.m_pRecordData = new BYTE[pTable->GetRowSize()];
		   
		   for (i = 0; i < cnt; i++)
		   {
			   if (!pHis->readRecord(i, pRow))
			   {
				   LOGERROR("%s读取记录[%d/%d]时失败(m_Head.records=%d)!", pTable->GetTableName().data(), i, cnt, pHis->getRowCount());
				   break;
			   }
			   if (pTable->FilterRecord(&rec, pOper->Condition))
			   {
				   pTable->SetRecordValue(&rec, pOper->SetValue);
				   if (!pHis->writeRecord(i, rec.m_pRecordData))
				   {
					   LOGERROR("修改历史记录时失败!");
					   break;
				   }
			   }
		   }
		   rec.m_pRecordData = NULL;
		   delete[] pRow;
		   pThis->PrepareHisSysTableRows(pOper->m_pHisTable->GetTablePtr());
		}
			break;
		case MDB_TRG_UPDATE_WITHPK:
		{
		   int i,j, cnt = pOper->m_pHisTable->getRowCount();
		   CMdbTable *pTable = pOper->m_pHisTable->GetTablePtr();
		   CHisDbTable *pHis = pOper->m_pHisTable;
		   CMdbRecord rec,uprec;
		   int cnt2 = pOper->m_iInsertRows;
		   if (pTable->m_pPkIndex == NULL)
			   break;
		   BYTE *pRow = rec.m_pRecordData = new BYTE[pTable->GetRowSize()];
		   for (i = 0; i < cnt; i++)
		   {
			   if (!pHis->readRecord(i, pRow))
			   {
				   LOGERROR("%s读取记录[%d/%d]时失败(m_Head.records=%d)!", pTable->GetTableName().data(), i, cnt, pHis->getRowCount());
				   break;
			   }
			   uprec.m_pRecordData = pOper->m_pInsertRowData;
			   for (j = 0; j < cnt2; j++)
			   {
				   
				   if (pTable->m_pPkIndex->SHash_Compare_Idx(pTable->m_pPkIndex, &rec, &uprec))
				   {
					   if (!pHis->writeRecord(i, uprec.m_pRecordData))
					   {
						   LOGERROR("修改历史记录时失败!");
					   }
					   break;
				   }
				   uprec.m_pRecordData += pTable->m_iRowSize;
			   }
		   }
		   rec.m_pRecordData = NULL;
		   delete[] pRow;
		   pThis->PrepareHisSysTableRows(pOper->m_pHisTable->GetTablePtr());
		}
			break;
		case MDB_TRG_DELETE:
		{
		   int i, cnt = pOper->m_pHisTable->getRowCount();
		   CMdbTable *pTable = pOper->m_pHisTable->GetTablePtr();
		   CHisDbTable *pHis = pOper->m_pHisTable;
		   CMdbRecord rec;
		   BYTE *pRow = rec.m_pRecordData = new BYTE[pTable->GetRowSize()];
		   
		   for (i = 0; i < cnt; i++)
		   {
			   if (!pHis->readRecord(i, pRow))
			   {
				   LOGERROR("%s读取记录[%d/%d]时失败(m_Head.records=%d)!", pTable->GetTableName().data(), i, cnt, pHis->getRowCount());
				   break;
			   }
			   if (pTable->FilterRecord(&rec, pOper->Condition))
			   {
				   if (!pHis->remove(i))
				   {
					   LOGERROR("删除指定的历史记录时失败!");
					   break;
				   }
				   i--;
				   cnt--;
			   }
		   }
		   rec.m_pRecordData = NULL;
		   delete[] pRow;
		   pThis->PrepareHisSysTableRows(pOper->m_pHisTable->GetTablePtr());
		}
			break;
		case MDB_TRG_TRUNCATE:
			if (pOper->m_pHisTable->clear())
			{
				pThis->PrepareHisSysTableRows(pOper->m_pHisTable->GetTablePtr());
			}
			else
			{
				LOGERROR("清空数据表时失败!");
			}

			break;
		}

		pThis->m_HisSyncOper.remove(pOper);
	}

	pThis->m_iThreads--;
	return NULL;
}

////////////////////////////////////////////////////////////////////////
// 描    述:  释放过期的无效记录
// 作    者:  邵凯田
// 创建时间:  2014-11-24 18:38
// 参数说明:  @lp为this指针
// 返 回 值:  NULL
//////////////////////////////////////////////////////////////////////////
void* CMdbManager::ThreadFreeOldRecord(void *lp)
{
#ifndef WIN32
	pthread_detach(pthread_self());
	signal(SIGPIPE, SIG_IGN);
	sigset_t signal_mask;
	sigemptyset(&signal_mask);
	sigaddset(&signal_mask, SIGPIPE);
#endif

	CMdbManager *pThis = (CMdbManager*)lp;
	pThis->m_iThreads++;
	int i,cnt;
	while (pThis->m_bQuit == false)
	{
		cnt = pThis->m_DeletedRecords.count();
		if (cnt == 0)
		{
			SApi::UsSleep(100000);
			continue;
		}
		//100毫秒后释放对象
		SApi::UsSleep(100000);
		for (i = 0; !pThis->m_bQuit && i < cnt; i++)
		{
			pThis->m_DeletedRecords.remove(0);
		}
	}
	pThis->m_iThreads--;
	return NULL;
}

////////////////////////////////////////////////////////////////////////
// 描    述:  创建新的表
// 作    者:  邵凯田
// 创建时间:  2014:4:23 15:06
// 参数说明:  @sql为建表SQL语法
// 返 回 值:  MDB_RET_SUCCESS表示成功，其他表示失败
//////////////////////////////////////////////////////////////////////////
int CMdbManager::CreateTable(SString sql)
{
	int ret;
	SString str = sql.mid(13).trim().toLower();
	SString table = SString::GetIdAttribute(1,str," ").trim();
	table = SString::GetIdAttribute(1,table,"(").trim();
	unsigned long pos=0;
	CMdbTable* pTable;
	m_Tables.lock();
	pTable = m_Tables.FetchFirst(pos);
	while(pTable)
	{
		if(pTable->GetTableName() == table)
		{
			m_Tables.unlock();
			return MDB_RET_OBJ_EXIST;
		}
		pTable = m_Tables.FetchNext(pos);
	}
	pTable = new CMdbTable();
	pTable->SetMdbMgr(this);
	if((ret=pTable->CreateTable(sql,m_sLastError)) != MDB_RET_SUCCESS)
	{
		delete pTable;
		m_Tables.unlock();
		return ret;
	}
	if(m_pSysTable != NULL)
	{
		stuSysTable systab;
		memset(&systab,0,sizeof(systab));
		strcpy(systab.name,pTable->GetTableName().data());
		systab.rows = 0;
		systab.rowsize = pTable->GetRowSize();
		BYTE *pBuff = new BYTE[sizeof(systab)];
		memcpy(pBuff, (BYTE*)&systab, sizeof(systab));
		if (m_pSysTable->Insert(pBuff, sizeof(systab), 1))
		{
			if (pBuff != NULL)
			{
				delete[] pBuff;
				pBuff = NULL;
			}
			CMdbRecord *pRec = m_pSysTable->GetRecord(m_pSysTable->GetRows()-1);
			pTable->SetSysTableRecord((stuSysTable*)pRec->m_pRecordData);
			if(pTable->GetSysTableRecord() == NULL)
			{
				sprintf(m_sLastError, "%s表创建失败!", MDB_SYS_TABLE);
				delete pTable;
				m_Tables.unlock();
				return MDB_RET_SYSTAB_ERR;
			}
		}
		if (pBuff != NULL)
			delete[] pBuff;
	}

	m_Tables.unlock();
	m_Tables.append(pTable);
	
	return ret;
}

////////////////////////////////////////////////////////////////////////
// 描    述:  创建新的表索引
// 作    者:  邵凯田
// 创建时间:  2014-9-17 18:53
// 参数说明:  @sql为建索引SQL语法
// 返 回 值:  MDB_RET_SUCCESS表示成功，其他表示失败
//////////////////////////////////////////////////////////////////////////
int CMdbManager::CreateIndex(SString sql)
{
	int ret = 0;
	//create index idx_his_faultinfo on t_his_faultinfo using hash (deviceno) hash_size(128) [unique]
	//MDB_RET_INDEX_EXIST
	sql = sql.toLower();
	int p1, p2;
	SString idx_name = SString::GetIdAttribute(3, sql, " ");
	SString tab_name = SString::GetIdAttribute(5, sql, " ");
	SString col_names;
	p1 = sql.find("(");
	if (p1 <= 0)
		return MDB_RET_SYNTAX_ERROR;
	p2 = sql.find(")", p1);
	if (p2 <= 0)
		return MDB_RET_SYNTAX_ERROR;
	col_names = sql.mid(p1 + 1, p2 - p1 - 1);
	int hash_size = 128;//默认128个散列
	if ((p1 = sql.find("(", p2)) > 0)
	{
		p2 = sql.find(")", p1);
		if (p2 > 0)
		{
			hash_size = sql.mid(p1 + 1, p2 - p1 - 1).toInt();
			if (hash_size < 10)
				hash_size = 10;
			if (hash_size > 10000000)
				hash_size = 10000000;
		}
	}
	bool bUnique = sql.find("unique", p2) > 0;
	CMdbTable *pTable = SearchTableByName(tab_name.data());
	if (pTable == NULL)
		return MDB_RET_TABLE_NOTEXIST;
	CMdbTableIndex *pNewIndex = new CMdbTableIndex(hash_size);
	pNewIndex->SetTablePtr(pTable);
	pNewIndex->SetName(idx_name);
	SPtrList<CMdbField> fields;
	fields.setAutoDelete(false);
	int i, cnt = SString::GetAttributeCount(col_names, ",");
	for (i = 1; i <= cnt; i++)
	{
		SString col = SString::GetIdAttribute(i, col_names, ",");
		col.stripWhiteSpace();
		if (col.length() == 0)
			continue;
		CMdbField *pFld = pTable->SearchFieldByName(col.data());
		if (pFld == NULL)
		{
			sprintf(m_sLastError, "未知的索引字段名:%s", col.data());
			return MDB_RET_FIELD_NOTEXIST;
		}
		fields.append(pFld);
	}
	if (fields.count() <= 0)
		return MDB_RET_SYNTAX_ERROR;
	if (!pNewIndex->CreateIndex(&fields, bUnique))
	{
		sprintf(m_sLastError, "创建索引时失败!");
		return MDB_RET_UNKNOWN;
	}
	return ret;
}

////////////////////////////////////////////////////////////////////////
// 描    述:  指定会话连接触发器
// 作    者:  邵凯田
// 创建时间:  2014-10-16 10：56
// 参数说明:  @pSession为会话实例指针
//			  @table为触发表
//			  @action为触发动作：bit0~bit3表示:insert/update/delete/truncate
// 返 回 值:  true/false
//////////////////////////////////////////////////////////////////////////
bool CMdbManager::ConnectTrigger(void *pSession, SString table, SString action)
{
	CMdbTable *pTable = SearchTableByName(table.data());
	if (pTable == NULL)
		return false;
	BYTE bit = ((BYTE)action.toInt())&0x0F;
	/*
	if (action.find("insert") >= 0)
		bit |= 1;
	if (action.find("update") >= 0)
		bit |= 2;
	if (action.find("delete") >= 0)
		bit |= 4;
	if (action.find("truncate") >= 0)
		bit |= 8;
	*/
	if (bit == 0)
		return false;
	stuSessionConnectTrigger *p = SearchConnTrg(pSession, pTable);
	if (p == NULL)
	{
		p = new stuSessionConnectTrigger();
		p->m_pSession = pSession;
		p->m_pTable = pTable;
		m_ConnTrg.append(p);
	}
	
	p->m_TriggerFlag |= bit;
	pTable->m_TriggerFlag |= p->m_TriggerFlag;
	return true;
}

////////////////////////////////////////////////////////////////////////
// 描    述:  删除指定会话连接触发器
// 作    者:  邵凯田
// 创建时间:  2014-10-16 10：56
// 参数说明:  @pSession为会话实例指针
//			  @table为触发表
//			  @action为触发动作：bit0~bit3表示:insert/update/delete/truncate
// 返 回 值:  true/false
//////////////////////////////////////////////////////////////////////////
bool CMdbManager::RemoveTrigger(void *pSession, SString table, SString action)
{
	CMdbTable *pTable = SearchTableByName(table.data());
	if (pTable == NULL)
		return false;
	BYTE bit = ((BYTE)action.toInt()) & 0x0F;
	/*
	if (action.find("insert") >= 0)
	bit |= 1;
	if (action.find("update") >= 0)
	bit |= 2;
	if (action.find("delete") >= 0)
	bit |= 4;
	if (action.find("truncate") >= 0)
	bit |= 8;
	*/
	if (bit == 0)
		return false;
	stuSessionConnectTrigger *p = SearchConnTrg(pSession, pTable);
	if (p == NULL)
	{
		p = new stuSessionConnectTrigger();
		p->m_pSession = pSession;
		p->m_pTable = pTable;
		m_ConnTrg.append(p);
	}

	p->m_TriggerFlag &= ~bit;
	if (p->m_TriggerFlag == 0)
		m_ConnTrg.remove(p);
	//重新计算触发标记
	bit = 0;
	unsigned long pos=0;
	m_ConnTrg.lock();
	p = m_ConnTrg.FetchFirst(pos);
	while (p)
	{
		if (p->m_pTable == pTable)
			bit |= p->m_TriggerFlag;
		p = m_ConnTrg.FetchNext(pos);
	}
	m_ConnTrg.unlock();
	pTable->m_TriggerFlag = bit;
	return true;
}

////////////////////////////////////////////////////////////////////////
// 描    述:  指定会话连接触发器
// 作    者:  邵凯田
// 创建时间:  2014-10-16 10：56
// 参数说明:  @pSession为会话实例指针
//			  @pTable为触发表
// 返 回 值:  stuSessionConnectTrigger* , NULL表示未找到
//////////////////////////////////////////////////////////////////////////
CMdbManager::stuSessionConnectTrigger* CMdbManager::SearchConnTrg(void *pSession, CMdbTable *pTable)
{
	unsigned long pos=0;
	stuSessionConnectTrigger *p = m_ConnTrg.FetchFirst(pos);
	while (p)
	{
		if (p->m_pSession == pSession && p->m_pTable == pTable)
			return p;
		p = m_ConnTrg.FetchNext(pos);
	}
	return p;
}


////////////////////////////////////////////////////////////////////////
// 描    述:  释放指定会话连接的所有触发器
// 作    者:  邵凯田
// 创建时间:  2014-10-16 10:58
// 参数说明:  @pSession为会话实例指针
// 返 回 值:  true/false
//////////////////////////////////////////////////////////////////////////
bool CMdbManager::ClearSessionTrigger(void *pSession)
{
	int i, cnt = m_ConnTrg.count();
	for (i = 0; i < cnt; i++)
	{
		stuSessionConnectTrigger *p = m_ConnTrg[i];
		if (p == NULL)
			break;
		if (p->m_pSession == pSession)
		{
			m_ConnTrg.remove(i);
			i--;
		}
	}
	return true;
}

////////////////////////////////////////////////////////////////////////
// 描    述:  判断指定的会话是否连接了指定表的指定类型的触发器
// 作    者:  邵凯田
// 创建时间:  2014-10-16 10:58
// 参数说明:  @pSession为会话实例指针
//			  @pTable为指定表的实例指针
//			  @iTrgType表示触发类型，1:insert; 2:update; 3:delete; 4:truncate
// 返 回 值:  true/false
//////////////////////////////////////////////////////////////////////////
bool CMdbManager::IsSessionTrgConnected(void *pSession, CMdbTable *pTable, int iTrgType)
{
	stuSessionConnectTrigger *p = SearchConnTrg(pSession, pTable);
	if (p == NULL || iTrgType < 1 || iTrgType > 4)
		return false;
	BYTE bit = (1 << (iTrgType - 1));

	return (p->m_TriggerFlag & bit) != 0;
}


//////////////////////////////////////////////////////////////////////////
// 名    称:  CHisDbTable
// 作    者:  邵凯田
// 创建时间:  2014-11-18 15:10
// 描    述:  历史数据表文件类
//////////////////////////////////////////////////////////////////////////
CHisDbTable::CHisDbTable(CMdbManager *pMgr,CMdbTable *pTable) :
SVarDbf<stuHisTableInfo>(pTable->GetRowSize())
{
	m_pMdbMgr = pMgr;
	m_pTable = pTable;
	SetMaxRecords(pTable->m_iHisMaxRows);
	SString dbf = m_pMdbMgr->GetMdbHomePath();
	SDir::createDir(dbf);
#ifdef WIN32
	dbf += "\\db\\";
#else
	dbf += "/db/";
#endif
	SDir::createDir(dbf);
	dbf += pTable->GetTableName() + ".db";
	setShared(true);
	int ret = open(dbf.data());
	if (ret <= 0)
	{
		m_pTable = NULL;
		LOGERROR("历史数据表文件打开/创建失败(%s)!", dbf.data());
	}
	else if (ret == 1)
	{
		//打开成功
		
	}
	else if (ret == 2)
	{
		//创建成功
		stuHisTableInfo *pHead = getHdr();
		memset(pHead, 0, sizeof(stuHisTableInfo));
		pHead->m_iHisMaxRows = pTable->GetHisMaxRows();
		pHead->m_iMemMaxRows = pTable->GetMemMaxRows();
		pHead->m_iFieldCnt = pTable->GetFieldPtr()->count();
		pHead->m_iIndexCnt = 0;
		int i;
		unsigned long pos=0;
		stuTableField *fld;
		CMdbField *pF = pTable->GetFieldPtr()->FetchFirst(pos);
		i = 0;
		while (pF)
		{
			fld = &pHead->fields[i++];
			strcpy(fld->name, pF->m_sName.left(sizeof(fld->name) - 1).data());
			fld->type		= pF->m_Type;
			fld->start_pos	= pF->m_iStartPos;
			fld->bytes		= pF->m_iBytes;
			fld->is_pk		= pF->m_bIsPK ? 1 : 0;
			fld->is_null	= pF->m_bIsNull ? 1 : 0;
			strcpy(fld->defval, pF->m_sDefaultVal.left(sizeof(fld->defval) - 1).data());
			pF = pTable->GetFieldPtr()->FetchNext(pos);
		}
		CMdbTableIndex *pIdx = pTable->GetIndexPtr()->FetchFirst(pos);
		i = 0;
		while (pIdx)
		{
			AddIndex(pIdx);
			pIdx = pTable->GetIndexPtr()->FetchNext(pos);
		}
		if (!save())
		{
			LOGERROR("保存数据表文件时失败!file=%s", getFileName().data());
		}
	}
}

CHisDbTable::CHisDbTable(CMdbManager *pMgr,SString tabname, int iRowSize) :
SVarDbf<stuHisTableInfo>(iRowSize)
{
	m_pMdbMgr = pMgr;
	SString dbf = m_pMdbMgr->GetMdbHomePath();
	SDir::createDir(dbf);
#ifdef WIN32
	dbf += "\\db\\";
#else
	dbf += "/db/";
#endif
	SDir::createDir(dbf);
	dbf += tabname + ".db";
	setShared(true);
	int ret = open(dbf);
	if (ret <= 0)
	{
		m_pTable = NULL;
		LOGERROR("历史数据表文件打开/创建失败(%s)!", dbf.data());
	}
	else if (ret == 1)
	{
		//打开成功
		m_pTable = new CMdbTable();
		m_pTable->m_sTableName = tabname;
		if (!RestoreTable())
		{
			delete m_pTable;
			m_pTable = NULL;
			LOGERROR("从历史表恢复内存表时失败!");
		}
	}
	else if (ret == 2)
	{
		LOGERROR("历史数据表不存在!file=%s", dbf.data());
		close();
		SFile::remove(dbf);
		m_pTable = NULL;
	}
}

CHisDbTable::~CHisDbTable()
{
	close();
}

////////////////////////////////////////////////////////////////////////
// 描    述:  添加一条索引到历史库
// 作    者:  邵凯田
// 创建时间:  2014:4:23 14:44
// 参数说明:  @pIdx为索引指针
// 返 回 值:  true表示成功，false表示失败
//////////////////////////////////////////////////////////////////////////
bool CHisDbTable::AddIndex(CMdbTableIndex *pIdx)
{
	stuHisTableInfo *pHead = getHdr();
	if (pHead->m_iIndexCnt >= C_MDB_MAX_INDEX_CNT)
		return false;
	unsigned long pos=0;
	int i = pHead->m_iIndexCnt++;
	stuHisIndexInfo *pInfo = &pHead->indexs[i];
	memset(pInfo, 0, sizeof(stuHisIndexInfo));
	pInfo->hash_size = pIdx->GetHashSize();
	if (pIdx->IsPk())
		pInfo->flags |= 0x01;
	if (pIdx->IsUnique())
		pInfo->flags |= 0x02;
	strcpy(pInfo->idx_name, pIdx->GetName().left(sizeof(pInfo->idx_name)-1).data());
	i = 0;
	CMdbField *pF = pIdx->GetIndexFieldsPtr()->FetchFirst(pos);
	while (pF)
	{
		pInfo->field[i++] = m_pTable->GetFieldPtr()->position(pF) + 1;
		pF = pIdx->GetIndexFieldsPtr()->FetchNext(pos);
	}
	return true;
}

////////////////////////////////////////////////////////////////////////
// 描    述:  将历史表结构、数据还原到内存表
// 作    者:  邵凯田
// 创建时间:  2014-11-24 11:00
// 参数说明:  void
// 返 回 值:  true表示成功，false表示失败
//////////////////////////////////////////////////////////////////////////
bool CHisDbTable::RestoreTable()
{
	if (!isOpen())
		return false;
	stuHisTableInfo *pInfo = getHdr();
	m_pTable->SetHisMaxRows(pInfo->m_iHisMaxRows);
	m_pTable->SetMemMaxRows(pInfo->m_iMemMaxRows);
	CMdbField *pF;
	CMdbTableIndex *pI;
	int i, cnt;
	int rowsize = 0;
	m_pTable->m_pHisDbTable = this;

	//还原字段
	cnt = pInfo->m_iFieldCnt;
	for (i = 0; i < cnt; i++)
	{
		pF = new CMdbField();
		pF->m_sName			= pInfo->fields[i].name;
		pF->m_Type			= (eMdbFieldType)pInfo->fields[i].type;
		pF->m_iStartPos		= pInfo->fields[i].start_pos;
		pF->m_iBytes		= pInfo->fields[i].bytes;
		pF->m_bIsNull		= pInfo->fields[i].is_null == 1;
		pF->m_bIsPK			= pInfo->fields[i].is_pk == 1;
		pF->m_sDefaultVal	= pInfo->fields[i].defval;
		m_pTable->m_Fields.append(pF);
		if (pF->m_bIsNull)
			m_pTable->m_PkFields.append(pF);
		rowsize += pF->m_iBytes;
	}
	m_pTable->m_iRowSize = rowsize;
	
	//还原索引
	cnt = pInfo->m_iIndexCnt;
	bool bPk, bUnique;
	for (i = 0; i < cnt; i++)
	{
		pI = new CMdbTableIndex(pInfo->indexs[i].hash_size);
		if ((pInfo->indexs[i].flags & 0x01) != 0)
			bPk = true;
		else
			bPk = false;
		if ((pInfo->indexs[i].flags & 0x02) != 0)
			bUnique = true;
		else
			bUnique = false;

		pI->SetTablePtr(m_pTable);
		SPtrList<CMdbField> flds;
		pI->SetName(pInfo->indexs[i].idx_name);
		flds.setAutoDelete(false);
		int j, cnt2 = 32;
		for (j = 0; j < cnt2; j++)
		{
			if (pInfo->indexs[i].field[j] > 0)
			{
				pF = m_pTable->m_Fields[pInfo->indexs[i].field[j] - 1];
				if (pF != NULL)
					flds.append(pF);
			}
		}
		
		if (!pI->CreateIndex(&flds, bUnique, bPk))
		{
			LOGERROR("还原索引时失败!");
			return false;
		}
	}

	//还原记录
	int rdcnt,start=0;
	rdcnt = cnt = getRowCount();
	if (rdcnt > 1000)
		rdcnt = 1000;
	BYTE *pObjs = NULL;
	if (cnt > 0)
	{
		pObjs = new BYTE[rdcnt*rowsize];
	}
	while (cnt > 0)
	{
		rdcnt = cnt;
		if (rdcnt > 1000)
			rdcnt = 1000;
		if (readRecords(start, pObjs, rdcnt) != rdcnt)
		{
			LOGERROR("读取历史记录时失败!");
			break;
		}
		start += rdcnt;
		cnt -= rdcnt;
		if (m_pTable->Insert(pObjs, rowsize, rdcnt, true) != rdcnt)
		{
			LOGERROR("将历史记录插入内存时失败!");
			break;
		}
	}
	if (pObjs != NULL)
		delete[] pObjs;
	return true;
}

