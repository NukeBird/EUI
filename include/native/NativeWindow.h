
#pragma once

#ifdef PLATFORM_WIN
#include "native/win/WinWidget.h"
#else
#include "NativeWidget.h"
#define WinWidget NativeWidget
#endif

class NativeWindow : public WinWidget
{
public:

	NativeWindow(EUIWidget* owner) : WinWidget(owner) {};

	virtual void Maximaze() = 0;
};
