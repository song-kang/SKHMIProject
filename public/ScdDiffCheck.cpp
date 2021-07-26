/**
 *
 * 文 件 名 : ScdDiffCheck.cpp
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

#include "ScdDiffCheck.h"

CScdDiffChecker::CScdDiffChecker()
{
	m_pDbOper = NULL;
}

CScdDiffChecker::~CScdDiffChecker()
{

}


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
bool CScdDiffChecker::CheckScd(SString sOldScd,SString sNewScd,int sub_no,int scd_no,SString *pResultText/*=NULL*/)
{
	SXmlConfig old_scd;
	SXmlConfig new_scd;
	m_iDiffNo = 0;
	if(pResultText != NULL)
		*pResultText = "";
	if(sOldScd.length() == 0)
	{
		NewDiffText("初始版本",sub_no,scd_no,pResultText);
		return true;
	}
	if(!old_scd.ReadConfig(sOldScd))
	{
		if(pResultText != NULL)
			*pResultText += SString::toFormat("基准文件[%s]不是合法的SCD文件!",sOldScd.data());
		return false;
	}
	if(!new_scd.ReadConfig(sNewScd))
	{
		if(pResultText != NULL)
			*pResultText += SString::toFormat("待检文件[%s]不是合法的SCD文件!",sNewScd.data());
		return false;
	}
	CheckNode("SCL",&old_scd,&new_scd,sub_no,scd_no,pResultText);
	return true;
}

//////////////////////////////////////////////////////////////////////////
// 描    述:  提交一条新的差异文本
// 作    者:  邵凯田
// 创建时间:  2016-2-21 9:38
// 参数说明:  
// 返 回 值:  void
//////////////////////////////////////////////////////////////////////////
void CScdDiffChecker::NewDiffText(SString sDiffText,int sub_no,int scd_no,SString *pResultText/*=NULL*/)
{
	if(pResultText != NULL)
	{
		*pResultText += sDiffText;
		*pResultText += "\r\n";
	}
	if(sub_no != 0 && scd_no != 0 && m_pDbOper != NULL)
	{
		SString sql;
		m_iDiffNo++;
		sDiffText.replace("'","`");
		sql.sprintf("insert into T_OMS_SCD_HISTORY_DIFF (sub_no,scd_no,diff_no,diff_txt) values (%d,%d,%d,'%s')",sub_no,scd_no,m_iDiffNo,sDiffText.data());
		m_pDbOper->Execute(sql);
	}
}

//////////////////////////////////////////////////////////////////////////
// 描    述:  取当前节点的所有属性的描述
// 作    者:  邵凯田
// 创建时间:  2016-2-21 21:29
// 参数说明:  
// 返 回 值:  
//////////////////////////////////////////////////////////////////////////
SString CScdDiffChecker::GetNodeAttrsDesc(SBaseConfig *pNode)
{
	SString sText;
	unsigned long pos;
	SBaseConfig::SConfigAttribute *pAttr = pNode->GetAttributePtr()->FetchFirst(pos);
	while(pAttr)
	{
		sText+=SString::toFormat("%s=%s;",pAttr->m_sName.data(),pAttr->m_sValue.data());
		pAttr = pNode->GetAttributePtr()->FetchNext(pos);
	}
	return sText;
}

