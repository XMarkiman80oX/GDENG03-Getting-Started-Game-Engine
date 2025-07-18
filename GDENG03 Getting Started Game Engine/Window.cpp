#include "Window.h"
#include <exception>

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
    switch (msg) {

    case WM_CREATE:
    {
        //Event fired when the window will be created
        //collected here...
        break;
    }
    case WM_SETFOCUS:
    {
        //Event fired when the window gains focus
        Window* window = (Window*)GetWindowLongPtr(hwnd, GWLP_USERDATA);
        if(window)
            window->onFocus();
        break;
    }
    case WM_KILLFOCUS:
    {
        //Event fired when the window loses focus
        Window* window = (Window*)GetWindowLongPtr(hwnd, GWLP_USERDATA);
        window->onKillFocus();
        break;
    }
    case WM_DESTROY:
    {
        //Event fired when the window will be destroyed
        Window* window = (Window*)GetWindowLongPtr(hwnd, GWLP_USERDATA);
        window->onDestroy();
        ::PostQuitMessage(0);
        break;
    }

    default:
        return ::DefWindowProcA(hwnd, msg, wparam, lparam);
    }

    return NULL;
}

Window::Window()
{ //Setting up WINDCLASSEX object
    WNDCLASSEX wc;
    wc.cbClsExtra = NULL;
    wc.cbSize = sizeof(WNDCLASSEX);
    wc.cbWndExtra = NULL;
    wc.hbrBackground = (HBRUSH)COLOR_WINDOW;
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    wc.hIconSm = LoadIcon(NULL, IDI_APPLICATION);
    wc.hInstance = NULL;
    wc.lpszClassName = L"MyWindowClass";
    wc.lpszMenuName = L"";
    wc.style = NULL;
    //where you handle events
    wc.lpfnWndProc = &WndProc;


    if (!::RegisterClassEx(&wc))
		throw std::exception("Failed to register window class.");

    m_hwnd = ::CreateWindowEx(WS_EX_OVERLAPPEDWINDOW, L"MyWindowClass", L"DirectX Application", WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, 1024, 768,
        NULL, NULL, NULL, NULL);

    if (!this->m_hwnd)
		throw std::exception("Failed to create window.");

    //show up the window
    ::ShowWindow(this->m_hwnd, SW_SHOW);
    ::UpdateWindow(this->m_hwnd);

    //set this flag to true to indicate that the window is initialized and running
    this->m_is_run = true;
}
bool Window::broadcast()
{
    MSG msg;

    
    if (!this->m_is_init)
    {
        SetWindowLongPtr(this->m_hwnd, GWLP_USERDATA, (LONG_PTR)this);
        this->onCreate();
        this->m_is_init = true;
    }

    EngineTime::LogFrameStart();
    this->onUpdate();

    while (::PeekMessage(&msg, NULL, 0, 0, PM_REMOVE) > 0) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    Sleep(1);

    EngineTime::LogFrameEnd();

    return true;
}


bool Window::isRun()
{
    if (this->m_is_run)
        this->broadcast();

    return this->m_is_run;
}

RECT Window::getClientWindowRect()
{
    RECT rc;

    ::GetClientRect(this->m_hwnd, &rc);
    return rc;

}

void Window::setHWND(HWND hwnd)
{
    this->m_hwnd = hwnd;
}

void Window::onFocus()
{
}

void Window::onKillFocus()
{
}

Window::~Window()
{
}

void Window::onDestroy()
{
    m_is_run = false;
}
