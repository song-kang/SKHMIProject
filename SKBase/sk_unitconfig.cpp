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
		printf("打开单元代理配置文件失败!将无法使用代理功能! file=%s",sPathFile.data());
		return false;
	}

	SBaseConfig *pCfg;
	pCfg = xml.SearchChild("unit");
	if(pCfg == NULL)
	{
		printf("缺少[unit]节点在单元配置文件:%s",sPathFile.data());
		return false;
	}

	m_sSystemType = xml.SearchNodeAttribute("system","soft_type");
	m_iUnitId = pCfg->GetAttributeI("id");
	m_sUnitName = pCfg->GetAttribute("name");
	m_sUnitDesc = pCfg->GetAttribute("desc");

	return true;
}
