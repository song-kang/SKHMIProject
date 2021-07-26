/**
 *
 * 文 件 名 : mdb_ssp_def.h
 * 创建日期 : 2015-11-12 15:55
 * 作    者 : 邵凯田(skt001@163.com)
 * 修改日期 : $Date: $
 * 当前版本 : $Revision: $
 * 功能描述 : 内存数据库结构定义-SSP01平台
 * 修改记录 : 
 *            $Log: $
 *
 * Ver  Date        Author  Comments
 * ---  ----------  ------  -------------------------------------------
 * 001	2015-11-12	邵凯田　创建文件
 *
 **/

#pragma pack(push, Old, 1)


struct t_ssp_tagged_text
{
	int tag_no;
	char tag_name[32];
	char tag_text[256];
	int tag_color;
};

#pragma pack(pop, Old)
