#include "csvgview.h"

CSvgView::CSvgView(QGraphicsScene *parent)
	: DrawView(parent)
{
	m_pCtrlWidget = NULL;
}

CSvgView::~CSvgView()
{

}

void CSvgView::OnClicked(QList<QGraphicsItem*> list)
{
	QGraphicsItem *item = list.at(0);
	if (((GraphicsItem*)item)->GetShowType() == 2)	//遥控界面
	{
		QString desc;
		QIcon icon;
		if (!m_pCtrlWidget)
		{
			CBaseView *view = SK_GUI->GotoFunPoint("plugin_telecontrol",desc,icon);
			if (view)
			{
				m_pCtrlWidget = new SKBaseWidget(NULL,view);
				m_pCtrlWidget->SetWindowsFlagsDialog();
				m_pCtrlWidget->SetWindowsModal();
				m_pCtrlWidget->SetWindowTitle("遥控操作");
				m_pCtrlWidget->SetWindowIcon(icon);
				m_pCtrlWidget->SetWindowFlags(0);
				m_pCtrlWidget->SetWindowSize(420,365);
				m_pCtrlWidget->SetIsDrag(true);
				connect(m_pCtrlWidget, SIGNAL(SigClose()), this, SLOT(SlotCtrlClose()));

				SString sCmd;
				SString sRet;
				view->OnCommand(sCmd,sRet);
				m_pCtrlWidget->Show();
			}
			else
				QMessageBox::warning(this, "告警", "遥控类插件[plugin_telecontrol]加载失败");
		}
		else
			m_pCtrlWidget->Show();
	}
	else if (((GraphicsItem*)item)->GetShowType() == 3)	//遥调界面
	{

	}
	else if	(((GraphicsItem*)item)->GetShowType() == 4)	//场景跳转
	{
		QString scene = ((GraphicsItem*)item)->GetLinkScene();
		if (scene.isEmpty())
			return;

		SK_GUI->GotoFunPoint(scene.split("::").at(0));
		ClearSelection();
	}
	else if (((GraphicsItem*)item)->GetShowType() == 5) //自定义
	{
		QString custom = ((GraphicsItem*)item)->GetCustom();
		QMessageBox::information(this, "提示", custom);
	}
}

void CSvgView::SlotCtrlClose()
{
	connect(m_pCtrlWidget, SIGNAL(SigClose()), this, SLOT(SlotCtrlClose()));
	delete m_pCtrlWidget;
	m_pCtrlWidget = NULL;
	ClearSelection();
}
