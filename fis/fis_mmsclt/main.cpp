#include "MMSCLTApp.h"

/**
 * Ver    Date        Author  Comments
 * -----  ----------  ------  -------------------------------------------
 * 1.0.0  2015-11-16  宋康    河北自动化运维主站HMI初始版本
 *
 **/

#define MOD_VERSION "1.0.2"

#ifndef MOD_DATE
#define MOD_DATE "2018-11-13"
#endif
#ifndef MOD_TIME
#define MOD_TIME "13:50:00"
#endif

#ifdef _DEBUG
//#include "vld.h"
#endif

int main(int argc, char* argv[])
{
	MMSCLTApp g_app;
	g_app.SetModuleDesc("IEC61850采集主站接入程序");
	g_app.SetVersion(MOD_VERSION);
	g_app.Run(argc,argv,MOD_DATE,MOD_TIME,true);
	g_app.Quit();
	return 0;
}

