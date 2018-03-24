/**
 *
 * �� �� �� : view_plugin_demo1.h
 * �������� : 2018-03-24 09:34
 * ��    �� : SspAssist(skt001@163.com)
 * �޸����� : $Date: $
 * ��ǰ�汾 : $Revision: $
 * �������� : ����ƽ̨���Բ��1
 * �޸ļ�¼ : 
 *            $Log: $
 *
 * Ver  Date        Author    Comments
 * ---  ----------  --------  -------------------------------------------
 * 001	 2018-03-24	SspAssist�������ļ�
 *
 **/

#ifndef __VIEW_PLUGIN_DEMO1_H__
#define __VIEW_PLUGIN_DEMO1_H__

#include "cbaseview.h"
#include "ui_view_plugin_demo1.h"

class view_plugin_demo1 : public CBaseView
{
	Q_OBJECT

public:
	view_plugin_demo1(QWidget *parent = 0);
	~view_plugin_demo1();

	virtual void SetBackgroundColor(int red = 240, int yellow = 240, int blue = 240, int alpha = 255)
	{
		setStyleSheet(tr("QWidget#%1{background:rgb(%2,%3,%4,%5);}").arg(objectName()).arg(red).arg(yellow).arg(blue).arg(alpha));
	}

private:
	Ui::view_plugin_demo1 ui;
};

#endif // __VIEW_PLUGIN_DEMO1_H__

