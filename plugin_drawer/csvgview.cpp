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
	if (((GraphicsItem*)item)->GetShowType() == 2)	//ң�ؽ���
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
				m_pCtrlWidget->SetWindowTitle("ң�ز���");
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
				QMessageBox::warning(this, "�澯", "ң������[plugin_telecontrol]����ʧ��");
		}
		else
			m_pCtrlWidget->Show();
	}
	else if (((GraphicsItem*)item)->GetShowType() == 3)	//ң������
	{

	}
	else if	(((GraphicsItem*)item)->GetShowType() == 4)	//������ת
	{
		QString scene = ((GraphicsItem*)item)->GetLinkScene();
		if (scene.isEmpty())
			return;

		SK_GUI->GotoFunPoint(scene.split("::").at(0));
		ClearSelection();
	}
	else if (((GraphicsItem*)item)->GetShowType() == 5) //�Զ���
	{
		QString custom = ((GraphicsItem*)item)->GetCustom();
		QMessageBox::information(this, "��ʾ", custom);
	}
}

void CSvgView::SlotCtrlClose()
{
	connect(m_pCtrlWidget, SIGNAL(SigClose()), this, SLOT(SlotCtrlClose()));
	delete m_pCtrlWidget;
	m_pCtrlWidget = NULL;
	ClearSelection();
}
