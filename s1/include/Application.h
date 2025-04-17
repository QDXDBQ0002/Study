#ifndef _APPLICATIONCLASS_H_
#define _APPLICATIONCLASS_H_
#include <windows.h>
#include "D3DRender.h"

const bool FULL_SCREEN = true;
const bool VSYNC_ENABLED = true;
const float SCREEN_DEPTH  = 1000.0f;
const float SCREEN_NEAR = 0.3f;

class Application
{
    private:
        bool Render();
        D3DRender* m_Direct3D;
    public:
        Application();
        Application(const Application&);
        ~Application();
        bool Initialize(int&, int&, HWND);
        void Shutdown();
        bool Frame();
};

#endif