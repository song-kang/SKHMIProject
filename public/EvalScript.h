/**
 *
 * 文 件 名 : EvalScript.h
 * 创建日期 : 2016-10-27 11:10
 * 作    者 : 邵凯田(skt001@163.com)
 * 修改日期 : $Date: $
 * 当前版本 : $Revision: $
 * 功能描述 : 运维评估模型-评估解析型脚本实现
 * 修改记录 : 
 *            $Log: $
 *
 * Ver  Date        Author  Comments
 * ---  ----------  ------  -------------------------------------------
 * 001	2016-10-27	邵凯田　创建文件
 *
 **/

#ifndef __NAM_EVAL_SCRIPT_H__
#define __NAM_EVAL_SCRIPT_H__

#include "ssp_base.h"
#include "SScriptParser.h"
#include "../nam/nam_main/ScoreCnt.h"
#include <map>

class CNamEvalMgr;

//////////////////////////////////////////////////////////////////////////
// 名    称:  CEvalScriptParser
// 作    者:  邵凯田
// 创建时间:  2016-10-27 11:16
// 描    述:  运维评估脚本解翻译器类
//////////////////////////////////////////////////////////////////////////
class CEvalScriptParser : public SScriptParser
{
public:
	struct stuDeductInfo
	{
		float fDeductVal;//扣分值
		SString sDeductCause;//扣分原因
	};
	struct stuDevice
	{
		stuDevice()
		{
			deduct.setAutoDelete(true);
		}
		void NewDeduct(float fVal,SString str)
		{
			stuDeductInfo* pNew = new stuDeductInfo();
			pNew->fDeductVal = fVal;
			pNew->sDeductCause = str;
			deduct.append(pNew);
		}
		int dev_sn;
		int dev_cls;
		int dev_id;
		float score;//当前分数
		float prev_score;//上一评分周期分数
		float weight;//权重
		SPtrList<stuDeductInfo> deduct;
	};
	
	typedef map<int,stuDevice*> MAP_DEVICE;
	CEvalScriptParser();
	virtual ~CEvalScriptParser();

	//////////////////////////////////////////////////////////////////////////
	// 描    述:  取指针分值对应的检修级别
	// 作    者:  邵凯田
	// 创建时间:  2016-5-12 16:20
	// 参数说明:  @score为分值0~100
	//         :  @rs为检修级别数据集，对应字段为:oh_level,score_from,score_to
	// 返 回 值:  反应对应级别的oh_level，0表示未找到
	//////////////////////////////////////////////////////////////////////////
	int GetOverhaulLevel(float score,SRecordset &rs);

	//////////////////////////////////////////////////////////////////////////
	// 描    述:  开始指定厂站的评估
	// 作    者:  邵凯田
	// 创建时间:  2016-12-9 13:19
	// 参数说明:  @subno：厂站编号
	//         :  @eval_type表示评分类型:1-5分钟,2-1小时,3-1天
	//         :  @base_soc表示基准时间点
	// 返 回 值:  void
	//////////////////////////////////////////////////////////////////////////
	void StartSubStationEval(int subno,int eval_type,int base_soc);

	//////////////////////////////////////////////////////////////////////////
	// 描    述:  取扩展函数的数量
	// 作    者:  邵凯田
	// 创建时间:  2016-12-9 10:52
	// 参数说明:  void
	// 返 回 值:  返回有效扩展函数的数量
	//////////////////////////////////////////////////////////////////////////
	static int GetExtFunctionCount();

	//////////////////////////////////////////////////////////////////////////
	// 描    述:  取指定序号的扩展函数指针
	// 作    者:  邵凯田
	// 创建时间:  2016-12-9 10:53
	// 参数说明:  @idx为序号，从0开始
	// 返 回 值:  stuExtFunction*, NULL表示不存在
	//////////////////////////////////////////////////////////////////////////
	static const stuExtFunction* GetExtFunctionByIdx(int idx);

	//////////////////////////////////////////////////////////////////////////
	// 描    述:  取扩展变量的数量 
	// 作    者:  邵凯田
	// 创建时间:  2016-12-9 17:18
	// 参数说明:  void
	// 返 回 值:  返回有效扩展变量的数量
	//////////////////////////////////////////////////////////////////////////
	static int GetExtVariantCount();

