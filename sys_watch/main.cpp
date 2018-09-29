/**
 *
 * 文 件 名 : main.cpp
 * 创建日期 : 2015-8-1 13:31
 * 修改日期 : $Date: $
 * 当前版本 : $Revision: $
 * 功能描述 : 看门狗主程序(含硬件看门狗)
 * 修改记录 : 
 *
 **/

#include "WatchApplication.h"

CWatchApplication g_app;

int main(int argc, char* argv[])
{
	g_app.SetModuleDesc("看门狗进程");
	g_app.SetVersion("1.0.0");
	g_app.Run(argc,argv,__DATE__,__TIME__);

	return 0;
}
