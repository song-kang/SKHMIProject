#ifndef _MDB_DEF_H_
#define _MDB_DEF_H_

#pragma pack(push, Old, 1)

struct t_oe_element_state
{
	int ied_no;				//IED唯一编号,
	BYTE cpu_no;			//CPU号,
	short group_no;			//组号
	short entry;			//条目号
	short fun;				//功能码
	short inf;				//信息序号
	char name[128];			//名称
	BYTE type;				//信息类型
	short evt_cls;			//事件性质分类
	BYTE val_type;			//值类型
	BYTE severity_level;	//事件等级
	char on_dsc[8];			//合描述
	char off_dsc[8];		//分描述
	char unknown_desc[8];	//未知描述
	int inver;				//标志位
	BYTE current_val;		//当前状态
	char measure_val[32];	//测量值
	int soc;				//世纪秒
	int usec;				//微秒
	char mms_path[128];		//MMS逻辑节点路径
	char details[256];		//细节描述
};

#pragma pack(pop, Old)

#endif//_MDB_DEF_H_
