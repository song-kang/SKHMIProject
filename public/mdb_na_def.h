/**
 *
 * 文 件 名 : mdb_na_def.h
 * 创建日期 : 2015-11-6 15:23
 * 作    者 : 邵凯田(skt001@163.com)
 * 修改日期 : $Date: $
 * 当前版本 : $Revision: $
 * 功能描述 : 内存数据库结构定义-网分模型
 * 修改记录 : 
 *            $Log: $
 *
 * Ver  Date        Author  Comments
 * ---  ----------  ------  -------------------------------------------
 * 001	2015-11-6	邵凯田　创建文件
 *
 **/

#pragma pack(push, Old, 1)

//--3.1.1	网分采集口配置表T_NA_CAPTURE_PORT
struct t_na_capture_port
{
	unsigned char sub_no;
	unsigned char na_no;
	unsigned char port_no;
	unsigned char port_type;
	char name[64];
	short bit_rate;
};

//--3.1.2	网分告警代码表T_NA_EVENT_CODE
struct t_na_event_code
{
	unsigned char prot_type;
	unsigned char event_id;
	char name[32];
	unsigned char event_type;
	unsigned char event_level;
	float score;
};

// --3.1.3	网分历史告警表T_NA_HISTORY_EVENT
// 	--历史告警表不建在内存库中

//--3.1.4	网分采集口实时流量表T_NA_PORT_REALFLOW
struct t_na_port_realflow
{
	unsigned char sub_no;
	unsigned char na_no;
	unsigned char port_no;
	int soc;
	int all_pkgs;
	int all_bytes;
	int all_evts;
	int sv_pkgs;
	int sv_bytes;
	int sv_evts;
	int goose_pkgs;
	int goose_bytes;
	int goose_evts;
	int ptp_pkgs;
	int ptp_bytes;
	int ptp_evts;
	int mms_pkgs;
	int mms_bytes;
	int mms_evts;
	int iec104_pkgs;
	int iec104_bytes;
	int iec104_evts;
	int ip_pkgs;
	int ip_bytes;
	int ip_evts;
	int tcp_pkgs;
	int tcp_bytes;
	int tcp_evts;
	int udp_pkgs;
	int udp_bytes;
	int udp_evts;
	int arp_pkgs;
	int arp_bytes;
	int arp_evts;
	int ntp_pkgs;
	int ntp_bytes;
	int ntp_evts;
	int icmp_pkgs;
	int icmp_bytes;
	int icmp_evts;
	int oth_pkgs;
	int oth_bytes;
	int oth_evts;
};

//--3.1.5	网分采集口告警统计表T_NA_PORT_EVENT_CNT
struct t_na_port_event_cnt
{
	unsigned char na_no;
	unsigned char port_no;
	unsigned char prot_type;
	unsigned char event_id;
	unsigned char sub_no;
	int soc;
	unsigned char status ;
	int shield_cnt;
	int start_cnt;
	int prohibit_cnt;
};

//--3.1.6	网分实时SV控制块实时状态表T_NA_SV_REALFLOW
struct t_na_sv_realflow
{
	unsigned char sub_no;
	short appid;
	int soc;
	int pkgs;
	int bytes;
	int evts;
};

//--3.1.7	网分实时SV控制块告警统计表T_NA_SV_EVENT_CNT
struct t_na_sv_event_cnt
{
	short appid;
	unsigned char prot_type;
	unsigned char event_id;
	unsigned char sub_no;
	int soc;
	unsigned char status;
	int shield_cnt;
	int start_cnt;
	int prohibit_cnt;
};

//--3.1.8	网分实时GOOSE控制块实时状态表T_NA_GSE_REALFLOW
struct t_na_gse_realflow
{
	unsigned char sub_no;
	short appid;
	int soc;
	int pkgs;
	int bytes;
	int evts;
};

//--3.1.9	网分实时GOOSE控制块告警统计表T_NA_GSE_EVENT_CNT
struct t_na_gse_event_cnt
{
	short appid;
	unsigned char prot_type;
	unsigned char event_id;
	unsigned char sub_no;
	int soc;
	unsigned char status;
	int shield_cnt;
	int start_cnt;
	int prohibit_cnt;
};

//--3.1.10	网分实时GOOSE数据集明细表T_NA_GSE_DATASET_ITEM
struct t_na_gse_dataset_item
{
	short appid;
	unsigned char chn_no;
	unsigned char sub_no;
	int soc;
	unsigned char flag;
	unsigned char stval;
	char sval[32];
	int qval;
};

//--3.1.11	网分实时MMS会话表T_NA_MMS_SESSION
struct t_na_mms_session
{
	unsigned char sub_no;
	int client_ip;
	int server_ip;
	int client_port;
	int server_port;
	unsigned char port_no;
	int soc;
	int real_pkgs;
	int real_bytes;
	unsigned char state;
	int connect_time;
};

