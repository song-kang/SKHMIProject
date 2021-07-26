/**
 *
 * 文 件 名 : ScdDiffCheck.h
 * 创建日期 : 2016-2-20 16:53
 * 作    者 : 邵凯田(skt001@163.com)
 * 修改日期 : $Date: $
 * 当前版本 : $Revision: $
 * 功能描述 : SCD文件差异性比较
 * 修改记录 : 
 *            $Log: $
 *
 * Ver  Date        Author  Comments
 * ---  ----------  ------  -------------------------------------------
 * 001	2016-2-20	邵凯田　创建文件
 *
 **/

#ifndef __UK9010_SCD_DIFF_CHECK_H__
#define __UK9010_SCD_DIFF_CHECK_H__

#include "ssp_base.h"
class CScdDiffChecker
{
public:
	CScdDiffChecker();
	~CScdDiffChecker();

	//////////////////////////////////////////////////////////////////////////
	// 描    述:  检查SCD文件差异
	// 作    者:  邵凯田
	// 创建时间:  2016-2-20 17:07
	// 参数说明:  @sOldScd为基准文件全路径
	//         :  @sNewScd待检测文件全路径
	//         :  @sub_no表示厂站编号，0表示不存储到数据库
	//         :  @scd_no表示SCD版本编号，0表示不存储到数据库
	//         :  @pResultText表示结果文本的返回指针，NULL表示不需要结果文本
	// 返 回 值:  true表示检查成功，false表示检查失败
	//////////////////////////////////////////////////////////////////////////
	bool CheckScd(SString sOldScd,SString sNewScd,int sub_no,int scd_no,SString *pResultText=NULL);

	SDatabaseOper *m_pDbOper;//数据库操作类，NULL表示不使用数据库

private:
	//////////////////////////////////////////////////////////////////////////
	// 描    述:  提交一条新的差异文本
	// 作    者:  邵凯田
	// 创建时间:  2016-2-21 9:38
	// 参数说明:  
	// 返 回 值:  void
	//////////////////////////////////////////////////////////////////////////
	void NewDiffText(SString sDiffText,int sub_no,int scd_no,SString *pResultText=NULL);

	//////////////////////////////////////////////////////////////////////////
	// 描    述:  对比指定的平级节点的以及下级节点，有差异时则不再递归
	// 作    者:  邵凯田
	// 创建时间:  2016-2-21 9:43
	// 参数说明:  
	// 返 回 值:  
	//////////////////////////////////////////////////////////////////////////
	void CheckNode(SString sNodePath,SBaseConfig *pOldCfg,SBaseConfig *pNewCfg,int sub_no,int scd_no,SString *pResultText=NULL);

	//////////////////////////////////////////////////////////////////////////
	// 描    述:  取当前节点的所有属性的描述
	// 作    者:  邵凯田
	// 创建时间:  2016-2-21 21:29
	// 参数说明:  
	// 返 回 值:  
	//////////////////////////////////////////////////////////////////////////
	SString GetNodeAttrsDesc(SBaseConfig *pNode);

	int m_iDiffNo;
};

#endif//__UK9010_SCD_DIFF_CHECK_H__
