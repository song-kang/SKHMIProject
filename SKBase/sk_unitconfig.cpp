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

	return true;
}
