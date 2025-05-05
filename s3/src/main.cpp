
/*
    需要渲染三角形，需要如下几步：
    1.告诉GPU如何渲染我们的几何
    2.创建三角形的三个顶点
    3.将这些顶点存储在显存中
    4.告诉GPU如何读取这些点
    5.渲染三角形
*/

/*
    使用shader
    顶点shader对每个顶点运行一次，像素shader对每个绘制的像素都运行一次
    为了渲染图像，我们必须将某些shader架子啊到GPU。加载shader需要几个步骤：
    1.从.shader文件中加载并编译shader
    2.将这个两个shader封装到shader对象中
    3.将这个两个shader都设置为活动的shader
*/

/*
    1.从.shader文件中加载并编译shader
    在这一步中，我们将编译顶点shader和像素shader，这是渲染所需的两个shader。要加载和编译shader，我们必须使用D3D11CompileFromFile函数。
    这个函数有大量的参数，但其中大部分都是高级参数，可以先设置为0
    D3DCompileFromFile()的方法原型是 
    D3DCompileFromFile(_In_ LPCWSTR pFileName,  //shader的文件名
                   _In_reads_opt_(_Inexpressible_(pDefines->Name != NULL)) CONST D3D_SHADER_MACRO* pDefines, //shader的宏定义
                   _In_opt_ ID3DInclude* pInclude, //shader的包含文件
                   _In_ LPCSTR pEntrypoint, //shader的入口函数
                   _In_ LPCSTR pTarget, //shader的类型
                   _In_ UINT Flags1, //shader的编译选项
                   _In_ UINT Flags2, //shader的编译选项
                   _Out_ ID3DBlob** ppCode, //编译后的shader,这个参数指向一个blob对象，这个对象将会用shader的编译代码填充。blob是一个精巧的COM对象，
                   //用于存储数据缓冲区，我们可以使用GetBufferPointer()和GetBufferSize()方法来获取他的内容
                   _Always_(_Outptr_opt_result_maybenull_) ID3DBlob** ppErrorMsgs  //编译错误信息);
    这个函数需要一个文件名，一个shader类型和一个编译选项。我们将使用D3DCOMPILE_ENABLE_STRICTNESS选项来确保我们不会在shader中犯错误。
    我们还将使用D3DCOMPILE_DEBUG选项来启用调试信息，这样我们就可以在调试器中查看shader的源代码。
    他将加载shader，并找到对应的入口函数，将其编译后存入ppCode指向的ID3DBlob对象中。
*/
/*
    2.将这个两个shader封装到shader对象中
    我们将使用ID3D11Device::CreateVertexShader()和ID3D11Device::CreatePixelShader()方法来创建shader对象。
    这两个方法的第一个参数是已编译的shader的数据地址，第二个是数据大小，第三个是一个类链接对象，第四个是一个指向ID3D11VertexShader或ID3D11PixelShader对象的指针。
    这两个方法都需要一个ID3DBlob对象作为参数，这个对象就是我们在上一步中创建的。
    这两个方法还需要一个ID3D11ClassLinkage对象作为参数，这个对象是一个空指针，因为我们不需要类链接。
*/
/*
    3.将这个两个shader都设置为活动的shader
    我们将使用ID3D11DeviceContext::VSSetShader()和ID3D11DeviceContext::PSSetShader()方法来设置活动的shader。
    这两个方法的第一个参数是一个指向ID3D11VertexShader或ID3D11PixelShader对象的指针，第二个参数是一个类链接对象，第三个参数是一个标志。
    这两个方法都需要一个ID3D11ClassLinkage对象作为参数，这个对象是一个空指针，因为我们不需要类链接。
*/

/*
    顶点缓冲区
    顶点就是3D空间中一个准确的位置和属性。顶点的位置是表示顶点坐标的三个数值，顶点的属性可以有很多，比如颜色、纹理坐标、法线等。
    D3D使用一种被成为input layout(输入布局)的数据格式来表示顶点的位置和属性的布局，我们可以根据需要进行修改和设置。
    在设置输入布局的时候，可以选择需要发送的信息，比如位置、颜色、纹理坐标等。我们可以使用D3D11_INPUT_ELEMENT_DESC结构体来描述输入布局。
*/

#include <Windows.h>
#include <tchar.h>
#include <d3d11.h>
#include <d3dcompiler.h>

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "d3dcompiler.lib")

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
void InitD3D(HWND hwnd);
void CleanupD3D();
void RenderFrame();

