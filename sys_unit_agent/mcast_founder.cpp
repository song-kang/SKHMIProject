/**
 *
 * �� �� �� : sp_mcast_founder.cpp
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

#include "mcast_founder.h"
#include "SApplication.h"

CMCastFounder::CMCastFounder(SString sLocalIps)
{
	m_sLocalIps = sLocalIps;
	m_pRecvMSocket = NULL;
	m_bFounding = false;
	m_pUnitInfo = NULL;
	SString ips = SSocket::GetLocalIp().c_str();
	memset(&m_iFoundingSelfIps,0,sizeof(m_iFoundingSelfIps));
	if(ips.length() > 0)
	{
		int i,cnt = SString::GetAttributeCount(ips,",");

		//���������IP�������ҵ��Լ�
		//if(m_bWithoutSelf)
		{
			for(i=1;i<=cnt&&i<=8;i++)
			{
				m_iFoundingSelfIps[i-1] = SSocket::IpStrToIpv4(SString::GetIdAttribute(i,ips,",").data());
			}
			for(;i<=8;i++)
			{
				m_iFoundingSelfIps[i-1] = 0;
			}
		}
	}
}


CMCastFounder::~CMCastFounder()
{
	if(m_pRecvMSocket != NULL)
		delete[] m_pRecvMSocket;
}

bool CMCastFounder::Start()
{
	if(m_sLocalIps.length() == 0)
		m_sLocalIps = SSocket::GetLocalIp().c_str();
	if(m_sLocalIps.length() == 0)
	{
		LOGERROR("��δ���������鲥�ı���IP��ַ!");
		return false;
	}
	int ips = SString::GetAttributeCount(m_sLocalIps,",");
	if(m_pRecvMSocket != NULL)
		delete[] m_pRecvMSocket;
	m_pRecvMSocket = new SSocket[ips];
	m_iIps = ips;
	int i;
	SString ip;
	for(i=1;i<=ips;i++)
	{
		ip = SString::GetIdAttribute(i,m_sLocalIps,",");
		if(!m_pRecvMSocket[i-1].CreateMultiCast((char*)C_MCAST_FOUNDER_IP,C_MCAST_FOUNDER_PORT,(char*)ip.data()))
		{
			LOGERROR("���������鲥�׽���[%s]ʧ��!",ip.data());
			return false;
		}
		m_pRecvMSocket[i-1].SetTimeout(10,3000);
	}
	if(!SService::Start())
		return false;
	S_CREATE_THREAD(ThreadRecv,this);
	return true;
}

void* CMCastFounder::ThreadRecv(void* lp)
{
	CMCastFounder *pThis = (CMCastFounder*)lp;
	pThis->BeginThread();
	BYTE buf[1024];
	sockaddr_in in;
	int ret,i;
	bool bRecv;
	while(!pThis->IsQuit())
	{
		memset(buf,0,1024);
		bRecv = false;
		for(i=0;i<pThis->m_iIps;i++)
		{
			ret =  pThis->m_pRecvMSocket[i].CheckForRecv();
			if(ret == 1)
			{
				bRecv = true;
				ret = pThis->m_pRecvMSocket[i].RecvFrom(buf,1024,in);
				if(ret > 0)				
				{
					pThis->ProcessMcastMsg(&pThis->m_pRecvMSocket[i],buf,ret);
				}
			}
		}
		if(bRecv)
			SApi::UsSleep(1000);
		else
			SApi::UsSleep(500000);
	}

	pThis->EndThread();
	return NULL;
}

void CMCastFounder::ProcessMcastMsg(SSocket *pSkt, BYTE *pMsg, int iLen)
{
	//LOGDEBUG("MCast Recv(len=%d):%s",iLen,SString::HexToStr(pMsg,iLen,true,1000).data());
	if(iLen == sizeof(stuMCastPackageReq))
	{
		stuMCastPackageReq *pReq = (stuMCastPackageReq*)pMsg;
		pReq->Swap();
		if(pReq->length != iLen - 2 || pReq->length != pReq->asdu_len + 8)
			return;
		if(pReq->type != 1 || pReq->start_word != 0x5350)
			return;
		if(pReq->checksum != pReq->GetChkSum())
			return;
		//������Լ����ģ��Լ�����Ҫ�ظ�
		if(pReq->unit_id == SApplication::GetPtr()->m_iUnitId && memcmp(pReq->unit_ip,m_iFoundingSelfIps,sizeof(m_iFoundingSelfIps)) == 0)
			return;
		
		//׼�����ͱ���Ԫ��Ϣ
		stuMCastPackageRes res;
		memset(&res,0,sizeof(res));
		res.length = sizeof(res) - 2;
		res.start_word = 0x5350;
		res.type = 2;
		res.asdu_len = res.length - 8;
		if(m_pUnitConfig != NULL)
		{
			res.unit_info.unit_id = m_pUnitConfig->m_iUnitId;
			SString::strncpy(res.unit_info.soft_type,sizeof(res.unit_info),m_pUnitConfig->m_sSystemType.data());
			strcpy(res.unit_info.unit_name,m_pUnitConfig->m_sUnitName.data());
			strcpy(res.unit_info.unit_desc,m_pUnitConfig->m_sUnitDesc.data());
			int i,cnt = SString::GetAttributeCount(m_sLocalIps,",");
			for(i=1;i<=cnt&&i<=8;i++)
			{
				res.unit_info.unit_ip[i-1] = SSocket::IpStrToIpv4(SString::GetIdAttribute(i,m_sLocalIps,",").data());
			}
		}
		res.Swap();
		int ret = pSkt->SendTo((char*)C_MCAST_FOUNDER_IP,C_MCAST_FOUNDER_PORT,(void*)&res,sizeof(res));
		if(ret != sizeof(res))
		{
			LOGERROR("����Ӧ���鲥����ʱʧ��!");
			return;
		}
		pSkt->SendTo((char*)C_MCAST_FOUNDER_IP,C_MCAST_FOUNDER_PORT,(void*)&res,sizeof(res));
		pSkt->SendTo((char*)C_MCAST_FOUNDER_IP,C_MCAST_FOUNDER_PORT,(void*)&res,sizeof(res));
	}
	else if(iLen == sizeof(stuMCastPackageRes) && m_bFounding && m_pUnitInfo != NULL)
	{
		SPtrList<stuMCastUnitInfo> *pList = m_pUnitInfo;
		if(pList == NULL)
			return;
		stuMCastPackageRes *pRes = (stuMCastPackageRes*)pMsg;
		pRes->Swap();
		if(pRes->length != iLen - 2 || pRes->length != pRes->asdu_len + 8)
			return;
		if(pRes->type != 2 || pRes->start_word != 0x5350)
			return;
		if(pRes->checksum != pRes->GetChkSum())
			return;
		//���˵�����ڵ�
		if(m_bWithoutSelf)
		{
			if(memcmp(m_iFoundingSelfIps,pRes->unit_info.unit_ip,sizeof(m_iFoundingSelfIps)) == 0)
				return;//���Ե�����
		}
		bool bExist=false;
		unsigned long pos=0;
		stuMCastUnitInfo *p = pList->FetchFirst(pos);
		while(p)
		{
			if(memcmp(p,&pRes->unit_info,sizeof(stuMCastUnitInfo)) == 0)
			{
				bExist = true;
				break;
			}
			p = pList->FetchNext(pos);
		}
		if(bExist == false)
		{
			p = new stuMCastUnitInfo();
			memcpy(p,&pRes->unit_info,sizeof(stuMCastUnitInfo));
			pList->append(p);
		}
	}


}
//////////////////////////////////////////////////////////////////////////
// ��    ��:  ͨ������IP��ַ���ҵ�Ԫ
// ��    ��:  �ۿ���
// ����ʱ��:  2015-7-20 16:32
// ����˵��:  @resΪ���ҵ������е�Ԫ��Ϣ
// �� �� ֵ:  ���ҵ��ĵ�Ԫ����
//////////////////////////////////////////////////////////////////////////
int CMCastFounder::FindUnit(SPtrList<stuMCastUnitInfo> &units,bool bWithoutSelf/*=true*/)
{
	units.clear();
	units.setAutoDelete(true);
	stuMCastPackageReq req;
	SString ips = SSocket::GetLocalIp().c_str();
	if(ips.length() == 0)
		return -1;
	int i,cnt = SString::GetAttributeCount(ips,",");
	//memset(&m_iFoundingSelfIps,0,sizeof(m_iFoundingSelfIps));
	//���������IP�������ҵ��Լ�
	//if(m_bWithoutSelf)
	{
		for(i=1;i<=cnt&&i<=8;i++)
		{
			m_iFoundingSelfIps[i-1] = SSocket::IpStrToIpv4(SString::GetIdAttribute(i,ips,",").data());
		}
		for(;i<=8;i++)
		{
			m_iFoundingSelfIps[i-1] = 0;
		}
	}
	memset(&req,0,sizeof(req));
	req.length = sizeof(req) - 2;
	req.start_word = 0x5350;
	req.type = 1;
	req.asdu_len = req.length - 8;
	memcpy(req.unit_ip,m_iFoundingSelfIps,sizeof(m_iFoundingSelfIps));
	req.unit_id = SApplication::GetPtr()->m_iUnitId;
	m_pUnitInfo = &units;
	if(m_bFounding)
	{
		return -2;
	}
	m_bFounding = true;
	m_bWithoutSelf = bWithoutSelf;


	for(i=1;i<=cnt;i++)
	{
		SString ip = SString::GetIdAttribute(i,ips,",");
		SSocket skt;
		if(!skt.CreateMultiCast((char*)C_MCAST_FOUNDER_IP,C_MCAST_FOUNDER_PORT,(char*)ip.data(),false))
			continue;
		req.SetSrcIp(ip);
		req.Swap();
		skt.SendTo((char*)C_MCAST_FOUNDER_IP,C_MCAST_FOUNDER_PORT,(void*)&req,sizeof(req));
		skt.SendTo((char*)C_MCAST_FOUNDER_IP,C_MCAST_FOUNDER_PORT,(void*)&req,sizeof(req));
		skt.SendTo((char*)C_MCAST_FOUNDER_IP,C_MCAST_FOUNDER_PORT,(void*)&req,sizeof(req));
	}
	SApi::UsSleep(1000000);
	m_bFounding = false;
	m_pUnitInfo = NULL;
	return units.count();
}