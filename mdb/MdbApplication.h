/**
 *
 * 文 件 名 : MdbApplication.h
 * 创建日期 : 2014-4-22 11:58
 * 作    者 : 邵凯田(skt001@163.com)
 * 修改日期 : $Date: $
 * 当前版本 : $Revision: $
 * 功能描述 : 内存数据库服务程序
 * 修改记录 : 
 *            $Log: $
 *
 * Ver  Date        Author  Comments
 * ---  ----------  ------  -------------------------------------------
 * 001	2014-4-22	邵凯田　创建文件
 *
 **/

#ifndef __MDB_APPLICATION_H__
#define __MDB_APPLICATION_H__

#include "SApplication.h"
#include "MdbService.h"
#include "plugin.h"
#include "sk_unitconfig.h"

class CMdbApplication : public SApplication 
{
public:
	CMdbApplication();
	virtual ~CMdbApplication();


	////////////////////////////////////////////////////////////////////////
	// 描    述:  启动服务，开启所有应用的服务，该函数必须由派生类实现，且启动服务完成后必须返回
	// 作    者:  邵凯田
	// 创建时间:  2014:4:22 11:55
	// 参数说明:  void
	// 返 回 值:  true/false
	//////////////////////////////////////////////////////////////////////////
	virtual bool Start();

	////////////////////////////////////////////////////////////////////////
	// 描    述:  停止服务，关闭所有应用的服务
	// 作    者:  邵凯田
	// 创建时间:  2014:4:22 11:55
	// 参数说明:  void
	// 返 回 值:  true/false
	//////////////////////////////////////////////////////////////////////////
	virtual bool Stop();

	//////////////////////////////////////////////////////////////////////////
	// 描    述:  处理文本命令
	// 作    者:  邵凯田
	// 创建时间:  2016-2-13 15:12
	// 参数说明:  @sCmd为命令内容，空字符串表示取表命令列表
	//         :  @sResult为返回结果，纯文本
	// 返 回 值:  true表示成功，false表示失败
	//////////////////////////////////////////////////////////////////////////
	virtual bool ProcessTxtCmd(SString &sCmd,SString &sResult);

	////////////////////////////////////////////////////////////////////////
	// 描    述:  代理消息处理接口，由派生类实现，处理函数必须尽量短小，快速返回
	// 作    者:  邵凯田
	// 创建时间:  2016-2-3 15:20
	// 参数说明:  @wMsgType表示消息类型
	//         :  @pMsgHead为消息头
	//         :  @sHeadStr消息头字符串
	//         :  @pBuffer为消息携带的二进制数据内容，NULL表示无二进制数据
	//         :  @iLength为二进制数据长度
	// 返 回 值:  true表示处理成功，false表示处理失败或未处理
	//////////////////////////////////////////////////////////////////////////
	bool ProcessAgentMsg(WORD wMsgType,stuSpUnitAgentMsgHead *pMsgHead,SString &sHeadStr,BYTE* pBuffer=NULL,int iLength=0);

	CMdbService m_MdbService;
	CMdbPluginMgr m_PluginMgr;

	//////////////////////////////////////////////////////////////////////////
	// 描    述:  运行初始化SQL的回调函数
	// 作    者:  邵凯田
	// 创建时间:  2015-11-24 11:33
	// 参数说明:  @sql为SQL语句
	// 返 回 值:  bool
	//////////////////////////////////////////////////////////////////////////
	static bool Mdb_Run_Sql_Fun(char* sql);

	//////////////////////////////////////////////////////////////////////////
	// 描    述:  内存库插入回调函数
	// 作    者:  邵凯田
	// 创建时间:  2015-11-24 13:13
	// 参数说明:  
	// 返 回 值:  int
	//////////////////////////////////////////////////////////////////////////
	static int Mdb_Insert_Fun(char* sTableName, void *pRowData, int iRowSize, int iRows);

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
	static int Mdb_Retrieve_Fun(char* sql, unsigned char** ppRecordset, int *piRowSize, int *piRows, char** ppsRowResult);

	//////////////////////////////////////////////////////////////////////////
	// 描    述:  取表字段信息
	// 作    者:  邵凯田
	// 创建时间:  2016-3-23 23:19
	// 参数说明:  @plFields
	// 返 回 值:  字段数量
	//////////////////////////////////////////////////////////////////////////
	static int Mdb_GetTableFields_Fun(char* tablename,SPtrList<stuTableField> *plFields);
};


#endif//__MDB_APPLICATION_H__
