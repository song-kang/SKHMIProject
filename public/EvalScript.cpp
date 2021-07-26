/**
 *
 * �� �� �� : EvalScript.cpp
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

#include "EvalScript.h"
#include <math.h>
#include "../public/UK9010App.h"

//������չ����
static const SScriptParser::stuExtFunction g_nam_eval_ext_fun[] = {
	{"$��ʷ״̬��ͳ��",		CEvalScriptParser::ExtFun_HisStateCnt,		CEvalScriptParser::VAL_BOOL,	0,	"",	"ͳ����������������״̬����������"	},
	{"$��һ����ʷ״̬��",	CEvalScriptParser::ExtFun_NextHisState,		CEvalScriptParser::VAL_BOOL,	0,	"",	"ָ����һ����ʷ״̬����$��ʷ״̬��ͳ��()���õ�һ��ʹ�ã���ʾ�ƶ�����һ����¼����������bool(true��ʾ��¼���ڣ�false��ʾ��¼������)"	},

	{"$��ʷ�¼���ͳ��",		CEvalScriptParser::ExtFun_HisEventCnt,		CEvalScriptParser::VAL_BOOL,	0,	"",	"��ȡ��ǰ��վ������δ������¼�����¼"	},
	{"$��һ����ʷ�¼���",	CEvalScriptParser::ExtFun_NextHisEvent,		CEvalScriptParser::VAL_BOOL,	0,	"",	"ָ����һ����ʷ�¼�����$��ʷ�¼���ͳ��()���õ�һ��ʹ�ã���ʾ�ƶ�����һ����¼����������bool(true��ʾ��¼���ڣ�false��ʾ��¼������)"	},
	
	{"$��ǰδ����״̬��",	CEvalScriptParser::ExtFun_UnResumedStCnt,	CEvalScriptParser::VAL_BOOL,	0,	"",	"��ȡ��ǰ��վ������δ�����״̬����¼"	},
	{"$��һ��δ����״̬��",	CEvalScriptParser::ExtFun_NextUnResumeSt,	CEvalScriptParser::VAL_BOOL,	0,	"",	"ָ����һ��δ����״̬����$��ǰδ����״̬��()���õ�һ��ʹ�ã���ʾ�ƶ�����һ����¼����������bool(true��ʾ��¼���ڣ�false��ʾ��¼������)"	},
	{"$ȡ��ǰ����ͨ��״̬",	CEvalScriptParser::ExtFun_ResunCommSt,		CEvalScriptParser::VAL_INT,		0,	"",	"ȡ��ǰδ����ͨ��״̬�����������µ�ͨ��״̬��0��ʾͨ����·�쳣����������������·��1��ʾͨ����·�쳣����������������·��2��ʾͨ����·����"	},

	{"$�۵�ǰ�豸��",		CEvalScriptParser::ExtFun_CurrDevDeduct,	CEvalScriptParser::VAL_VOID,	2,	"float �۷�ֵ,string ��ѡ�۷�ԭ��",	"�Ե�ǰװ�ý��п۷�"	},//�������۷�ֵ(float),string �۷�ԭ��
	{"$�۵�ǰ��վ��",		CEvalScriptParser::ExtFun_CurrSubDeduct,	CEvalScriptParser::VAL_VOID,	2,	"float �۷�ֵ,string ��ѡ�۷�ԭ��",	"�Ե�ǰ��վ���п۷�"	},//�������۷�ֵ(float),string �۷�ԭ��

	{NULL,						NULL,									CEvalScriptParser::VAL_UNKNOWN,	0,	NULL,	NULL	}
};

//�ⲿ����
static const SScriptParser::stuExtVariant g_nam_eval_ext_var[] = {
	{"@��վ���",			SScriptParser::VAL_INT,			"",		""	},
	{"@�豸���",			SScriptParser::VAL_INT,			"",		""	},
	{"@�豸����",			SScriptParser::VAL_STRING,		"",		""	},
	{"@Э������",			SScriptParser::VAL_INT,			"",		""	},
	{"@��������",			SScriptParser::VAL_STRING,		"",		""	},
	{"@��Ϣ����",			SScriptParser::VAL_STRING,		"",		""	},
	{"@�쳣״̬����",		SScriptParser::VAL_STRING,		"",		""	},
	{"@��ʷ״̬������",		SScriptParser::VAL_INT,			"",		""	},
	{"@��ʷ�¼�������",		SScriptParser::VAL_INT,			"",		""	},
	{"@δ����״̬ʱ��",		SScriptParser::VAL_INT,			"",		""	},

	{NULL,					SScriptParser::VAL_VOID,		NULL,	NULL}
};


CEvalScriptParser::CEvalScriptParser()
{
	m_vSubNo.SetName("@��վ���");
	m_vDevSn.SetName("@�豸���");
	m_vDevName.SetName("@�豸����");
	m_vProtType.SetName("@Э������");
	m_vClName.SetName("@��������");
	m_vInfName.SetName("@��Ϣ����");
	m_vInfWarnDesc.SetName("@�쳣״̬����");
	m_vHisStAmount.SetName("@��ʷ״̬������");
	m_vHisEvtAmount.SetName("@��ʷ�¼�������");
	m_vUnStSecs.SetName("@δ����״̬ʱ��");
	
	m_vSubNo.SetType(VAL_INT);
	m_vDevSn.SetType(VAL_INT);
	m_vDevName.SetType(VAL_STRING);
	m_vProtType.SetType(VAL_INT);
	m_vClName.SetType(VAL_STRING);
	m_vInfName.SetType(VAL_STRING);
	m_vInfWarnDesc.SetType(VAL_STRING);
	m_vHisStAmount.SetType(VAL_INT);
	m_vHisEvtAmount.SetType(VAL_INT);
	m_vUnStSecs.SetType(VAL_INT);
	
	m_Devices.setAutoDelete(true);
	m_SubDeduct.setAutoDelete(true);

	int i,cnt;
	cnt = GetExtFunctionCount();
	for(i=0;i<cnt;i++)
	{
		const stuExtFunction *p = GetExtFunctionByIdx(i);
		this->RegisterFunction(p->sFunName,p->pFun,this);
	}
	RegisterInstVariant(&m_vSubNo);
	RegisterInstVariant(&m_vDevSn);
	RegisterInstVariant(&m_vDevName);
	RegisterInstVariant(&m_vProtType);
	RegisterInstVariant(&m_vClName);
	RegisterInstVariant(&m_vInfName);
	RegisterInstVariant(&m_vInfWarnDesc);
	RegisterInstVariant(&m_vHisStAmount);
	RegisterInstVariant(&m_vHisEvtAmount);
	RegisterInstVariant(&m_vUnStSecs);

}

CEvalScriptParser::~CEvalScriptParser()
{

}


//////////////////////////////////////////////////////////////////////////
// ��    ��:  ȡ��չ����������
// ��    ��:  �ۿ���
// ����ʱ��:  2016-12-9 10:52
// ����˵��:  void
// �� �� ֵ:  ������Ч��չ����������
//////////////////////////////////////////////////////////////////////////
int CEvalScriptParser::GetExtFunctionCount()
{
	return (sizeof(g_nam_eval_ext_fun)/sizeof(stuExtFunction))-1;
}

//////////////////////////////////////////////////////////////////////////
// ��    ��:  ȡָ����ŵ���չ����ָ��
// ��    ��:  �ۿ���
// ����ʱ��:  2016-12-9 10:53
// ����˵��:  @idxΪ��ţ���0��ʼ
// �� �� ֵ:  stuExtFunction*, NULL��ʾ������
//////////////////////////////////////////////////////////////////////////
const CEvalScriptParser::stuExtFunction* CEvalScriptParser::GetExtFunctionByIdx(int idx)
{
	if(idx < 0 || idx >= (sizeof(g_nam_eval_ext_fun)/sizeof(stuExtFunction))-1)
		return NULL;
	return &g_nam_eval_ext_fun[idx];
}


//////////////////////////////////////////////////////////////////////////
// ��    ��:  ȡ��չ���������� 
// ��    ��:  �ۿ���
// ����ʱ��:  2016-12-9 17:18
// ����˵��:  void
// �� �� ֵ:  ������Ч��չ����������
//////////////////////////////////////////////////////////////////////////
int CEvalScriptParser::GetExtVariantCount()
{
	return (sizeof(g_nam_eval_ext_var)/sizeof(stuExtVariant))-1;
}

//////////////////////////////////////////////////////////////////////////
// ��    ��:  ȡָ����ŵ���չ������ָ��
// ��    ��:  �ۿ���
// ����ʱ��:  2016-12-9 17:18
// ����˵��:  @idxΪ��ţ���0��ʼ
// �� �� ֵ:  stuExtVariant*�� NULL��ʾ������
//////////////////////////////////////////////////////////////////////////
const CEvalScriptParser::stuExtVariant* CEvalScriptParser::GetExtVariantByIdx(int idx)
{
	if(idx < 0 || idx >= (sizeof(g_nam_eval_ext_var)/sizeof(stuExtVariant))-1)
		return NULL;
	return &g_nam_eval_ext_var[idx];
}

//MEMO: ORACLEƽ̨�¿�������������SQL��ƴ�� [2016-12-12 11:18 �ۿ���]
#define MULTI_SQL_DYN_EXECUTE_BEGIN \
	SString multi_sql;

#define MULTI_SQL_DYN_EXECUTE \
	if(m_eDbType == DB_ORACLE) \
	{ \
		if(multi_sql.length() == 0) \
			multi_sql = "begin "; \
		multi_sql += sql + ";"; \
		if(multi_sql.length() > 65536) \
		{ \
		DB->Execute(C_SQL_ASYNC_RUN+multi_sql+"end;"); \
		multi_sql = ""; \
		} \
	}\
	else \
	{ \
		DB->Execute(C_SQL_ASYNC_RUN+sql); \
	}

#define MULTI_SQL_DYN_EXECUTE_END \
	if(m_eDbType == DB_ORACLE) \
	{ \
		if(multi_sql.length() > 0) \
		{ \
			DB->Execute(C_SQL_ASYNC_RUN+multi_sql+"end;"); \
			multi_sql = ""; \
		} \
	}


//////////////////////////////////////////////////////////////////////////
// ��    ��:  ȡָ���ֵ��Ӧ�ļ��޼���
// ��    ��:  �ۿ���
// ����ʱ��:  2016-5-12 16:20
// ����˵��:  @scoreΪ��ֵ0~100
//         :  @rsΪ���޼������ݼ�����Ӧ�ֶ�Ϊ:oh_level,score_from,score_to
// �� �� ֵ:  ��Ӧ��Ӧ�����oh_level��0��ʾδ�ҵ�
//////////////////////////////////////////////////////////////////////////
int CEvalScriptParser::GetOverhaulLevel(float score,SRecordset &rs)
{
	register SRecord *pRec;
	register int i,cnt = rs.GetRows();
	for(i=0;i<cnt;i++)
	{
		pRec = rs.GetRecord(i);
		if(score >= pRec->GetValueFloat(1) && score <= pRec->GetValueFloat(2))
			return pRec->GetValueInt(0);
	}
	return 0;
}


//////////////////////////////////////////////////////////////////////////
// ��    ��:  ��ʼָ����վ������
// ��    ��:  �ۿ���
// ����ʱ��:  2016-12-9 13:19
// ����˵��:  @subno����վ���
//         :  @eval_type��ʾ��������:1-5����,2-1Сʱ,3-1��
//         :  @base_soc��ʾ��׼ʱ���
// �� �� ֵ:  void
//////////////////////////////////////////////////////////////////////////
void CEvalScriptParser::StartSubStationEval(int subno,int eval_type,int base_soc)
{
	m_eDbType = GET_DB_CFG->GetMasterType();
	m_iSubNo = subno;
	m_iEvalType = eval_type;
	m_iBaseSoc = base_soc;
	m_fStationScore = 100.0f;
	m_rsHisEvent.clear();
	m_rsHisState.clear();
	m_Devices.clear();
	m_mapDevices.clear();
	m_rsDevice.clear();
	int soc,usec;
	SDateTime::getSystemTime(soc,usec);
	SScriptParser::stuValue RetValue;
	SString fun = "$��վ����";

// 	//�����������
// 	SString ins_t_nam_dev_real_score;
// 	SString ins_t_nam_dev_his_mscore;
// 	SString ins_t_nam_dev_his_hscore;
// 	SString ins_t_nam_dev_his_dscore;
// 	SString ins_t_nam_station_real_score;
// 	SString ins_t_nam_station_his_mscore;
// 	SString ins_t_nam_station_his_hscore;
// 	SString ins_t_nam_station_his_dscore;


	SRecordset m_rsDevOverhaulLevel;//�豸���޼�����
	SRecordset m_rsStationOverhaulLevel;//��վ���޼�����
	SRecordset m_rsDevOverhaulLevelCmg;//�豸���޼�����
	SRecordset m_rsStationOverhaulLevelCmg;//��վ���޼�����

	//���ؼ��޼���
	DB->Retrieve("select oh_level,score_from,score_to from t_nam_dev_overhaul_level order by score_from desc",m_rsDevOverhaulLevel);	
	DB->Retrieve("select oh_level,score_from,score_to from t_nam_station_overhaul_level order by score_from desc",m_rsStationOverhaulLevel);
	DB->Retrieve("select oh_level,score_from,score_to from t_nam_dev_overhaul_level_ex where usergroup='cmg' order by score_from desc",m_rsDevOverhaulLevelCmg);	
	DB->Retrieve("select oh_level,score_from,score_to from t_nam_station_overhaul_lvl_ex where usergroup='cmg' order by score_from desc",m_rsStationOverhaulLevelCmg);

	//�����豸��Ϣ
	SString sql;
	SString sTablePrex;
	SString sCalcName;
	switch(eval_type)
	{
	case 1:
		sTablePrex = "m";
		sCalcName = "5����";
		break;
	case 2:
		sTablePrex = "h";
		sCalcName = "1Сʱ";
		break;
	case 3:
		sTablePrex = "d";
		sCalcName = "1��";
		break;
	default:
		LOGERROR("��Ч����������:%d",eval_type);
		return ;
	}

	SString sBaseTime;//��׼ʱ��
	SString sFromTime;
	bool bAddHisRec;
	SDateTime dt = SDateTime::makeDateTime((time_t)m_iBaseSoc);
	sBaseTime = dt.toString("yyyy-MM-dd hh:mm:ss");
	LOGDEBUG("ִ��һ��%s����,��׼ʱ��:%s",sCalcName.data(),sBaseTime.data());
	SString sPrevDay;//��׼ʱ��
	SDateTime dt2 = SDateTime::makeDateTime((time_t)(m_iBaseSoc-3600*24));
	sPrevDay = dt2.toString("yyyy-MM-dd hh:mm:ss");


	sql.sprintf("select dev_sn,weight,%sscore,dev_cls,dev_id from v_nam_device where sub_no=%d",
		eval_type==1?"curr_":sTablePrex.data(),subno);
	DB->Retrieve(sql,m_rsDevice);
	int i,cnt = m_rsDevice.GetRows();
	stuDevice *pDev;
	SRecord *pRec;
	for(i=0;i<cnt;i++)
	{
		pRec = m_rsDevice.GetRecord(i);
		pDev = SearchDevice(pRec->GetValueInt(0));
		pDev->weight = pRec->GetValueFloat(1);
		pDev->prev_score = pRec->GetValueFloat(2);
		pDev->dev_cls = pRec->GetValueInt(3);
		pDev->dev_id = pRec->GetValueInt(4);
	}

	bool bRet = RunFunction(RetValue,fun);

	MULTI_SQL_DYN_EXECUTE_BEGIN;
	//�����豸�۷�
	unsigned long pos;
	int dev_sn,dev_cls,dev_id;
	float dev_curr_score,dev_new_score;
	float sub_new_score = 100.0f;
	sql.sprintf("select %sscore from t_nam_station_real_score where sub_no=%d",m_iEvalType==1?"":sTablePrex.data(),subno);
	float sub_curr_score = DB->SelectIntoF(sql);
	int oh_level,oh_level_cmg;
	pDev = m_Devices.FetchFirst(pos);
	while(pDev)
	{
		dev_sn = pDev->dev_sn;
		dev_cls = pDev->dev_cls;
		dev_id = pDev->dev_id;
		dev_new_score = pDev->score;
		dev_curr_score = pDev->prev_score;

		if(m_iEvalType == 2)//1Сʱ
		{
			if(m_pEvalMgr->m_bEvalToDb)
			{
				sql.sprintf("update t_nam_dev_real_score set hscore=%f where dev_sn=%d",dev_new_score,dev_sn);
				m_pEvalMgr->m_db_multi_sql.AddSql(sql);
			}
		}
		else if(m_iEvalType == 3)//1��
		{
			if(m_pEvalMgr->m_bEvalToDb)
			{
				sql.sprintf("update t_nam_dev_real_score set dscore=%f where dev_sn=%d",dev_new_score,dev_sn);
				m_pEvalMgr->m_db_multi_sql.AddSql(sql);
			}
		}
		bAddHisRec = true;
		if(fabs(dev_curr_score-dev_new_score) >= 0.001)
		{
			//�豸��ֵ�б仯
			//LOGDEBUG("��վ%d�豸%d-%d��%s���ֱ仯����%f���Ϊ%f",subno,dev_cls,dev_id,sCalcName.data(),dev_curr_score,dev_new_score);
			if(m_iEvalType == 1)
			{
				//����5���ӵļ���ʱ����ʵʱ��
				oh_level = GetOverhaulLevel(dev_new_score,m_rsDevOverhaulLevel);
				oh_level_cmg = GetOverhaulLevel(dev_new_score,m_rsDevOverhaulLevelCmg);
				if(dev_sn == 0)
				{
					dev_sn = DB->SelectIntoI("select max(dev_sn) from t_nam_dev_real_score")+1;
					m_pEvalMgr->m_Ins_nam_dev_real_score.AddInsertValues(SString::toFormat("%d,%d,%d,%d,%f,%d,%d",
						dev_sn,subno,dev_cls,dev_id,dev_new_score,oh_level,oh_level_cmg));

				}
				else
				{
					if(m_pEvalMgr->m_bEvalToDb)
					{
						sql.sprintf("update t_nam_dev_real_score set score=%f,oh_level=%d,oh_level_cmg=%d where dev_sn=%d",dev_new_score,oh_level,oh_level_cmg,dev_sn);
						m_pEvalMgr->m_db_multi_sql.AddSql(sql);
					}
				}
			}
		}
		else
		{
			//�豸��ֵδ�仯
			sql.sprintf("select %s from t_nam_dev_his_%sscore where dev_sn=%d",
				m_pEvalMgr->m_db_multi_sql.Date2String("max(rectime_from)").data(),sTablePrex.data(),dev_sn);
			sFromTime = DB->SelectInto(sql);
			//����һ�����л���¼
			if(sFromTime.length() == 0 || (dt.day()==1 && dt.hour()==0 && dt.minute()==0))//û���ҵ�����¼����ǰʱ��Ϊһ���µĵ�һ����
			{
				bAddHisRec = true;
			}
			else if(sFromTime > sBaseTime)
			{
				bAddHisRec = true;
				LOGWARN("�쳣%s��¼��dev_sn=%d,max_from=%s,base_time=%s",sCalcName.data(),dev_sn,sFromTime.data(),sBaseTime.data());
			}
			else
			{
				//���µ�ԭ��ʷ��¼��
				bAddHisRec = false;
				if(m_pEvalMgr->m_bEvalToDb)
				{
					sql.sprintf("update t_nam_dev_his_%sscore set rectime_to=%s where dev_sn=%d and rectime_from=%s",
						sTablePrex.data(),m_pEvalMgr->m_db_multi_sql.String2Date(sBaseTime).data(),dev_sn,m_pEvalMgr->m_db_multi_sql.String2Date(sFromTime).data());
					m_pEvalMgr->m_db_multi_sql.AddSql(sql);
				}
			}
		}
		if(bAddHisRec)
		{
			//д��ʷ����
			if(m_pEvalMgr->m_bEvalToDb)
			{
				sql.sprintf("dev_sn,rectime_from,rectime_to,score",
					dev_sn,m_pEvalMgr->m_db_multi_sql.String2Date(sBaseTime).data(),m_pEvalMgr->m_db_multi_sql.String2Date(sBaseTime).data(),dev_new_score);
				switch(sTablePrex.at(0))
				{
				case 'm':
					m_pEvalMgr->m_Ins_nam_dev_his_mscore.AddInsertValues(sql);
					break;
				case 'h':
					m_pEvalMgr->m_Ins_nam_dev_his_hscore.AddInsertValues(sql);
					break;
				case 'd':
					m_pEvalMgr->m_Ins_nam_dev_his_dscore.AddInsertValues(sql);
					m_pEvalMgr->m_Ins_nam_his_score_cnt.AddInsertValues(SString::toFormat("%d,2,3,%d,%s,%f,%f,%f",subno,dev_sn,m_pEvalMgr->m_Ins_nam_his_score_cnt.String2Date(sPrevDay).data(),dev_new_score,dev_new_score,dev_new_score));
					break;
				}
			}
		}

		//�豸��Ȩ���ۼ�
		sub_new_score -= (100.0f-dev_new_score) * pDev->weight;
		if(sub_new_score < 0)
			sub_new_score = 0.0f;
		pDev = m_Devices.FetchNext(pos);
	}

	if(m_iEvalType == 2)//1Сʱ
	{
		if(m_pEvalMgr->m_bEvalToDb)
		{
			sql.sprintf("update t_nam_station_real_score set hscore=%f where sub_no=%d",sub_new_score,subno);
			m_pEvalMgr->m_db_multi_sql.AddSql(sql);
		}
	}
	else if(m_iEvalType == 3)//1��
	{
		if(m_pEvalMgr->m_bEvalToDb)
		{
			sql.sprintf("update t_nam_station_real_score set dscore=%f where sub_no=%d",sub_new_score,subno);
			m_pEvalMgr->m_db_multi_sql.AddSql(sql);
		}
	}


	bAddHisRec = true;
	if(fabs(sub_curr_score-sub_new_score) >= 0.001)
	{
		if(m_iEvalType == 1)
		{
			//��վ��ֵ�б仯
			oh_level = GetOverhaulLevel(sub_new_score,m_rsStationOverhaulLevel);
			oh_level_cmg = GetOverhaulLevel(sub_new_score,m_rsStationOverhaulLevelCmg);
			LOGDEBUG("��վ%d��5�������ֱ仯����%f���Ϊ%f",subno,sub_curr_score,sub_new_score);
			if(pRec->GetValueInt(3) == 0)
			{
				m_pEvalMgr->m_Ins_nam_station_real_score.AddInsertValues(SString::toFormat("%d,%f,%d,%d",subno,sub_new_score,oh_level,oh_level_cmg));

			}
			else
			{
				if(m_pEvalMgr->m_bEvalToDb)
				{
					sql.sprintf("update t_nam_station_real_score set score=%f,oh_level=%d,oh_level_cmg=%d where sub_no=%d",sub_new_score,oh_level,oh_level_cmg,subno);
					m_pEvalMgr->m_db_multi_sql.AddSql(sql);
				}
			}
		}
	}
	else
	{
		//��վ��ֵδ�仯
		sql.sprintf("select %s from t_nam_station_his_%sscore where sub_no=%d",
			m_pEvalMgr->m_db_multi_sql.Date2String("max(rectime_from)").data(),sTablePrex.data(),subno);
		sFromTime = DB->SelectInto(sql);
		//����һ�����л���¼
		if(sFromTime.length() == 0 || (dt.day()==1 && dt.hour()==0 && dt.minute()==0))//û���ҵ�����¼����ǰʱ��Ϊһ���µĵ�һ����
		{
			bAddHisRec = true;
		}
		else
		{
			//��ֹ��ʷ������ʵʱ���£��ж�һ��
			sql.sprintf("select score from t_nam_station_his_%sscore where sub_no=%d and rectime_from=%s",
				sTablePrex.data(),subno,m_pEvalMgr->m_db_multi_sql.String2Date(sFromTime).data());
			float his_score = DB->SelectIntoF(sql);
			if(fabs(his_score-sub_new_score) >= 0.001)
			{
				//��ʷ������ʵʱ����
				bAddHisRec = true;
			}
			else
			{
				//���µ�ԭ��ʷ��¼��
				bAddHisRec = false;
				if(m_pEvalMgr->m_bEvalToDb)
				{
					sql.sprintf("update t_nam_station_his_%sscore set rectime_to=%s where sub_no=%d and rectime_from=%s",
						sTablePrex.data(),m_pEvalMgr->m_db_multi_sql.String2Date(sBaseTime).data(),subno,m_pEvalMgr->m_db_multi_sql.String2Date(sFromTime).data());
					m_pEvalMgr->m_db_multi_sql.AddSql(sql);
				}
			}
		}
	}
	if(bAddHisRec)
	{
		if(m_pEvalMgr->m_bEvalToDb)
		{
			sql.sprintf("%d,%s,%s,%f",
				subno,m_pEvalMgr->m_db_multi_sql.String2Date(sBaseTime).data(),m_pEvalMgr->m_db_multi_sql.String2Date(sBaseTime).data(),sub_new_score);
			switch(sTablePrex.at(0))
			{
			case 'm':
				m_pEvalMgr->m_Ins_nam_station_his_mscore.AddInsertValues(sql);
				break;
			case 'h':
				m_pEvalMgr->m_Ins_nam_station_his_hscore.AddInsertValues(sql);
				break;
			case 'd':
				m_pEvalMgr->m_Ins_nam_station_his_dscore.AddInsertValues(sql);
				m_pEvalMgr->m_Ins_nam_his_score_cnt.AddInsertValues(SString::toFormat("%d,1,3,%d,%s,%f,%f,%f",subno,subno,m_pEvalMgr->m_Ins_nam_his_score_cnt.String2Date(sPrevDay).data(),sub_new_score,sub_new_score,sub_new_score));
				break;
			}
		}
	}
	if(m_iEvalType == 3)//1��
	{
		m_pEvalMgr->m_ScoreCnt.DayCnt(subno,base_soc);
	}
	//��ǰ��վ�������
	m_pEvalMgr->m_Ins_nam_dev_his_deduct.CheckTimeOut();
	m_pEvalMgr->m_Ins_nam_station_his_deduct.CheckTimeOut();
	m_pEvalMgr->m_Ins_nam_dev_real_score.CheckTimeOut();
	m_pEvalMgr->m_Ins_nam_dev_his_mscore.CheckTimeOut();
	m_pEvalMgr->m_Ins_nam_dev_his_hscore.CheckTimeOut();
	m_pEvalMgr->m_Ins_nam_dev_his_dscore.CheckTimeOut();
	m_pEvalMgr->m_Ins_nam_station_real_score.CheckTimeOut();
	m_pEvalMgr->m_Ins_nam_station_his_mscore.CheckTimeOut();
	m_pEvalMgr->m_Ins_nam_station_his_hscore.CheckTimeOut();
	m_pEvalMgr->m_Ins_nam_station_his_dscore.CheckTimeOut();
	m_pEvalMgr->m_Ins_nam_his_score_cnt.CheckTimeOut();
	m_pEvalMgr->m_db_multi_sql.CheckTimeOut();

	int soc2,usec2;
	SDateTime::getSystemTime(soc2,usec2);
	LOGDEBUG("��վ(%d)���ַ���:%s,����ʱ:%f��!\n",subno,RetValue.GetValue().data(),(float)(soc2-soc)+(float)(usec2-usec)/1000000);
}

//////////////////////////////////////////////////////////////////////////
// ��    ��:  ��ǰ��վ����ʷ״̬��ͳ��
// ��    ��:  �ۿ���
// ����ʱ��:  2016-12-8 15:40
// ����˵��:  @pCbParam��ʾ�ص�����(ע��ʱָ����)
//         :  @ReturnVal��ʾ����ֵ��������
//         :  @pParamValues��ʽ�����б�ָ�룬NULL��ʾû���βΣ������������÷�ʽ
// �� �� ֵ:  true��ʾ���гɹ���false��ʾ����ʧ��
//////////////////////////////////////////////////////////////////////////
bool CEvalScriptParser::ExtFun_HisStateCnt(void *pCbParam,SScriptParser::stuValue &ReturnVal,SPtrList<SScriptParser::stuValue> *pParamValues)
{
	CEvalScriptParser *pThis = (CEvalScriptParser*)pCbParam;
	SString sql;
	int soc_from;
	int soc_to = pThis->m_iBaseSoc;
	if(pThis->m_iEvalType == 1)
		soc_from = soc_to-300;
	else if(pThis->m_iEvalType == 2)
		soc_from = soc_to-3600;
	else if(pThis->m_iEvalType == 3)
		soc_from = soc_to-3600*24;
	else
	{
		ReturnVal.Set(VAL_BOOL,"0");
		return false;
	}
	SString sFrom = SDateTime::makeDateTime((time_t)soc_from).toString("yyyy-MM-dd hh:mm:ss");
	SString sTo = SDateTime::makeDateTime((time_t)soc_to).toString("yyyy-MM-dd hh:mm:ss");
	LOGDEBUG("���ڼ�����վ%dͳ������[%s ~ %s]�ڵ���ʷ״̬����Ϣ...",
		pThis->m_iSubNo,sFrom.data(),sTo.data());
	sql.sprintf("select v.dev_sn,v.dev_name,v.cl_name,v.inf_name,v.off_desc,h.cnt,v.prot_type from ( "
		"select stat_id,count(*)+sum(repeat_cnt) as cnt from t_nam_his_element_state "
		"where sub_no=%d and val=0 and rectime>=%s and  rectime<%s "
		"group by stat_id) h "
		"left join v_nam_real_state v on h.stat_id=v.stat_id",
		pThis->m_iSubNo,pThis->m_pEvalMgr->m_db_multi_sql.String2Date(sFrom).data(),pThis->m_pEvalMgr->m_db_multi_sql.String2Date(sTo).data());
	DB->Retrieve(sql,pThis->m_rsHisState);
	LOGDEBUG("��վ%dͳ������[%s ~ %s]�ڵ���ʷ״̬������Ϊ%d",pThis->m_iSubNo,sFrom.data(),sTo.data(),pThis->m_rsHisState.GetRows());
	pThis->m_iCurrHisStateRow = -1;
	pThis->m_pCurrHisStateRow = NULL;
	ReturnVal.Set(VAL_BOOL,"1");
	return true;
}

//////////////////////////////////////////////////////////////////////////
// ��    ��:  ��ת����һ����ʷ״̬��
// ��    ��:  �ۿ���
// ����ʱ��:  2016-12-8 15:44
// ����˵��:  @pCbParam��ʾ�ص�����(ע��ʱָ����)
//         :  @ReturnVal��ʾ����ֵ��������
//         :  @pParamValues��ʽ�����б�ָ�룬NULL��ʾû���βΣ������������÷�ʽ
// �� �� ֵ:  true��ʾ���гɹ���false��ʾ����ʧ��
//////////////////////////////////////////////////////////////////////////
bool CEvalScriptParser::ExtFun_NextHisState(void *pCbParam,SScriptParser::stuValue &ReturnVal,SPtrList<SScriptParser::stuValue> *pParamValues)
{
	CEvalScriptParser *pThis = (CEvalScriptParser*)pCbParam;
	pThis->m_iCurrHisStateRow ++;
	if(pThis->m_iCurrHisStateRow >= pThis->m_rsHisState.GetRows())
	{
		pThis->m_sLastDeductCause = "";
		ReturnVal.Set(VAL_BOOL,"0");
		pThis->m_pCurrHisStateRow = NULL;
		return true;
	}
	SRecord *pRec = pThis->m_pCurrHisStateRow = pThis->m_rsHisState.GetRecord(pThis->m_iCurrHisStateRow);
	pThis->m_iDevSn = pRec->GetValueInt(0);
	pThis->m_vDevSn.SetValue(pRec->GetValueStr(0));
	pThis->m_vDevName.SetValue(pRec->GetValueStr(1));
	pThis->m_vClName.SetValue(pRec->GetValueStr(2));
	pThis->m_vInfName.SetValue(pRec->GetValueStr(3));
	pThis->m_vInfWarnDesc.SetValue(pRec->GetValueStr(4));
	pThis->m_vHisStAmount.SetValue(pRec->GetValueStr(5));
	pThis->m_vProtType.SetValue(pRec->GetValueStr(6));

	pThis->m_sLastDeductCause.sprintf("%s %s %s %s %s��",
		pThis->m_vDevName.GetValue().data(),
		pThis->m_vClName.GetValue().data(),
		pThis->m_vInfName.GetValue().data(),
		pThis->m_vInfWarnDesc.GetValue().data(),
		pThis->m_vHisEvtAmount.GetValue().data());

	ReturnVal.Set(VAL_BOOL,"1");
	return true;
}

//////////////////////////////////////////////////////////////////////////
// ��    ��:  ��ǰ��վ����ʷ�¼���ͳ��
// ��    ��:  �ۿ���
// ����ʱ��:  2016-12-8 15:40
// ����˵��:  @pCbParam��ʾ�ص�����(ע��ʱָ����)
//         :  @ReturnVal��ʾ����ֵ��������
//         :  @pParamValues��ʽ�����б�ָ�룬NULL��ʾû���βΣ������������÷�ʽ
// �� �� ֵ:  true��ʾ���гɹ���false��ʾ����ʧ��
//////////////////////////////////////////////////////////////////////////
bool CEvalScriptParser::ExtFun_HisEventCnt(void *pCbParam,SScriptParser::stuValue &ReturnVal,SPtrList<SScriptParser::stuValue> *pParamValues)
{
	CEvalScriptParser *pThis = (CEvalScriptParser*)pCbParam;
	SString sql;
	int soc_from;
	int soc_to = pThis->m_iBaseSoc;
	if(pThis->m_iEvalType == 1)
		soc_from = soc_to-300;
	else if(pThis->m_iEvalType == 2)
		soc_from = soc_to-3600;
	else if(pThis->m_iEvalType == 3)
		soc_from = soc_to-3600*24;
	else
	{
		ReturnVal.Set(VAL_BOOL,"0");
		return false;
	}
	SString sFrom = SDateTime::makeDateTime((time_t)soc_from).toString("yyyy-MM-dd hh:mm:ss");
	SString sTo = SDateTime::makeDateTime((time_t)soc_to).toString("yyyy-MM-dd hh:mm:ss");
	LOGDEBUG("���ڼ�����վ%dͳ������[%s ~ %s]�ڵ���ʷ�¼�����Ϣ...",
		pThis->m_iSubNo,sFrom.data(),sTo.data());
	sql.sprintf("select v.dev_sn,v.dev_name,v.cl_name,v.inf_name,v.off_desc,h.cnt,v.prot_type from ( "
		"select evt_id,count(*)+sum(repeat_cnt) as cnt from t_nam_his_element_event "
		"where sub_no=%d and val=0 and rectime>=%s and  rectime<%s "
		"group by evt_id) h "
		"left join v_nam_real_event v on h.evt_id=v.evt_id",
		pThis->m_iSubNo,pThis->m_pEvalMgr->m_db_multi_sql.String2Date(sFrom).data(),pThis->m_pEvalMgr->m_db_multi_sql.String2Date(sTo).data());
	DB->Retrieve(sql,pThis->m_rsHisEvent);
	LOGDEBUG("��վ%dͳ������[%s ~ %s]�ڵ���ʷ�¼�������Ϊ%d",pThis->m_iSubNo,sFrom.data(),sTo.data(),pThis->m_rsHisEvent.GetRows());
	pThis->m_iCurrHisEventRow = -1;
	pThis->m_pCurrHisEventRow = NULL;
	ReturnVal.Set(VAL_BOOL,"1");
	return true;
}

//////////////////////////////////////////////////////////////////////////
// ��    ��:  ��ת����һ����ʷ�¼���
// ��    ��:  �ۿ���
// ����ʱ��:  2016-12-8 15:45
// ����˵��:  @pCbParam��ʾ�ص�����(ע��ʱָ����)
//         :  @ReturnVal��ʾ����ֵ��������
//         :  @pParamValues��ʽ�����б�ָ�룬NULL��ʾû���βΣ������������÷�ʽ
// �� �� ֵ:  true��ʾ���гɹ���false��ʾ����ʧ��
//////////////////////////////////////////////////////////////////////////
bool CEvalScriptParser::ExtFun_NextHisEvent(void *pCbParam,SScriptParser::stuValue &ReturnVal,SPtrList<SScriptParser::stuValue> *pParamValues)
{
	CEvalScriptParser *pThis = (CEvalScriptParser*)pCbParam;
	pThis->m_iCurrHisEventRow ++;
	if(pThis->m_iCurrHisEventRow >= pThis->m_rsHisEvent.GetRows())
	{
		pThis->m_sLastDeductCause = "";
		ReturnVal.Set(VAL_BOOL,"0");
		pThis->m_pCurrHisEventRow = NULL;
		return true;
	}
	SRecord *pRec = pThis->m_pCurrHisEventRow = pThis->m_rsHisEvent.GetRecord(pThis->m_iCurrHisEventRow);
	pThis->m_iDevSn = pThis->m_pCurrHisEventRow->GetValueInt(0);
	pThis->m_vDevSn.SetValue(pRec->GetValueStr(0));
	pThis->m_vDevName.SetValue(pRec->GetValueStr(1));
	pThis->m_vClName.SetValue(pRec->GetValueStr(2));
	pThis->m_vInfName.SetValue(pRec->GetValueStr(3));
	pThis->m_vInfWarnDesc.SetValue(pRec->GetValueStr(4));
	pThis->m_vHisEvtAmount.SetValue(pRec->GetValueStr(5));
	pThis->m_vProtType.SetValue(pRec->GetValueStr(6));

	pThis->m_sLastDeductCause.sprintf("%s %s %s %s %s��",
		pThis->m_vDevName.GetValue().data(),
		pThis->m_vClName.GetValue().data(),
		pThis->m_vInfName.GetValue().data(),
		pThis->m_vInfWarnDesc.GetValue().data(),
		pThis->m_vHisEvtAmount.GetValue().data());
	ReturnVal.Set(VAL_BOOL,"1");
	return true;
}

//////////////////////////////////////////////////////////////////////////
// ��    ��:  ��ǰδ����״̬��
// ��    ��:  �ۿ���
// ����ʱ��:  2016-12-9 10:34
// ����˵��:  @pCbParam��ʾ�ص�����(ע��ʱָ����)
//         :  @ReturnVal��ʾ����ֵ��������
//         :  @pParamValues��ʽ�����б�ָ�룬NULL��ʾû���βΣ������������÷�ʽ
// �� �� ֵ:  true��ʾ���гɹ���false��ʾ����ʧ��
//////////////////////////////////////////////////////////////////////////
bool CEvalScriptParser::ExtFun_UnResumedStCnt(void *pCbParam,SScriptParser::stuValue &ReturnVal,SPtrList<SScriptParser::stuValue> *pParamValues)
{
	CEvalScriptParser *pThis = (CEvalScriptParser*)pCbParam;
	SString sql;	
	LOGDEBUG("���ڼ�����վ%dʵʱδ����״̬����Ϣ...",pThis->m_iSubNo);
	sql.sprintf("select dev_sn,dev_name,cl_name,inf_name,off_desc,soc,stat_id from v_nam_real_state where sub_no=%d and val=0 and soc>0",pThis->m_iSubNo);
	DB->Retrieve(sql,pThis->m_rsUnResumedState);
	LOGDEBUG("��վ%dʵʱδ����״̬������Ϊ%d",pThis->m_iSubNo,pThis->m_rsUnResumedState.GetRows());
	pThis->m_iCurrUnResumeStRow = -1;
	pThis->m_pCurrUnResumeStRow = NULL;
	ReturnVal.Set(VAL_BOOL,"1");
	return true;
}

//////////////////////////////////////////////////////////////////////////
// ��    ��:  ��ǰδ����״̬��
// ��    ��:  �ۿ���
// ����ʱ��:  2016-12-9 10:34
// ����˵��:  @pCbParam��ʾ�ص�����(ע��ʱָ����)
//         :  @ReturnVal��ʾ����ֵ��������
//         :  @pParamValues��ʽ�����б�ָ�룬NULL��ʾû���βΣ������������÷�ʽ
// �� �� ֵ:  true��ʾ���гɹ���false��ʾ����ʧ��
//////////////////////////////////////////////////////////////////////////
bool CEvalScriptParser::ExtFun_NextUnResumeSt(void *pCbParam,SScriptParser::stuValue &ReturnVal,SPtrList<SScriptParser::stuValue> *pParamValues)
{
	CEvalScriptParser *pThis = (CEvalScriptParser*)pCbParam;
	pThis->m_iCurrUnResumeStRow ++;
	if(pThis->m_iCurrUnResumeStRow >= pThis->m_rsUnResumedState.GetRows())
	{
		pThis->m_sLastDeductCause = "";
		ReturnVal.Set(VAL_BOOL,"0");
		pThis->m_pCurrUnResumeStRow = NULL;
		return true;
	}
	SRecord *pRec = pThis->m_pCurrUnResumeStRow = pThis->m_rsUnResumedState.GetRecord(pThis->m_iCurrUnResumeStRow);
	pThis->m_iDevSn = pThis->m_pCurrUnResumeStRow->GetValueInt(0);
	pThis->m_vDevSn.SetValue(pRec->GetValueStr(0));
	pThis->m_vDevName.SetValue(pRec->GetValueStr(1));
	pThis->m_vClName.SetValue(pRec->GetValueStr(2));
	pThis->m_vInfName.SetValue(pRec->GetValueStr(3));
	int secs = pThis->m_iBaseSoc-pRec->GetValueInt(5);
	pThis->m_iStatId = pRec->GetValueInt(6);
	pThis->m_vUnStSecs.SetValue(SString::toFormat("%d",secs).data());
	SString str_sec;
	if(secs >= 3600*24)
	{
		str_sec += SString::toFormat("%d��",secs/(3600*24));
		secs %= 3600*24;
	}
	if(secs >= 3600)
	{
		str_sec += SString::toFormat("%dʱ",secs/3600);
		secs %= 3600;
	}
	if(secs >= 60)
	{
		str_sec += SString::toFormat("%d��",secs/60);
		secs %= 60;
	}
	if(secs>0)
		str_sec += SString::toFormat("%d��",secs);
	pThis->m_vInfWarnDesc.SetValue(SString::toFormat("%s %s",pRec->GetValueStr(4),str_sec.data()).data());

	pThis->m_sLastDeductCause.sprintf("%s %s %s %s",
		pThis->m_vDevName.GetValue().data(),
		pThis->m_vClName.GetValue().data(),
		pThis->m_vInfName.GetValue().data(),
		pThis->m_vInfWarnDesc.GetValue().data());

	ReturnVal.Set(VAL_BOOL,"1");
	return true;
}

//////////////////////////////////////////////////////////////////////////
// ��    ��:  ȡ��ǰδ����ͨ��״̬������ͨ��״̬
// ��    ��:  �ۿ���
// ����ʱ��:  2017-2-7 10:05
// ����˵��:  @pCbParam��ʾ�ص�����(ע��ʱָ����)
//         :  @ReturnVal��ʾ����ֵ��������
//         :  @pParamValues��ʽ�����б�ָ�룬NULL��ʾû���βΣ������������÷�ʽ
// �� �� ֵ:  true��ʾ���гɹ���false��ʾ����ʧ��
//////////////////////////////////////////////////////////////////////////
bool CEvalScriptParser::ExtFun_ResunCommSt(void *pCbParam,SScriptParser::stuValue &ReturnVal,SPtrList<SScriptParser::stuValue> *pParamValues)
{
	CEvalScriptParser *pThis = (CEvalScriptParser*)pCbParam;
	SString sql;
	sql.sprintf("select f_nam_commlink_state(cl_id) from v_nam_real_state where stat_id=%d",pThis->m_iStatId);
	ReturnVal.Set(VAL_INT,DB->SelectInto(sql).data());
	return true;
}


//////////////////////////////////////////////////////////////////////////
// ��    ��:  ��ǰ�豸�۷�
// ��    ��:  �ۿ���
// ����ʱ��:  2016-12-9 10:34
// ����˵��:  @pCbParam��ʾ�ص�����(ע��ʱָ����)
//         :  @ReturnVal��ʾ����ֵ��������
//         :  @pParamValues��ʽ�����б�ָ�룬NULL��ʾû���βΣ������������÷�ʽ
// �� �� ֵ:  true��ʾ���гɹ���false��ʾ����ʧ��
//////////////////////////////////////////////////////////////////////////
bool CEvalScriptParser::ExtFun_CurrDevDeduct(void *pCbParam,SScriptParser::stuValue &ReturnVal,SPtrList<SScriptParser::stuValue> *pParamValues)
{
	CEvalScriptParser *pThis = (CEvalScriptParser*)pCbParam;
	stuValue *pScore = pParamValues->at(0);
	if(pScore == NULL)
		return false;
	float f = pScore->GetValue().toFloat();
	stuDevice *pDev = pThis->SearchDevice(pThis->m_iDevSn);
	pDev->score -= f;
	if(pDev->score < 0)
		pDev->score = 0;
	stuValue *pCause = pParamValues->at(1);
	if(pCause != NULL && pCause->val.length() > 0)
	{
		//���ڿ۷�ԭ����������ʷ�۷ּ�¼
		pThis->m_pEvalMgr->m_Ins_nam_dev_his_deduct.AddInsertValues(SString::toFormat("%d,%d,%d,%f,%d,'%s'",
			pThis->m_iDevSn,pThis->m_iEvalType,pThis->m_iBaseSoc-1,f,pThis->m_iStatId,pCause->GetValue().data()));
		pDev->NewDeduct(f,pCause->GetValue());
	}
	else
	{
		pDev->NewDeduct(f,pThis->m_sLastDeductCause);
	}

	return true;
}

//////////////////////////////////////////////////////////////////////////
// ��    ��:  ��ǰ��վ�۷�
// ��    ��:  �ۿ���
// ����ʱ��:  2016-12-9 10:34
// ����˵��:  @pCbParam��ʾ�ص�����(ע��ʱָ����)
//         :  @ReturnVal��ʾ����ֵ��������
//         :  @pParamValues��ʽ�����б�ָ�룬NULL��ʾû���βΣ������������÷�ʽ
// �� �� ֵ:  true��ʾ���гɹ���false��ʾ����ʧ��
//////////////////////////////////////////////////////////////////////////
bool CEvalScriptParser::ExtFun_CurrSubDeduct(void *pCbParam,SScriptParser::stuValue &ReturnVal,SPtrList<SScriptParser::stuValue> *pParamValues)
{
	CEvalScriptParser *pThis = (CEvalScriptParser*)pCbParam;
	stuValue *pScore = pParamValues->at(0);
	if(pScore == NULL)
		return false;
	
	float f = pScore->GetValue().toFloat();
	pThis->m_fStationScore -= f;
	if(pThis->m_fStationScore < 0)
	{
		pThis->m_fStationScore = 0;
	}
	stuValue *pCause = pParamValues->at(1);
	if(pCause != NULL && pCause->val.length() > 0)
	{
		//���ڿ۷�ԭ����������ʷ�۷ּ�¼
		pThis->m_pEvalMgr->m_Ins_nam_station_his_deduct.AddInsertValues(SString::toFormat("%d,%d,%d,%f,'%s'",
			pThis->m_iSubNo,pThis->m_iEvalType,pThis->m_iBaseSoc-1,f,pCause->GetValue().data()));
		pThis->NewDeduct(f,pCause->GetValue());
	}
	else
	{
		pThis->NewDeduct(f,pThis->m_sLastDeductCause);
	}
	return true;
}


CNamEvalMgr::CNamEvalMgr()
{
	m_bEvalToDb = true;
	for(int i=0;i<C_MAX_EVAL_THREADS;i++)
		m_Parser[i].m_pEvalMgr = this;
}

CNamEvalMgr::~CNamEvalMgr()
{

}


////////////////////////////////////////////////////////////////////////
// ��    ��:  ��������,�麯�������������б����ȵ��ô˺���
// ��    ��:  �ۿ���
// ����ʱ��:  2016-12-12 14:35
// ����˵��:  void
// �� �� ֵ:  true��ʾ���������ɹ�,false��ʾ��������ʧ��
//////////////////////////////////////////////////////////////////////////
bool CNamEvalMgr::Start()
{
	if(!SService::Start())
		return false;
	m_Ins_nam_dev_his_deduct.Init(DB,GET_DB_CFG->GetMasterType(),"t_nam_dev_his_deduct","dev_sn,score_type,soc,deduct_val,deduct_statid,deduct_cause",0);
	m_Ins_nam_station_his_deduct.Init(DB,GET_DB_CFG->GetMasterType(),"t_nam_station_his_deduct","sub_no,score_type,soc,deduct_val,deduct_cause",0);
	m_Ins_nam_dev_real_score.Init(DB,GET_DB_CFG->GetMasterType(),"t_nam_dev_real_score","dev_sn,sub_no,dev_cls,dev_id,score,oh_level,oh_level_cmg",0);
	m_Ins_nam_dev_his_mscore.Init(DB,GET_DB_CFG->GetMasterType(),"t_nam_dev_his_mscore","dev_sn,rectime_from,rectime_to,score",0);
	m_Ins_nam_dev_his_hscore.Init(DB,GET_DB_CFG->GetMasterType(),"t_nam_dev_his_hscore","dev_sn,rectime_from,rectime_to,score",0);
	m_Ins_nam_dev_his_dscore.Init(DB,GET_DB_CFG->GetMasterType(),"t_nam_dev_his_dscore","dev_sn,rectime_from,rectime_to,score",0);
	m_Ins_nam_station_real_score.Init(DB,GET_DB_CFG->GetMasterType(),"t_nam_station_real_score","sub_no,score,oh_level,oh_level_cmg",0);
	m_Ins_nam_station_his_mscore.Init(DB,GET_DB_CFG->GetMasterType(),"t_nam_station_his_mscore","sub_no,rectime_from,rectime_to,score",0);
	m_Ins_nam_station_his_hscore.Init(DB,GET_DB_CFG->GetMasterType(),"t_nam_station_his_hscore","sub_no,rectime_from,rectime_to,score",0);
	m_Ins_nam_station_his_dscore.Init(DB,GET_DB_CFG->GetMasterType(),"t_nam_station_his_dscore","sub_no,rectime_from,rectime_to,score",0);
	m_Ins_nam_his_score_cnt.Init(DB,GET_DB_CFG->GetMasterType(),"t_nam_his_score_cnt","sub_no,score_obj,score_type,obj_id,rectime,score,min_score,max_score",0);
	m_db_multi_sql.Init(DB,GET_DB_CFG->GetMasterType(),0);
	m_ScoreCnt.Init();
	SKT_CREATE_THREAD(ThreadWait,this);
	return true;
}

////////////////////////////////////////////////////////////////////////
// ��    ��:  ֹͣ�������������б����ȵ��ô˺���
// ��    ��:  �ۿ���
// ����ʱ��:  2016-12-12 14:35
// ����˵��:  void
// �� �� ֵ:  true��ʾ����ֹͣ�ɹ�,false��ʾ����ֹͣʧ��
//////////////////////////////////////////////////////////////////////////
bool CNamEvalMgr::Stop()
{
	if(!SService::Stop())
		return false;
	return true;
}


//////////////////////////////////////////////////////////////////////////
// ��    ��:  ��ʼһ����������
// ��    ��:  �ۿ���
// ����ʱ��:  2016-12-12 8:35
// ����˵��:  @sScriptTextΪ�ű�����ȫ�ı�
//         :  @eval_type��ʾ��������:1-5����,2-1Сʱ,3-1��
//         :  @base_soc��ʾ��׼ʱ���
// �� �� ֵ:  void
//////////////////////////////////////////////////////////////////////////
void CNamEvalMgr::BeginEval(SString &sScriptText,int eval_type,int base_soc)
{
	m_sScriptText = sScriptText;
	m_iEvalType = eval_type;
	m_iBaseSoc = base_soc;
	int soc = (int)SDateTime::getNowSoc();
	SString sql = "select sub_no from t_cim_substation order by sub_no";
	SRecordset rsSub;
	DB->Retrieve(sql,rsSub);
	int i,cnt = rsSub.GetRows();
	for(i=0;i<C_MAX_EVAL_THREADS;i++)
	{
		m_Param[i].pThis = this;
		m_Param[i].iThreadIdx = i;
		m_Param[i].sSubNos = "";
	}
	for(i=0;i<cnt;i++)
	{
		int sub_no = rsSub.GetValueInt(i,0);
		int idx = sub_no%C_MAX_EVAL_THREADS;
		if(m_Param[idx].sSubNos.length() > 0)
			m_Param[idx].sSubNos += ",";
		m_Param[idx].sSubNos += SString::toString(sub_no);		
	}
	for(i=0;i<C_MAX_EVAL_THREADS;i++)
	{
		SKT_CREATE_THREAD(ThreadEval,&m_Param[i]);
	}
	SApi::UsSleep(1000000);
	//�ȴ���һ�������������
	while(GetThreadCount() > 1)
		SApi::UsSleep(500000);

	int soc2 = (int)SDateTime::getNowSoc();
	LOGDEBUG("ϵͳ������������ʱ:%d��",soc2-soc);
}

//////////////////////////////////////////////////////////////////////////
// ��    ��:  ����ȴ��߳�
// ��    ��:  �ۿ���
// ����ʱ��:  2016-12-12 14:46
// ����˵��:  @lp,this
// �� �� ֵ:  NULL
//////////////////////////////////////////////////////////////////////////
void* CNamEvalMgr::ThreadWait(void *lp)
{
	CNamEvalMgr *pThis = (CNamEvalMgr*)lp;
	pThis->BeginThread();
	int last_min=-1,soc=0;
	SDateTime now;
	SString sScriptText;
	while(!pThis->IsQuit())
	{
		//�ȴ�5����
#if 0
		now = SDateTime::currentDateTime();
#else
		while(!pThis->IsQuit())
		{
			now = SDateTime::currentDateTime();
			if(last_min != now.minute() && now.minute()%5 == 0)
				break;
			SApi::UsSleep(1000000);
		}
#endif
		if(pThis->IsQuit())
			break;
		//5�����豸����վ����һ��
		soc = now.soc();
		last_min = now.minute();
		soc = (soc/300)*300 - 300;//ʱ����뵽5���Ӳ�ǰ��5���ӽ��д���

		//MEMO: ��ʱ�ӱ����ļ���ȡ�ű� [2016-12-12 16:56 �ۿ���]
		SFile f("c:\\nam.sc");
		sScriptText = "";
		if(f.open(IO_ReadOnly))
		{
			while(!f.atEnd())
			{
				sScriptText += f.readLine()+"\r\n";
			}
			f.close();
		}


		pThis->BeginEval(sScriptText,1,soc);

		if(soc % 3600 == 0)
		{
			//Сʱ����
			pThis->BeginEval(sScriptText,2,soc);

			SDateTime dt = SDateTime::makeDateTime((time_t)soc);
			if(dt.hour() == 0)
			{
				//������
				pThis->BeginEval(sScriptText,3,soc);
			}
		}
		//���ֽ���
	}
	pThis->EndThread();
	return NULL;
}


//////////////////////////////////////////////////////////////////////////
// ��    ��:  �����߳�
// ��    ��:  �ۿ���
// ����ʱ��:  2016-12-12 8:42
// ����˵��:  @lp,stuThreadParam*
// �� �� ֵ:  NULL
//////////////////////////////////////////////////////////////////////////
void* CNamEvalMgr::ThreadEval(void *lp)
{
	stuThreadParam *pParam = (stuThreadParam*)lp;
	pParam->pThis->BeginThread();
	CEvalScriptParser *pParser = &pParam->pThis->m_Parser[pParam->iThreadIdx];
	pParser->ClearFunctions();
	if(!pParser->LoadSyntaxText(pParam->pThis->m_sScriptText.data()))
	{
		LOGERROR("���������ű�ʱʧ��!");
		pParam->pThis->EndThread();
		return NULL;
	}
	if(pParam->sSubNos.length() == 0)
	{
		pParam->pThis->EndThread();
		return NULL;
	}
	int subno;
	int i,cnt = SString::GetAttributeCount(pParam->sSubNos,",");
	for(i=1;i<=cnt;i++)
	{
		subno = SString::GetIdAttributeI(i,pParam->sSubNos,",");
		pParser->StartSubStationEval(subno,pParam->pThis->m_iEvalType,pParam->pThis->m_iBaseSoc);

	}
	pParam->pThis->EndThread();
	return NULL;
}


