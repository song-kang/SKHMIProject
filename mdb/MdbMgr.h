/**
 *
 * 文 件 名 : RealData.h
 * 创建日期 : 2014-4-22 13:34
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

//定义最大的字段数量
#define C_MDB_MAX_FIELD_CNT 128

//定义最大的索引数量
#define C_MDB_MAX_INDEX_CNT 16

//定义主健的散列数量
#define C_MDB_PK_INDEX_HASHSIZE 512

//最大缓存触发记录数量
#define C_MAX_BUFFERED_TRIGGER 10000

//最大缓历史库同步记录数量
#define C_MAX_BUFFERED_DBSYNC 100000

//最大一次插入记录数量
#define C_MAX_INSERT_ONCE 100000

//定义过滤回调函数
typedef bool (*CB_Filter)(stuMdbCompare *pCmp,CMdbRecord *pRecord);

//两个定义回调函数,<0表示记录1小于记录2，>0表示记录1大于记录2，==0表示相等
typedef int(*CB_FieldCompareFun)(CMdbRecord *pRecord1, CMdbRecord *pRecord2, CMdbField *m_pField);


enum eMdbCompareType
{
	MDB_CMP_BIGTHAN=1,		//大于 >
	MDB_CMP_LESSTHAN,		//小于 <
	MDB_CMP_EQUAL,			//等于 =
	MDB_CMP_BIGTHAN_EQUAL,	//大于等于 >=
	MDB_CMP_LESSTHAN_EQUAL,	//小于等于 <=
	MDB_CMP_NOT_EQUAL,		//不等于 <>
	MDB_CMP_LIKE,			//字符串含子串 like
	MDB_CMP_NOT_LIKE,		//字符串不含子串 not like
};

#define MDB_SYS_TABLE "systable"
#define MDB_SQL_SYSTABLE_CREATE_TABLE "create table systable (name char(32),rows int,rowsize int)"



//////////////////////////////////////////////////////////////////////////
// 名    称:  stuMdbCompare
// 作    者:  邵凯田
// 创建时间:  2014-04-22 15:20
// 描    述:  条件比较定义
//////////////////////////////////////////////////////////////////////////
struct stuMdbCompare
{
	CMdbField *m_pField;		//参照字段
	eMdbCompareType m_CmpType;	//比较类型
	CB_Filter m_pCallbackFun;	//回调函数
	bool m_bAnd;				//与上一条件是否为与关系，false表示或关系，该字段在第一个条件中无意义

	//以下值根据字段类型选择其中之一
	SString m_sCmpVal;			//字符串比较值
	INT64 m_i64CmpVal;			//长整型比较值
	int m_iCmpVal;				//整型比较值
	float m_fCmpVal;			//浮点比较值
};

//////////////////////////////////////////////////////////////////////////
// 名    称:  stuMdbSetValue
// 作    者:  邵凯田
// 创建时间:  2014-04-22 15:25
// 描    述:  赋值定义
//////////////////////////////////////////////////////////////////////////
struct stuMdbSetValue
{
	CMdbField *m_pField;		//待赋值的字段
	SString m_sValue;			//新值
};

//////////////////////////////////////////////////////////////////////////
// 名    称:  stuMdbSort
// 作    者:  邵凯田
// 创建时间:  2014-04-22 15:30
// 描    述:  排序定义
//////////////////////////////////////////////////////////////////////////
struct stuMdbSort
{
	CMdbField *m_pField;		//待排序的字段
	bool m_bAsc;				//是否为升序排列
	CB_FieldCompareFun m_pCallbackFun;	//排序比较的回调函数
};


//////////////////////////////////////////////////////////////////////////
// 名    称:  CMdbField
// 作    者:  邵凯田
// 创建时间:  2014-04-22 14:22
// 描    述:  内存库字段类
//////////////////////////////////////////////////////////////////////////
class CMdbField
{
public:
	CMdbField();
	~CMdbField();
	
	SString m_sName;		//字段名称
	eMdbFieldType m_Type;	//字段类型
	int m_iStartPos;		//在一行记录中的起始位置，第N个字节，从0开始
	int m_iBytes;			//值内容所占存储字节数
	bool m_bIsPK;			//是否为主键
	bool m_bIsNull;			//是否可以为空,true表示可以为空，false表示不可为空
	SString m_sDefaultVal;	//缺省值
};

//////////////////////////////////////////////////////////////////////////
// 名    称:  CMdbRecord
// 作    者:  邵凯田
// 创建时间:  2014-04-22 14:47
// 描    述:  内存库记录类
//////////////////////////////////////////////////////////////////////////
class CMdbRecord
{
public:
	CMdbRecord(BYTE *pRecData,int iLen/*,bool bClone=true*/);
	CMdbRecord();
	~CMdbRecord();
	//////////////////////////////////////////////////////////////////////////
	// 描    述:  取指定字段的值缓存
	// 作    者:  邵凯田
	// 创建时间:  2014-6-11 17:09
	// 参数说明:  @pField为字段指针
	// 返 回 值:  NULL表示失败，否则使用字段的偏移
	//////////////////////////////////////////////////////////////////////////
	inline BYTE* GetValue(CMdbField *pField)
	{
		return m_pRecordData+pField->m_iStartPos;
	}
public:
	BYTE *m_pRecordData;	//记录内容
};

class CMdbTable;
//////////////////////////////////////////////////////////////////////////
// 名    称:  CMdbTableIndex
// 作    者:  邵凯田
// 创建时间:  2014-6-11 8:56
// 描    述:  内存表索引类(为降低处理复杂度，暂不使用索引)
//////////////////////////////////////////////////////////////////////////
class CMdbTableIndex
{
public:
	friend class CMdbTable;
	CMdbTableIndex(int hash_size=1024);
	~CMdbTableIndex();

