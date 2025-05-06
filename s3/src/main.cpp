
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
    顶点是由结构体组成的，创建任何一种3D图形都需要一个结构体来提供相关的数据。为了显示图像，我们要将所有的信息复制到GPU，然后命令Direct3D将这些数据渲染到后台缓冲区
    
    在设置输入布局的时候，可以选择需要发送的信息，比如位置、颜色、纹理坐标等。我们可以使用D3D11_INPUT_ELEMENT_DESC结构体来描述输入布局。
*/

//创建顶点信息
/*
    顶点通常使用结构体创建，我们一个在结构体中以任何格式放置有关这个结构的任何数据
    例如，如果我们希望每个顶点都包含一个位置和一个颜色，则可以构建一下格式的结构
*/

/*
    创建一个顶点缓冲区
        在C++中吹昂见结构时，数据存储在系统内存中，但是我们需要将其复制到GPU内存中，但是我们不能直接访问显存。为了让我们访问显存，D3D提供了一个特定的COM对象，
        该对象将在系统内存和显存中格子维护一个缓冲区。如何在系统内存和显存中都维护一个缓冲区呢？首先，将数据存储在系统内存的缓冲区中，当需要渲染时，D3D自动将
        其复制到显存的缓冲区中，如果显存不足，D3D将删除一段时间都未使用的缓冲区或被是为”低优先级“的缓冲区，为新的资源腾出空间
        为什么需要D3D来帮助访问显存呢？因为这是一个非常复杂的过程，根据显卡和操作系统的不同，访问显存的方式也不同，让D3D来处理这个问题是最好的选择
        这个COM对象就是ID3D11Buffer对象，ID3D11Buffer对象是一个缓冲区对象，它可以在系统内存和显存中都维护一个缓冲区。
        我们可以使用ID3D11Device::CreateBuffer()方法来创建一个ID3D11Buffer对象。
*/
/*
    填充顶点缓冲区
        我们现在有了三角形的三个顶点，并且有一个顶点缓冲区可以放置他们，现在我们要做的就是将顶点复制到缓冲区。但是，由于D3D可能在后台使用缓冲区，因此他并没有给
        CPU直接访问缓冲区的权限。为了让CPU可以访问缓冲区，我们必须将缓冲区进行映射，这意味着在映射期间，D3D允许对缓冲区执行任何操作，直到取消映射之后，再阻止
        GPU使用缓冲区。因此，为了填充顶点缓冲区，我们需要采取一下步骤：
        1.映射顶点缓冲区(并由此获得缓冲区的位置)
        2.将数据复制到缓冲区(使用mencpy函数)
        3.取消映射缓冲区
*/
/*
    验证输入布局
        本节到目前为止，我们已经知道:1.加载并设置shader以控制管道；2.通过使用顶点来创建形状，并将其准备好供GPU使用。当我们将顶点放在我们自己创建的结构中时，GPU
        如何知道我们将顶点的位置放在颜色前面？GPU如何知道我们没有其他的意思？GPU如何读取我们的顶点？这一切的答案就是输入布局。如前所述，我们能够选择再每个顶点中
        存储哪些信息，以提高渲染速度。输入布局时一个包含顶点结构的布局对象，可以让GPU适当切有效的组织数据。ID3D11InputLayout对象就是存储了我们的VERTEX结构提的
        布局，要创建这个对象，需要调用CreateInputLayout()方法。这里主要由两个部分：1.首先需要定义顶点的每个元素；2.其次需要创建输入布局对象。
*/
/*
    创建输入元素
        顶点布局由一个或多个输入元素组成，输入元素是顶点的一种属性，例如位置和颜色。每个输入元素由一个称为D3D11_INPUT_ELEMENT_DESC的结构体描述。
        这个结构体包含了输入元素的名称、格式、大小和偏移量等信息。我们可以使用D3D11_INPUT_ELEMENT_DESC结构体来描述输入元素。
        这个结构体共有七个成员变量：
        1.称为语义，语义是一个字符串，他告诉GPU该值的而用途，此外还有一些其他的语义值，我们稍后进行介绍
        2.语义索引，例如如果我们的顶点具有两种颜色属性，则他们都将使用COLOR语义，为了避免混淆，我们希望每个属性再此处具有不同的编码
        3.数据格式，再许多语义中，值得数量是任意的,重要的是格式和我们再顶点着色器中的格式相匹配
        4.输入插槽，这是高级成员，稍后会进行介绍
        5.偏移量，表示输入元素进入结构体中的字节数，例如VERTEX结构中的位置是从0开始的，颜色是从12开始的。实际上可以直接使用D3D11_APPEND_ALIGNED_ELEMENT宏来自动计算偏移量
        6.表示输入元素用作什么，这里由两个可选的标志，D3D11_INPUT_PER_VERTEX_DATA和D3D11_INPUT_PER_INSTANCE_DATA，表示输入元素是每个顶点数据还是每个实例数据
        7.第七个成员是输入元素的实例数据步长，这里我们不需要使用，所以设置为0
*/

