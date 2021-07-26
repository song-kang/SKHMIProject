/**
 *
 * �� �� �� : UK9010App.h
 * �������� : 2016-2-6 9:38
 * ��    �� : �ۿ���(skt001@163.com)
 * �޸����� : $Date: $
 * ��ǰ�汾 : $Revision: $
 * �������� : UK9010��ά��վӦ�û�����
 * �޸ļ�¼ : 
 *            $Log: $
 *
 * Ver  Date        Author  Comments
 * ---  ----------  ------  -------------------------------------------
 * 001	2016-2-6	�ۿ�������ļ�
 *
 **/

#ifndef __UK9011_APPLICATION_H__
#define __UK9011_APPLICATION_H__

#include "SApplication.h"
#include "sp_config_mgr.h"
#include "ssp_base.h"
#include "ssp_database.h"
#include "sp_unit_config.h"
//#include "mib_window_cfg.h"
//#include "switch_cfg_fun.h"
//#include "switch_cfg.h"
#define C_SOFTWARE_TYPE "UK9011"

#ifndef OMS_MYSQL_DB
#define OMS_MYSQL_DB
#endif

#define OMCCFG_MIBWND SPCFG_USER
#define OMCCFG_MIBFUN SPCFG_USER2

enum eUK9011AppNo
{
	UK9011_APP_HMI = SP_UA_APPNO_USER,	//101 ͼ�ι���վ����
	UK9011_APP_MMSCLT,					//102 MS�ͻ��˽���
	UK9011_APP_IA_MAIN,					//103 ���ܸ澯������
	UK9011_APP_NAI_MAIN,				//104 ���ֽӿڼ��������������
	UK9011_APP_NPM_MAIN,				//105 ���籨�Ĵ洢ģ��
	UK9011_APP_MDBPLUGIN,				//106 �ڴ���ʼ�����
	UK9011_APP_OMS_MAIN,				//107 ��ά��վ������̨����
	UK9011_APP_NAM_IEC104,				//108 �㽭������վIEC104����ģ��
	UK9011_APP_NAM_MAIN,				//109 ������վ������̨����
	UK9011_APP_NAM104CLT,				//110 ������վIEC104����ģ��
	UK9011_APP_SVG_FACTORY,				//111 SVGͳһ���ɳ���ģ��
	UK9011_APP_SMART_RTU104				//112 ����Զ����һ�廯������չ104ģ��
};

//////////////////////////////////////////////////////////////////////////
// ��    ��:  eUK9010SysLogType
// ��    ��:  �ۿ���
// ����ʱ��:  2016-2-6 10:42
// ��    ��:  ϵͳ��־����
//////////////////////////////////////////////////////////////////////////
enum eUK9011SysLogType
{
	UK9011_LT_COMM_CONN = SP_UA_APPNO_USER,	//101 ������ͨѶ�ָ�
	UK9011_LT_COMM_DISC,				//102 ������ͨѶ�Ͽ�
	UK9011_LT_SWITCH_CFG_UPD,			//103 ������������װ
	UK9011_LT_SWITCH_CFG_DIFF,			//104 ���������ò�һ��
	UK9011_LT_SWITCH_WARN,				//105 �������澯