//--3.1.12	网分实时IEC104会话表T_NA_IEC104_SESSION
struct t_na_iec104_session
{
	unsigned char sub_no;
	int client_ip;
	int server_ip;
	int client_port;
	int server_port;
	unsigned char port_no ;
	int soc;
	int real_pkgs;
	int real_bytes;
	unsigned char state;
	int connect_time;
};

//--3.1.13	网分采集单元配置表T_NA_UNIT
struct t_na_unit
{
	unsigned char na_no;
	char name[32];
	unsigned char unit_type;
	char na_ip[32];
	unsigned char comm_state;
};

//--3.1.14	网分协议类型表T_NA_PROTOCOL_TYPE
struct t_na_protocol_type
{
	unsigned char prot_type;
	char prot_name[32];
};

//--3.1.15	网分采集口历史流量表T_NA_PORT_HISTORY_FLOW
struct t_na_port_history_flow
{
	unsigned char sub_no;
	unsigned char na_no;
	unsigned char port_no;
	int soc;
	float bps;
};

//--3.1.16	装置告警实时统计表T_NA_IED_EVENT_COUNT
struct t_na_ied_event_count
{
	int ied_id;
	unsigned char id_type;//标识类型
				//1-	SV appid
				//2-	GOOSE appid
				//3-	IP
	unsigned char state;//状态:	0有效，1无效
	int evts;
};

//--3.1.17	物理子网表T_NA_SUB_NETWORK
struct t_na_sub_network
{
	unsigned char net_id;// byte NOT NULL,
	char net_name[16];// char(16),
	unsigned char net_type;// byte,
	char net_desc[32];// char(32),
	int net_bitrate;// int,
	int uiwnd_sn;
};

//--3.1.18	通讯设备表T_NA_COMM_DEVICE
struct t_na_comm_device
{
	unsigned char com_id;// byte NOT NULL,
	unsigned char net_id;// byte,
	unsigned char dev_type;// byte,
	char com_name[32];// char(32),
	unsigned char mgr_prot;
	char mgr_ip[20];// char(20),
	int mgr_port;// int,
	unsigned char mgr_state;
	unsigned char state_confirm;
	char dev_factory[16];// char(16),
	char dev_model[32];// char(32),
	char dev_version[16];// char(16),
	char dev_crc[20];// char(20)
	char up_time[64];// char(64),
	char ext_attr[512];// char(512),

};

//--3.1.19	通讯设备端口表T_NA_COMM_DEVICE_PORT
struct t_na_comm_device_port
{
	unsigned char net_id;// byte,
	unsigned char dev_cls;// byte,
	int dev_id;// int,
	unsigned char port_id;// int NOT NULL,
	char port_name[32];// char(32),
	unsigned char port_type;// byte,
	unsigned char if_type;// byte,
	char ip_addr[20];// char(20),
	char mac_addr[20];// char(20),
	char mcast_mac_addr[20];// char(20),
	unsigned char port_state;// byte,
	unsigned char state_confirm;//numeric(1,0)
	int port_flow;//	int
	int port_inflow;//	int
	int port_outflow;//	int
	int in_bytes;//	int
	int in_pkgs;//	int
	int in_bcast_pkgs;//	int
	int in_mcast_pkgs;//	int
	int sum_in_pkts_64;//	int
	int sum_in_pkts_65_127;//	int
	int sum_in_pkts_128_255;//	int
	int sum_in_pkts_256_511;//	int
	int sum_in_pkts_512_1023;//	int
	int sum_in_pkts_1024_1518;//	int
};

//--3.1.20	通讯端口连接线表T_NA_COMM_CONNECT_LINE
struct t_na_comm_connect_line
{
	unsigned char net_id_a;// byte,
	unsigned char dev_cls_a;// byte,
	int dev_id_a;// int,
	unsigned char port_id_a;// int NOT NULL,
	unsigned char net_id_b;// byte,
	unsigned char dev_cls_b;// byte,
	int dev_id_b;// int,
	unsigned char port_id_b;// int NOT NULL,
	unsigned char line_type;
	unsigned char line_stat;// byte,
	unsigned char state_confirm;
	int soc;
};

//--3.1.21	通讯MAC地址表T_NA_COMM_MAC_ADDR
struct t_na_comm_mac_addr
{
	int ip_addr;
	char mac_addr[20];
	unsigned char na_no;
	unsigned char port_no;
	int soc;
};

//--3.1.22	交换机MAC地址表T_NA_SWITCH_PORT_MAC
struct t_na_switch_port_mac
{
	unsigned char net_id;// byte NOT NULL,
	int com_id;// int NOT NULL,
	unsigned char port_id;// byte NOT NULL,
	int ip_addr;// int ,
	short         sub_no;
	char mac_addr[20];// char(20) NOT NULL,	
	int soc;// int,
};

#pragma pack(pop, Old)