/*
    创建输入布局对象
        创建输入布局对象需要调用ID3D11Device::CreateInputLayout()方法，这个方法的第一个参数是一个指向D3D11_INPUT_ELEMENT_DESC结构体的指针，
        第二个参数是一个整数，表示输入的数据有哪些东西，比如VERTEX结构中就有位置和颜色，这里就填2
        第三个参数是指向管道中第一个shader(也就是顶点shader)的指针，我们使用GetBufferPointer()方法来获取这个指针
        第四个参数表示shader文件的长度，我们使用GetBufferSize()方法来获取
        第五个参数是一个指向ID3D11InputLayout对象的指针，这个对象就是我们要创建的输入布局对象
    创建输入布局对象后不会对其进行任何设置，要设置输入布局，我们需要调用IASetInputLayout方法，他唯一的参数就是输入布局对象
*/
/*
    绘制图元
        我们调用三个简单的函数来进行渲染，第一个函数设置了我们打算使用哪个顶点缓冲区，第二个函数设置了我们打算使用那种类型的图元，例如三角形列表、线带等，
        第三个函数才真正开始绘制形状
        这三个函数分别是IASetVertexBuffers()、IASetPrimitiveTopology()和Draw()方法
*/
/*
    1.IASetVertexBuffers()方法的第一个参数是缓冲区的索引，第二个参数是缓冲区的数量，用于告诉GPU我们要设置多少个缓冲区，
        第三个参数是一个指向顶点缓冲区数组的指针，第四个参数是一个指向UINT类型的指针，该指针存储了单个顶点的尺寸大小，要填充此参数，我们可以使用sizeof(VERTEX)来获取顶点的大小
        第五个参数是一个uint类型的指针，表示缓冲区的偏移量，这里我们设置为0
*/
/*
    2.IASetPrimitiveTopology()方法的参数是一个D3D11_PRIMITIVE_TOPOLOGY类型的值，这个值表示我们要绘制的图元类型，例如三角形列表、线带等
        我们使用D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST来表示我们要绘制的是三角形列表
        其他的还可以使用D3D11_PRIMITIVE_TOPOLOGY_LINELIST表示线段列表，D3D11_PRIMITIVE_TOPOLOGY_POINTLIST表示点列表
        还有D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP表示三角形带，D3D11_PRIMITIVE_TOPOLOGY_LINELIST_ADJ表示线段列表的附加线段
        还有D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST_ADJ表示三角形列表的附加三角形
*/
/*
    3.Draw()方法的第一个参数是要绘制的顶点数量，第二个参数是绘制的起始顶点索引
    例如，如果我们要绘制一个三角形，我们需要绘制3个顶点，所以我们将第一个参数设置为3，第二个参数设置为0
    如果我们要绘制一个线段，我们需要绘制2个顶点，所以我们将第一个参数设置为2，第二个参数设置为0
    如果我们要绘制一个点，我们需要绘制1个顶点，所以我们将第一个参数设置为1，第二个参数设置为0
*/

#include <Windows.h>
#include <tchar.h>
#include <d3d11.h>
#include <d3dcompiler.h>
#include <iostream>

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "d3dcompiler.lib")

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
void InitD3D(HWND hwnd);
void CleanupD3D();
void RenderFrame();
void InitPipeline();
void InitGraphics();

IDXGISwapChain* g_pSwapChain = NULL;
ID3D11Device* g_pd3dDevice = NULL;
ID3D11DeviceContext* g_pImmediateContext = NULL;
ID3D11RenderTargetView* g_pRenderTargetView = NULL;

ID3D11VertexShader* g_pVertexShader = NULL;
ID3D11PixelShader* g_pPixelShader = NULL;

ID3D11Buffer* g_pVertexBuffer;

ID3D11InputLayout* g_pInputLayout = NULL;

struct VERTEX
{
    float x, y, z;
    float r, g, b, a;
};


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
    InitPipeline();
    InitGraphics();
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
    UINT stride = sizeof(VERTEX); //每个顶点的大小
    UINT offset = 0;
    g_pImmediateContext->IASetVertexBuffers(0, 1, &g_pVertexBuffer, &stride, &offset);
    g_pImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    g_pImmediateContext->Draw(3, 0);
    g_pSwapChain->Present(0, 0);
}