	UK9011_LT_CALL_RYABAN,				//106 �ٻ���ѹ��
	UK9011_LT_EDIT_SETTING,				//107 �༭��ֵ
	UK9011_LT_EDIT_SETTING_SUCC,		//108 �༭��ֵ�ɹ�
	UK9011_LT_EDIT_SETTING_FAIL,		//109 �༭��ֵʧ��
	UK9011_LT_EDIT_RYABAN_SUCC,			//110 �༭��ѹ��ɹ�
	UK9011_LT_EDIT_RYABAN_FAIL,			//111 �༭��ѹ��ʧ��
	UK9011_LT_EDIT_SECTOR,				//112 �༭��ֵ��				
	UK9011_LT_EDIT_SECTOR_SUCC,			//113 �༭��ֵ���ɹ�
	UK9011_LT_EDIT_SECTOR_FAIL,			//114 �༭��ֵ��ʧ��
	UK9011_LT_CTRL,						//115 ���Ʋ���
	UK9011_LT_CTRL_DIR_SUCC,			//116 ֱ�ӿ��Ƴɹ�
	UK9011_LT_CTRL_DIR_FAIL,			//117 ֱ�ӿ���ʧ��
	UK9011_LT_CTRL_SEL_SUCC,			//118 ѡ����Ƴɹ�
	UK9011_LT_CTRL_SEL_FAIL,			//119 ѡ�����ʧ��
	UK9011_LT_SCD_IMP,					//120 ��վSCDģ�͵���
	UK9011_LT_SCD_CHG,					//121 ��վSCD�ļ����
	UK9011_LT_SCD_HIS_DEL,				//122 ��վSCD�ļ���ʷ�ļ��Զ�����
	UK9011_LT_EFILE_DIFF,				//123 Զ��E�ļ�������
	UK9011_LT_DEV_MMSLOG,				//124 �豸MMS��־
	UK9011_LT_FILE,						//125 �ļ�����
	UK9011_LT_JOURNAL,					//126 ��־����
	UK9011_LT_NAM_SETTIME_SUCC,			//127 ������վ����Уʱ�ɹ�
	UK9011_LT_NAM_SETTIME_FAIL,			//127 ������վ����Уʱʧ��
	UK9011_LT_YD_MAINTENANCE = 201,     // 201 �㶫Զ���豸һ�廯��ά������־
	UK9011_LT_HT_MAINTENANCE,           // 202 �㶫��̨�豸һ�廯��ά������־
	UK9011_LT_CK_MAINTENANCE,           // 203 �㶫����豸һ�廯��ά������־

};

//////////////////////////////////////////////////////////////////////////
// ��    ��:  eUK9010MsgType
// ��    ��:  �ۿ���
// ����ʱ��:  2016-2-6 10:42
// ��    ��:  ������Ϣ���ͣ�����Ϣ��Ӧ��ASDU�ӡ�stu��ǰ׺
//////////////////////////////////////////////////////////////////////////
enum eUK9011MsgType
{
	UK9011_MSG_REQ_SETTING =SP_UA_MSG_USER,		//101 ���붨ֵ
	UK9011_MSG_RES_SETTING,						//102 ���붨ֵ�ظ�
	UK9011_MSG_REQ_SETTING_AREA,				//103 ���붨ֵ��
	UK9011_MSG_RES_SETTING_AREA,				//104 ���붨ֵ���ظ�
	UK9011_MSG_REQ_ANALOG,						//105 ����ģ����
	UK9011_MSG_RES_ANALOG,						//106 ����ģ�����ظ�
	UK9011_MSG_REQ_SWITCH,						//107 ���뿪����
	UK9011_MSG_RES_SWITCH,						//108 ���뿪�����ظ�
	UK9011_MSG_REQ_RYABAN,						//109 ������ѹ��
	UK9011_MSG_RES_RYABAN,						//110 ������ѹ��ظ�
	UK9011_MSG_REQ_EDIT_SETTING,				//111 �����޸Ķ�ֵ
	UK9011_MSG_RES_EDIT_SETTING,				//112 �����޸Ķ�ֵ�ظ�
	UK9011_MSG_REQ_EDIT_SETTING_AREA,			//113 �����޸Ķ�ֵ��
	UK9011_MSG_RES_EDIT_SETTING_AREA,			//114 �����޸Ķ�ֵ���ظ�
	UK9011_MSG_REQ_CURE_SETTING,				//115 ����̻���ֵ
	UK9011_MSG_RES_CURE_SETTING,				//116 ����̻���ֵ�ظ�
	UK9011_MSG_REQ_CTRL_SELECT,					//117 ����ѡ��
	UK9011_MSG_RES_CTRL_SELECT,					//118 ����ѡ��ظ�
	UK9011_MSG_REQ_CTRL_EXECUTE,				//119 ����ִ��
	UK9011_MSG_RES_CTRL_EXECUTE,				//120 ����ִ�лظ�
	UK9011_MSG_REQ_CTRL_CANCEL,					//121 ���Ƴ���
	UK9011_MSG_RES_CTRL_CANCEL,					//122 ���Ƴ����ظ�
	UK9011_MSG_REQ_CTRL_DIRECT,					//123 ֱ�ӿ���
	UK9011_MSG_RES_CTRL_DIRECT,					//124 ֱ�ӿ��ƻظ�
	UK9011_MSG_REQ_FILE_LIST,					//125 �����ļ��б�
	UK9011_MSG_RES_FILE_LIST,					//126 �����ļ��б�ظ�
	UK9011_MSG_REQ_FILE_READ,					//127 �����ļ���ȡ
	UK9011_MSG_RES_FILE_READ,					//128 �����ļ��б�ظ�
	UK9011_MSG_REQ_JOURNAL_VMD_SPEC,			//129 ������־ֵ��
	UK9011_MSG_RES_JOURNAL_VMD_SPEC,			//130 ������־ֵ���ظ�
	UK9011_MSG_REQ_JOURNAL_READ,				//131 ������־��ȡ
	UK9011_MSG_RES_JOURNAL_READ,				//132 ������־��ȡ�ظ�
	UK9011_MSG_REQ_JOURNAL_STATE_READ,			//133 ������־״̬
	UK9011_MSG_RES_JOURNAL_STATE_READ,			//134 ������־״̬�ظ�

