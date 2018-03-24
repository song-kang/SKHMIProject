#include "cbaseview.h"
#include "skgui.h"

CBaseView::CBaseView(QWidget *parent)
	: SKWidget(parent)
{
	if (SK_GUI != NULL && SK_GUI->GetListBaseView() != NULL)
		SK_GUI->GetListBaseView()->append(this);
}

CBaseView::~CBaseView()
{
	if (SK_GUI != NULL && SK_GUI->GetListBaseView() != NULL)
		SK_GUI->GetListBaseView()->removeOne(this);
}
