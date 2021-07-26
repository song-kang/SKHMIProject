/**
 *
 * �� �� �� : EvalScript.h
 * �������� : 2016-10-27 11:10
 * ��    �� : �ۿ���(skt001@163.com)
 * �޸����� : $Date: $
 * ��ǰ�汾 : $Revision: $
 * �������� : ��ά����ģ��-���������ͽű�ʵ��
 * �޸ļ�¼ : 
 *            $Log: $
 *
 * Ver  Date        Author  Comments
 * ---  ----------  ------  -------------------------------------------
 * 001	2016-10-27	�ۿ�������ļ�
 *
 **/

#ifndef __NAM_EVAL_SCRIPT_H__
#define __NAM_EVAL_SCRIPT_H__

#include "ssp_base.h"
#include "SScriptParser.h"
#include "../nam/nam_main/ScoreCnt.h"
#include <map>

class CNamEvalMgr;

//////////////////////////////////////////////////////////////////////////
// ��    ��:  CEvalScriptParser
// ��    ��:  �ۿ���
// ����ʱ��:  2016-10-27 11:16
// ��    ��:  ��ά�����ű��ⷭ������
//////////////////////////////////////////////////////////////////////////
class CEvalScriptParser : public SScriptParser
{
public:
	struct stuDeductInfo
	{
		float fDeductVal;//�۷�ֵ
		SString sDeductCause;//�۷�ԭ��
	};
	struct stuDevice
	{
		stuDevice()
		{
			deduct.setAutoDelete(true);
		}
		void NewDeduct(float fVal,SString str)
		{
			stuDeductInfo* pNew = new stuDeductInfo();
			pNew->fDeductVal = fVal;
			pNew->sDeductCause = str;
			deduct.append(pNew);
		}
		int dev_sn;
		int dev_cls;
		int dev_id;
		float score;//��ǰ����
		float prev_score;//��һ�������ڷ���
		float weight;//Ȩ��
		SPtrList<stuDeductInfo> deduct;
	};
	
	typedef map<int,stuDevice*> MAP_DEVICE;
	CEvalScriptParser();
	virtual ~CEvalScriptParser();

	//////////////////////////////////////////////////////////////////////////
	// ��    ��:  ȡָ���ֵ��Ӧ�ļ��޼���
	// ��    ��:  �ۿ���
	// ����ʱ��:  2016-5-12 16:20
	// ����˵��:  @scoreΪ��ֵ0~100
	//         :  @rsΪ���޼������ݼ�����Ӧ�ֶ�Ϊ:oh_level,score_from,score_to
	// �� �� ֵ:  ��Ӧ��Ӧ�����oh_level��0��ʾδ�ҵ�
	//////////////////////////////////////////////////////////////////////////
	int GetOverhaulLevel(float score,SRecordset &rs);

	//////////////////////////////////////////////////////////////////////////
	// ��    ��:  ��ʼָ����վ������
	// ��    ��:  �ۿ���
	// ����ʱ��:  2016-12-9 13:19
	// ����˵��:  @subno����վ���
	//         :  @eval_type��ʾ��������:1-5����,2-1Сʱ,3-1��
	//         :  @base_soc��ʾ��׼ʱ���
	// �� �� ֵ:  void
	//////////////////////////////////////////////////////////////////////////
	void StartSubStationEval(int subno,int eval_type,int base_soc);

	//////////////////////////////////////////////////////////////////////////
	// ��    ��:  ȡ��չ����������
	// ��    ��:  �ۿ���
	// ����ʱ��:  2016-12-9 10:52
	// ����˵��:  void
	// �� �� ֵ:  ������Ч��չ����������
	//////////////////////////////////////////////////////////////////////////
	static int GetExtFunctionCount();

	//////////////////////////////////////////////////////////////////////////
	// ��    ��:  ȡָ����ŵ���չ����ָ��
	// ��    ��:  �ۿ���
	// ����ʱ��:  2016-12-9 10:53
	// ����˵��:  @idxΪ��ţ���0��ʼ
	// �� �� ֵ:  stuExtFunction*, NULL��ʾ������
	//////////////////////////////////////////////////////////////////////////
	static const stuExtFunction* GetExtFunctionByIdx(int idx);

	//////////////////////////////////////////////////////////////////////////
	// ��    ��:  ȡ��չ���������� 
	// ��    ��:  �ۿ���
	// ����ʱ��:  2016-12-9 17:18
	// ����˵��:  void
	// �� �� ֵ:  ������Ч��չ����������
	//////////////////////////////////////////////////////////////////////////
	static int GetExtVariantCount();