	//////////////////////////////////////////////////////////////////////////
	// 描    述:  取指定序号的扩展变量的指针
	// 作    者:  邵凯田
	// 创建时间:  2016-12-9 17:18
	// 参数说明:  @idx为序号，从0开始
	// 返 回 值:  stuExtVariant*， NULL表示不存在
	//////////////////////////////////////////////////////////////////////////
	static const stuExtVariant* GetExtVariantByIdx(int idx);

	//////////////////////////////////////////////////////////////////////////
	// 描    述:  当前厂站内历史状态量统计
	// 作    者:  邵凯田
	// 创建时间:  2016-12-8 15:40
	// 参数说明:  @pCbParam表示回调参数(注册时指定的)
	//         :  @ReturnVal表示返回值内容引用
	//         :  @pParamValues形式参数列表指针，NULL表示没有形参，参数采用引用方式
	// 返 回 值:  true表示运行成功，false表示运行失败
	//////////////////////////////////////////////////////////////////////////
	static bool ExtFun_HisStateCnt(void *pCbParam,SScriptParser::stuValue &ReturnVal,SPtrList<SScriptParser::stuValue> *pParamValues);


	//////////////////////////////////////////////////////////////////////////
	// 描    述:  当前厂站内历史事件量统计
	// 作    者:  邵凯田
	// 创建时间:  2016-12-8 15:40
	// 参数说明:  @pCbParam表示回调参数(注册时指定的)
	//         :  @ReturnVal表示返回值内容引用
	//         :  @pParamValues形式参数列表指针，NULL表示没有形参，参数采用引用方式
	// 返 回 值:  true表示运行成功，false表示运行失败
	//////////////////////////////////////////////////////////////////////////
	static bool ExtFun_HisEventCnt(void *pCbParam,SScriptParser::stuValue &ReturnVal,SPtrList<SScriptParser::stuValue> *pParamValues);

	//////////////////////////////////////////////////////////////////////////
	// 描    述:  跳转到下一个历史状态量
	// 作    者:  邵凯田
	// 创建时间:  2016-12-8 15:44
	// 参数说明:  @pCbParam表示回调参数(注册时指定的)
	//         :  @ReturnVal表示返回值内容引用
	//         :  @pParamValues形式参数列表指针，NULL表示没有形参，参数采用引用方式
	// 返 回 值:  true表示运行成功，false表示运行失败
	//////////////////////////////////////////////////////////////////////////
	static bool ExtFun_NextHisState(void *pCbParam,SScriptParser::stuValue &ReturnVal,SPtrList<SScriptParser::stuValue> *pParamValues);


	//////////////////////////////////////////////////////////////////////////
	// 描    述:  跳转到下一个历史事件量
	// 作    者:  邵凯田
	// 创建时间:  2016-12-8 15:45
	// 参数说明:  @pCbParam表示回调参数(注册时指定的)
	//         :  @ReturnVal表示返回值内容引用
	//         :  @pParamValues形式参数列表指针，NULL表示没有形参，参数采用引用方式
	// 返 回 值:  true表示运行成功，false表示运行失败
	//////////////////////////////////////////////////////////////////////////
	static bool ExtFun_NextHisEvent(void *pCbParam,SScriptParser::stuValue &ReturnVal,SPtrList<SScriptParser::stuValue> *pParamValues);

	
	//////////////////////////////////////////////////////////////////////////
	// 描    述:  当前未复归状态量
	// 作    者:  邵凯田
	// 创建时间:  2016-12-9 10:34
	// 参数说明:  @pCbParam表示回调参数(注册时指定的)
	//         :  @ReturnVal表示返回值内容引用
	//         :  @pParamValues形式参数列表指针，NULL表示没有形参，参数采用引用方式
	// 返 回 值:  true表示运行成功，false表示运行失败
	//////////////////////////////////////////////////////////////////////////
	static bool ExtFun_UnResumedStCnt(void *pCbParam,SScriptParser::stuValue &ReturnVal,SPtrList<SScriptParser::stuValue> *pParamValues);

	//////////////////////////////////////////////////////////////////////////
	// 描    述:  当前未复归状态量
	// 作    者:  邵凯田
	// 创建时间:  2016-12-9 10:34
	// 参数说明:  @pCbParam表示回调参数(注册时指定的)
	//         :  @ReturnVal表示返回值内容引用
	//         :  @pParamValues形式参数列表指针，NULL表示没有形参，参数采用引用方式
	// 返 回 值:  true表示运行成功，false表示运行失败
	//////////////////////////////////////////////////////////////////////////
	static bool ExtFun_NextUnResumeSt(void *pCbParam,SScriptParser::stuValue &ReturnVal,SPtrList<SScriptParser::stuValue> *pParamValues);

