/**
 *
 * �� �� �� : sp_mcast_founder.h
 * �������� : 2015-7-20 9:27
 * ��    �� : �ۿ���(skt001@163.com)
 * �޸����� : $Date: $
 * ��ǰ�汾 : $Revision: $
 * �������� : �����鲥�ĵ�Ԫ�Է��ֻ���ʵ��
 * �޸ļ�¼ : 
 *            $Log: $
 *
 * Ver  Date        Author  Comments
 * ---  ----------  ------  -------------------------------------------
 * 001	2015-7-20	�ۿ�������ļ�
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
	char soft_type[32];//����ͺ�
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
	// ��    ��:  ���ÿ����鲥̽��ı���IP��ַ����IP�ö��ŷָ���ȱʡ����ʱ��ʾ������б���IP��ַ
	// ��    ��:  �ۿ���
	// ����ʱ��:  2015-7-20 11:28
	// ����˵��:  @ipsΪ����IP��ַ����:192.168.0.1,192.168.1.1
	// �� �� ֵ:  void
	//////////////////////////////////////////////////////////////////////////
	void SetLocalIps(SString ips){m_sLocalIps = ips;};

	//////////////////////////////////////////////////////////////////////////
	// ��    ��:  ���õ�Ԫ����ָ��
	// ��    ��:  �ۿ���
	// ����ʱ��:  2015-7-21 11:58
	// ����˵��:  @pΪ��Ԫ����ָ��
	// �� �� ֵ:  void
	//////////////////////////////////////////////////////////////////////////
	void SetUnitConfig(CSKUnitconfig *p){m_pUnitConfig = p;};

	//////////////////////////////////////////////////////////////////////////
	// ��    ��:  ��������
	// ��    ��:  �ۿ���
	// ����ʱ��:  2015-7-20 11:30
	// ����˵��:  void
	// �� �� ֵ:  true/false
	//////////////////////////////////////////////////////////////////////////
	virtual bool Start();

	//////////////////////////////////////////////////////////////////////////
	// ��    ��:  ͨ������IP��ַ���ҵ�Ԫ
	// ��    ��:  �ۿ���
	// ����ʱ��:  2015-7-20 16:32
	// ����˵��:  @resΪ���ҵ������е�Ԫ��Ϣ
	//         :  @bWithoutSelf��ʾ�ǲ�������������
	// �� �� ֵ:  ���ҵ��ĵ�Ԫ����
	//////////////////////////////////////////////////////////////////////////
	int FindUnit(SPtrList<stuMCastUnitInfo> &units,bool bWithoutSelf=true);

	//////////////////////////////////////////////////////////////////////////
	// ��    ��:  ȡ��ǰ��Ԫ��IP��ַ���飬���8��IP������ʱ���鲹0
	// ��    ��:  �ۿ���
	// ����ʱ��:  2016-2-12 9:28
	// ����˵��:  void
	// �� �� ֵ:  DWORD*
	//////////////////////////////////////////////////////////////////////////
	DWORD* GetSelfIps(){return m_iFoundingSelfIps;};

private:
	//////////////////////////////////////////////////////////////////////////
	// ��    ��:  �鲥�����߳�
	// ��    ��:  �ۿ���
	// ����ʱ��:  2015-7-21 11:31
	// ����˵��:  @lpΪthis
	// �� �� ֵ:  NULL
	//////////////////////////////////////////////////////////////////////////
	static void* ThreadRecv(void* lp);

	//////////////////////////////////////////////////////////////////////////
	// ��    ��:  
	// ��    ��:  �ۿ���
	// ����ʱ��:  2015-7-21 11:32
	// ����˵��:  
	// �� �� ֵ:  
	//////////////////////////////////////////////////////////////////////////
	void ProcessMcastMsg(SSocket *pSkt, BYTE *pMsg, int iLen);

	SString m_sLocalIps;					//�����鲥����ı��ض˿�
	SSocket *m_pRecvMSocket;				//�鲥�����׽�������
	int m_iIps;								//�鲥�׽�������
	bool m_bFounding;						//�Ƿ����ڲ��ҵ�Ԫ״̬����ʱ�ȴ���������Ӧ����
	bool m_bWithoutSelf;					//�Ƿ񲻰�������
	DWORD m_iFoundingSelfIps[8];			//���һ�β��ҵ�����IP
	SPtrList<stuMCastUnitInfo> *m_pUnitInfo;//�ȴ����ĵ�Ԫ��Ϣ�б�ָ�룬NULLʱ��ʾ���Ƶȴ�״̬
	CSKUnitconfig *m_pUnitConfig;			//��Ԫ����ָ��
};


#endif//__SP_MCAST_FOUNDER_H__
