#ifndef _SYSTEMCLASS_H_
#define _SYSTEMCLASS_H_
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include "Input.h"
#include "Application.h"

class System
{
    private:
        LPCSTR m_applicationName;
        HINSTANCE m_hinstance;
        HWND m_hwnd;
        Input* m_Input;
        Application* m_Application;
        bool Frame();
        void InitializeWindows(int&, int&);
        void ShutdownWindows();
    public:
        System();
        System(const System&);
        ~System();
        bool Initialize();
        void Shutdown();
        void Run();
        LRESULT CALLBACK MessageHandler(HWND, UINT, WPARAM, LPARAM);
};

static LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

static System* ApplicationHandle = 0;

#endif