	UK9011_MSG_SWITCH_TOPO_UPDATE,				//135 ���������˷����ı��֪ͨ
	UK9011_MSG_REQ_SMART_RTU104,				//136 ����Զ����һ�廯������ѯ
	UK9011_MSG_RES_SMART_RTU104,				//136 ����Զ����һ�廯���ûظ�
};


#pragma pack(push)
#pragma pack(1)



#pragma pack(4)
#pragma pack(pop)


class CUK9011Application : public SApplication
{
public:
	CUK9011Application();
	virtual ~CUK9011Application();

	//////////////////////////////////////////////////////////////////////////
	// ��    ��:  �������񣬿�������Ӧ�õķ��񣬸ú���������������ʵ�֣�������������ɺ���뷵��
	// ��    ��:  �ۿ���
	// ����ʱ��:  2015-7-30 16:05
	// ����˵��:  void
	// �� �� ֵ:  true/false
	//////////////////////////////////////////////////////////////////////////
	virtual bool Start();

	////////////////////////////////////////////////////////////////////////
	// ��    ��:  ������Ϣ����ӿڣ���������ʵ�֣����������뾡����С�����ٷ���
	// ��    ��:  �ۿ���
	// ����ʱ��:  2016-2-3 15:20
	// ����˵��:  @wMsgType��ʾ��Ϣ����
	//         :  @pMsgHeadΪ��Ϣͷ
	//         :  @sHeadStr��Ϣͷ�ַ���
	//         :  @pBufferΪ��ϢЯ���Ķ������������ݣ�NULL��ʾ�޶���������
	//         :  @iLengthΪ���������ݳ���
	// �� �� ֵ:  true��ʾ����ɹ���false��ʾ����ʧ��
	//////////////////////////////////////////////////////////////////////////
	virtual bool ProcessAgentMsg(WORD wMsgType,stuSpUnitAgentMsgHead *pMsgHead,SString &sHeadStr,BYTE* pBuffer=NULL,int iLength=0);

	//////////////////////////////////////////////////////////////////////////
	// ��    ��:  ��SQL�������������תΪ�ַ��������ֶ�
	// ��    ��:  �ۿ���
	// ����ʱ��:  2016-5-6 16:23
	// ����˵��:  @sFieldΪ�ֶ�����
	// �� �� ֵ:  ���ݲ�ͬ�����ݿ����Ͷ���ͬ
	//////////////////////////////////////////////////////////////////////////
	static SString Date2String(SString sField);

	//////////////////////////////////////////////////////////////////////////
	// ��    ��:  ��SQL������ַ�����Ϊ��������
	// ��    ��:  �ۿ���
	// ����ʱ��:  2016-5-6 16:25
	// ����˵��:  @sDateTime��ʽΪ:yyyy-MM-dd hh:mm:ss
	// �� �� ֵ:  ���ݲ�ͬ�����ݿ����Ͷ���ͬ
	//////////////////////////////////////////////////////////////////////////
	static SString String2Date(SString sDateTime);

	CUnitConfig m_UnitConfig;

};

#endif//__UK9010_APPLICATION_H__
