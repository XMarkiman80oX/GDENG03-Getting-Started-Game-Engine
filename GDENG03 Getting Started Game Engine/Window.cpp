#include "Window.h"

//Window* window = NULL;
Window::Window()
{
}

LRESULT CALLBACK WndProc( HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
    switch (msg) {

        case WM_CREATE:
        {
            //Event fired when the window will be created
            //collected here...
            Window* window = (Window*)((LPCREATESTRUCT)lparam)->lpCreateParams;
            //.. and then stored for later lookup
            SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)window);
            window->setHWND(hwnd);
            window->onCreate();
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

bool Window::init()
{
    //Setting up WINDCLASSEX object
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
        return false;

    m_hwnd = ::CreateWindowEx(WS_EX_OVERLAPPEDWINDOW, L"MyWindowClass", L"DirectX Application", WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, 1024, 768,
        NULL, NULL, NULL, this);

    if (!this->m_hwnd)
        return false;

    //show up the window
    ::ShowWindow(this->m_hwnd, SW_SHOW);
    ::UpdateWindow(this->m_hwnd);

    //set this flag to true to indicate that the window is initialized and running
    this->m_is_run = true;
    return true;
}

bool Window::broadcast()
{
    MSG msg;

    EngineTime::LogFrameStart();
    this->onUpdate();

    while (::PeekMessage(&msg, NULL, 0, 0, PM_REMOVE) > 0) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    Sleep(1);
    EngineTime::LogFrameEnd();

    this->elapsedTime += EngineTime::getDeltaTime();

    if (this->elapsedTime >= 10.0f)
    {
        std::cout << "HI" << std::endl;
        this->elapsedTime = 0.0f;
    }

    return true;
}

bool Window::release()
{
    //Destroy the window

    if (::DestroyWindow(this->m_hwnd))
        return false;

    return true;
}

bool Window::isRun()
{
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

Window::~Window()
{
}

void Window::onDestroy()
{
    m_is_run = false;
}
