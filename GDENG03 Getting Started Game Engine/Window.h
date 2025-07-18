#pragma once
#include <Windows.h>
#include "EngineTime.h"
class Window
{
public:
	//Initialize the window
	Window();

	bool isRun();

	RECT getClientWindowRect();

	//EVENTS
	virtual void onCreate() = 0;
	virtual void onUpdate() = 0;
	virtual void onDestroy();
	//So that mouse doesn't keep getting registered when OOF
	virtual void onFocus();
	virtual void onKillFocus();

	//Release the window
	~Window();
private:
	/*
		"the gear that allows the window to work because its purpose is to pick
		the event messages of the window from the os and to translate and dispatch them to the window"
		|
		|
		v
	*/
	bool broadcast();
protected:
	HWND m_hwnd;
	bool m_is_run;
	bool m_is_init = false;
};

