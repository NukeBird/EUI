#include "EUICheckBox.h"
#include "WinCheckBox.h"

WinCheckBox::WinCheckBox(EUIWidget* owner) : NativeCheckBox(owner)
{
	handle = CreateWindow("STATIC", "", SS_LEFT | WS_CHILD | WS_VISIBLE | SS_OWNERDRAW | SS_NOTIFY,
							(int)Owner()->x, (int)Owner()->y, (int)Owner()->width, (int)Owner()->height,
							((WinWidget*)Owner()->parent->nativeWidget)->GetHandle(), win_id, NULL, NULL);
	win_id++;

	MakeSubClassing();

	SendMessage(handle, WM_SETFONT, (WPARAM)theme->GetFont("FONT_NORMAL"), MAKELPARAM(TRUE, 0));
}

WinCheckBox::~WinCheckBox()
{
}

EUICheckBox* WinCheckBox::Owner()
{
	return (EUICheckBox*)owner;
}

bool WinCheckBox::ProcessWidget(long msg, WPARAM wParam, LPARAM lParam)
{
	NativeCheckBox::ProcessWidget(msg, wParam, lParam);

	if (msg == WM_LBUTTONUP)
	{
		Owner()->checked = !Owner()->checked;
		Redraw();
	}

	return true;
}

void WinCheckBox::SetText(const char* txt)
{
	Redraw();
}

void WinCheckBox::SetChecked(bool set)
{
	Redraw();
}

void WinCheckBox::Draw()
{
	UINT uState = EUITheme::UISTATE_NORMAL;

	if (!Owner()->IsEnabled())
	{
		uState = EUITheme::UISTATE_DISABLED;
	}

	if (Owner()->checked)
	{
		uState |= EUITheme::UISTATE_PUSHED;
	}

	RECT m_rcItem = { 0, 0, (LONG)Owner()->width, (LONG)Owner()->height };

	theme->DrawCheckBox(GetDC(handle), m_rcItem, Owner()->text.c_str(), uState, DT_SINGLELINE);
}