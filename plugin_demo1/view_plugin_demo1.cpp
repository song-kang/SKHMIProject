/**
 *
 * 文 件 名 : view_plugin_demo1.cpp
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
#include "view_plugin_demo1.h"
#include "mdb_def.h"

view_plugin_demo1::view_plugin_demo1(QWidget *parent)
: CBaseView(parent)
{
	ui.setupUi(this);

	SetBackgroundColor();
	RegisterMdbTrigger();

	TestCommandSend();
	Start();
}

view_plugin_demo1::~view_plugin_demo1()
{
	RemoveMdbTrigger();
}

int view_plugin_demo1::OnCommand(SString sCmd,SString &sResult)
{
	return 0;
}

BYTE* view_plugin_demo1::OnMdbTrgCallback(void* cbParam,SString &sTable,eMdbTriggerType eType,int iTrgRows,int iRowSize,BYTE *pTrgData)
{
	t_oe_element_state stuElemState;
	if (sTable == "t_oe_element_state")
	{
		int size = sizeof(t_oe_element_state);
		if (size != iRowSize)
		{
			LOGWARN("结构大小不一致(%s:%d|%d)，请检查mdb_def.h和mdb.xml是否对应.",sTable.data(),size,iRowSize);
			return 0;
		}

		for (int i = 0; i < iTrgRows; i++)
		{
			memset(&stuElemState,0,sizeof(t_oe_element_state));
			memcpy(&stuElemState,pTrgData+i*sizeof(t_oe_element_state),sizeof(t_oe_element_state));
		}
	}
	
	return 0;
}

bool view_plugin_demo1::ProcessAgentMsg(WORD wMsgType,stuSpUnitAgentMsgHead *pMsgHead,SString &sHeadStr,BYTE* pBuffer,int iLength)
{
	return true;
}

void view_plugin_demo1::TestCommandSend()
{
	stuSpUnitAgentProcessId dst_id;
	dst_id.m_iUnitId = 0;
	dst_id.SetApplicationId(SP_UA_APPNO_AGENT);

	bool ret = SApplication::GetPtr()->SendAgentMsg(&dst_id,SP_UA_MSG_REQ_GET_UNIT,SApplication::NewMsgSn());
	if (!ret)
	{
		int b = 0;
	}
}

void view_plugin_demo1::Start()
{
	CreatSvgXml();
	SString sContent;
	m_svgXml.SaveToXml(sContent);

	m_pSvgView = new DrawView;
	m_pSvgView->Load(sContent.data());
	ui.gridLayoutCentral->addWidget(m_pSvgView);
}

void view_plugin_demo1::CreatSvgXml()
{
	stuPoint *p;
	stuPoint point;
	CSKItem *item;
	CSKTextItem *textitem;
	SPtrList<stuPoint> points;

	/////////////////////////////////
	point.m_x = 100;
	point.m_y = 100;
	item = m_svgXml.CreateRoundedRect(point, 300, 250, 5, 5);
	item->SetBrushColor("#69541b");
	point.m_x = 130;
	point.m_y = 120;
	textitem = m_svgXml.CreateText(point, 240, 100, "PM2201A 220kV母线第一套保护");
	textitem->SetFamily("微软雅黑");
	textitem->SetPointSize(20);
	point.m_x = 400;
	point.m_y = 150;
	item = m_svgXml.CreateRect(point, 200, 50);
	item->SetBrushColor("#494e8f");
	point.m_x = 400;
	point.m_y = 160;
	textitem = m_svgXml.CreateText(point, 200, 30, "GOOSE 0x0123");
	textitem->SetFamily("微软雅黑");
	textitem->SetPointSize(18);
	points.clear();
	p = new stuPoint;
	p->m_x = 600;
	p->m_y = 175;
	points.append(p);
	p = new stuPoint;
	p->m_x = 750;
	p->m_y = 175;
	points.append(p);
	m_svgXml.CreateArrowLine(&points,1.5,"#00ff00",3,0.1);
	point.m_x = 400;
	point.m_y = 250;
	item = m_svgXml.CreateRect(point, 200, 50);
	item->SetBrushColor("#feeeed");
	point.m_x = 400;
	point.m_y = 260;
	textitem = m_svgXml.CreateText(point, 200, 30, "接收");
	textitem->SetFamily("微软雅黑");
	textitem->SetPointSize(18);
	textitem->SetPenColor("#000000");
	points.clear();
	p = new stuPoint;
	p->m_x = 780;
	p->m_y = 375;
	points.append(p);
	p = new stuPoint;
	p->m_x = 600;
	p->m_y = 275;
	points.append(p);
	m_svgXml.CreateArrowLine(&points,1.5,"#00ff00",3,0.1);

	/////////////////////////////////
	point.m_x = 100;
	point.m_y = 500;
	item = m_svgXml.CreateRoundedRect(point, 300, 150, 5, 5);
	item->SetBrushColor("#69541b");
	point.m_x = 130;
	point.m_y = 520;
	textitem = m_svgXml.CreateText(point, 240, 100, "MT2201A #1主变第一套合并单元");
	textitem->SetFamily("微软雅黑");
	textitem->SetPointSize(20);
	point.m_x = 400;
	point.m_y = 550;
	item = m_svgXml.CreateRect(point, 200, 50);
	item->SetBrushColor("#feeeed");
	point.m_x = 400;
	point.m_y = 560;
	textitem = m_svgXml.CreateText(point, 200, 30, "接收");
	textitem->SetFamily("微软雅黑");
	textitem->SetPointSize(18);
	textitem->SetPenColor("#000000");
	points.clear();
	p = new stuPoint;
	p->m_x = 780;
	p->m_y = 375;
	points.append(p);
	p = new stuPoint;
	p->m_x = 600;
	p->m_y = 575;
	points.append(p);
	m_svgXml.CreateArrowLine(&points,1.5,"#00ff00",3,0.1);

	/////////////////////////////////
	point.m_x = 750;
	point.m_y = 100;
	item = m_svgXml.CreateRoundedRect(point, 300, 550, 5, 5);
	item->SetBrushColor("#008792");
	point.m_x = 780;
	point.m_y = 120;
	textitem = m_svgXml.CreateText(point, 240, 100, "PT2201A #1主变第一套保护");
	textitem->SetFamily("微软雅黑");
	textitem->SetPointSize(20);
	point.m_x = 780;
	point.m_y = 350;
	item = m_svgXml.CreateRect(point, 240, 50);
	item->SetBrushColor("#494e8f");
	point.m_x = 800;
	point.m_y = 360;
	textitem = m_svgXml.CreateText(point, 200, 30, "GOOSE 0x0153");
	textitem->SetFamily("微软雅黑");
	textitem->SetPointSize(18);

	/////////////////////////////////
	point.m_x = 1400;
	point.m_y = 200;
	item = m_svgXml.CreateRoundedRect(point, 300, 350, 5, 5);
	item->SetBrushColor("#69541b");
	point.m_x = 1430;
	point.m_y = 220;
	textitem = m_svgXml.CreateText(point, 240, 100, "ME1101A 110kV母分智能单元");
	textitem->SetFamily("微软雅黑");
	textitem->SetPointSize(20);
	point.m_x = 1200;
	point.m_y = 250;
	item = m_svgXml.CreateRect(point, 200, 50);
	item->SetBrushColor("#f15b6c");
	point.m_x = 1200;
	point.m_y = 260;
	textitem = m_svgXml.CreateText(point, 200, 30, "SV 0x4042");
	textitem->SetFamily("微软雅黑");
	textitem->SetPointSize(18);
	points.clear();
	p = new stuPoint;
	p->m_x = 1200;
	p->m_y = 275;
	points.append(p);
	p = new stuPoint;
	p->m_x = 1050;
	p->m_y = 275;
	points.append(p);
	m_svgXml.CreateArrowLine(&points,1.5,"#00ff00",3,0.1);
	point.m_x = 1200;
	point.m_y = 350;
	item = m_svgXml.CreateRect(point, 200, 50);
	item->SetBrushColor("#494e8f");
	point.m_x = 1200;
	point.m_y = 360;
	textitem = m_svgXml.CreateText(point, 200, 30, "GOOSE 0x0132");
	points.clear();
	p = new stuPoint;
	p->m_x = 1200;
	p->m_y = 375;
	points.append(p);
	p = new stuPoint;
	p->m_x = 1050;
	p->m_y = 375;
	points.append(p);
	m_svgXml.CreateArrowLine(&points,1.5,"#00ff00",3,0.1);
	textitem->SetFamily("微软雅黑");
	textitem->SetPointSize(18);
	point.m_x = 1200;
	point.m_y = 450;
	item = m_svgXml.CreateRect(point, 200, 50);
	item->SetBrushColor("#feeeed");
	point.m_x = 1200;
	point.m_y = 460;
	textitem = m_svgXml.CreateText(point, 200, 30, "接收");
	textitem->SetFamily("微软雅黑");
	textitem->SetPointSize(18);
	textitem->SetPenColor("#000000");
	points.clear();
	p = new stuPoint;
	p->m_x = 1020;
	p->m_y = 375;
	points.append(p);
	p = new stuPoint;
	p->m_x = 1200;
	p->m_y = 475;
	points.append(p);
	m_svgXml.CreateArrowLine(&points,1.5,"#00ff00",3,0.1);

	m_svgXml.SetBackgroundWidth(1800);
	m_svgXml.SetBackgroundHeight(750);
}
