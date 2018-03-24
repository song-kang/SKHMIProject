#ifndef CPLUGINMGR_H
#define CPLUGINMGR_H

#include "skhead.h"
#include "skplugin.h"
#include "cbaseview.h"
#include "SApi.h"

//////////////////// CPlugin ////////////////////////////
class CPlugin
{
public:
	CPlugin(){};
	~CPlugin(){};

	SString m_sPluginFile;
	
#ifdef WIN32
	HMODULE m_hInstance;
#else
	void* m_hInstance;
#endif

	pFunPluginInit				m_pPlugin_Init;
	pFunPluginExit				m_pPlugin_Exit;
	pFunPluginGetPluginName		m_pPlugin_GetPluginName;
	pFunPluginGetPluginVer		m_pPlugin_GetPluginVer;
	pFunPluginFunPointSupported	m_pPlugin_FunPointSupported;
	pFunPluginNewView			m_pPlugin_NewView;
	pFunPluginGActSupported		m_pPlugin_GActSupported;

	bool Load(SString sPluginFile);
};

//////////////////// CPluginMgr ////////////////////////////
class CPluginMgr : public QObject
{
	Q_OBJECT

public:
	CPluginMgr(QObject *parent = 0);
	~CPluginMgr();

	bool Init();
	bool Exit();
	CBaseView* NewView(QString sPluginName,QWidget* parent);

private:
	QList<CPlugin*> m_lstPlugin;
	
};

#endif // CPLUGINMGR_H
