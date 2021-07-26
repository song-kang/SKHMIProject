#pragma once

#include "MMSClient.h"


// һ��ģ����ж�����ӣ�һ�����Ӷ�Ӧһ̨װ�á�
class MMSClientSubStation : public MMSClient
{
public:
	MMSClientSubStation(MMSCLTApp *mmscltApp, int nodeNo, int substationNo, SString substationName, int iedNo, SString iedDesc, SString iedName);
	virtual ~MMSClientSubStation();

public:
	virtual bool Start();

	// Description: ��ʼ�����б�Ҫ��Ϣ
	// Author:      qianchenglin
	// Date:        2016-01-01
	// Parameter:   void
	// Return:      Return true if succeed. Otherwise return false
	virtual bool Init();

	// Description: ����reference·���ҵ������
	// Author:      qianchenglin
	// Date:        2016-01-01
	// Parameter:   @reference[in]reference·��
	// Return:      ����reference����δ�ҵ�����NULL
	virtual t_reference *findReference(std::string reference);

	// Description: ���ģ����ͨ��������м����е�ģ����ͨ�������·���ֵ�ٻ���Ȼ��Է���ֵ���жԱ�
	// Author:      qianchenglin
	// Date:        2016-01-01
	// Parameter:   void
	// Return:      Return true if succeed. Otherwise return false
	//virtual bool checkAnalogChannel();

	Ied *getIed();

protected:
	// Description: ����Entry ID
	// Author:      qianchenglin
	// Date:        2016-01-01
	// Parameter:   @rptInfo[in]������Ϣ�ṹ
	//              @entryId[in]��ĿID
	// Return:      void
	virtual void setEntryID(stuSMmsReportInfo *rptInfo,SString entryId);

	// Description: ����ͨ��Aͨ��״̬
	// Author:      qianchenglin
	// Date:        2016-01-01
	// Parameter:   @state[in]0�Ͽ���1����
	// Return:      Return true if succeed. Otherwise return false
	virtual bool SetChannelStateA(int state);

	// Description: ����ͨ��Bͨ��״̬
	// Author:      qianchenglin
	// Date:        2016-01-01
	// Parameter:   @state[in]0�Ͽ���1����
	// Return:      Return true if succeed. Otherwise return false
	virtual bool SetChannelStateB(int state);

private:
	unsigned char m_reportID;		// ������ƿ�ID
};