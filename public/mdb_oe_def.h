#ifndef _MDB_OE_DEF_H_
#define _MDB_OE_DEF_H_

#pragma pack(push, Old, 1)

//--2.8	通用分类组条目表T_OE_ELEMENT_GENERAL
struct t_oe_element_general
{
	int gen_sn;// GEN唯一编号,
	int ied_no;// IED唯一编号,
	int cpu_no;// CPU号,
	int group_no;//组号
	int entry;//条目号
	char name[128];
	int valtype;//值类型
	char dime[32];
	float maxval;
	float minval;
	float stepval;
	int precision_n;
	int precision_m;
	int itemtype;//条目类型
	float factor;
	float offset;
	float threshold;
	float smooth;
	char current_val[32];//当前值
	char reference_val[32];//参考值
	char mms_path[128];//MMS逻辑节点路径
	char basesignal[64];//智能告警基础信号
	int is_analog_cnt;
	int beginAddress;
	char regProperty[64];
	char da_name[64];
    int is_confirm;
};

//--2.9	状态量组条目表T_OE_ELEMENT_STATE
struct t_oe_element_state
{
	int st_sn;// ST唯一编号,
	int ied_no;// IED唯一编号,
	int cpu_no;// CPU号,
	int group_no;//组号
	int entry;//条目号
	int fun;//功能码
	int inf;//信息序号
	char name[128];//名称
	int type;//信息类型
	int evt_cls;//事件性质分类
	int val_type;//值类型
	int severity_level;//事件等级
	char on_dsc[32];//合描述
	char off_dsc[32];//分描述
	char unknown_desc[32];//未知描述
	int inver;//标志位
	int current_val;//当前状态
	char measure_val[32];//测量值
	int soc;//世纪秒
	int usec;//微秒
	char mms_path[128];//MMS逻辑节点路径
	char details[256];//细节描述
	char basesignal[64];//智能告警基础信号
	int beginAddress;
	char regProperty[64];
    char da_name[64];
    int is_confirm;
};

// - 历史统计
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


//--智能告警（IA）2.15	告警报告表t_ia_report
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

//4.6.36	网分历史事件表T_NAM_HIS_ELEMENT_EVENT
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

//4.6.35	网分历史遥信表T_NAM_HIS_ELEMENT_STATE
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

//4.6.48 运行工况状态量表T_OMS_RUN_HEALTH_ST
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

//4.6.49	运行工况模拟量表T_OMS_RUN_HEALTH_ANA
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
