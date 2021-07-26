#ifndef _MDB_OE_DEF_H_
#define _MDB_OE_DEF_H_

#pragma pack(push, Old, 1)

//--2.8	ͨ�÷�������Ŀ��T_OE_ELEMENT_GENERAL
struct t_oe_element_general
{
	int gen_sn;// GENΨһ���,
	int ied_no;// IEDΨһ���,
	int cpu_no;// CPU��,
	int group_no;//���
	int entry;//��Ŀ��
	char name[128];
	int valtype;//ֵ����
	char dime[32];
	float maxval;
	float minval;
	float stepval;
	int precision_n;
	int precision_m;
	int itemtype;//��Ŀ����
	float factor;
	float offset;
	float threshold;
	float smooth;
	char current_val[32];//��ǰֵ
	char reference_val[32];//�ο�ֵ
	char mms_path[128];//MMS�߼��ڵ�·��
	char basesignal[64];//���ܸ澯�����ź�
	int is_analog_cnt;
	int beginAddress;
	char regProperty[64];
	char da_name[64];
    int is_confirm;
};

//--2.9	״̬������Ŀ��T_OE_ELEMENT_STATE
struct t_oe_element_state
{
	int st_sn;// STΨһ���,
	int ied_no;// IEDΨһ���,
	int cpu_no;// CPU��,
	int group_no;//���
	int entry;//��Ŀ��
	int fun;//������
	int inf;//��Ϣ���
	char name[128];//����
	int type;//��Ϣ����
	int evt_cls;//�¼����ʷ���
	int val_type;//ֵ����
	int severity_level;//�¼��ȼ�
	char on_dsc[32];//������
	char off_dsc[32];//������
	char unknown_desc[32];//δ֪����
	int inver;//��־λ
	int current_val;//��ǰ״̬
	char measure_val[32];//����ֵ
	int soc;//������
	int usec;//΢��
	char mms_path[128];//MMS�߼��ڵ�·��
	char details[256];//ϸ������
	char basesignal[64];//���ܸ澯�����ź�
	int beginAddress;
	char regProperty[64];
    char da_name[64];
    int is_confirm;
};

// - ��ʷͳ��
struct t_oe_his_analog_hcnt
{
	int gen_sn;
	int cnt_soc;
	float val1;
	float val2;
	float val3;
	float val4;
	float val5;
	float val6;
	float val7;
	float val8;
	float val9;
	float val10;
	float val11;
	float val12;
	float val13;
	float val14;
	float val15;
	float val16;
	float val17;
	float val18;
	float val19;
	float val20;
	float val21;
	float val22;
	float val23;
	float val24;
	float max_val;
	int max_time;
	float min_val;
	int min_time;
	float avg_val;
};

struct t_oe_his_analog_dcnt
{
	int gen_sn;
	int cnt_soc;
	float val1;
	float val2;
	float val3;
	float val4;
	float val5;
	float val6;
	float val7;
	float val8;
	float val9;
	float val10;
	float val11;
	float val12;
	float val13;
	float val14;
	float val15;
	float val16;
	float val17;
	float val18;
	float val19;
	float val20;
	float val21;
	float val22;
	float val23;
	float val24;
	float val25;
	float val26;
	float val27;
	float val28;
	float val29;
	float val30;
	float val31;
	float max_val;
	int max_time;
	float min_val;
	int min_time;
	float avg_val;
};


//--���ܸ澯��IA��2.15	�澯�����t_ia_report
struct t_ia_report
{
	int id;
	int bay_id;
	char bay_name[100];
	int base_fault_id;
	char base_fault_name[100];
	int delay_alarm_id;
	char delay_alarm_name[100];
	int grade;
	int soc;
	int usec;
	char report[256];
};

//4.6.36	������ʷ�¼���T_NAM_HIS_ELEMENT_EVENT
struct t_nam_element_event
{
	int           evt_id;
	int           cl_id;
	short         inf;
	short         template_inf;
	int           soc;
	unsigned char val;
	int           usec;
	unsigned char q_invalid;
	unsigned char q_lock;
	int           repeat_cnt;
};

//4.6.35	������ʷң�ű�T_NAM_HIS_ELEMENT_STATE
struct t_nam_element_state
{
	int           stat_id;
	int           cl_id;
	short         inf;
	short         template_inf;
	int           soc;
	int           usec;
	unsigned char val;
	unsigned char q_invalid;
	unsigned char q_lock;
	int           repeat_cnt;
};

//4.6.48 ���й���״̬����T_OMS_RUN_HEALTH_ST
struct t_oms_run_health_st
{
    int            hlt_st;
	short          sub_no;
	short          hlt_inf;
	char           hlt_desc[128];
	char           st_on_desc[16];
	char           st_off_desc[16];
	unsigned char  st_val;
	int            pid1;
	int            pid2;
	int            pid3;
};

//4.6.49	���й���ģ������T_OMS_RUN_HEALTH_ANA
struct t_oms_run_health_ana
{
	int            hlt_ana;
	short          sub_no;
	short          hlt_inf;
	char           hlt_desc[128];
	char           hlt_dime[16];
	float          min_val;
	float          max_val;
	float          ana_val;
	int            pid1;
	int            pid2;
	int            pid3;
};
//--t_tmp_cpu_rate
struct t_tmp_cpu_rate
{
	int soc;
	int cpu_no;
	float rate;
};

struct t_tmp_system_time
{
	int id;
	int soc;
	float usec;
};

struct t_tmp_time_offset
{
	int soc;
	int offset;
};

struct t_oe_ied
{
	int           ied_no;
	short         sub_no;
	short         device_no;
	char          name[32];
	short		  type;
	short         pri_code;
	char          uri[32];
	char          model[32];
	char          version[32];
	char          crc[32];
	char          version_time[32];
	char          mms_path[32];
	short         comstate;
	short         comm_addr;
	float         weight;
	short		  tool_mode;
	char		  tool_path[128];
};

#pragma pack(pop, Old)

#endif//_MDB_OE_DEF_H_
