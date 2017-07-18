
#pragma once

#include "WinWidget.h"
#include "native/NativeCategories.h"

class EUICategories;

class WinCategories : public NativeCategories
{
public:

	int   overallHeight;
	int   thumbHeight;
	int   thumbPos;
	int   thumbMaxPos;
	float thumbDelta;
	bool  thumbDraged;
	int   thumbPressed;

	POINT prev_point;

	WinCategories(EUIWidget* owner);
	virtual ~WinCategories();

	EUICategories* Owner();

	virtual bool ProcessWidget(long msg, WPARAM wParam, LPARAM lParam);
	virtual void OnMouseLeave();

	virtual void Draw();
	virtual void CalcThumb();
	virtual void UpdateChildPos();
	virtual void Resize();
};