	//////////////////////////////////////////////////////////////////////////
	// 描    述:  设置索引关联表的指针
	// 作    者:  邵凯田
	// 创建时间:  2014-6-12 8:37
	// 参数说明:  @p关联的内存表实例
	// 返 回 值:  void
	//////////////////////////////////////////////////////////////////////////
	inline void SetTablePtr(CMdbTable *p){m_pMdbTable = p;};

	//////////////////////////////////////////////////////////////////////////
	// 描    述:  散列键生成函数
	// 作    者:  邵凯田
	// 创建时间:  2014-6-11 15:25
	// 参数说明:  @pItem待生成的行
	// 返 回 值:  键值
	//////////////////////////////////////////////////////////////////////////
	static int SHash_Key_Idx(void *cb,void *pItem);

	//////////////////////////////////////////////////////////////////////////
	// 描    述:  记录内容的精确比较函数（根据索引键）
	// 作    者:  邵凯田
	// 创建时间:  2014-6-11 15:28
	// 参数说明:  @pItem1为比较参照记录1
	//         :  @pItem2为比较参照记录2
	// 返 回 值:  true表示相等，false表示不相等
	//////////////////////////////////////////////////////////////////////////
	static bool SHash_Compare_Idx(void *cb,void *pItem1,void *pItem2);

	//////////////////////////////////////////////////////////////////////////
	// 描    述:  创建索引
	// 作    者:  邵凯田
	// 创建时间:  2014-6-11 15:31
	// 参数说明:  @fields为索引字段列表
	//            @bUnique表示是否为唯一键
	//            @bPk表示是否为主键
	// 返 回 值:  true表示索引成功，false表示索引失败
	//////////////////////////////////////////////////////////////////////////
	bool CreateIndex(SPtrList<CMdbField> *fields, bool bUnique = false, bool bPk = false);

	//////////////////////////////////////////////////////////////////////////
	// 描    述:  添加一条索引记录
	// 作    者:  邵凯田
	// 创建时间:  2014-6-11 15:32
	// 参数说明:  @pRow为新记录指针
	// 返 回 值:  true表示添加成功，false表示添加失败
	//////////////////////////////////////////////////////////////////////////
	bool AddRecord(CMdbRecord *pRow);

	//////////////////////////////////////////////////////////////////////////
	// 描    述:  删除一条索引记录
	// 作    者:  邵凯田
	// 创建时间:  2014-6-11 15:32
	// 参数说明:  @pRow为待删除的记录指针
	// 返 回 值:  true表示删除成功，false表示删除失败
	//////////////////////////////////////////////////////////////////////////
	bool RemoveRecord(CMdbRecord *pRow);

	//////////////////////////////////////////////////////////////////////////
	// 描    述:  清除所有索引记录
	// 作    者:  邵凯田
	// 创建时间:  2014-6-11 15:33
	// 参数说明:  void
	// 返 回 值:  true表示清空成功，false表示清空失败
	//////////////////////////////////////////////////////////////////////////
	bool ClearRecord();

	//////////////////////////////////////////////////////////////////////////
	// 描    述:  按指定的参考记录中的索引项值搜索匹配的所有记录
	// 作    者:  邵凯田
	// 创建时间:  2015-1-16 10:38
	// 参数说明:  @pObj参考对象指针
	//            @list为引用返回的匹配记录集合
	// 返 回 值:  >0匹配记录的数量，<0表示失败,=0表示没有匹配记录
	//////////////////////////////////////////////////////////////////////////
	int SearchByIndex(CMdbRecord *pObj, SPtrList<CMdbRecord> &list);
	CMdbRecord* SearchOnByIndex(CMdbRecord *pObj);

	//////////////////////////////////////////////////////////////////////////
	// 描    述:  累加各字节内容
	// 作    者:  邵凯田
	// 创建时间:  2014-6-11 17:14
	// 参数说明:  @pBuf字节缓冲区
	//         :  @iSize为缓冲长度
	// 返 回 值:  int
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
	SPtrList<CMdbField> m_IndexFiends;//索引字段，一条索引可以包含多个字段
	class CMdbTable *m_pMdbTable;//所属内存表
	bool m_bPk;//是否为主键
	bool m_bUnique;//是否为唯一键
	//SPtrList<CMdbRecord> m_Records;	//记录列表
	SGeneralHash<CMdbRecord,SHash_Key_Idx,SHash_Compare_Idx> m_IdxHash;//散列索引
};


//////////////////////////////////////////////////////////////////////////
// 名    称:  CTableTrigger
// 作    者:  邵凯田
// 创建时间:  2014-10-15 16:35
// 描    述:  表数据的触发记录
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

	int iTriggerType;//触发类型：1-insert 2-update 3-delete 4-truncate(delete all rows)
	BYTE *m_pTrgRowsData;//触发的记录内容，（4-truncate时不含数据）
	int m_iTrgRows;//触发的记录数总行数
};

