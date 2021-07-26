/**
 *
 * �� �� �� : NaModel.h
 * �������� : 2017-8-24 11:09
 * ��    �� : �ۿ���(skt001@163.com)
 * �޸����� : $Date: $
 * ��ǰ�汾 : $Revision: $
 * �������� : �������ģ��
 * �޸ļ�¼ : 
 *            $Log: $
 *
 * Ver  Date        Author  Comments
 * ---  ----------  ------  -------------------------------------------
 * 001	2017-8-24	�ۿ�������ļ�
 *
 **/

#ifndef __NA_MODEL_H__
#define __NA_MODEL_H__

#include "SString.h"
#include "SList.h"
#include "SXmlConfig.h"
#include "SFile.h"
#include "SDateTime.h"
#include "ssp_base.h"
#include <map>

class CNaModelMgr;
class CNaBaseModel
{
public:
	CNaBaseModel();
	~CNaBaseModel();

	//////////////////////////////////////////////////////////////////////////
	// ��    ��:  �����ݿⱸ��������Ϣ��ָ��Ŀ¼��������δ�ı�ʱ������
	// ��    ��:  �ۿ���
	// ����ʱ��:  2017-8-24 14:45
	// ����˵��:  @sPathΪĿ¼����
	// �� �� ֵ:  int, >=0��ʾ���ݱ�������<0��ʾʧ��
	//////////////////////////////////////////////////////////////////////////
	int BackupFromDb(SString sPath);

	//////////////////////////////////////////////////////////////////////////
	// ��    ��:  ��ԭ������Ϣ���ݵ����ݿ⣬���ļ��޸�ʱ�����һ�β���δ������Ի�ԭ
	// ��    ��:  �ۿ���
	// ����ʱ��:  2017-8-24 14:33
	// ����˵��:  @sPathΪĿ¼����
	// �� �� ֵ:  int,>=0��ʾ�ɹ���ԭ��������<0��ʾʧ��
	//////////////////////////////////////////////////////////////////////////
	int ChkRestoreToDb(SString sPath);

	//////////////////////////////////////////////////////////////////////////
	// ��    ��:  ��ԭ������Ϣ���ݵ����ݿ⣬
	// ��    ��:  �ۿ���
	// ����ʱ��:  2017-8-24 14:33
	// ����˵��:  @sPathΪĿ¼����
	// �� �� ֵ:  int,>=0��ʾ�ɹ���ԭ��������<0��ʾʧ��
	//////////////////////////////////////////////////////////////////////////
	int RestoreToDb(SString sPathFile);

	SString m_sTableName;
	SString m_sFieldNames;
	SString m_sFieldTypes;//int,str,float,blob
	SString m_sOrderBy;
	int m_iLastRestoreTime;//���һ�λָ�XML��ʱ��
	SString m_sLastBackupXml;//���һ�α��ݵ�XML�ļ�
	SXmlConfig m_Config;
	CNaModelMgr *m_pMgr;
};

class CNaModelMgr
{
public:
	CNaModelMgr();
	~CNaModelMgr();

	//////////////////////////////////////////////////////////////////////////
	// ��    ��:  ���½���NETID��ӳ���¼
	// ��    ��:  �ۿ���
	// ����ʱ��:  2017-8-24 16:22
	// ����˵��:  @num��ʾ��¼����
	// �� �� ֵ:  void
	//////////////////////////////////////////////////////////////////////////
	//void NewNetIdMap(int num);


	//////////////////////////////////////////////////////////////////////////
	// ��    ��:  ���NETID����վ����վӳ��
	// ��    ��:  �ۿ���
	// ����ʱ��:  2018-6-1 17:01
	// ����˵��:  
	// �� �� ֵ:  
	//////////////////////////////////////////////////////////////////////////
	void AddNewNetId(int sub_net_id,int main_net_id);

