#include "sk_unitconfig.h"

static CSKUnitconfig* g_pUnitconfig = NULL;

CSKUnitconfig::CSKUnitconfig(void)
{

}

CSKUnitconfig::~CSKUnitconfig(void)
{

}

CSKUnitconfig* CSKUnitconfig::GetPtr()
{
	if(g_pUnitconfig == NULL)
		g_pUnitconfig = new CSKUnitconfig();

	return g_pUnitconfig;
}

bool CSKUnitconfig::Load(SString sPathFile)
{
	SXmlConfig xml;
	if(!xml.ReadConfig(sPathFile))
	{
		printf("�򿪵�Ԫ���������ļ�ʧ��!���޷�ʹ�ô�����! file=%s",sPathFile.data());
		return false;
	}

	SBaseConfig *pCfg;
	pCfg = xml.SearchChild("unit");
	if(pCfg == NULL)
	{
		printf("ȱ��[unit]�ڵ��ڵ�Ԫ�����ļ�:%s",sPathFile.data());
		return false;
	}

	m_sSystemType = xml.SearchNodeAttribute("system","soft_type");
	m_iUnitId = pCfg->GetAttributeI("id");
	m_sUnitName = pCfg->GetAttribute("name");
	m_sUnitDesc = pCfg->GetAttribute("desc");

	SBaseConfig *pUnitList = xml.SearchChild("unit-list");
	if(pUnitList == NULL)
	{
		LOGFAULT("ȱ��[unit-list]�ڵ��ڵ�Ԫ�����ļ�:%s",sPathFile.data());
		return false;
	}

	unsigned long pos = 0;
	pCfg = pUnitList->GetChildPtr()->FetchFirst(pos);
	while(pCfg)
	{
		if(pCfg->GetNodeName() == "unit")
		{
			CSpUnit *pUnit = new CSpUnit();
			pUnit->m_iUnitId = pCfg->GetAttributeI("id");
			pUnit->m_sUnitCommIp = pCfg->GetAttribute("ip");
			pUnit->m_sUnitName = pCfg->GetAttribute("name");
			pUnit->m_sUnitDesc = pCfg->GetAttribute("desc");
			if(pUnit->m_iUnitId == m_iUnitId)
			{
				if(pUnit->m_sUnitName.length() > 0)
					m_sUnitName = pUnit->m_sUnitName;
				if(pUnit->m_sUnitDesc.length() > 0)
					m_sUnitDesc = pUnit->m_sUnitDesc;
				m_sThisIp = pUnit->m_sUnitCommIp;
			}
			m_Units.append(pUnit);
		}
		pCfg = pUnitList->GetChildPtr()->FetchNext(pos);
	}

	return true;
}
