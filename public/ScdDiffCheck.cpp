/**
 *
 * �� �� �� : ScdDiffCheck.cpp
 * �������� : 2016-2-20 16:53
 * ��    �� : �ۿ���(skt001@163.com)
 * �޸����� : $Date: $
 * ��ǰ�汾 : $Revision: $
 * �������� : SCD�ļ������ԱȽ�
 * �޸ļ�¼ : 
 *            $Log: $
 *
 * Ver  Date        Author  Comments
 * ---  ----------  ------  -------------------------------------------
 * 001	2016-2-20	�ۿ�������ļ�
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
// ��    ��:  ���SCD�ļ�����
// ��    ��:  �ۿ���
// ����ʱ��:  2016-2-20 17:07
// ����˵��:  @sOldScdΪ��׼�ļ�ȫ·��
//         :  @sNewScd������ļ�ȫ·��
//         :  @sub_no��ʾ��վ��ţ�0��ʾ���洢�����ݿ�
//         :  @scd_no��ʾSCD�汾��ţ�0��ʾ���洢�����ݿ�
//         :  @pResultText��ʾ����ı��ķ���ָ�룬NULL��ʾ����Ҫ����ı�
// �� �� ֵ:  true��ʾ���ɹ���false��ʾ���ʧ��
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
		NewDiffText("��ʼ�汾",sub_no,scd_no,pResultText);
		return true;
	}
	if(!old_scd.ReadConfig(sOldScd))
	{
		if(pResultText != NULL)
			*pResultText += SString::toFormat("��׼�ļ�[%s]���ǺϷ���SCD�ļ�!",sOldScd.data());
		return false;
	}
	if(!new_scd.ReadConfig(sNewScd))
	{
		if(pResultText != NULL)
			*pResultText += SString::toFormat("�����ļ�[%s]���ǺϷ���SCD�ļ�!",sNewScd.data());
		return false;
	}
	CheckNode("SCL",&old_scd,&new_scd,sub_no,scd_no,pResultText);
	return true;
}

//////////////////////////////////////////////////////////////////////////
// ��    ��:  �ύһ���µĲ����ı�
// ��    ��:  �ۿ���
// ����ʱ��:  2016-2-21 9:38
// ����˵��:  
// �� �� ֵ:  void
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
// ��    ��:  ȡ��ǰ�ڵ���������Ե�����
// ��    ��:  �ۿ���
// ����ʱ��:  2016-2-21 21:29
// ����˵��:  
// �� �� ֵ:  
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
// ��    ��:  �Ա�ָ����ƽ���ڵ���Լ��¼��ڵ㣬�в���ʱ���ٵݹ�
// ��    ��:  �ۿ���
// ����ʱ��:  2016-2-21 9:43
// ����˵��:  
// �� �� ֵ:  
//////////////////////////////////////////////////////////////////////////
void CScdDiffChecker::CheckNode(SString sNodePath,SBaseConfig *pOldCfg,SBaseConfig *pNewCfg,int sub_no,int scd_no,SString *pResultText/*=NULL*/)
{
	//�ȱȽϵ�ǰ�ڵ�������ƥ���
	unsigned long pos1,pos2,pos3,pos4;
	SBaseConfig *pNewSub;
	SBaseConfig::SConfigAttribute *pOldAttr,*pNewAttr;
	SBaseConfig *pOldSub = pOldCfg->GetChildPtr()->FetchFirst(pos1);
	while(pOldSub)
	{
		//��ѯ�Զ���ͬ�Ľڵ�
		pNewSub = pNewCfg->GetChildPtr()->FetchFirst(pos2);
		while(pNewSub)
		{
			//�Ƚ��Ƿ�ƥ��ڵ�
			if(pOldSub->m_sNodeName == pNewSub->m_sNodeName)
			{
				//�ڵ�������ͬ
				//�ж��Ƿ���������ֵ��ͬ
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
			//����ȫ��ͬ,δ�ҵ�ƥ��Ľڵ�
			NewDiffText(SString::toFormat("%s.%sԭ�ڵ㱻�޸Ļ�ɾ��!����Ϊ:%s",
				sNodePath.data(),pOldSub->m_sNodeName.data(),
				GetNodeAttrsDesc(pOldSub).data()),sub_no,scd_no,pResultText);
		}
		else
		{
			//��ȫ��ͬ���Ƚ���һ��
			CheckNode(SString::toFormat("%s.%s",sNodePath.data(),pOldSub->m_sNodeName.data()),
				pOldSub,pNewSub,sub_no,scd_no,pResultText);
		}
		pOldSub = pOldCfg->GetChildPtr()->FetchNext(pos1);
	}

	pNewSub = pNewCfg->GetChildPtr()->FetchFirst(pos1);
	while(pNewSub)
	{
		//��ѯ�Զ���ͬ�Ľڵ�
		pOldSub = pOldCfg->GetChildPtr()->FetchFirst(pos2);
		while(pOldSub)
		{
			//�Ƚ��Ƿ�ƥ��ڵ�
			if(pNewSub->m_sNodeName == pOldSub->m_sNodeName)
			{
				//�ڵ�������ͬ
				//�ж��Ƿ���������ֵ��ͬ
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
			//����ȫ��ͬ,δ�ҵ�ƥ��Ľڵ�
			NewDiffText(SString::toFormat("%s.%s�ڵ㱻�޸Ļ�����!����Ϊ:%s",
				sNodePath.data(),pNewSub->m_sNodeName.data(),
				GetNodeAttrsDesc(pNewSub).data()),sub_no,scd_no,pResultText);
		}

		pNewSub = pNewCfg->GetChildPtr()->FetchNext(pos1);
	}
}