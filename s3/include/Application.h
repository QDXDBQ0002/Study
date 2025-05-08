#ifndef APPLICATION_H
#define APPLICATION_H
#include <windows.h>

const bool FULL_SCREEN = false;
const bool VSYNC_ENABLED = true;
const float SCREEN_DEPTH = 1000.0f;
const float SCREEN_NEAR = 0.1f;

class Application
{
private:
    bool Render();
public:
    Application(/* args */);
    Application(const Application& other);
    ~Application();
    bool Initialize(int screenWidth, int screenHeight, HWND hwnd);
    void Shutdown();
    bool Frame();
};

#endif
