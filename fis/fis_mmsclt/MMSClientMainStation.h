#pragma once

#include "MMSClient.h"


// һ��ģ����ж�����ӣ�һ�����Ӷ�Ӧһ̨װ�á�
class MMSClientMainStation : public MMSClient
{
public:
	// Description: MMSClientMainStation
	// Author:      qianchenglin
	// Date:        2016-01-01
	// Parameter:   @mmscltApp[in]����ָ��
	//              @nodeNo[in]���нڵ�
	//              @substationNo[in]վ��
	//              @substationName[in]վ��
	// Return:      Return true if succeed. Otherwise return false
	MMSClientMainStation(MMSCLTApp *mmscltApp, int nodeNo, int substationNo, SString substationName);
	virtual ~MMSClientMainStation();

public:
	// Description: ��ʼ�����б�Ҫ��Ϣ
	// Author:      qianchenglin
	// Date:        2016-01-01
	// Parameter:   void
	// Return:      Return true if succeed. Otherwise return false
	virtual bool Init();


	// Description: ����reference·���ҵ������
	// Author:      �ۿ���
	// Date:        2016-01-01
	// Parameter:   @reference[in]reference·��
	// Return:      ����reference����δ�ҵ�����NULL	
	virtual t_reference * findReference(std::string reference);

protected:
	// Description: ����ͨ��ͨ��״̬
	// Author:      qianchenglin
	// Date:        2016-01-01
	// Parameter:   @state[in]0�Ͽ���1����
	// Return:      Return true if succeed. Otherwise return false
	virtual bool SetChannelState(int state);

	// Description: ����Entry ID
	// Author:      qianchenglin
	// Date:        2016-01-01
	// Parameter:   @rptInfo[in]������Ϣ�ṹ
	//              @entryId[in]��ĿID
	// Return:      void
	virtual void setEntryID(stuSMmsReportInfo *rptInfo,SString entryId);

private:
	// Description: ��ʼ�����в���
	// Author:      qianchenglin
	// Date:        2016-01-01
	// Parameter:   void
	// Return:      Return true if succeed. Otherwise return false
	bool InitParam();

	// Description: ��ʼ���ڵ�������IED
	// Author:      qianchenglin
	// Date:        2016-01-01
	// Parameter:   void
	// Return:      Return true if succeed. Otherwise return false
	bool InitIeds();

private:
	unsigned char m_reportID;		// ������ƿ�ID
	SString m_iedIPA;				// ��վģʽʹ��t_oe_module_param��IP
	SString m_iedIPB;
};