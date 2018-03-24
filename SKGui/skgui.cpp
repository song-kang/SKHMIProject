#include "skgui.h"
#include "cinitwidget.h"
#include "sk_unitconfig.h"
#include "cpluginmgr.h"

#define HMI_NAME	"UK9000 HMI"
#define HMI_VERSION	"1.0.0"

SKGui g_gui;
SKGui::SKGui()
{
	Init();
}

SKGui::~SKGui()
{
	if (w)
		delete ((SKBaseWidget*)w);
}

SKGui* SKGui::GetPtr()
{
	return &g_gui;
}

void SKGui::Init()
{
	m_pPluginMgr = new CPluginMgr;
	m_pListBaseView = new QList<CBaseView*>;

	SetModuleDesc(m_sHmiName = HMI_NAME);
	SetVersion(m_sHmiVersion = HMI_VERSION);
}

bool SKGui::Begin()
{
	CInitWidget *wiget = new CInitWidget();
	if (wiget)
	{
		w = new SKBaseWidget(NULL,wiget);
		((SKBaseWidget*)w)->SetWindowFixSize(550,320);
		((SKBaseWidget*)w)->SetWindowBackgroundImage(QPixmap(":/images/load"));
		((SKBaseWidget*)w)->HideTopFrame();
		((SKBaseWidget*)w)->Show();
	}
	else
		return false;

	return true;
}

bool SKGui::BeginAgent()
{
	if (SK_UNITCONFIG->Load(QCoreApplication::applicationDirPath().toStdString()+"\\..\\conf\\sys_unitconfig.xml"))
	{
		m_iUnitId = SK_UNITCONFIG->GetUnitId();
		m_sUnitName = SK_UNITCONFIG->GetUnitName();
		m_sUnitDesc = SK_UNITCONFIG->GetUnitDesc();
		m_sSoftwareType = SK_UNITCONFIG->GetSystemType();

		StartAgent(false);
		return true;
	}

	return false;
}

void SKGui::End()
{
	for (int i = 0; i < m_pListBaseView->count(); i++)
		delete m_pListBaseView->at(i);
	delete m_pListBaseView;

	m_pPluginMgr->Exit();
	delete m_pPluginMgr;

	CSpAgentClient::Stop();
	StopAndWait(10);
}

bool SKGui::ProcessAgentMsg(WORD wMsgType,stuSpUnitAgentMsgHead *pMsgHead,SString &sHeadStr,BYTE* pBuffer,int iLength)
{
	for (int i = 0; i < m_pListBaseView->count(); i++)
		m_pListBaseView->at(i)->ProcessAgentMsg(wMsgType,pMsgHead,sHeadStr,pBuffer,iLength);

	return true;
}

CBaseView* SKGui::NewView(QString sPluginName,QWidget* parent)
{
	CBaseView *view = NULL;

	view = m_pPluginMgr->NewView(sPluginName,parent);
	if (view)
		return view;

	return NULL;
}
