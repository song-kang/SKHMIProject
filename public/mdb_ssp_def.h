/**
 *
 * �� �� �� : mdb_ssp_def.h
 * �������� : 2015-11-12 15:55
 * ��    �� : �ۿ���(skt001@163.com)
 * �޸����� : $Date: $
 * ��ǰ�汾 : $Revision: $
 * �������� : �ڴ����ݿ�ṹ����-SSP01ƽ̨
 * �޸ļ�¼ : 
 *            $Log: $
 *
 * Ver  Date        Author  Comments
 * ---  ----------  ------  -------------------------------------------
 * 001	2015-11-12	�ۿ�������ļ�
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
