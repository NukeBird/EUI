
#include "EUIWidget.h"
#include "WinWidget.h"

WinWidget* WinWidget::mouse_over = NULL;

WinWidget::WinWidget(EUIWidget* set_owner) : NativeWidget(set_owner)
{
	handle = 0;
	tooltip = 0;
	tooltip_inited = false;
	processRes = 0;
}

WinWidget::~WinWidget()
{
    DestroyWindow(handle);
}

HWND WinWidget::GetHandle()
{
	return handle;
}

void* WinWidget::GetNative()
{
	return &handle;
}

void WinWidget::Show(bool set)
{
	ShowWindow(handle, set);
}

void WinWidget::Enable(bool set)
{
	EnableWindow(handle, set);
}

void WinWidget::SetPos(float set_x, float set_y)
{
	SetWindowPos(handle, 0, set_x, set_y, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
}

float WinWidget::GetXOffset()
{
	return 0.0f;
}

float WinWidget::GetYOffset()
{
	return 0.0f;
}

void WinWidget::SetSize(float w, float h)
{
	SetWindowPos(handle, 0, 0, 0, w, h, SWP_NOMOVE | SWP_NOZORDER);
}

void WinWidget::SetText(const char* txt)
{
	CreateTooltip();
}

void WinWidget::Release()
{
	delete this;
}

bool WinWidget::ProcessWidget(long msg, WPARAM wParam, LPARAM lParam)
{
	int xPos = GET_X_LPARAM(lParam);
	int yPos = GET_Y_LPARAM(lParam);

	static HBRUSH hbrBkgnd = NULL;

	switch (msg)
	{
		case WM_PAINT:
		{
			Draw();
		}
		break;
		case WM_LBUTTONDOWN:
		{
			if (owner->listiner)
			{
				owner->listiner->OnLeftMouseDown(owner, xPos, yPos);
			}
		}
		break;
		case WM_LBUTTONUP:
		{
			if (owner->listiner)
			{
				owner->listiner->OnLeftMouseUp(owner, xPos, yPos);
			}
		}
		break;
		case WM_LBUTTONDBLCLK:
		{
			if (owner->listiner)
			{
				owner->listiner->OnLeftDoubliClick(owner, xPos, yPos);
			}
		}
		break;
		case WM_RBUTTONDOWN:
		{
			if (owner->listiner)
			{
				owner->listiner->OnRightMouseDown(owner, xPos, yPos);
			}
		}
		break;
		case WM_RBUTTONUP:
		{
			if (owner->listiner)
			{
				owner->listiner->OnRightMouseUp(owner, xPos, yPos);
			}
		}
		break;
		case WM_MOUSEMOVE:
		{
			NotifyMouseOver(this);

			if (owner->listiner)
			{
				owner->listiner->OnMouseMove(owner, xPos, yPos);
			}
		}
		break;
	}

	return true;
}

void WinWidget::Draw()
{
}

void WinWidget::Redraw()
{
	RECT rc;
	rc.left = 0;
	rc.top = 0;
	rc.right = owner->width + 40;
	rc.bottom = owner->height + 40;

	EUIWidget* wgt = owner;

	while (wgt->parent)
	{
		rc.left += wgt->x;
		rc.top += wgt->y;

		rc.right += wgt->x;
		rc.bottom += wgt->y;

		wgt = wgt->parent;
	}

	InvalidateRect(((WinWidget*)wgt->nativeWidget)->GetHandle(), &rc, false);
}

void WinWidget::Resize()
{
	if (owner->listiner && (owner->listinerFlag & EUIWidget::OnResize))
	{
		owner->listiner->OnResize(owner);
	}

	for (int i = 0; i<owner->childs.size(); i++)
	{
		owner->childs[i]->nativeWidget->Resize();
	}
}

void WinWidget::NotifyMouseOver(WinWidget* widget)
{
	if (mouse_over != widget)
	{
		if (mouse_over) mouse_over->OnMouseLeave();
		mouse_over = widget;
	}
}

void WinWidget::OnMouseLeave()
{
}

void WinWidget::SetFocused()
{
	SetFocus(handle);
}

bool WinWidget::IsFocused()
{
	return (GetFocus() == handle);
}

void WinWidget::CreateTooltip()
{
	if (tooltip_inited)
	{
		ti.lpszText = (char*)owner->GetText();
		SendMessage(tooltip, TTM_SETTOOLINFO , 0, (LPARAM) (LPTOOLINFO) &ti);
		return;
	}

	RECT rect;

	tooltip = CreateWindowEx(WS_EX_TOPMOST, TOOLTIPS_CLASS, NULL, WS_POPUP | TTS_NOPREFIX | TTS_ALWAYSTIP,
							 CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,  handle,
							 NULL, GetModuleHandle(NULL), NULL);
 
	SetWindowPos(tooltip, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE); 
	GetClientRect (handle, &rect);

	ti.cbSize = sizeof(TOOLINFO);
	ti.uFlags = TTF_SUBCLASS;
	ti.hwnd = handle;
	ti.hinst = GetModuleHandle(NULL);
	ti.uId = 0;
	ti.lpszText = (char*)owner->GetText();
	ti.rect.left = rect.left;
	ti.rect.top = rect.top;
	ti.rect.right = rect.right;
	ti.rect.bottom = rect.bottom;

	SendMessage(tooltip, TTM_ADDTOOL, 0, (LPARAM) (LPTOOLINFO) &ti);
	tooltip_inited = true;
}

bool WinWidget::IsHoveredByMouse()
{
	POINT point;

	if (GetCursorPos(&point))
	{
		RECT windowRect;

		GetWindowRect( handle, &windowRect );

		return PtInRect( &windowRect, point );
	}

	return false;
}

void WinWidget::MakeSubClassing()
{
	SetWindowSubclass(handle, &WinWidgetProc, 0, (DWORD_PTR)this);
}

LRESULT CALLBACK WinWidget::WinWidgetProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData)
{
	WinWidget* widget = (WinWidget*)dwRefData;

	if (!widget->ProcessWidget(uMsg, wParam, lParam))
	{
		int res = widget->processRes;
		widget->processRes = 0;

		return res;
	}

	return ::DefSubclassProc(hWnd, uMsg, wParam, lParam);
}