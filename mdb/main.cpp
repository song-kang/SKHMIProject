/**
 *
 * 文 件 名 : main.cpp
 * 创建日期 : 2014-4-22 11:24
 * 作    者 : 邵凯田(skt001@163.com)
 * 修改日期 : $Date: $
 * 当前版本 : $Revision: $
 * 功能描述 : 内存数据库主程序
 * 修改记录 : 
 *            $Log: $
 *
 * Ver  Date        Author  Comments
 * ---  ----------  ------  -------------------------------------------
 * 001	2014-4-22	邵凯田　创建文件
 *
 **/

//////////////////////////////////////////////////////////////////////////
// History
// 2.0.0    2014-04-22  邵凯田　起始版本
// 2.0.4    2015-08-15  邵凯田　增加处理能力保护机制，丢弃来不及发送或同步的数据，确保所有队列在有限数量范围内
// 2.0.5    2015-08-19  邵凯田　select语法中增加limit x功能支持，实现限制记录行数
//////////////////////////////////////////////////////////////////////////


#define MOD_VERSION "2.0.6"

#ifndef MOD_DATE
#define MOD_DATE "2017-09-05"
#endif
#ifndef MOD_TIME
#define MOD_TIME "19:08:00"
#endif

#include "MdbApplication.h"

#ifdef _DEBUG
#include "vld.h"
#endif

int main(int argc, char* argv[])
{
	CMdbApplication g_app;
	g_app.SetModuleDesc("内存数据库主程序");
	g_app.SetVersion(MOD_VERSION);
	g_app.Run(argc,argv,MOD_DATE,MOD_TIME);

	g_app.Stop();
	return 0;
}