//////////////////////////////////////////////////////////////////////////
// 名    称:  CMdbTable
// 作    者:  邵凯田
// 创建时间:  2014-04-22 14:42
// 描    述:  内存式数据表
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
	// 描    述:  读取指定字段号对应的字段名称
	// 作    者:  邵凯田
	// 创建时间:  2014-04-22 14:38
	// 参数说明:  @iFieldNo为字段序号，从0开始编号
	// 返 回 值:  对应字段名称，空串表示指定序号的字段不存在
	//////////////////////////////////////////////////////////////////////////
	SString GetFieldName(int iFieldNo);

	////////////////////////////////////////////////////////////////////////
	// 描    述:  检索指定字段名称对应的字段实例
	// 作    者:  邵凯田
	// 创建时间:  2014-04-23 16:18
	// 参数说明:  @sFieldName为字段名称
	// 返 回 值:  CMdbField*
	//////////////////////////////////////////////////////////////////////////
	CMdbField* SearchFieldByName(char* sFieldName);

	////////////////////////////////////////////////////////////////////////
	// 描    述:  读取指定字段名称对应的字段号
	// 作    者:  邵凯田
	// 创建时间:  2014-04-22 14:39
	// 参数说明:  @sFieldName为字段名称，不区分大小写
	// 返 回 值:  返回字段号，从0开始编号，<0表示字段名不存在
	//////////////////////////////////////////////////////////////////////////
	int GetFieldNo(char* sFieldName);

	////////////////////////////////////////////////////////////////////////
	// 描    述:  插入1行或多行记录
	// 作    者:  邵凯田
	// 创建时间:  2014-04-22 15:05
	// 参数说明:  @pRecData表示记录缓冲区，可在内部修改为NULL，内容负责释放
	//         :  @iRowSize表示一行记录的长度（字节数）
	//         :  @iRows表示有多少条记录待插入，默认为1
	//		   :  @bHisRestore表示是否为历史表在恢复数据
	// 返 回 值:  成功插件的记录数量
	//////////////////////////////////////////////////////////////////////////
	int Insert(BYTE* &pRecData,int iRowSize,int iRows=1,bool bHisRestore=false);

	////////////////////////////////////////////////////////////////////////
	// 描    述:  更新或插入一批记录，以主键为唯一标识,无主键的表不允许使用，如果记录存在更新，不存在则插入
	// 作    者:  邵凯田
	// 创建时间:  2014-04-22 15:05
	// 参数说明:  @pRecData表示记录缓冲区，可在内部修改为NULL，内容负责释放
	//         :  @iRowSize表示一行记录的长度（字节数）
	//         :  @iRows表示有多少条记录待插入，默认为1
	//		   :  @bHisRestore表示是否为历史表在恢复数据
	// 返 回 值:  成功更新或插入的记录数量
	//////////////////////////////////////////////////////////////////////////
	int UpdateWithInsert(BYTE* &pRecData, int iRowSize, int iRows = 1);

	////////////////////////////////////////////////////////////////////////
	// 描    述:  删除指定条件的记录
	// 作    者:  邵凯田
	// 创建时间:  2014-04-22 15:22
	// 参数说明:  @Condition表示过滤条件
	// 返 回 值:  成功删除的记录数量，<0表示失败
	//////////////////////////////////////////////////////////////////////////
	int Delete(SPtrList<stuMdbCompare> &Condition);

	////////////////////////////////////////////////////////////////////////
	// 描    述:  更新指定条件记录中的指定值为新内容
	// 作    者:  邵凯田
	// 创建时间:  2014-04-22 15:26
	// 参数说明:  @SetValue为赋值列表
	//         :  @Condition表示过滤条件
	// 返 回 值:  成功更新的记录数量，<0表示失败
	//////////////////////////////////////////////////////////////////////////
	int Update(SPtrList<stuMdbSetValue> &SetValue, SPtrList<stuMdbCompare> &Condition);

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
	int UpdateBlob(BYTE *pBlobBuf, int iBlobSize, SString &sFieldName, SPtrList<stuMdbCompare> &Condition);

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
	int ReadBlob(BYTE* &pBlobBuf, int &iBlobSize, SString sFieldName, SPtrList<stuMdbCompare> &Condition);

	////////////////////////////////////////////////////////////////////////
	// 描    述:  截断表的全部内容
	// 作    者:  邵凯田
	// 创建时间:  2014-04-22 15:27
	// 参数说明:  void
	// 返 回 值:  被截断记录的总数
	//////////////////////////////////////////////////////////////////////////
	int Truncate();

	////////////////////////////////////////////////////////////////////////
	// 描    述:  查询条件的记录，并按指定条件排序
	// 作    者:  邵凯田
	// 创建时间:  2014-04-22 15:31
	// 参数说明:  @Condition表示过滤条件
	//         :  @Sort表示排序条件，可以指定多个字段为排序条件，并指定升序排列或降序排列
	//         :  @Result表示结果记录集
	//         :  @pIndexRows表示被索引的部分记录集合，NULL表示没有匹配的索引集合（使用全记录过滤）
	//         :  @iLimit表示限制记录行数，0表示不限制
	//         :  @bAutoDel表示是否自动删除选中的记录
	// 返 回 值:  被查询出的记录数量，<0表示失败
	//////////////////////////////////////////////////////////////////////////
	int Select(SPtrList<stuMdbCompare> &Condition, SPtrList<stuMdbSort> &Sort, SPtrList<CMdbRecord> &Result, 
			   SPtrList<CMdbRecord> *pIndexRows=NULL,int iLimit=0,bool bAutoDel=false);

	////////////////////////////////////////////////////////////////////////
	// 描    述:  查询满足条件的记录总数
	// 作    者:  邵凯田
	// 创建时间:  2014-11-5 11:03
	// 参数说明:  @Condition表示过滤条件
	//         :  @pIndexRows表示被索引的部分记录集合，NULL表示没有匹配的索引集合（使用全记录过滤）
	// 返 回 值:  被查询出的记录数量，<0表示失败
	//////////////////////////////////////////////////////////////////////////
	int Count(SPtrList<stuMdbCompare> &Condition, SPtrList<CMdbRecord> *pIndexRows = NULL);

	////////////////////////////////////////////////////////////////////////
	// 描    述:  取表名
	// 作    者:  邵凯田
	// 创建时间:  2014-04-23 12:03
	// 参数说明:  void
	// 返 回 值:  SString
	//////////////////////////////////////////////////////////////////////////
	inline SString GetTableName(){return m_sTableName;};

	////////////////////////////////////////////////////////////////////////
	// 描    述:  取一行记录的总长度
	// 作    者:  邵凯田
	// 创建时间:  2014-04-24 11:32
	// 参数说明:  void
	// 返 回 值:  int
	//////////////////////////////////////////////////////////////////////////
	inline int GetRowSize(){return m_iRowSize;};

	////////////////////////////////////////////////////////////////////////
	// 描    述:  取当前的总记录数
	// 作    者:  邵凯田
	// 创建时间:  2014-04-24 11:43
	// 参数说明:  void
	// 返 回 值:  int
	//////////////////////////////////////////////////////////////////////////
	inline int GetRows(){return m_Records.count();};

	////////////////////////////////////////////////////////////////////////
	// 描    述:  取指定序号的记录
	// 作    者:  邵凯田
	// 创建时间:  2014-04-24 11:42
	// 参数说明:  @idx为记录序号，从0开始
	// 返 回 值:  CMdbRecord*
	//////////////////////////////////////////////////////////////////////////
	inline CMdbRecord* GetRecord(int idx){return m_Records[idx];};

	////////////////////////////////////////////////////////////////////////
	// 描    述:  创建新表
	// 作    者:  邵凯田
	// 创建时间:  2014-04-23 15:17
	// 参数说明:  @sql为建表SQL语法
	//         :  @sLastError为错误描述缓冲区
	// 返 回 值:  MDB_RET_SUCCESS表示成功，其他表示失败
	//////////////////////////////////////////////////////////////////////////
	int CreateTable(SString sql,char* sLastError);

	////////////////////////////////////////////////////////////////////////
	// 描    述:  取记录中指定序号的字段字符串值内容
	// 作    者:  邵凯田
	// 创建时间:  2014-11-14 10:46
	// 参数说明:  @pRow为记录内容
	//			  @pFld为字段信息
	// 返 回 值:  字符串型值内容
	//////////////////////////////////////////////////////////////////////////
	SString GetValueStr(BYTE *pRow, CMdbField *pFld);

	////////////////////////////////////////////////////////////////////////
	// 描    述:  取记录中指定序号的字段整数值内容
	// 作    者:  邵凯田
	// 创建时间:  2014-11-14 10:46
	// 参数说明:  @pRow为记录内容
	//			  @pFld为字段信息
	// 返 回 值:  字符串型值内容
	//////////////////////////////////////////////////////////////////////////
	int GetValueInt(BYTE *pRow, CMdbField *pFld);

	////////////////////////////////////////////////////////////////////////
	// 描    述:  取记录中指定序号的字段浮点数值内容
	// 作    者:  邵凯田
	// 创建时间:  2014-11-14 10:46
	// 参数说明:  @pRow为记录内容
	//			  @pFld为字段信息
	// 返 回 值:  字符串型值内容
	//////////////////////////////////////////////////////////////////////////
	float GetValueFloat(BYTE *pRow, CMdbField *pFld);

	////////////////////////////////////////////////////////////////////////
	// 描    述:  取指定条件下的汇总函数执行结果
	// 作    者:  邵凯田
	// 创建时间:  2014-12-22 17:12
	// 参数说明:  @Condition为过滤条件
	//			  @sFunName为函数名称(count,sum,min,max,avg)
	//			  @pField为对应字段，NULL表示不对应具体字段
	// 返 回 值:  字符串型值内容
	//////////////////////////////////////////////////////////////////////////
	SString GetTotalFunctonVal(SPtrList<stuMdbCompare> &Condition, SString sFunName, CMdbField *pField);

