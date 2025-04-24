/*
    窗口尺寸和客户端尺寸
    窗口尺寸：窗口的实际尺寸，包括标题栏、边框、菜单栏、工具栏等
    客户端尺寸：窗口的实际内容区域，不包括标题栏、边框、菜单栏、工具栏等
 */
/*
    与其先设置窗口尺寸然后调整客户端尺寸，不如先设置客户端尺寸然后调整窗口尺寸，为此，我们需要在创建窗口之前使用AdjustWindowRect函数
    AdjustWindowRect函数原型为：BOOL AdjustWindowRect(LPRECT lpRect, DWORD dwStyle, BOOL bMenu)
    其作用是获取客户端区域的尺寸和位置，然后计算必要的窗口尺寸和位置
    1. 第一个参数lpRect: 指向一个RECT结构体，其包含所需的客户端区域的坐标，调用改函数后，lpRect的坐标会被修改为窗口的尺寸和位置
    2. 第二个参数dwStyle: 窗口样式，如WS_OVERLAPPED、WS_CAPTION、WS_SYSMENU等，用过这个参数来确定窗口边框的大小
    3. 第三个参数bMenu: 是否包含菜单栏，如果包含菜单栏，则传入true，否则传入false
*/

#include <windows.h>
#include <tchar.h>

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    WNDCLASSEX wc;
    ZeroMemory(&wc, sizeof(WNDCLASSEX));

    wc.cbSize = sizeof(WNDCLASSEX);
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)COLOR_WINDOW;
    wc.lpszClassName = _T("WindowClass");

    RegisterClassEx(&wc);

    //创建了一个RECT结构体，并初始化其坐标为(0, 0)，宽度为800，高度为600
    RECT rect = {0, 0, 800, 600};
    //AdjustWindowRect函数会根据窗口样式和菜单栏的存在与否，调整RECT结构体的坐标和尺寸，使其适应窗口的实际尺寸
    AdjustWindowRect(&rect, WS_OVERLAPPEDWINDOW, FALSE);

    HWND hwnd = CreateWindowEx(0, _T("WindowClass"), _T("Hello, World!"), WS_OVERLAPPEDWINDOW, 300, 300, rect.right - rect.left, rect.bottom - rect.top, NULL, NULL, hInstance, NULL);

    ShowWindow(hwnd, nCmdShow);

    //消息循环
    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return msg.wParam;
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
        case WM_DESTROY:
        {
            PostQuitMessage(0);
            return 0;
        }
        default:
        {
            return DefWindowProc(hwnd, uMsg, wParam, lParam);
        }
    }
    
}