#pragma once
#include <Windows.h>

class Window
{
public:
	Window();
	//Initialize the window
	bool init();

	/*
		"the gear that allows the window to work because its purpose is to pick 
		the event messages of the window from the os and to translate and dispatch them to the window"
		|
		|
		v
	*/
	bool broadcast();
	//Release the window
	bool release();
	bool isRun();

	RECT getClientWindowRect();
	void setHWND(HWND hwnd);

	//EVENTS
	virtual void onCreate() = 0;
	virtual void onUpdate() = 0;
	virtual void onDestroy();
	~Window();

protected:
	HWND m_hwnd;
	bool m_is_run;
};

