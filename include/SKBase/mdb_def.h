#ifndef _MDB_DEF_H_
#define _MDB_DEF_H_

#pragma pack(push, Old, 1)

struct t_oe_element_state
{
	int ied_no;				//IEDΨһ���,
	BYTE cpu_no;			//CPU��,
	short group_no;			//���
	short entry;			//��Ŀ��
	short fun;				//������
	short inf;				//��Ϣ���
	char name[128];			//����
	BYTE type;				//��Ϣ����
	short evt_cls;			//�¼����ʷ���
	BYTE val_type;			//ֵ����
	BYTE severity_level;	//�¼��ȼ�
	char on_dsc[8];			//������
	char off_dsc[8];		//������
	char unknown_desc[8];	//δ֪����
	int inver;				//��־λ
	BYTE current_val;		//��ǰ״̬
	char measure_val[32];	//����ֵ
	int soc;				//������
	int usec;				//΢��
	char mms_path[128];		//MMS�߼��ڵ�·��
	char details[256];		//ϸ������
};

#pragma pack(pop, Old)

#endif//_MDB_DEF_H_
