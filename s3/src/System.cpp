#include "System.h"

System::System()
{
    m_Input = nullptr;
    m_Application = nullptr;
}

System::System(const System& other)
{
}

System::~System()
{
}

bool System::Initialize()
{
    int screenWidth = 0;
    int screenHeight = 0;
    InitializeWindows(screenWidth, screenHeight);

    m_Input = new Input();

    m_Input -> Initialize();

    m_Application = new Application();

    bool result = m_Application -> Initialize(screenWidth, screenHeight, m_hwnd);
    if (!result)
    {
        return false;
    }
    
    return true;
}

void System::Shutdown()
{
    if (m_Application)
    {
        m_Application -> Shutdown();
        delete m_Application;
        m_Application = nullptr;
    }

    if (m_Input)
    {
        delete m_Input;
        m_Input = nullptr;
    }

    ShutdownWindows();
}

void System::Run()
{
    MSG msg;
    ZeroMemory(&msg, sizeof(MSG));

    bool done = false;
    while (!done)
    {
        if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        if (msg.message == WM_QUIT)
        {
            done = true;
        }
        else
        {
            bool result = Frame();
            if (!result)
            {
                done = true;
            }
        }
    }
    return;
}

bool System::Frame()
{
    if (m_Input -> IsKeyDown(VK_ESCAPE))
    {
        return false;
    }
    return m_Application -> Frame();
}

/*
MessageHandler函数是我们引导windows系统消息进入的地方。这样我们就可以听到我们感兴趣的某些信息。目前，我们只是读取键是否被按下或键是否被释放，并将该信息传递给输入对象。
我们将把所有其他信息传递回windows默认消息处理程序。
*/
LRESULT CALLBACK System::MessageHandler(HWND hwnd, UINT umsg, WPARAM wparam, LPARAM lparam)
{
    switch (umsg)
    {
        case WM_KEYDOWN:
        {
            m_Input -> KeyDown((unsigned int)wparam);
            return 0;
        }
        case WM_KEYUP:
        {
            m_Input -> KeyUp((unsigned int)wparam);
            return 0;
        }
        default:
        {
            return DefWindowProc(hwnd, umsg, wparam, lparam);
        }
    }
}   

