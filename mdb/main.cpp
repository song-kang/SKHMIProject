/**
 *
 * �� �� �� : main.cpp
 * �������� : 2014-4-22 11:24
 * ��    �� : �ۿ���(skt001@163.com)
 * �޸����� : $Date: $
 * ��ǰ�汾 : $Revision: $
 * �������� : �ڴ����ݿ�������
 * �޸ļ�¼ : 
 *            $Log: $
 *
 * Ver  Date        Author  Comments
 * ---  ----------  ------  -------------------------------------------
 * 001	2014-4-22	�ۿ�������ļ�
 *
 **/

//////////////////////////////////////////////////////////////////////////
// History
// 2.0.0    2014-04-22  �ۿ����ʼ�汾
// 2.0.4    2015-08-15  �ۿ�����Ӵ��������������ƣ��������������ͻ�ͬ�������ݣ�ȷ�����ж���������������Χ��
// 2.0.5    2015-08-19  �ۿ��select�﷨������limit x����֧�֣�ʵ�����Ƽ�¼����
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
	g_app.SetModuleDesc("�ڴ����ݿ�������");
	g_app.SetVersion(MOD_VERSION);
	g_app.Run(argc,argv,MOD_DATE,MOD_TIME);

	g_app.Stop();
	return 0;
}
