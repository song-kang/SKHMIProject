#ifndef __SK_CONFIG_MANAGER_H__
#define __SK_CONFIG_MANAGER_H__

#include "SApi.h"
#include "SList.h"
#include "SFile.h"
#include "SDateTime.h"
#include "sk_base_inc.h"

enum eSpConfigType
{
	SPCFG_UNKNOWN=0,		//未知配置文件
	SPCFG_LOG_CONFIG,		//日志配置文件
	SPCFG_UNIT_CONFIG,		//单元配置文件
	SPCFG_DATA_WINDOW,		//数据窗口配置文件
	SPCFG_DB_CONFIG,		//数据库配置文件
	SPCFG_FUN_POINT,		//系统功能点配置文件
	SPCFG_SVGLIB,			//SVG图元库配置文件
	SPCFG_SYN_SHIELD,		//正向隔离同步配置文件（内、外网格式相同）

	SPCFG_USER,				//应用层自定义配置文件，多个配置可依此累加
	SPCFG_USER2,			//应用层自定义配置文件
	SPCFG_USER3,			//应用层自定义配置文件
	SPCFG_USER4,			//应用层自定义配置文件
	SPCFG_USER5,			//应用层自定义配置文件
	SPCFG_USER6,			//应用层自定义配置文件
	SPCFG_USER7,			//应用层自定义配置文件
	SPCFG_USER8,			//应用层自定义配置文件
	SPCFG_USER9,			//应用层自定义配置文件
	SPCFG_USER10,			//应用层自定义配置文件

	SPCFG_MAX_SIZE=100,		//最大支持的配置文件数量
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
	SString m_sPathFile;//配置文件全路径名
	bool m_bAutoRelaod;//当文件发生变化时是否自动重新加载配置
	SDateTime m_LastModifyTime;//最后修改时间
	eSpConfigType m_ConfigType;//配置文件类型
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
	SPtrList<CConfigBase> m_Configs;//配置文件列表
	SPtrList<CConfigBase> m_RemovedConfigs;//准备删除配置文件列表
	bool m_bQuit;//是否准备退出
	int m_iReloadSecs;//重新加载的扫描时间间隔（秒），默认5秒
	CConfigBase** m_ppConfig;//配置文件的实例指针数组
};

#endif//__SP_CONFIG_MANAGER_H__
