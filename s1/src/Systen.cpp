#include "System.h"

System::System() : m_Input(0), m_Application(0)
{
}

System::~System()
{

}

bool System::Initialize()
{
    int screenWidth = 0;
    int screenHeight = 0;
    bool result;

    InitializeWindows( screenWidth, screenHeight);

    m_Input = new Input();
    m_Input->Initialize();

    m_Application = new Application();
    result = m_Application->Initialize(screenWidth, screenHeight, m_hwnd);
    if(!result)
    {
        return false;
    }
    return true;
}