	//////////////////////////////////////////////////////////////////////////
	// ��    ��:  ���COMID����վ����վӳ��
	// ��    ��:  �ۿ���
	// ����ʱ��:  2018-6-1 17:01
	// ����˵��:  
	// �� �� ֵ:  
	//////////////////////////////////////////////////////////////////////////
	void AddNewComId(int sub_com_id,int main_com_id);

	//////////////////////////////////////////////////////////////////////////
	// ��    ��:  ȡ��վNETID
	// ��    ��:  �ۿ���
	// ����ʱ��:  2018-6-1 17:02
	// ����˵��:  
	// �� �� ֵ:  
	//////////////////////////////////////////////////////////////////////////
	int GetMainNetId(int sub_net_id);

	//////////////////////////////////////////////////////////////////////////
	// ��    ��:  ȡ��վCOMID
	// ��    ��:  �ۿ���
	// ����ʱ��:  2018-6-1 17:02
	// ����˵��:  
	// �� �� ֵ:  
	//////////////////////////////////////////////////////////////////////////
	int GetMainComId(int sub_com_id);

	//////////////////////////////////////////////////////////////////////////
	// ��    ��:  �����ݿⱸ��������Ϣ��ָ��Ŀ¼��������δ�ı�ʱ������
	// ��    ��:  �ۿ���
	// ����ʱ��:  2017-8-24 14:32
	// ����˵��:  @sPathΪĿ¼����
	// �� �� ֵ:  int, >=0��ʾ���ݱ�������<0��ʾʧ��
	//////////////////////////////////////////////////////////////////////////
	int BackupFromDb(SString sPath);

	//////////////////////////////////////////////////////////////////////////
	// ��    ��:  ��ԭ������Ϣ���ݵ����ݿ⣬���ļ��޸�ʱ�����һ�β���δ������Ի�ԭ
	// ��    ��:  �ۿ���
	// ����ʱ��:  2017-8-24 14:33
	// ����˵��:  @sPathΪĿ¼����
	// �� �� ֵ:  int,>=0��ʾ�ɹ���ԭ��������<0��ʾʧ��
	//////////////////////////////////////////////////////////////////////////
	int RestoreToDb(SString sPath);

	CNaBaseModel m_na_sub_network;			//net_id
	CNaBaseModel m_na_comm_device;			//com_id
	CNaBaseModel m_na_comm_device_port;		//net_id,dev_cls,dev_id,port_id
	CNaBaseModel m_na_comm_connect_line;	//net_id_a,dev_cls_a,dev_id_a,port_id_a,net_id_b,dev_cls_b,dev_id_b,port_id_b
	CNaBaseModel m_na_iec104_session;		//sub_no,client_ip,server_ip,client_port,server_port
	CNaBaseModel m_na_mms_session;			//sub_no,client_ip,server_ip,client_port,server_port
// 	CNaBaseModel m_na_unit;					//na_no
// 	CNaBaseModel m_na_capture_port;			//na_no,port_no
// 	CNaBaseModel m_na_comm_mac_addr;		//na_no,ip_addr,mac_addr
// 	CNaBaseModel m_na_gse_dataset_item;		//sub_no,appid,chn_no
// 	CNaBaseModel m_na_gse_event_cnt;
// 	CNaBaseModel m_na_gse_realflow;
// 	CNaBaseModel m_na_port_event_cnt;
// 	CNaBaseModel m_na_port_realflow;
// 	CNaBaseModel m_na_sv_event_cnt;
// 	CNaBaseModel m_na_sv_realflow;
// 	CNaBaseModel m_na_switch_port_mac;
	int m_iSubNo;//��վ��Ķ�Ӧ��վ��
/*
	int m_iNetIdNum;//net_id����
	int* m_piSubNetId;//��վ���net_id����
	int* m_piMainNetId;//��վ���net_id����*/
	std::map<int, int> m_mapNetId;//��վ��ID-->��վ��ID
	std::map<int, int> m_mapComId;//��վ��ID-->��վ��ID

};

#endif//__NA_MODEL_H__