/*
InitializeWindows函数是我们放置代码的地方，用来构建我们将要渲染的窗口。它将screenWidth和screenHeight返回给调用函数，这样我们就可以在整个应用程序中使用它们。
我们使用一些默认设置来初始化一个没有边框的纯黑色窗口。该函数将根据一个名为FULL_SCREEN的全局变量创建一个小窗口或一个全屏窗口。
如果设置为true，则屏幕覆盖整个用户桌面窗口。如果设置为false，我们就在屏幕中间创建一个800x600的窗口。我将FULL_SCREEN全局变量放在applicationclass.h文件的顶部，以便您想要修改它。
稍后就会明白为什么我把全局变量放在那个文件中而不是这个文件的头文件中。
*/
void System::InitializeWindows(int &screenWidth, int &screenHeight)
{
    //WNDCLASSEX结构体是Windows API中用于定义窗口类的一个结构体。其属性有：
    //style：窗口样式，如CS_HREDRAW | CS_VREDRAW | CS_OWNDC，表示窗口可以水平和垂直重绘，并且拥有自己的设备上下文。
    //lpfnWndProc：窗口过程函数，即消息处理函数，当窗口收到消息时，会调用这个函数。
    //cbClsExtra：窗口类附加内存，通常为0。
    //cbWndExtra：窗口实例附加内存，通常为0。
    //hInstance：应用程序实例句柄。
    //hIcon：窗口图标句柄。
    //hIconSm：窗口小图标句柄。
    //hCursor：鼠标光标句柄。
    //hbrBackground：窗口背景画刷句柄。
    //lpszMenuName：菜单名称，通常为nullptr。
    //lpszClassName：窗口类名称。
    //cbSize：结构体大小。
    WNDCLASSEX wc;
    //DEVMODE结构体是Windows API中用于定义显示设备模式的一个结构体。其属性有：
    //dmSize：结构体大小。
    //dmPelsWidth：屏幕宽度。
    //dmPelsHeight：屏幕高度。
    //dmBitsPerPel：每个像素的位数。
    //dmFields：表示结构体中哪些成员有效。  
    //dmPositionX：窗口左上角在屏幕上的X坐标。
    //dmPositionY：窗口左上角在屏幕上的Y坐标。
    //dmDisplayFrequency：显示器的刷新率。
    //dmBitsPerPel：每个像素的位数。
    //dmFields：表示结构体中哪些成员有效。
    //dmDisplayFlags：显示标志。
    //dmDisplayFrequency：显示器的刷新率。
    //dmPelsWidth：屏幕宽度。
    //dmPelsHeight：屏幕高度。
    //dmBitsPerPel：每个像素的位数。
    //dmFields：表示结构体中哪些成员有效。
    DEVMODE dmScreenSettings;
    int posX, posY;

    ApplicationHandle = this;

    //获取当前执行的EXE文件的实例句柄
    m_hinstance = GetModuleHandle(nullptr);

    m_applicationName = L"Engine";

    wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
    wc.lpfnWndProc = WndProc;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hInstance = m_hinstance;
    wc.hIcon = LoadIcon(nullptr, IDI_WINLOGO);
    wc.hIconSm = wc.hIcon;
    wc.hCursor = LoadCursor(nullptr, IDC_ARROW);

    //GetStockObject函数返回一个标准的画刷，BLACK_BRUSH是Windows定义的画刷之一，它返回一个黑色的画刷。
    wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);

    wc.lpszMenuName = nullptr;
    wc.lpszClassName = m_applicationName;
    wc.cbSize = sizeof(WNDCLASSEX);

    RegisterClassEx(&wc);

    //获取屏幕分辨率
    //GetSystemMetrics函数返回屏幕的分辨率
    screenWidth = GetSystemMetrics(SM_CXSCREEN);
    screenHeight = GetSystemMetrics(SM_CYSCREEN);

    if (FULL_SCREEN)
    {
        //如果设置为true，则屏幕覆盖整个用户桌面窗口。
        // ZeroMemory(&dmScreenSettings, sizeof(dmScreenSettings));
        memset(&dmScreenSettings, 0, sizeof(dmScreenSettings));
        //设置屏幕分辨率
        dmScreenSettings.dmSize = sizeof(dmScreenSettings);
        //设置屏幕宽度
        dmScreenSettings.dmPelsWidth = (unsigned long)screenWidth;
        //设置屏幕高度
        dmScreenSettings.dmPelsHeight = (unsigned long)screenHeight;
        //设置每个像素的位数，32位表示每个像素有32位，即4个字节
        dmScreenSettings.dmBitsPerPel = 32;
        //设置屏幕分辨率，表示屏幕分辨率的有效成员，他们分别表示屏幕宽度、屏幕高度、每个像素的位数
        dmScreenSettings.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;

        //尝试设置屏幕分辨率
        //ChangeDisplaySettings函数用于设置显示设备的显示模式。
        //参数1：指向DEVMODE结构的指针，定义了显示设备的显示模式。
        //参数2：标志，表示显示模式的变化方式。CDS_FULLSCREEN表示全屏模式。其他还有CDS_NO_WARNINGBAR，表示不显示警告栏。
        //返回值：如果函数成功，则返回DISP_CHANGE_SUCCESSFUL，否则返回DISP_CHANGE_FAILED。
        ChangeDisplaySettings(&dmScreenSettings, CDS_FULLSCREEN);
        //设置窗口左上角在屏幕上的X坐标和Y坐标
        posX = 0;
        posY = 0;
    }  
    else
    {
        screenWidth = 800;
        screenHeight = 600;
        //计算窗口左上角在屏幕上的X坐标和Y坐标
        posX = (screenWidth - screenWidth) / 2;
        posY = (screenHeight - screenHeight) / 2;
    }


    //创建窗口
    //CreateWindowEx函数用于创建一个窗口。
    //参数1：扩展窗口样式，WS_EX_APPWINDOW表示应用程序窗口。
    //参数2：窗口类名称。
    //参数3：窗口标题。
    //参数4：窗口样式，WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_POPUP表示窗口可以剪切子窗口和兄弟窗口。
    //参数5：窗口左上角在屏幕上的X坐标。
    //参数6：窗口左上角在屏幕上的Y坐标。
    //参数7：窗口宽度。
    //参数8：窗口高度。
    //参数9：父窗口句柄，nullptr表示没有父窗口。
    //参数10：菜单句柄，nullptr表示没有菜单。
    //参数11：应用程序实例句柄。
    //参数12：传递给窗口过程的参数，nullptr表示没有传递参数。
    m_hwnd = CreateWindowEx(WS_EX_APPWINDOW, m_applicationName, m_applicationName, WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_POPUP, posX, posY, screenWidth, screenHeight, nullptr, nullptr, m_hinstance, nullptr);
    
    //显示窗口
    //ShowWindow函数用于显示一个窗口。
    //参数1：窗口句柄。
    //参数2：显示方式，SW_SHOW表示显示窗口。
    ShowWindow(m_hwnd, SW_SHOW);

    //设置窗口为前台窗口
    SetForegroundWindow(m_hwnd);

    //设置窗口为当前活动窗口
    SetFocus(m_hwnd);

    //显示鼠标光标
    ShowCursor(true);
}

//ShutdownWindows就是这么做的。它将屏幕设置返回到正常状态，并释放窗口和与之关联的句柄。
void System::ShutdownWindows()
{
    //隐藏鼠标光标
    ShowCursor(false);
    

    if(FULL_SCREEN)
    {
        //这里我们使用ChangeDisplaySettings函数来恢复显示模式。
        //参数1：nullptr表示恢复到默认显示模式。
        //参数2：0表示不使用任何标志。
        ChangeDisplaySettings(nullptr, 0);
    }

    //销毁窗口
    DestroyWindow(m_hwnd);
    m_hwnd = nullptr;
    
    //注销窗口类
    UnregisterClass(m_applicationName, m_hinstance);
    m_hinstance = nullptr;
    ApplicationHandle = nullptr;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT umessage, WPARAM wparam, LPARAM lparam)
{
    switch (umessage)
    {
        case WM_DESTROY:
        {
            PostQuitMessage(0);
            return 0;
        }
        case WM_CLOSE:
        {
            PostQuitMessage(0);
            return 0;
        }
        default:
        {
            return ApplicationHandle -> MessageHandler(hwnd, umessage, wparam, lparam);
        }
    }
}       


