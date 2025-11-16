#include "Window.h"

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
    Window* window = (Window*)GetWindowLongPtr(hwnd, GWLP_USERDATA);

    switch (msg)
    {
    case WM_CREATE:
    {
        window = (Window*)((LPCREATESTRUCT)lparam)->lpCreateParams;
        SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)window);
        window->setHWND(hwnd);
        window->onCreate();
        break;
    }

    case WM_DESTROY:
    {
        if (window)
            window->onDestroy();
        ::PostQuitMessage(0);
        break;
    }

    default:
        return ::DefWindowProc(hwnd, msg, wparam, lparam);
    }

    return 0;
}

Window::Window()
{
}

bool Window::init()
{
    WNDCLASSEX wc = {};
    wc.cbSize = sizeof(WNDCLASSEX);
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = &WndProc;
    wc.hInstance = GetModuleHandle(NULL);
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.lpszClassName = L"winClass";

    if (!::RegisterClassEx(&wc))
        return false;

    m_hwnd = ::CreateWindowEx(
        0, L"winClass", L"DirectX App", WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, 1024, 768,
        NULL, NULL, wc.hInstance, this); // 'this' gönderildi

    if (!m_hwnd)
        return false;

    ::ShowWindow(m_hwnd, SW_SHOW);
    ::UpdateWindow(m_hwnd);

    m_is_run = true;
    return true;
}

bool Window::broadcast()
{
    MSG msg;

    this->onUpdate();

    while (::PeekMessage(&msg, NULL, 0, 0, PM_REMOVE) > 0)
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

     // pencere her frame güncelleniyor
    Sleep(1);
    return true;
}

bool Window::isRun()
{
    return m_is_run;
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

void Window::onCreate()
{
}

void Window::onUpdate()
{
}

void Window::onDestroy()
{
    m_is_run = false;
}

bool Window::release()
{
    if (!DestroyWindow(m_hwnd))
        return false;
    return true;
}

Window::~Window()
{
}