protected:

	////////////////////////////////////////////////////////////////////////
	// 描    述:  向数据表添加对应的索引，索引只加不删
	// 作    者:  邵凯田
	// 创建时间:  2014-9-18 09:22
	// 参数说明:  @pIdx为索引指针
	// 返 回 值:  void
	//////////////////////////////////////////////////////////////////////////
	void AddIndex(CMdbTableIndex *pIdx);

	////////////////////////////////////////////////////////////////////////
	// 描    述:  对指定数据集进行排序
	// 作    者:  邵凯田
	// 创建时间:  2014-9-18 09:22
	// 参数说明:  @Sort为排序条件
	//			  @SrcResult为源记录集
	//			  @DstResult为目标记录集
	// 返 回 值:  void
	//////////////////////////////////////////////////////////////////////////
	void SortResultset(SPtrList<stuMdbSort> &Sort, SPtrList<CMdbRecord> &SrcResult, SPtrList<CMdbRecord> &DstResult);

public:
	////////////////////////////////////////////////////////////////////////
	// 描    述:  判断指定记录在给定过滤条件下是否满足
	// 作    者:  邵凯田
	// 创建时间:  2014-04-22 15:44
	// 参数说明:  @pRecord为待比较的记录
	//         :  @Condition为过滤条件
	// 返 回 值:  true表示满足条件,false表示不满足条件
	//////////////////////////////////////////////////////////////////////////
	bool FilterRecord(CMdbRecord *pRecord, SPtrList<stuMdbCompare> &Condition);

	////////////////////////////////////////////////////////////////////////
	// 描    述:  设定指定记录的字段值
	// 作    者:  邵凯田
	// 创建时间:  2014-11-25 08:41
	// 参数说明:  @pRec为待赋值的记录
	//         :  @SetValue为赋值字段及值
	// 返 回 值:  true表示满足条件,false表示不满足条件
	//////////////////////////////////////////////////////////////////////////
	bool SetRecordValue(CMdbRecord *pRec, SPtrList<stuMdbSetValue> &SetValue);

	////////////////////////////////////////////////////////////////////////
	// 描    述:  条件过滤各组合情况下的预处理函数
	// 作    者:  邵凯田
	// 创建时间:  2014-04-23 10:29
	// 参数说明:  @pCmp为比较条件
	//         :  @pRecord为记录实例
	// 返 回 值:  true表示满足条件，false表示不满足条件
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
	// 描    述:  设置杜systab表记录指针
	// 作    者:  邵凯田
	// 创建时间:  2014-04-24 15:01
	// 参数说明:  stuSysTable*
	// 返 回 值:  void
	//////////////////////////////////////////////////////////////////////////
	inline void SetSysTableRecord(stuSysTable* p){m_pSysTableRecord=p;};

	////////////////////////////////////////////////////////////////////////
	// 描    述:  取systab记录指针
	// 作    者:  邵凯田
	// 创建时间:  2014-04-24 15:01
	// 参数说明:  void
	// 返 回 值:  stuSysTable*
	//////////////////////////////////////////////////////////////////////////
	inline stuSysTable* GetSysTableRecord(){return m_pSysTableRecord;};

	//////////////////////////////////////////////////////////////////////////
	// 描    述:  取行记录指针
	// 作    者:  邵凯田
	// 创建时间:  2014-6-12 8:49
	// 参数说明:  void
	// 返 回 值:  SPtrList<CMdbRecord>*
	//////////////////////////////////////////////////////////////////////////
	inline SPtrList<CMdbRecord>* GetRecordPtr(){return &m_Records;};

	//////////////////////////////////////////////////////////////////////////
	// 描    述:  取主键字段的指针
	// 作    者:  邵凯田
	// 创建时间:  2014-6-12 8:49
	// 参数说明:  void
	// 返 回 值:  SPtrList<CMdbField>*
	//////////////////////////////////////////////////////////////////////////
	inline SPtrList<CMdbField>* GetPkFieldPtr(){ return &m_PkFields; };

	//////////////////////////////////////////////////////////////////////////
	// 描    述:  取字段的指针
	// 作    者:  邵凯田
	// 创建时间:  2014-11-12 18:19
	// 参数说明:  void
	// 返 回 值:  SPtrList<CMdbField>*
	//////////////////////////////////////////////////////////////////////////
	inline SPtrList<CMdbField>* GetFieldPtr(){ return &m_Fields; };

	//////////////////////////////////////////////////////////////////////////
	// 描    述:  设置最大允许的记录数量
	// 作    者:  邵凯田
	// 创建时间:  2014-11-20 09:27
	// 参数说明:  @r为最大数量
	// 返 回 值:  void
	//////////////////////////////////////////////////////////////////////////
	inline void SetMemMaxRows(int r){ m_iMemMaxRows = r; };
	inline void SetHisMaxRows(int r){ m_iHisMaxRows = r; };

	//////////////////////////////////////////////////////////////////////////
	// 描    述:  取最大允许的记录数量
	// 作    者:  邵凯田
	// 创建时间:  2014-11-20 09:27
	// 参数说明:  @r为最大数量
	// 返 回 值:  void
	//////////////////////////////////////////////////////////////////////////
	inline int GetMemMaxRows(){ return m_iMemMaxRows; };
	inline int GetHisMaxRows(){ return m_iHisMaxRows; };

	//////////////////////////////////////////////////////////////////////////
	// 描    述:  取索引队列指针
	// 作    者:  邵凯田
	// 创建时间:  2014-11-20 09:27
	// 参数说明:  void
	// 返 回 值:  SPtrList<CMdbTableIndex>*
	//////////////////////////////////////////////////////////////////////////
	inline SPtrList<CMdbTableIndex>* GetIndexPtr(){ return &m_MyIndex; };

	//////////////////////////////////////////////////////////////////////////
	// 描    述:  设置数据库管理指针
	// 作    者:  邵凯田
	// 创建时间:  2014-11-21 13:46
	// 参数说明:  @p
	// 返 回 值:  void
	//////////////////////////////////////////////////////////////////////////
	inline void SetMdbMgr(CMdbManager *p){ m_pMgr = p; };

	inline void lock(){m_Lock.lock();};
	inline void unlock(){m_Lock.unlock();};