	//////////////////////////////////////////////////////////////////////////
	// ��    ��:  ȡָ����ŵ���չ������ָ��
	// ��    ��:  �ۿ���
	// ����ʱ��:  2016-12-9 17:18
	// ����˵��:  @idxΪ��ţ���0��ʼ
	// �� �� ֵ:  stuExtVariant*�� NULL��ʾ������
	//////////////////////////////////////////////////////////////////////////
	static const stuExtVariant* GetExtVariantByIdx(int idx);

	//////////////////////////////////////////////////////////////////////////
	// ��    ��:  ��ǰ��վ����ʷ״̬��ͳ��
	// ��    ��:  �ۿ���
	// ����ʱ��:  2016-12-8 15:40
	// ����˵��:  @pCbParam��ʾ�ص�����(ע��ʱָ����)
	//         :  @ReturnVal��ʾ����ֵ��������
	//         :  @pParamValues��ʽ�����б�ָ�룬NULL��ʾû���βΣ������������÷�ʽ
	// �� �� ֵ:  true��ʾ���гɹ���false��ʾ����ʧ��
	//////////////////////////////////////////////////////////////////////////
	static bool ExtFun_HisStateCnt(void *pCbParam,SScriptParser::stuValue &ReturnVal,SPtrList<SScriptParser::stuValue> *pParamValues);


	//////////////////////////////////////////////////////////////////////////
	// ��    ��:  ��ǰ��վ����ʷ�¼���ͳ��
	// ��    ��:  �ۿ���
	// ����ʱ��:  2016-12-8 15:40
	// ����˵��:  @pCbParam��ʾ�ص�����(ע��ʱָ����)
	//         :  @ReturnVal��ʾ����ֵ��������
	//         :  @pParamValues��ʽ�����б�ָ�룬NULL��ʾû���βΣ������������÷�ʽ
	// �� �� ֵ:  true��ʾ���гɹ���false��ʾ����ʧ��
	//////////////////////////////////////////////////////////////////////////
	static bool ExtFun_HisEventCnt(void *pCbParam,SScriptParser::stuValue &ReturnVal,SPtrList<SScriptParser::stuValue> *pParamValues);

	//////////////////////////////////////////////////////////////////////////
	// ��    ��:  ��ת����һ����ʷ״̬��
	// ��    ��:  �ۿ���
	// ����ʱ��:  2016-12-8 15:44
	// ����˵��:  @pCbParam��ʾ�ص�����(ע��ʱָ����)
	//         :  @ReturnVal��ʾ����ֵ��������
	//         :  @pParamValues��ʽ�����б�ָ�룬NULL��ʾû���βΣ������������÷�ʽ
	// �� �� ֵ:  true��ʾ���гɹ���false��ʾ����ʧ��
	//////////////////////////////////////////////////////////////////////////
	static bool ExtFun_NextHisState(void *pCbParam,SScriptParser::stuValue &ReturnVal,SPtrList<SScriptParser::stuValue> *pParamValues);


	//////////////////////////////////////////////////////////////////////////
	// ��    ��:  ��ת����һ����ʷ�¼���
	// ��    ��:  �ۿ���
	// ����ʱ��:  2016-12-8 15:45
	// ����˵��:  @pCbParam��ʾ�ص�����(ע��ʱָ����)
	//         :  @ReturnVal��ʾ����ֵ��������
	//         :  @pParamValues��ʽ�����б�ָ�룬NULL��ʾû���βΣ������������÷�ʽ
	// �� �� ֵ:  true��ʾ���гɹ���false��ʾ����ʧ��
	//////////////////////////////////////////////////////////////////////////
	static bool ExtFun_NextHisEvent(void *pCbParam,SScriptParser::stuValue &ReturnVal,SPtrList<SScriptParser::stuValue> *pParamValues);

	
	//////////////////////////////////////////////////////////////////////////
	// ��    ��:  ��ǰδ����״̬��
	// ��    ��:  �ۿ���
	// ����ʱ��:  2016-12-9 10:34
	// ����˵��:  @pCbParam��ʾ�ص�����(ע��ʱָ����)
	//         :  @ReturnVal��ʾ����ֵ��������
	//         :  @pParamValues��ʽ�����б�ָ�룬NULL��ʾû���βΣ������������÷�ʽ
	// �� �� ֵ:  true��ʾ���гɹ���false��ʾ����ʧ��
	//////////////////////////////////////////////////////////////////////////
	static bool ExtFun_UnResumedStCnt(void *pCbParam,SScriptParser::stuValue &ReturnVal,SPtrList<SScriptParser::stuValue> *pParamValues);

