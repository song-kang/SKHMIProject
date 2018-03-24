#ifndef CBASEVIEW_H
#define CBASEVIEW_H

#include "skgui_global.h"
#include "skhead.h"
#include "skwidget.h"
#include "SApplication.h"

class SKGUI_EXPORT CBaseView : public SKWidget
{
	Q_OBJECT

public:
	CBaseView(QWidget *parent = 0);
	~CBaseView();

	virtual bool ProcessAgentMsg(WORD wMsgType,stuSpUnitAgentMsgHead *pMsgHead,SString &sHeadStr,BYTE* pBuffer=NULL,int iLength=0)
	{
		S_UNUSED(wMsgType);
		S_UNUSED(pMsgHead);
		S_UNUSED(sHeadStr);
		S_UNUSED(pBuffer);
		S_UNUSED(iLength);
		return false;
	}

	virtual void SetBackgroundColor(int red = 240, int yellow = 240, int blue = 240, int alpha = 255)=0;

private:
	SString m_sFunCode;

};

#endif // CBASEVIEW_H