private:
	SPtrList<CMdbField> m_Fields;	//字段列表
	SPtrList<CMdbRecord> m_Records;	//记录列表
	int m_iRowSize;					//一行记录的固定长度
	SPtrList<CMdbField> m_PkFields;	//主键字段列表，有主键时插入及更新操作需要检查主键的唯一性
	SString m_sTableName;			//表名，强制小写
	stuSysTable *m_pSysTableRecord;	//对应系统表中的记录指针
	SPtrList<CMdbTableIndex> m_MyIndex;//当前表对应的索引实例
	CMdbTableIndex *m_pPkIndex;		//主键索引
	BYTE m_TriggerFlag;//触发选项,(bit0:insert,bit1:update,bit2:delete,bit3:truncate)
	int m_iMemMaxRows;	//内存中允许的最大记录数量
	int m_iHisMaxRows;	//历史表中允许的最大记录数量
	CMdbManager *m_pMgr;//管理类指针
	CHisDbTable *m_pHisDbTable;//历史表对象
	SLock m_Lock;
public:
	//1-insert 2-update 3-delete 4-truncate(delete all rows)
	SPtrList<CTableTrigger> m_InsertTrigger;//插入触发信息列表
	SPtrList<CTableTrigger> m_UpdateTrigger;//更新触发信息列表
	SPtrList<CTableTrigger> m_DeleteTrigger;//删除触发信息列表
	SPtrList<CTableTrigger> m_TruncateTrigger;//清空触发信息列表

};