IDXGISwapChain* g_pSwapChain = NULL;
ID3D11Device* g_pd3dDevice = NULL;
ID3D11DeviceContext* g_pImmediateContext = NULL;
ID3D11RenderTargetView* g_pRenderTargetView = NULL;

ID3D11VertexShader* g_pVertexShader = NULL;
ID3D11PixelShader* g_pPixelShader = NULL;


int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    WNDCLASSEX wc;
    ZeroMemory(&wc, sizeof(WNDCLASSEX));
    wc.cbSize = sizeof(WNDCLASSEX);
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = WindowProc;
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hInstance = hInstance;
    wc.lpszClassName = _T("WindowClass");
    RegisterClassEx(&wc);

    RECT rc = { 0, 0, 800, 600 };
    AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW, FALSE);
    HWND hwnd = CreateWindowEx(
        0,
        _T("WindowClass"),
        _T("Direct3D 11 Example"),
        WS_OVERLAPPEDWINDOW,
        0, 0, rc.right - rc.left, rc.bottom - rc.top,
        NULL,
        NULL,
        hInstance,
        NULL
    );
    ShowWindow(hwnd, nCmdShow);
    InitD3D(hwnd);
    MSG msg;
    while(true)
    {
        if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
            if (msg.message == WM_QUIT)
                break;
            RenderFrame();
        }
    }
    CleanupD3D();
    return msg.wParam;
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;
    }
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

void InitD3D(HWND hwnd)
{
    DXGI_SWAP_CHAIN_DESC swapDesc;
    ZeroMemory(&swapDesc, sizeof(DXGI_SWAP_CHAIN_DESC));
    swapDesc.BufferCount = 1;
    swapDesc.BufferDesc.Width = 800;
    swapDesc.BufferDesc.Height = 600;
    swapDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    swapDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    swapDesc.OutputWindow = hwnd;
    swapDesc.SampleDesc.Count = 1;
    swapDesc.Windowed = TRUE;

    D3D11CreateDeviceAndSwapChain(NULL, 
                                   D3D_DRIVER_TYPE_HARDWARE, 
                                   NULL, 
                                   0, 
                                   NULL, 
                                   0, 
                                   D3D11_SDK_VERSION, 
                                   &swapDesc, 
                                   &g_pSwapChain, 
                                   &g_pd3dDevice, 
                                   NULL, 
                                   &g_pImmediateContext);
    ID3D11Texture2D* pBackBuffer;
    g_pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBuffer);
    g_pd3dDevice->CreateRenderTargetView(pBackBuffer, NULL, &g_pRenderTargetView);
    pBackBuffer->Release();
    g_pImmediateContext->OMSetRenderTargets(1, &g_pRenderTargetView, NULL);
    D3D11_VIEWPORT viewport;
    viewport.Width = 800;
    viewport.Height = 600;
    viewport.TopLeftX = 0;
    viewport.TopLeftY = 0;
    g_pImmediateContext->RSSetViewports(1, &viewport);
}

void CleanupD3D()
{
    g_pRenderTargetView->Release();
    g_pSwapChain->Release();
    g_pd3dDevice->Release();
    g_pImmediateContext->Release();
    g_pVertexShader->Release();
    g_pPixelShader->Release();
}

void RenderFrame()
{
    float color[4] = { 0.0f, 0.0f, 1.0f, 1.0f };
    g_pImmediateContext->ClearRenderTargetView(g_pRenderTargetView, color);
    g_pSwapChain->Present(0, 0);
}

void InitPipeline()
{
    ID3D10Blob *VS, *PS;
    D3DCompileFromFile(L"point.shader", NULL, NULL, "main", "vs_5_0", 0, 0, &VS, NULL);
    D3DCompileFromFile(L"pixel.shader", NULL, NULL, "main", "ps_5_0", 0, 0, &PS, NULL);

    g_pd3dDevice -> CreateVertexShader(VS->GetBufferPointer(), VS->GetBufferSize(), NULL, &g_pVertexShader);
    g_pd3dDevice -> CreatePixelShader(PS->GetBufferPointer(), PS->GetBufferSize(), NULL, &g_pPixelShader);

    g_pImmediateContext->VSSetShader(g_pVertexShader, NULL, 0);
    g_pImmediateContext->PSSetShader(g_pPixelShader, NULL, 0);
}