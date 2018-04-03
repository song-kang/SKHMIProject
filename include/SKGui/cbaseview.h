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

	void SetPluginName(QString name) { m_sPluginName = name; }
	QString GetPluginName() { return m_sPluginName; }

public:
	virtual bool ProcessAgentMsg(WORD wMsgType,stuSpUnitAgentMsgHead *pMsgHead,SString &sHeadStr,BYTE* pBuffer=NULL,int iLength=0)
	{
		S_UNUSED(wMsgType);
		S_UNUSED(pMsgHead);
		S_UNUSED(sHeadStr);
		S_UNUSED(pBuffer);
		S_UNUSED(iLength);
		return false;
	}

	virtual void SetBackgroundColor(int red = 240, int yellow = 240, int blue = 240, int alpha = 230)=0;

private:
	QString m_sPluginName;

};

#endif // CBASEVIEW_H