//////////////////////////////////////////////////////////////////////////
// 描    述:  对比指定的平级节点的以及下级节点，有差异时则不再递归
// 作    者:  邵凯田
// 创建时间:  2016-2-21 9:43
// 参数说明:  
// 返 回 值:  
//////////////////////////////////////////////////////////////////////////
void CScdDiffChecker::CheckNode(SString sNodePath,SBaseConfig *pOldCfg,SBaseConfig *pNewCfg,int sub_no,int scd_no,SString *pResultText/*=NULL*/)
{
	//先比较当前节点下所有匹配的
	unsigned long pos1,pos2,pos3,pos4;
	SBaseConfig *pNewSub;
	SBaseConfig::SConfigAttribute *pOldAttr,*pNewAttr;
	SBaseConfig *pOldSub = pOldCfg->GetChildPtr()->FetchFirst(pos1);
	while(pOldSub)
	{
		//查询对端相同的节点
		pNewSub = pNewCfg->GetChildPtr()->FetchFirst(pos2);
		while(pNewSub)
		{
			//比较是否匹配节点
			if(pOldSub->m_sNodeName == pNewSub->m_sNodeName)
			{
				//节点名称相同
				//判断是否所有属性值相同
				pOldAttr = pOldSub->GetAttributePtr()->FetchFirst(pos3);
				bool bAttrSame=true;
				while(pOldAttr)
				{
					pNewAttr = pNewSub->GetAttributePtr()->FetchFirst(pos4);
					while(pNewAttr)
					{
						if(SString::equals(pNewAttr->m_sName.data(),pOldAttr->m_sName.data()))
							break;
						pNewAttr = pNewSub->GetAttributePtr()->FetchNext(pos4);
					}
					if(pNewAttr == NULL || pNewAttr->m_sValue != pOldAttr->m_sValue)
					{
						bAttrSame = false;
						break;
					}
					pOldAttr = pOldSub->GetAttributePtr()->FetchNext(pos3);
				}
				if(bAttrSame)
					break;
			}
			pNewSub = pNewCfg->GetChildPtr()->FetchNext(pos2);
		}
		if(pNewSub == NULL)
		{
			//不完全相同,未找到匹配的节点
			NewDiffText(SString::toFormat("%s.%s原节点被修改或删除!属性为:%s",
				sNodePath.data(),pOldSub->m_sNodeName.data(),
				GetNodeAttrsDesc(pOldSub).data()),sub_no,scd_no,pResultText);
		}
		else
		{
			//完全相同，比较下一层
			CheckNode(SString::toFormat("%s.%s",sNodePath.data(),pOldSub->m_sNodeName.data()),
				pOldSub,pNewSub,sub_no,scd_no,pResultText);
		}
		pOldSub = pOldCfg->GetChildPtr()->FetchNext(pos1);
	}

	pNewSub = pNewCfg->GetChildPtr()->FetchFirst(pos1);
	while(pNewSub)
	{
		//查询对端相同的节点
		pOldSub = pOldCfg->GetChildPtr()->FetchFirst(pos2);
		while(pOldSub)
		{
			//比较是否匹配节点
			if(pNewSub->m_sNodeName == pOldSub->m_sNodeName)
			{
				//节点名称相同
				//判断是否所有属性值相同
				pNewAttr = pNewSub->GetAttributePtr()->FetchFirst(pos3);
				bool bAttrSame=true;
				while(pNewAttr)
				{
					pOldAttr = pOldSub->GetAttributePtr()->FetchFirst(pos4);
					while(pOldAttr)
					{
						if(SString::equals(pNewAttr->m_sName.data(),pOldAttr->m_sName.data()))
							break;
						pOldAttr = pOldSub->GetAttributePtr()->FetchNext(pos4);
					}
					if(pOldAttr == NULL || pNewAttr->m_sValue != pOldAttr->m_sValue)
					{
						bAttrSame = false;
						break;
					}
					pNewAttr = pOldSub->GetAttributePtr()->FetchNext(pos3);
				}
				if(bAttrSame)
					break;
			}
			pOldSub = pOldCfg->GetChildPtr()->FetchNext(pos2);
		}
		if(pOldSub == NULL)
		{
			//不完全相同,未找到匹配的节点
			NewDiffText(SString::toFormat("%s.%s节点被修改或新增!属性为:%s",
				sNodePath.data(),pNewSub->m_sNodeName.data(),
				GetNodeAttrsDesc(pNewSub).data()),sub_no,scd_no,pResultText);
		}

		pNewSub = pNewCfg->GetChildPtr()->FetchNext(pos1);
	}
}