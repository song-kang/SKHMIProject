#include "MMSSVRApp.h"

/**
 * Ver    Date        Author  Comments
 * -----  ----------  ------  -------------------------------------------
 * 1.0.0        
 *
 **/

#define MOD_VERSION "1.0.2"

#ifndef MOD_DATE
#define MOD_DATE "2018-07-14"
#endif
#ifndef MOD_TIME
#define MOD_TIME "11:09:00"
#endif

// #ifdef _DEBUG
// #include "vld.h"
// #endif

int main(int argc, char* argv[])
{
	MMSSVRApp g_app;
	g_app.SetModuleDesc("IEC61850采集主站服务程序");
	g_app.SetVersion(MOD_VERSION);
	g_app.Run(argc, argv, MOD_DATE, MOD_TIME, true);
	g_app.Quit();
	return 0;
}

