/**
 *
 * 文 件 名 : sp_mcast_founder.h
 * 创建日期 : 2015-7-20 9:27
 * 作    者 : 邵凯田(skt001@163.com)
 * 修改日期 : $Date: $
 * 当前版本 : $Revision: $
 * 功能描述 : 基于组播的单元自发现机制实现
 * 修改记录 : 
 *            $Log: $
 *
 * Ver  Date        Author  Comments
 * ---  ----------  ------  -------------------------------------------
 * 001	2015-7-20	邵凯田　创建文件
 *
 **/

#ifndef __SP_MCAST_FOUNDER_H__
#define __SP_MCAST_FOUNDER_H__

#include "SSocket.h"
#include "SApi.h"
#include "SService.h"
#include "sk_unitconfig.h"

#define C_MCAST_FOUNDER_IP "238.115.107.116"
#define C_MCAST_FOUNDER_PORT 6666

#pragma pack(push)  
#pragma pack(1)

struct stuMCastPackageReq
{
	WORD length;
	WORD start_word;
	WORD type;
	WORD asdu_len;
	DWORD unit_id;
	DWORD srcip;
	DWORD unit_ip[8];
	WORD checksum;
	void SetSrcIp(SString ip)
	{
		BYTE *p = (BYTE*)&srcip;
		p[0] = (BYTE)SString::GetIdAttributeI(4,ip,".");
		p[1] = (BYTE)SString::GetIdAttributeI(3,ip,".");
		p[2] = (BYTE)SString::GetIdAttributeI(2,ip,".");
		p[3] = (BYTE)SString::GetIdAttributeI(1,ip,".");
	}
	WORD GetChkSum()
	{
		WORD sum = 0;
		BYTE *p = (BYTE*)this;
		int cnt = sizeof(*this)-2;
		for(int i=0;i<cnt;i++)
			sum += p[i];
		return sum;
	}
	void Swap()
	{
		checksum = GetChkSum();
		SKT_SWAP_WORD(length);
		SKT_SWAP_WORD(start_word);
		SKT_SWAP_WORD(type);
		SKT_SWAP_WORD(asdu_len);
		SKT_SWAP_DWORD(unit_id);
		for(int i=0;i<8;i++)
		{
			SKT_SWAP_DWORD(unit_ip[i]);
		}
		SKT_SWAP_WORD(checksum);
	}
};

struct stuMCastUnitInfo
{
	DWORD unit_id;
	char soft_type[32];//软件型号
	char unit_name[32];
	char unit_desc[64];
	DWORD unit_ip[8];
};

struct stuMCastPackageRes
{
	WORD length;
	WORD start_word;
	WORD type;
	WORD asdu_len;
	stuMCastUnitInfo unit_info;
	WORD checksum;
	WORD GetChkSum()
	{
		WORD sum = 0;
		BYTE *p = (BYTE*)this;
		int cnt = sizeof(*this)-2;
		for(int i=0;i<cnt;i++)
			sum += p[i];
		return sum;
	}
	void Swap()
	{
		checksum = GetChkSum();
		SKT_SWAP_WORD(length);
		SKT_SWAP_WORD(start_word);
		SKT_SWAP_WORD(type);
		SKT_SWAP_WORD(asdu_len);
		SKT_SWAP_DWORD(unit_info.unit_id);
		SKT_SWAP_DWORD(unit_info.unit_ip[0]);
		SKT_SWAP_DWORD(unit_info.unit_ip[1]);
		SKT_SWAP_DWORD(unit_info.unit_ip[2]);
		SKT_SWAP_DWORD(unit_info.unit_ip[3]);
		SKT_SWAP_WORD(checksum);
	}
};

#pragma pack()
#pragma pack(pop)  

class CMCastFounder : public SService
{
public:
	CMCastFounder(SString sLocalIps="");
	virtual ~CMCastFounder();
	//////////////////////////////////////////////////////////////////////////
	// 描    述:  设置开启组播探测的本地IP地址，多IP用逗号分隔，缺省不设时表示针对所有本机IP地址
	// 作    者:  邵凯田
	// 创建时间:  2015-7-20 11:28
	// 参数说明:  @ips为本地IP地址，如:192.168.0.1,192.168.1.1
	// 返 回 值:  void
	//////////////////////////////////////////////////////////////////////////
	void SetLocalIps(SString ips){m_sLocalIps = ips;};

	//////////////////////////////////////////////////////////////////////////
	// 描    述:  设置单元配置指针
	// 作    者:  邵凯田
	// 创建时间:  2015-7-21 11:58
	// 参数说明:  @p为单元配置指针
	// 返 回 值:  void
	//////////////////////////////////////////////////////////////////////////
	void SetUnitConfig(CSKUnitconfig *p){m_pUnitConfig = p;};

	//////////////////////////////////////////////////////////////////////////
	// 描    述:  启动服务
	// 作    者:  邵凯田
	// 创建时间:  2015-7-20 11:30
	// 参数说明:  void
	// 返 回 值:  true/false
	//////////////////////////////////////////////////////////////////////////
	virtual bool Start();

	//////////////////////////////////////////////////////////////////////////
	// 描    述:  通过所有IP地址查找单元
	// 作    者:  邵凯田
	// 创建时间:  2015-7-20 16:32
	// 参数说明:  @res为查找到的所有单元信息
	//         :  @bWithoutSelf表示是不包含本机自身
	// 返 回 值:  查找到的单元数量
	//////////////////////////////////////////////////////////////////////////
	int FindUnit(SPtrList<stuMCastUnitInfo> &units,bool bWithoutSelf=true);

	//////////////////////////////////////////////////////////////////////////
	// 描    述:  取当前单元的IP地址数组，最多8个IP，不足时数组补0
	// 作    者:  邵凯田
	// 创建时间:  2016-2-12 9:28
	// 参数说明:  void
	// 返 回 值:  DWORD*
	//////////////////////////////////////////////////////////////////////////
	DWORD* GetSelfIps(){return m_iFoundingSelfIps;};

private:
	//////////////////////////////////////////////////////////////////////////
	// 描    述:  组播接收线程
	// 作    者:  邵凯田
	// 创建时间:  2015-7-21 11:31
	// 参数说明:  @lp为this
	// 返 回 值:  NULL
	//////////////////////////////////////////////////////////////////////////
	static void* ThreadRecv(void* lp);

	//////////////////////////////////////////////////////////////////////////
	// 描    述:  
	// 作    者:  邵凯田
	// 创建时间:  2015-7-21 11:32
	// 参数说明:  
	// 返 回 值:  
	//////////////////////////////////////////////////////////////////////////
	void ProcessMcastMsg(SSocket *pSkt, BYTE *pMsg, int iLen);

	SString m_sLocalIps;					//启动组播服务的本地端口
	SSocket *m_pRecvMSocket;				//组播服务套接字数组
	int m_iIps;								//组播套接字数量
	bool m_bFounding;						//是否正在查找单元状态，此时等待并处理响应报文
	bool m_bWithoutSelf;					//是否不包含自身
	DWORD m_iFoundingSelfIps[8];			//最后一次查找的自身IP
	SPtrList<stuMCastUnitInfo> *m_pUnitInfo;//等待到的单元信息列表指针，NULL时表示不牌等待状态
	CSKUnitconfig *m_pUnitConfig;			//单元配置指针
};


#endif//__SP_MCAST_FOUNDER_H__