	//////////////////////////////////////////////////////////////////////////
	// 描    述:  取当前未复归通信状态的冗余通信状态
	// 作    者:  邵凯田
	// 创建时间:  2017-2-7 10:05
	// 参数说明:  @pCbParam表示回调参数(注册时指定的)
	//         :  @ReturnVal表示返回值内容引用
	//         :  @pParamValues形式参数列表指针，NULL表示没有形参，参数采用引用方式
	// 返 回 值:  true表示运行成功，false表示运行失败
	//////////////////////////////////////////////////////////////////////////
	static bool ExtFun_ResunCommSt(void *pCbParam,SScriptParser::stuValue &ReturnVal,SPtrList<SScriptParser::stuValue> *pParamValues);

	//////////////////////////////////////////////////////////////////////////
	// 描    述:  当前设备扣分
	// 作    者:  邵凯田
	// 创建时间:  2016-12-9 10:34
	// 参数说明:  @pCbParam表示回调参数(注册时指定的)
	//         :  @ReturnVal表示返回值内容引用
	//         :  @pParamValues形式参数列表指针，NULL表示没有形参，参数采用引用方式
	// 返 回 值:  true表示运行成功，false表示运行失败
	//////////////////////////////////////////////////////////////////////////
	static bool ExtFun_CurrDevDeduct(void *pCbParam,SScriptParser::stuValue &ReturnVal,SPtrList<SScriptParser::stuValue> *pParamValues);

	//////////////////////////////////////////////////////////////////////////
	// 描    述:  当前厂站扣分
	// 作    者:  邵凯田
	// 创建时间:  2016-12-9 10:34
	// 参数说明:  @pCbParam表示回调参数(注册时指定的)
	//         :  @ReturnVal表示返回值内容引用
	//         :  @pParamValues形式参数列表指针，NULL表示没有形参，参数采用引用方式
	// 返 回 值:  true表示运行成功，false表示运行失败
	//////////////////////////////////////////////////////////////////////////
	static bool ExtFun_CurrSubDeduct(void *pCbParam,SScriptParser::stuValue &ReturnVal,SPtrList<SScriptParser::stuValue> *pParamValues);

public:
	CNamEvalMgr *m_pEvalMgr;
	float m_fStationScore;

protected:

	inline stuDevice* SearchDevice(int dev_sn)
	{
		MAP_DEVICE::iterator it = m_mapDevices.find(dev_sn);
		if(it == m_mapDevices.end())
		{
			stuDevice *p = new stuDevice();
			p->dev_sn = dev_sn;
			p->score = 100.0f;
			m_Devices.append(p);
			m_mapDevices[dev_sn] = p;
			return p;
		}
		return it->second;
	}

	void NewDeduct(float fVal,SString str)
	{
		stuDeductInfo* pNew = new stuDeductInfo();
		pNew->fDeductVal = fVal;
		pNew->sDeductCause = str;
		m_SubDeduct.append(pNew);
	}


	eSsp_Database_Type m_eDbType;//数据库类型
	int m_iSubNo;
	int m_iDevSn;
	int m_iStatId;
	int m_iEvalType;
	int m_iBaseSoc;
	SRecordset m_rsDevice;//当前厂站下所有设备的信息及上次扣分
	SRecordset m_rsHisState,m_rsHisEvent;//dev_sn,dev_name,cl_name,inf_name,his_cnt,prot_type
	SRecordset m_rsUnResumedState;//未复归实时状态量
	int m_iCurrHisStateRow,m_iCurrHisEventRow;
	SRecord *m_pCurrHisStateRow,*m_pCurrHisEventRow;
	int m_iCurrUnResumeStRow;
	SRecord *m_pCurrUnResumeStRow;
	stuValue m_vSubNo,m_vDevSn,m_vDevName,m_vProtType,m_vClName,m_vInfName,m_vInfWarnDesc;
	stuValue m_vHisStAmount,m_vHisEvtAmount;
	stuValue m_vUnStSecs;
	MAP_DEVICE m_mapDevices;
	SPtrList<stuDevice> m_Devices;
	SString m_sLastDeductCause;//最后一次异常状态、事件次数越限扣分原因
	SPtrList<stuDeductInfo> m_SubDeduct;//厂站扣分详情
// 	SString m_sSubHisDeduct;//厂站历史扣分表，不含历史事件、状态触发
// 	SString m_sDevHisDeduct;//设备历史扣分表，不含历史事件、状态触发

};