void InitPipeline()
{
    ID3D10Blob *VS, *PS;
    D3DCompileFromFile(L"../shader/point.shader", NULL, NULL, "main", "vs_4_0", 0, 0, &VS, NULL);
    D3DCompileFromFile(L"../shader/pixel.shader", NULL, NULL, "main", "ps_4_0", 0, 0, &PS, NULL);

    g_pd3dDevice -> CreateVertexShader(VS->GetBufferPointer(), VS->GetBufferSize(), NULL, &g_pVertexShader);
    g_pd3dDevice -> CreatePixelShader(PS->GetBufferPointer(), PS->GetBufferSize(), NULL, &g_pPixelShader);

    g_pImmediateContext->VSSetShader(g_pVertexShader, NULL, 0);
    g_pImmediateContext->PSSetShader(g_pPixelShader, NULL, 0);

    D3D11_INPUT_ELEMENT_DESC layout[] =
    {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 }
    };
    g_pd3dDevice -> CreateInputLayout(layout, 2, VS->GetBufferPointer(), VS->GetBufferSize(), &g_pInputLayout);
    g_pImmediateContext -> IASetInputLayout(g_pInputLayout); //设置输入布局
}

void InitGraphics()
{
    //这是一个包含缓冲区设置的结构体
    VERTEX vertices[] =
    {
        {0.0f, 0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f} , //顶点1
        { 0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f} , //顶点2
        { -0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f}  //顶点3
    };
    D3D11_BUFFER_DESC bufferDesc;
    ZeroMemory(&bufferDesc, sizeof(D3D11_BUFFER_DESC));
    //缓冲区应该设置的尽可能搞笑，为了正确执行此操作，D3D需要知道我们打算如何访问它，在本例中，我们使用 动态用法 ， 即GPU只可写，CPU只可读
    bufferDesc.Usage = D3D11_USAGE_DYNAMIC; //CPU和GPU的读写访问权限
    //该值包含将要创建的缓冲区的大小，这与我们要放入其中的顶点数组的大小一样
    bufferDesc.ByteWidth = sizeof(vertices); //顶点缓冲区的大小，因为是绘制三角形，所以是三个顶点大小
    //该值告诉D3D创建那种缓冲区，我们可以创建几种类型的缓冲区，在这里我们需要创建的是顶点缓冲区，为此，我们将使用D3D11_BIND_VERTEX_BUFFER标志
    bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER; //绑定类型 用于顶点缓冲区
    //该值用于告诉D3D我们访问CPU的方式，这是对Usage的补充，我们在这使用D3D11_CPU_ACCESS_WRITE，因为我们要将数据冲系统内存复制到缓冲区中
    bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE; //CPU访问权限
    bufferDesc.MiscFlags = 0;
    bufferDesc.StructureByteStride = 0;
    //创建缓冲区的函数，第一个参数是描述缓冲区的结构体地址，第二个参数可用于在创建时使用某些数据来初始化缓冲区，但此处我们将其设置为NULL
    //第三个参数是缓冲区对象的地址
    D3D11_SUBRESOURCE_DATA initData;
    ZeroMemory(&initData, sizeof(D3D11_SUBRESOURCE_DATA));
    initData.pSysMem = vertices; //指向顶点数据的指针
    HRESULT hr = g_pd3dDevice -> CreateBuffer(&bufferDesc, &initData, &g_pVertexBuffer); //创建顶点缓冲区
    if (FAILED(hr))
    {
        //将hr转换为错误代码
        std::cout << "CreateBuffer failed: " << std::hex << hr << std::endl;
        return;
    }

    //这是一个结构体，一旦我们映射了他，就会填充有关缓冲区的信息，该信息将包含指向缓冲区位置的指针，我们可以使用mappedResource.pData来访问缓冲区
    D3D11_MAPPED_SUBRESOURCE mappedResource;
    ZeroMemory(&mappedResource, sizeof(D3D11_MAPPED_SUBRESOURCE));
    //这一行的作用是映射缓冲区，使我们可以访问他。Map函数的参数非常简单，第一个参数是缓冲区对象的地址，第二个参数是缓冲区的索引，
    //第三个参数是一组标志，使我们可以控制CPU再映射缓冲区期间对缓冲区访问，这里我们使用D3D11_MAP_WRITE_DISCARD，表示缓冲区中新写入的内容将覆盖旧得内容，
    //第四个参数是标志，在这里我们设置为0或者D3D11_MAP_FLAG_DO_NOT_WAIT，表示即使GPU仍在使用缓冲区，此标志也会强制程序继续运行，
    //第五个参数是指向D3D11_MAPPED_SUBRESOURCE结构体的指针
    hr = g_pImmediateContext -> Map(g_pVertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource); //映射缓冲区
    if (FAILED(hr))
    {
        //将hr转换为错误代码
        std::cout << "Map failed: " << std::hex << hr << std::endl;
        return;
    }
    //数据拷贝，使用mappedResource.pData作为目标，使用vertices作为源，使用sizeof(vertices)作为大小
    memcpy(mappedResource.pData, vertices, sizeof(vertices)); //将数据复制到缓冲区
    //取消映射缓冲区，这样可以降低GPU对缓冲区得访问，并重新阻塞CPU，第一个参数是缓冲区对象，第二个参数是高级参数
    g_pImmediateContext -> Unmap(g_pVertexBuffer, 0); //取消映射缓冲区
}