//////////////////////////////////////////////////////////////////////////
// 名    称:  CMdbManager
// 作    者:  邵凯田
// 创建时间:  2014-04-23 11:56
// 描    述:  内存数据库管理类
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
	// 名    称:  stuMdbMgrHisDbSyncOper
	// 作    者:  邵凯田
	// 创建时间:  2014-11:24 19:40
	// 描    述:  历史表同步操作参数
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
	// 描    述:  根据表名称查询表实例指针
	// 作    者:  邵凯田
	// 创建时间:  2014-04-23 12:01
	// 参数说明:  sTableName为表名称
	// 返 回 值:  表实例指针，NULL表示未打到指定表
	//////////////////////////////////////////////////////////////////////////
	CMdbTable* SearchTableByName(char* sTableName);

	////////////////////////////////////////////////////////////////////////
	// 描    述:  根据表名称查询表句柄
	// 作    者:  邵凯田
	// 创建时间:  2014-04-23 12:50
	// 参数说明:  @sTableName为表名称
	// 返 回 值:  表的句柄，0表示未打到指定表
	//////////////////////////////////////////////////////////////////////////
	inline long GetTableHandle(char* sTableName){return (long)SearchTableByName(sTableName);};

	////////////////////////////////////////////////////////////////////////
	// 描    述:  初始化内存数据库管理器
	// 作    者:  邵凯田
	// 创建时间:  2014-04-24 11:25
	// 参数说明:  @sParams为参数
	// 返 回 值:  0表示成功，其它表示失败
	//////////////////////////////////////////////////////////////////////////
	int Init(SString sParams);

	////////////////////////////////////////////////////////////////////////
	// 描    述:  加载历史库
	// 作    者:  邵凯田
	// 创建时间:  2014-11-17 09:25
	// 参数说明:  void
	// 返 回 值:  0表示成功，其它表示失败
	//////////////////////////////////////////////////////////////////////////
	int LoadHisDb();

	////////////////////////////////////////////////////////////////////////
	// 描    述:  创建新的表
	// 作    者:  邵凯田
	// 创建时间:  2014-04-23 15:06
	// 参数说明:  @sql为建表SQL语法
	// 返 回 值:  MDB_RET_SUCCESS表示成功，其他表示失败
	//////////////////////////////////////////////////////////////////////////
	int CreateTable(SString sql);

	////////////////////////////////////////////////////////////////////////
	// 描    述:  创建新的表索引
	// 作    者:  邵凯田
	// 创建时间:  2014-9-17 18:53
	// 参数说明:  @sql为建索引SQL语法
	// 返 回 值:  MDB_RET_SUCCESS表示成功，其他表示失败
	//////////////////////////////////////////////////////////////////////////
	int CreateIndex(SString sql);

	////////////////////////////////////////////////////////////////////////
	// 描    述:  插入新的一批记录
	// 作    者:  邵凯田
	// 创建时间:  2014-04-23 12:52
	// 参数说明:  @sTableName为表名
	//         :  @pRowData表示记录数据，含与建表时定义一致的所有记录,可在内部修改为NULL，内部负责释放
	//         :  @iRowSize表示一条记录的长度
	//         :  @iRows表示当前一共有多少条记录待插入，多条记录连续存储在pRowData中
	// 返 回 值:  成功插件的记录数量
	//////////////////////////////////////////////////////////////////////////
	int Insert(char* sTableName,BYTE* &pRowData,int iRowSize,int iRows,CMdbTable* &pTable);

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
	int UpdateWithInsert(char* sTableName,BYTE* &pRowData,int iRowSize,int iRows,CMdbTable* &pTable);

	////////////////////////////////////////////////////////////////////////
	// 描    述:  执行SQL语句
	// 作    者:  邵凯田
	// 创建时间:  2014-04-23 12:05
	// 参数说明:  @sql为可执行的SQL语句，支持简化语法
	// 返 回 值:  int
	//////////////////////////////////////////////////////////////////////////
	int Execute(SString &sql, CMdbTable* &pTable);

	////////////////////////////////////////////////////////////////////////
	// 描    述:  查询指定SQL语句的数据集
	// 作    者:  邵凯田
	// 创建时间:  2014-04-23 12:07
	// 参数说明:  @sql为可执行的SQL语句，支持简化语法
	//         :  @pTable为对应的表实例，当pTable不为空时，调用后必须调用pTable->unlock()释放表锁
	//         :  @Result为对应的记录集列表
	//         :  @sRowResult表示以一行为结果的统计信息，每列为一个字符串值(count(*),min,max等,不支持group by）
	// 返 回 值:  被查询出的记录数量，<0表示失败
	//////////////////////////////////////////////////////////////////////////
	int Retrieve(SString &sql,CMdbTable* &pTable,SPtrList<CMdbRecord> &Result,SString *sRowResult=NULL);

	////////////////////////////////////////////////////////////////////////
	// 描    述:  取最后的错误描述
	// 作    者:  邵凯田
	// 创建时间:  2014-04-23 14:41
	// 参数说明:  void
	// 返 回 值:  SString
	//////////////////////////////////////////////////////////////////////////
	inline SString GetLastError(){return m_sLastError;};

	////////////////////////////////////////////////////////////////////////
	// 描    述:  增加指定会话连接触发器
	// 作    者:  邵凯田
	// 创建时间:  2014-10-16 10：56
	// 参数说明:  @pSession为会话实例指针
	//			  @table为触发表
	//			  @action为触发动作：bit0~bit3表示:insert/update/delete/truncate
	// 返 回 值:  true/false
	//////////////////////////////////////////////////////////////////////////
	bool ConnectTrigger(void *pSession, SString table, SString action);

	////////////////////////////////////////////////////////////////////////
	// 描    述:  删除指定会话连接触发器
	// 作    者:  邵凯田
	// 创建时间:  2014-10-16 10：56
	// 参数说明:  @pSession为会话实例指针
	//			  @table为触发表
	//			  @action为触发动作：bit0~bit3表示:insert/update/delete/truncate
	// 返 回 值:  true/false
	//////////////////////////////////////////////////////////////////////////
	bool RemoveTrigger(void *pSession, SString table, SString action);

	////////////////////////////////////////////////////////////////////////
	// 描    述:  指定会话连接触发器
	// 作    者:  邵凯田
	// 创建时间:  2014-10-16 10：56
	// 参数说明:  @pSession为会话实例指针
	//			  @pTable为触发表
	// 返 回 值:  stuSessionConnectTrigger* , NULL表示未找到
	//////////////////////////////////////////////////////////////////////////
	stuSessionConnectTrigger* SearchConnTrg(void *pSession, CMdbTable *pTable);

	////////////////////////////////////////////////////////////////////////
	// 描    述:  释放指定会话连接的所有触发器
	// 作    者:  邵凯田
	// 创建时间:  2014-10-16 10:58
	// 参数说明:  @pSession为会话实例指针
	// 返 回 值:  true/false
	//////////////////////////////////////////////////////////////////////////
	bool ClearSessionTrigger(void *pSession);

	////////////////////////////////////////////////////////////////////////
	// 描    述:  判断指定的会话是否连接了指定表的指定类型的触发器
	// 作    者:  邵凯田
	// 创建时间:  2014-10-16 10:58
	// 参数说明:  @pSession为会话实例指针
	//			  @pTable为指定表的实例指针
	//			  @iTrgType表示触发类型，1:insert; 2:update; 3:delete; 4:truncate
	// 返 回 值:  true/false
	//////////////////////////////////////////////////////////////////////////
	bool IsSessionTrgConnected(void *pSession, CMdbTable *pTable, int iTrgType);

	//////////////////////////////////////////////////////////////////////////
	// 描    述:  设置历史库是否使能
	// 作    者:  邵凯田
	// 创建时间:  2015-7-14 14:33
	// 参数说明:  @en为true表示使能持久化历史存储，false表示关闭持久化
	// 返 回 值:  void
	//////////////////////////////////////////////////////////////////////////
	void SetHisDbEnable(bool en){m_bHisDb=en;};

	////////////////////////////////////////////////////////////////////////
	// 描    述:  设置MDB本地存储路径
	// 作    者:  邵凯田
	// 创建时间:  2014-11-17 09:24
	// 参数说明:  @path为本地路径
	// 返 回 值:  void
	//////////////////////////////////////////////////////////////////////////
	void SetMdbHomePath(SString path){ m_sMdbHomePath = path; };

	////////////////////////////////////////////////////////////////////////
	// 描    述:  取MDB本地存储路径
	// 作    者:  邵凯田
	// 创建时间:  2014-11-18 15:34
	// 参数说明:  void
	// 返 回 值:  本地路径
	//////////////////////////////////////////////////////////////////////////
	SString GetMdbHomePath(){ return m_sMdbHomePath; };

	////////////////////////////////////////////////////////////////////////
	// 描    述:  添加历史表同步操作
	// 作    者:  邵凯田
	// 创建时间:  2014-11-24 19:36
	// 参数说明:  stuMdbMgrHisDbSyncOper*
	// 返 回 值:  void
	//////////////////////////////////////////////////////////////////////////
	inline void AddHisSyncOper(stuMdbMgrHisDbSyncOper *p)
	{
		if (m_HisSyncOper.count() > C_MAX_BUFFERED_DBSYNC)
		{
			static int last_soc = 0;
			if((int)::time(NULL) != last_soc)
			{
				last_soc = (int)::time(NULL);
				LOGWARN("历史库同步列队满(>%d)，将丢弃部分数据!",C_MAX_BUFFERED_DBSYNC);
			}
			delete p;
		}
		else
			m_HisSyncOper.append(p); 
	};

	////////////////////////////////////////////////////////////////////////
	// 描    述:  取系统表指针
	// 作    者:  邵凯田
	// 创建时间:  2014-12-17 15:19
	// 参数说明:  void
	// 返 回 值:  CMdbTable*
	//////////////////////////////////////////////////////////////////////////
	inline CMdbTable* GetSysTable(){ return m_pSysTable; };

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
	int UpdateBlob(BYTE *pBlobBuf, int iBlobSize, SString sTableName, SString sFieldName, SString sWhere);

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
	int ReadBlob(BYTE* &pBlobBuf, int &iBlobSize, SString sTableName, SString sFieldName, SString sWhere);

	////////////////////////////////////////////////////////////////////////
	// 描    述:  取模块的信息描述
	// 作    者:  邵凯田
	// 创建时间:  2015-01-09 16:07
	// 参数说明:  void
	// 返 回 值:  SString
	//////////////////////////////////////////////////////////////////////////
	SString GetMdbMgrInfo();

	inline void ClearDeletedRecords() { m_DeletedRecords.clear(); };
	inline void AddDelRecord(CMdbRecord *p){ m_DeletedRecords.append(p); };
	inline SPtrList<CMdbRecord>* GetDelRecordPtr(){ return &m_DeletedRecords; };
	inline bool IsHisDb(){return m_bHisDb;};
	void Quit();
