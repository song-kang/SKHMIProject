/**
 *
 * 文 件 名 : view_plugin_demo1.h
 * 创建日期 : 2018-03-24 09:34
 * 作    者 : SspAssist(skt001@163.com)
 * 修改日期 : $Date: $
 * 当前版本 : $Revision: $
 * 功能描述 : 界面平台测试插件1
 * 修改记录 : 
 *            $Log: $
 *
 * Ver  Date        Author    Comments
 * ---  ----------  --------  -------------------------------------------
 * 001	 2018-03-24	SspAssist　创建文件
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

