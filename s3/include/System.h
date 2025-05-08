#ifndef SYSTEM_H
#define SYSTEM_H

// 定义 UNICODE 和 _UNICODE 宏，确保使用 Unicode 版本的 Windows API
/*
    在 Windows 编程中，有两种方式定义 UNICODE 宏：
    1. 在代码中定义（推荐方式）：
    #define UNICODE
    #define _UNICODE
    2. 在项目设置中定义：
    在项目属性中，找到 C/C++ -> 预处理器 -> 预处理器定义，添加 UNICODE 和 _UNICODE。
    定义了UNICODE宏后，Windows API函数会使用Unicode版本，例如CreateWindowExW，而不是CreateWindowEx、RegisterClassEx等。
*/
#define UNICODE
#define _UNICODE

// 定义WIN32_LEAN_AND_MEAN宏，减少头文件的依赖
//这里我们定义WIN32_LEAN_AND_MEAN。我们这样做是为了加快构建过程，它通过排除一些较少使用的api来减少Win32头文件的大小。
#define WIN32_LEAN_AND_MEAN

//包含windows .h以便我们可以调用函数来创建/销毁窗口，并能够使用其他有用的win32函数。
#include <windows.h>
#include <tchar.h>

#include "Input.h"
#include "Application.h"

class System
{
private:
    bool Frame();
    void InitializeWindows(int &screenWidth, int &screenHeight);
    void ShutdownWindows();
    LPCWSTR m_applicationName;
    HINSTANCE m_hinstance;
    HWND m_hwnd;
    Input* m_Input;
    Application* m_Application;
public:
    System(/* args */);
    System(const System &other);
    ~System();
    bool Initialize();
    void Run();
    void Shutdown();
    LRESULT CALLBACK MessageHandler(HWND hwnd, UINT umsg, WPARAM wparam, LPARAM lparam);
};

//WndProc函数和ApplicationHandle指针也包含在这个类文件中，因此我们可以将windows系统消息传递重定向到System类中的MessageHandler函数中。

///方法原型
static LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

//全局变量
static System* ApplicationHandle = nullptr;

#endif