protected:
	////////////////////////////////////////////////////////////////////////
	// 描    述:  根据SQL中的where条件预处理条件
	// 作    者:  邵凯田
	// 创建时间:  2014-04-23 14:44
	// 参数说明:  @pTable为对应的表实例
	//         :  @sWhere为条件字符串，SQL中的where部分
	//         :  @Condition为条件，引用返回
	// 返 回 值:  true表示成功，false表示失败
	//////////////////////////////////////////////////////////////////////////
	bool PrepareWhere(CMdbTable *pTable,SString &sWhere,SPtrList<stuMdbCompare> &Condition);

	////////////////////////////////////////////////////////////////////////
	// 描    述:  修改系统表对应的历史表中的指定数据表的记录数量
	// 作    者:  邵凯田
	// 创建时间:  2014-12-17 15:27
	// 参数说明:  @pTable为表实例指针
	// 返 回 值:  void
	//////////////////////////////////////////////////////////////////////////
	void PrepareHisSysTableRows(CMdbTable *pTable);

	////////////////////////////////////////////////////////////////////////
	// 描    述:  历史库的同步更新线程，负责所有表的串行更新
	// 作    者:  邵凯田
	// 创建时间:  2014-11-24 18:38
	// 参数说明:  @lp为this指针
	// 返 回 值:  NULL
	//////////////////////////////////////////////////////////////////////////
	static void* ThreadHisDbSync(void *lp);

	////////////////////////////////////////////////////////////////////////
	// 描    述:  释放过期的无效记录
	// 作    者:  邵凯田
	// 创建时间:  2014-11-24 18:38
	// 参数说明:  @lp为this指针
	// 返 回 值:  NULL
	//////////////////////////////////////////////////////////////////////////
	static void* ThreadFreeOldRecord(void *lp);

