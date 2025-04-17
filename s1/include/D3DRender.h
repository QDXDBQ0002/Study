#ifndef _D3DRENDER_H_
#define _D3DRENDER_H_

//第一个库包含所有Direct3D功能，用于在DirectX 11中设置和绘制3D图形。
//第二个库包含与计算机上的硬件接口的工具，以获取有关监视器的刷新率、正在使用的视频卡等信息。
//第三个库包含编译着色器的功能
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3dcompiler.lib")

#include <d3d11.h>
#include <DirectxMath.h>
using namespace DirectX;

class D3DRender
{
private:
    bool m_vsync_enabled;
    int m_videoCardMemory;
    char m_videoCardDescription[128];
    IDXGISwapChain* m_swapChain;
    ID3D11Device* m_device;
    ID3D11DeviceContext* m_deviceContext;
    ID3D11RenderTargetView* m_renderTargetView;
    ID3D11Texture2D* m_depthStencilBuffer;
    ID3D11DepthStencilState* m_depthStencilState;
    ID3D11DepthStencilView* m_depthStencilView;
    ID3D11RasterizerState* m_rasterState;
    XMMATRIX m_projectionMatrix;
    XMMATRIX m_worldMatrix;
    XMMATRIX m_orthoMatrix;
    D3D11_VIEWPORT m_viewport;
    

public:
    D3DRender();
    D3DRender(const D3DRender&);
    ~D3DRender();
    bool Initialize(int&, int&, bool, HWND, bool, float, float);
    void Shutdown();
    void BeginScene(float, float, float, float);
    void EndScene();
    void Render();
    ID3D11Device* GetDevice();
    ID3D11DeviceContext* GetDeviceContext();
    void GetProjectionMatrix(XMMATRIX&);
    void GetWorldMatrix(XMMATRIX&);
    void GetOrthoMatrix(XMMATRIX&);
    void GetVideoCardInfo(char*, int&);
    void SetBackBufferRenderTarget();
    void ResetViewport();
    

};

#endif