#include "cpluginmgr.h"

//////////////////// CPlugin ////////////////////////////
bool CPlugin::Load(SString sPluginFile)
{
	m_sPluginFile = sPluginFile;
	bool err = false;

#ifdef WIN32
	m_hInstance = LoadLibrary(sPluginFile.data());
	if(m_hInstance == NULL)
	{
		LOGWARN("���ز��%sʧ��!",sPluginFile.data());
		return false;
	}

	m_pPlugin_Init = (pFunPluginInit)GetProcAddress(m_hInstance, "PluginInit");
	m_pPlugin_Exit = (pFunPluginExit)GetProcAddress(m_hInstance, "PluginExit");
	m_pPlugin_NewView = (pFunPluginNewView)GetProcAddress(m_hInstance, "PluginNewView");
	m_pPlugin_GetPluginVer = (pFunPluginGetPluginVer)GetProcAddress(m_hInstance, "PluginGetVer");
	m_pPlugin_GetPluginName = (pFunPluginGetPluginName)GetProcAddress(m_hInstance, "PluginGetName");
	m_pPlugin_GActSupported = (pFunPluginGActSupported)GetProcAddress(m_hInstance, "PluginGActSupported");
	m_pPlugin_FunPointSupported = (pFunPluginFunPointSupported)GetProcAddress(m_hInstance, "PluginFunPointSupported");

#else
	m_hInstance = dlopen(sPluginFile.data(),RTLD_LAZY);
	char *pErr;

	if ((pErr = dlerror()) != NULL)
	{
		err = true;
		LOGWARN("dlopen error:%s\n",pErr);
	}
	if(m_hInstance == NULL)
	{
		LOGWARN("���ز��%sʧ��!",sPluginFile.data());
		return false;
	}
	m_pPlugin_Init = (pFunPluginInit)dlsym(m_hInstance, "PluginInit");
	if ((pErr = dlerror()) != NULL)
	{
		err = true;
		LOGWARN("dlsym error:%s\n",pErr);
	}
	m_pPlugin_Exit = (pFunPluginExit)dlsym(m_hInstance, "PluginExit");
	if ((pErr = dlerror()) != NULL)
	{
		err = true;
		LOGWARN("dlsym error:%s\n",pErr);
	}
	m_pPlugin_GetPluginName = (pFunPluginGetPluginName)dlsym(m_hInstance, "PluginGetName");
	if ((pErr = dlerror()) != NULL)
	{
		err = true;
		LOGWARN("dlsym error:%s\n",pErr);
	}
	m_pPlugin_GetPluginVer = (pFunPluginGetPluginVer)dlsym(m_hInstance, "PluginGetVer");
	if ((pErr = dlerror()) != NULL)
	{
		err = true;
		LOGWARN("dlsym error:%s\n",pErr);
	}
	m_pPlugin_FunPointSupported = (pFunPluginFunPointSupported)dlsym(m_hInstance, "PluginFunPointSupported");
	if ((pErr = dlerror()) != NULL)
	{
		err = true;
		LOGWARN("dlsym error:%s\n",pErr);
	}
	m_pPlugin_NewView = (pFunPluginNewView)dlsym(m_hInstance, "PluginNewView");
	if ((pErr = dlerror()) != NULL)
	{
		err = true;
		LOGWARN("dlsym error:%s\n",pErr);
	}
	m_pPlugin_GActSupported = (pFunPluginGActSupported)dlsym(m_hInstance, "PluginGActSupported");
	if ((pErr = dlerror()) != NULL)
	{
		err = true;
		LOGWARN("dlsym error:%s\n",pErr);
	}

#endif
	if (err || 
		m_pPlugin_Init == NULL || 
		m_pPlugin_GetPluginName == NULL || 
		m_pPlugin_GetPluginVer == NULL || 
		m_pPlugin_FunPointSupported == NULL || 
		m_pPlugin_NewView == NULL)
	{
		if (m_pPlugin_Init == NULL && 
			m_pPlugin_GetPluginName == NULL && 
			m_pPlugin_GetPluginVer == NULL && 
			m_pPlugin_FunPointSupported == NULL && 
			m_pPlugin_NewView == NULL)
		{
			LOGDEBUG("���ز��%sʧ��!����������ھ����߱���DLL����ȫ����HMI���!",sPluginFile.data());
			return false;//����������ھ����߱���DLL����ȫ���ǲ��
		}

		LOGWARN("���ز��%sʧ��!",sPluginFile.data());
		return false;
	}

#ifdef _DEBUG
	int ret = m_pPlugin_Init("debug=true;");
#else
	int ret = m_pPlugin_Init("");
#endif
	if(ret != 0)
	{
		if(ret == 1 || ret == 2)
		{
			SString text;
#ifdef _DEBUG
			text.sprintf("���ܼ��ط�DEBUG�汾���[%s]!",sPluginFile.data());
#else
			text.sprintf("���ܼ���DEBUG�汾���[%s]!",sPluginFile.data());
#endif
			LOGDEBUG("���%s��ʼ��ʧ��!ret=%d, DEBUG�汾��һ�£�����",sPluginFile.data(),ret);
		}
		else
		{
			LOGWARN("���%s��ʼ��ʧ��!ret=%d",sPluginFile.data(),ret);
		}

		return false;
	}

	LOGDEBUG("���%s���سɹ�!",sPluginFile.data());
	return true;
}

//////////////////// CPluginMgr ////////////////////////////
CPluginMgr::CPluginMgr(QObject *parent)
	: QObject(parent)
{

}

CPluginMgr::~CPluginMgr()
{
	foreach (CPlugin *p, m_lstPlugin)
		delete p;

	m_lstPlugin.clear();
}

bool CPluginMgr::Init()
{
	SString sPath = Common::GetCurrentAppPath().toStdString().data();
	LOGDEBUG("׼������Ŀ¼[%s]HMI���",sPath.data());

#ifdef WIN32
	SDir dir(sPath,"*.dll");
#else
	SDir dir(sPath,"*.so");
#endif

	int i,cnt = dir.count();
	for (i = 0; i < cnt; i++)
	{
		SString sFile = dir[i];
#ifdef WIN32
		if (sFile == "." || sFile == ".." || sFile.left(7) != "plugin_" || sFile.right(4) != ".dll")
#else
		if (sFile == "." || sFile == ".." || sFile.left(10) != "libplugin_" || sFile.right(3) != ".so")
#endif
		{
			LOGDEBUG("��%d����̬��%s����! \n",i+1,sFile.data());
			continue;
		}
		LOGDEBUG("��%d����̬��%s����...",i+1,sFile.data());

		SString attr = dir.attribute(i);
		if (SString::GetAttributeValueI(attr,"isdir") != 1)
		{
			CPlugin *pPlugin = new CPlugin();
			if (!pPlugin->Load(sPath + sFile))
				delete pPlugin;
			else
				m_lstPlugin.append(pPlugin);
		}
	}

	return true;
}

bool CPluginMgr::Exit()
{
	foreach (CPlugin *p, m_lstPlugin)
	{
		if (p->m_pPlugin_Exit)
			p->m_pPlugin_Exit();
	}

	return true;
}

CBaseView* CPluginMgr::NewView(QString sPluginName,QWidget* parent)
{
	CBaseView *w;
	foreach (CPlugin *p, m_lstPlugin)
	{
		w = (CBaseView *)p->m_pPlugin_NewView(sPluginName.toStdString().data(),parent);
		if (w)
			return w;
	}

	return NULL;
}