	//////////////////////////////////////////////////////////////////////////
	// ��    ��:  ��ǰδ����״̬��
	// ��    ��:  �ۿ���
	// ����ʱ��:  2016-12-9 10:34
	// ����˵��:  @pCbParam��ʾ�ص�����(ע��ʱָ����)
	//         :  @ReturnVal��ʾ����ֵ��������
	//         :  @pParamValues��ʽ�����б�ָ�룬NULL��ʾû���βΣ������������÷�ʽ
	// �� �� ֵ:  true��ʾ���гɹ���false��ʾ����ʧ��
	//////////////////////////////////////////////////////////////////////////
	static bool ExtFun_NextUnResumeSt(void *pCbParam,SScriptParser::stuValue &ReturnVal,SPtrList<SScriptParser::stuValue> *pParamValues);

	//////////////////////////////////////////////////////////////////////////
	// ��    ��:  ȡ��ǰδ����ͨ��״̬������ͨ��״̬
	// ��    ��:  �ۿ���
	// ����ʱ��:  2017-2-7 10:05
	// ����˵��:  @pCbParam��ʾ�ص�����(ע��ʱָ����)
	//         :  @ReturnVal��ʾ����ֵ��������
	//         :  @pParamValues��ʽ�����б�ָ�룬NULL��ʾû���βΣ������������÷�ʽ
	// �� �� ֵ:  true��ʾ���гɹ���false��ʾ����ʧ��
	//////////////////////////////////////////////////////////////////////////
	static bool ExtFun_ResunCommSt(void *pCbParam,SScriptParser::stuValue &ReturnVal,SPtrList<SScriptParser::stuValue> *pParamValues);

	//////////////////////////////////////////////////////////////////////////
	// ��    ��:  ��ǰ�豸�۷�
	// ��    ��:  �ۿ���
	// ����ʱ��:  2016-12-9 10:34
	// ����˵��:  @pCbParam��ʾ�ص�����(ע��ʱָ����)
	//         :  @ReturnVal��ʾ����ֵ��������
	//         :  @pParamValues��ʽ�����б�ָ�룬NULL��ʾû���βΣ������������÷�ʽ
	// �� �� ֵ:  true��ʾ���гɹ���false��ʾ����ʧ��
	//////////////////////////////////////////////////////////////////////////
	static bool ExtFun_CurrDevDeduct(void *pCbParam,SScriptParser::stuValue &ReturnVal,SPtrList<SScriptParser::stuValue> *pParamValues);

	//////////////////////////////////////////////////////////////////////////
	// ��    ��:  ��ǰ��վ�۷�
	// ��    ��:  �ۿ���
	// ����ʱ��:  2016-12-9 10:34
	// ����˵��:  @pCbParam��ʾ�ص�����(ע��ʱָ����)
	//         :  @ReturnVal��ʾ����ֵ��������
	//         :  @pParamValues��ʽ�����б�ָ�룬NULL��ʾû���βΣ������������÷�ʽ
	// �� �� ֵ:  true��ʾ���гɹ���false��ʾ����ʧ��
	//////////////////////////////////////////////////////////////////////////
	static bool ExtFun_CurrSubDeduct(void *pCbParam,SScriptParser::stuValue &ReturnVal,SPtrList<SScriptParser::stuValue> *pParamValues);

public:
	CNamEvalMgr *m_pEvalMgr;
	float m_fStationScore;

protected:

	inline stuDevice* SearchDevice(int dev_sn)
	{
		MAP_DEVICE::iterator it = m_mapDevices.find(dev_sn);
		if(it == m_mapDevices.end())
		{
			stuDevice *p = new stuDevice();
			p->dev_sn = dev_sn;
			p->score = 100.0f;
			m_Devices.append(p);
			m_mapDevices[dev_sn] = p;
			return p;
		}
		return it->second;
	}

	void NewDeduct(float fVal,SString str)
	{
		stuDeductInfo* pNew = new stuDeductInfo();
		pNew->fDeductVal = fVal;
		pNew->sDeductCause = str;
		m_SubDeduct.append(pNew);
	}


	eSsp_Database_Type m_eDbType;//���ݿ�����
	int m_iSubNo;
	int m_iDevSn;
	int m_iStatId;
	int m_iEvalType;
	int m_iBaseSoc;
	SRecordset m_rsDevice;//��ǰ��վ�������豸����Ϣ���ϴο۷�
	SRecordset m_rsHisState,m_rsHisEvent;//dev_sn,dev_name,cl_name,inf_name,his_cnt,prot_type
	SRecordset m_rsUnResumedState;//δ����ʵʱ״̬��
	int m_iCurrHisStateRow,m_iCurrHisEventRow;
	SRecord *m_pCurrHisStateRow,*m_pCurrHisEventRow;
	int m_iCurrUnResumeStRow;
	SRecord *m_pCurrUnResumeStRow;
	stuValue m_vSubNo,m_vDevSn,m_vDevName,m_vProtType,m_vClName,m_vInfName,m_vInfWarnDesc;
	stuValue m_vHisStAmount,m_vHisEvtAmount;
	stuValue m_vUnStSecs;
	MAP_DEVICE m_mapDevices;
	SPtrList<stuDevice> m_Devices;
	SString m_sLastDeductCause;//���һ���쳣״̬���¼�����Խ�޿۷�ԭ��
	SPtrList<stuDeductInfo> m_SubDeduct;//��վ�۷�����
// 	SString m_sSubHisDeduct;//��վ��ʷ�۷ֱ�������ʷ�¼���״̬����
// 	SString m_sDevHisDeduct;//�豸��ʷ�۷ֱ�������ʷ�¼���״̬����

};

