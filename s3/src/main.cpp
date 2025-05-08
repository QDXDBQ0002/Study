#include "System.h"

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    System* system = new System();
    bool result = system->Initialize();
    if (result)
    {
        system->Run();
    }
    system->Shutdown();
    delete system;
    system = nullptr;
    return 0;
}