#define C_MAX_EVAL_THREADS 8
class CNamEvalMgr : public SService
{
public:
	struct stuThreadParam
	{
		CNamEvalMgr *pThis;
		int iThreadIdx;
		SString sSubNos;//逗号分隔的多个厂站编号
	};
	CNamEvalMgr();
	~CNamEvalMgr();


	////////////////////////////////////////////////////////////////////////
	// 描    述:  启动服务,虚函数，派生函数中必须先调用此函数
	// 作    者:  邵凯田
	// 创建时间:  2016-12-12 14:35
	// 参数说明:  void
	// 返 回 值:  true表示服务启动成功,false表示服务启动失败
	//////////////////////////////////////////////////////////////////////////
	virtual bool Start();

	////////////////////////////////////////////////////////////////////////
	// 描    述:  停止服务，派生函数中必须先调用此函数
	// 作    者:  邵凯田
	// 创建时间:  2016-12-12 14:35
	// 参数说明:  void
	// 返 回 值:  true表示服务停止成功,false表示服务停止失败
	//////////////////////////////////////////////////////////////////////////
	virtual bool Stop();


	//////////////////////////////////////////////////////////////////////////
	// 描    述:  开始一次评估任务
	// 作    者:  邵凯田
	// 创建时间:  2016-12-12 8:35
	// 参数说明:  @sScriptText为脚本语言全文本
	//         :  @eval_type表示评分类型:1-5分钟,2-1小时,3-1天
	//         :  @base_soc表示基准时间点
	// 返 回 值:  void
	//////////////////////////////////////////////////////////////////////////
	void BeginEval(SString &sScriptText,int eval_type,int base_soc);

	void SetEvalToDb(bool bEvalToDb)
	{
		m_bEvalToDb = bEvalToDb;
	}
private:
	//////////////////////////////////////////////////////////////////////////
	// 描    述:  任务等待线程
	// 作    者:  邵凯田
	// 创建时间:  2016-12-12 14:46
	// 参数说明:  @lp,this
	// 返 回 值:  NULL
	//////////////////////////////////////////////////////////////////////////
	static void* ThreadWait(void *lp);

	//////////////////////////////////////////////////////////////////////////
	// 描    述:  评分线程
	// 作    者:  邵凯田
	// 创建时间:  2016-12-12 8:42
	// 参数说明:  @lp,stuThreadParam*
	// 返 回 值:  NULL
	//////////////////////////////////////////////////////////////////////////
	static void* ThreadEval(void *lp);

	CEvalScriptParser m_Parser[C_MAX_EVAL_THREADS];
	stuThreadParam m_Param[C_MAX_EVAL_THREADS];
	SString m_sScriptText;
	int m_iEvalType;
	int m_iBaseSoc;

public:
	CSsp_BatchDmlInsert m_Ins_nam_dev_his_deduct;
	CSsp_BatchDmlInsert m_Ins_nam_station_his_deduct;
	CSsp_BatchDmlInsert m_Ins_nam_dev_real_score;
	CSsp_BatchDmlInsert m_Ins_nam_dev_his_mscore;
	CSsp_BatchDmlInsert m_Ins_nam_dev_his_hscore;
	CSsp_BatchDmlInsert m_Ins_nam_dev_his_dscore;
	CSsp_BatchDmlInsert m_Ins_nam_station_real_score;
	CSsp_BatchDmlInsert m_Ins_nam_station_his_mscore;
	CSsp_BatchDmlInsert m_Ins_nam_station_his_hscore;
	CSsp_BatchDmlInsert m_Ins_nam_station_his_dscore;
	CSsp_BatchDmlInsert m_Ins_nam_his_score_cnt;
	CSsp_BatchDml m_db_multi_sql;
	bool m_bEvalToDb;//是否将评估结果存储到数据，true表示存入数据库，false表示存入内存结构
	CNamScoreCnt m_ScoreCnt;
};

#endif//__NAM_EVAL_SCRIPT_H__