#define C_MAX_EVAL_THREADS 8
class CNamEvalMgr : public SService
{
public:
	struct stuThreadParam
	{
		CNamEvalMgr *pThis;
		int iThreadIdx;
		SString sSubNos;//���ŷָ��Ķ����վ���
	};
	CNamEvalMgr();
	~CNamEvalMgr();


	////////////////////////////////////////////////////////////////////////
	// ��    ��:  ��������,�麯�������������б����ȵ��ô˺���
	// ��    ��:  �ۿ���
	// ����ʱ��:  2016-12-12 14:35
	// ����˵��:  void
	// �� �� ֵ:  true��ʾ���������ɹ�,false��ʾ��������ʧ��
	//////////////////////////////////////////////////////////////////////////
	virtual bool Start();

	////////////////////////////////////////////////////////////////////////
	// ��    ��:  ֹͣ�������������б����ȵ��ô˺���
	// ��    ��:  �ۿ���
	// ����ʱ��:  2016-12-12 14:35
	// ����˵��:  void
	// �� �� ֵ:  true��ʾ����ֹͣ�ɹ�,false��ʾ����ֹͣʧ��
	//////////////////////////////////////////////////////////////////////////
	virtual bool Stop();


	//////////////////////////////////////////////////////////////////////////
	// ��    ��:  ��ʼһ����������
	// ��    ��:  �ۿ���
	// ����ʱ��:  2016-12-12 8:35
	// ����˵��:  @sScriptTextΪ�ű�����ȫ�ı�
	//         :  @eval_type��ʾ��������:1-5����,2-1Сʱ,3-1��
	//         :  @base_soc��ʾ��׼ʱ���
	// �� �� ֵ:  void
	//////////////////////////////////////////////////////////////////////////
	void BeginEval(SString &sScriptText,int eval_type,int base_soc);

	void SetEvalToDb(bool bEvalToDb)
	{
		m_bEvalToDb = bEvalToDb;
	}
private:
	//////////////////////////////////////////////////////////////////////////
	// ��    ��:  ����ȴ��߳�
	// ��    ��:  �ۿ���
	// ����ʱ��:  2016-12-12 14:46
	// ����˵��:  @lp,this
	// �� �� ֵ:  NULL
	//////////////////////////////////////////////////////////////////////////
	static void* ThreadWait(void *lp);

	//////////////////////////////////////////////////////////////////////////
	// ��    ��:  �����߳�
	// ��    ��:  �ۿ���
	// ����ʱ��:  2016-12-12 8:42
	// ����˵��:  @lp,stuThreadParam*
	// �� �� ֵ:  NULL
	//////////////////////////////////////////////////////////////////////////
	static void* ThreadEval(void *lp);

	CEvalScriptParser m_Parser[C_MAX_EVAL_THREADS];
	stuThreadParam m_Param[C_MAX_EVAL_THREADS];
	SString m_sScriptText;
	int m_iEvalType;
	int m_iBaseSoc;

public:
	CSsp_BatchDmlInsert m_Ins_nam_dev_his_deduct;
	CSsp_BatchDmlInsert m_Ins_nam_station_his_deduct;
	CSsp_BatchDmlInsert m_Ins_nam_dev_real_score;
	CSsp_BatchDmlInsert m_Ins_nam_dev_his_mscore;
	CSsp_BatchDmlInsert m_Ins_nam_dev_his_hscore;
	CSsp_BatchDmlInsert m_Ins_nam_dev_his_dscore;
	CSsp_BatchDmlInsert m_Ins_nam_station_real_score;
	CSsp_BatchDmlInsert m_Ins_nam_station_his_mscore;
	CSsp_BatchDmlInsert m_Ins_nam_station_his_hscore;
	CSsp_BatchDmlInsert m_Ins_nam_station_his_dscore;
	CSsp_BatchDmlInsert m_Ins_nam_his_score_cnt;
	CSsp_BatchDml m_db_multi_sql;
	bool m_bEvalToDb;//�Ƿ���������洢�����ݣ�true��ʾ�������ݿ⣬false��ʾ�����ڴ�ṹ
	CNamScoreCnt m_ScoreCnt;
};

#endif//__NAM_EVAL_SCRIPT_H__