private:

	SPtrList<CMdbTable> m_Tables;//所有的数据表实例
	SPtrList<CMdbRecord> m_DeletedRecords;	//所有表即将要被删除的记录列表
	char m_sLastError[1024];
	CMdbTable *m_pSysTable;		//系统表
	SPtrList<CMdbTableIndex> m_Index;//所有的索引
	SPtrList<stuSessionConnectTrigger> m_ConnTrg;//触发连接关系
	SString m_sMdbHomePath;//路径主目录
	bool m_bHisDb;//是否含持久化存储库(本地文件存储)
	bool m_bQuit;
	int m_iThreads;
	SPtrList<stuMdbMgrHisDbSyncOper> m_HisSyncOper;
};



#pragma pack(push)  
#pragma pack(1)

struct stuHisIndexInfo
{
	int hash_size;	  //HASH数量
	unsigned int flags;//bit0表示主键状态，bit1表示唯一键状态
	char idx_name[32];//索引名称
	BYTE field[32];//索引字段序号，从1开始编号，0表示无效
};
//////////////////////////////////////////////////////////////////////////
// 名    称:  stuHisTableInfo
// 作    者:  邵凯田
// 创建时间:  2014-11-18 15:10
// 描    述:  历史数据表文件信息
//////////////////////////////////////////////////////////////////////////
struct stuHisTableInfo
{
	int m_iMemMaxRows;	//内存中允许的最大记录数量
	int m_iHisMaxRows;	//历史表中允许的最大记录数量
	int m_iFieldCnt;	//字段数量
	int m_iIndexCnt;	//索引数量
	stuTableField fields[C_MDB_MAX_FIELD_CNT];	//所有的字段内容，最多C_MDB_MAX_FIELD_CNT个
	stuHisIndexInfo indexs[C_MDB_MAX_INDEX_CNT];//所有的索引内容
};

#pragma pack()
#pragma pack(pop)  

//////////////////////////////////////////////////////////////////////////
// 名    称:  CHisDbTable
// 作    者:  邵凯田
// 创建时间:  2014-11-18 15:10
// 描    述:  历史数据表文件类
//////////////////////////////////////////////////////////////////////////
class CHisDbTable : public SVarDbf<stuHisTableInfo>
{
public:
	CHisDbTable(CMdbManager *pMgr,CMdbTable *pTable);
	CHisDbTable(CMdbManager *pMgr, SString tabname, int iRowSize);
	virtual ~CHisDbTable();

	////////////////////////////////////////////////////////////////////////
	// 描    述:  添加一条索引到历史库
	// 作    者:  邵凯田
	// 创建时间:  2014-04-23 14:44
	// 参数说明:  @pIdx为索引指针
	// 返 回 值:  true表示成功，false表示失败
	//////////////////////////////////////////////////////////////////////////
	bool AddIndex(CMdbTableIndex *pIdx);

	////////////////////////////////////////////////////////////////////////
	// 描    述:  将历史表结构、数据还原到内存表
	// 作    者:  邵凯田
	// 创建时间:  2014-11-24 11:00
	// 参数说明:  void
	// 返 回 值:  true表示成功，false表示失败
	//////////////////////////////////////////////////////////////////////////
	bool RestoreTable();
	
	////////////////////////////////////////////////////////////////////////
	// 描    述:  取对应的表指针
	// 作    者:  邵凯田
	// 创建时间:  2014-11-24 14:47
	// 参数说明:  void
	// 返 回 值:  CMdbTable*
	//////////////////////////////////////////////////////////////////////////
	CMdbTable* GetTablePtr(){ return m_pTable; };

private:
	CMdbManager *m_pMdbMgr;
	CMdbTable *m_pTable;
};

#endif // !defined(AFX_REALDATA_H__426DEEA4_DEF8_4F0F_A173_C1F7461AF378__INCLUDED_)
