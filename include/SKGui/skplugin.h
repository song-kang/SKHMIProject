#ifndef SKPLUGIN_H
#define SKPLUGIN_H

#include <string.h>

#ifdef WIN32
# define PLUGIN_EXPORT __declspec(dllexport)
#else
# define PLUGIN_EXPORT
#endif

#ifndef WIN32
#ifndef QT_NO_DEBUG
#ifndef _DEBUG
#define _DEBUG
#endif
#endif
#endif

#ifdef _DEBUG	//��ǰ�����DEBUG�汾��������������DEBUG�汾
#define _PLUGIN_INIT \
	if(strstr(sExtAttr,"debug=true;") == NULL) \
	{ \
		return 1; \
	} 

#else	//��ǰ�������DEBUG�汾�������ڳ�����DEBUG�汾
#define _PLUGIN_INIT \
	if(strstr(sExtAttr,"debug=true;") != NULL) \
	{ \
		return 2; \
	} 
#endif

typedef int (*pFunPluginInit)(const char* sExtAttr);
typedef int (*pFunPluginExit)();
typedef const char* (*pFunPluginGetPluginName)();
typedef const char* (*pFunPluginGetPluginVer)();
typedef const char* (*pFunPluginFunPointSupported)();
typedef const char* (*pFunPluginGActSupported)();
typedef void* (*pFunPluginNewView)(const char* sFunName,const void* parentWidget);

#endif // SKPLUGIN_H
