#ifndef __SK_CONFIG_MANAGER_H__
#define __SK_CONFIG_MANAGER_H__

#include "SApi.h"
#include "SList.h"
#include "SFile.h"
#include "SDateTime.h"
#include "sk_base_inc.h"

enum eSpConfigType
{
	SPCFG_UNKNOWN=0,		//δ֪�����ļ�
	SPCFG_LOG_CONFIG,		//��־�����ļ�
	SPCFG_UNIT_CONFIG,		//��Ԫ�����ļ�
	SPCFG_DATA_WINDOW,		//���ݴ��������ļ�
	SPCFG_DB_CONFIG,		//���ݿ������ļ�
	SPCFG_FUN_POINT,		//ϵͳ���ܵ������ļ�
	SPCFG_SVGLIB,			//SVGͼԪ�������ļ�
	SPCFG_SYN_SHIELD,		//�������ͬ�������ļ����ڡ�������ʽ��ͬ��

	SPCFG_USER,				//Ӧ�ò��Զ��������ļ���������ÿ������ۼ�
	SPCFG_USER2,			//Ӧ�ò��Զ��������ļ�
	SPCFG_USER3,			//Ӧ�ò��Զ��������ļ�
	SPCFG_USER4,			//Ӧ�ò��Զ��������ļ�
	SPCFG_USER5,			//Ӧ�ò��Զ��������ļ�
	SPCFG_USER6,			//Ӧ�ò��Զ��������ļ�
	SPCFG_USER7,			//Ӧ�ò��Զ��������ļ�
	SPCFG_USER8,			//Ӧ�ò��Զ��������ļ�
	SPCFG_USER9,			//Ӧ�ò��Զ��������ļ�
	SPCFG_USER10,			//Ӧ�ò��Զ��������ļ�

	SPCFG_MAX_SIZE=100,		//���֧�ֵ������ļ�����
};

//==================== CConfigBase =======================
class SK_BASE_EXPORT CConfigBase
{
public:
	friend class CConfigMgr;
	CConfigBase();
	virtual ~CConfigBase();

	void SetPathFile(SString sPathFile) {m_sPathFile = sPathFile;};
	inline SString GetPathFile() {return m_sPathFile;};

	void SetAutoReload(bool bAutoReload) {m_bAutoRelaod = bAutoReload;};
	inline bool GetAutoReload() {return m_bAutoRelaod;};

	virtual bool Load(SString sPathFile) {m_LastModifyTime = SFile::filetime(sPathFile);m_sPathFile = sPathFile;return true;};
	virtual bool ReLoad() {return Load(GetPathFile());};
	void SetConfigType(eSpConfigType type) {m_ConfigType = type;};
	inline eSpConfigType GetConfigType() {return m_ConfigType;};

private:
	SString m_sPathFile;//�����ļ�ȫ·����
	bool m_bAutoRelaod;//���ļ������仯ʱ�Ƿ��Զ����¼�������
	SDateTime m_LastModifyTime;//����޸�ʱ��
	eSpConfigType m_ConfigType;//�����ļ�����
};

//==================== CConfigMgr =======================
class SK_BASE_EXPORT CConfigMgr
{
public:
	CConfigMgr();
	virtual ~CConfigMgr();

	static CConfigMgr* GetPtr();
	static void SetPtr(CConfigMgr *ptr);
	static void SetReloadSeconds(int iSec);
	static void StartReload();
	static void Quit();
	static bool AddConfig(CConfigBase *pCfg);
	static void RemoveConfig(CConfigBase *pCfg,bool bAddtoRemovedList=false);
	static CConfigBase* GetConfig(eSpConfigType cfgType);

private:
	static void* ThreadReload(void* lp);

private:
	SPtrList<CConfigBase> m_Configs;//�����ļ��б�
	SPtrList<CConfigBase> m_RemovedConfigs;//׼��ɾ�������ļ��б�
	bool m_bQuit;//�Ƿ�׼���˳�
	int m_iReloadSecs;//���¼��ص�ɨ��ʱ�������룩��Ĭ��5��
	CConfigBase** m_ppConfig;//�����ļ���ʵ��ָ������
};

#endif//__SP_CONFIG_MANAGER_H__
