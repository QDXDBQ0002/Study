#include "Application.h"
#include <d3d11.h>
#include <DirectXMath.h>
using namespace DirectX;

Application::Application() : m_Direct3D(0)
{
}

Application::Application(const Application& other)
{

}

Application::~Application()
{

}

bool Application::Initialize(int& screenWidth, int& screenHeight, HWND hwnd)
{
    m_Direct3D = new D3DRender();
    bool result = m_Direct3D->Initialize(screenWidth, screenHeight, VSYNC_ENABLED, hwnd, FULL_SCREEN, SCREEN_DEPTH, SCREEN_NEAR);
    if(!result)
    {
        MessageBoxW(hwnd, L"Could not initialize Direct3D.", L"Error", MB_OK);
        return false;
    }
    return true;    
}

void Application::Shutdown()
{
    if(m_Direct3D)
    {
        m_Direct3D->Shutdown();
        delete m_Direct3D;
        m_Direct3D = 0;
    }
}

bool Application::Frame()
{
    bool result;
    result = Render();
    if(!result)
    {
        return false;
    }
    return true;
}

bool Application::Render()
{
    m_Direct3D->BeginScene(0.5f, 0.5f, 0.5f, 1.0f);
    m_Direct3D->EndScene();
    return true;
}   






