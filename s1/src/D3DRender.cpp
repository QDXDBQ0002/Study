#include "D3DRender.h"

D3DRender::D3DRender()
{
    m_swapChain = 0;
    m_device = 0;
    m_deviceContext = 0;
    m_renderTargetView = 0;
    m_depthStencilBuffer = 0;
    m_depthStencilView = 0;
    m_depthStencilState = 0;
    m_rasterState = 0;
}

D3DRender::D3DRender(const D3DRender& other)
{
}

D3DRender::~D3DRender()
{
}

//给这个函数的screenWidth和screenHeight变量是我们在SystemClass中创建的窗口的宽度和高度。
//Direct3D将使用这些初始化和使用相同的窗口尺寸。
//hwnd变量是窗口的句柄。Direct3D需要这个句柄来访问之前创建的窗口。
//fullScreen是指我们是在窗口模式还是全屏模式下运行。Direct3D也需要这个来创建具有正确设置的窗口
//screenDepth和screenNear变量是我们将在窗口中渲染的3D环境的深度设置。
//vsync变量指示我们是希望Direct3D根据用户显示器刷新率来渲染还是尽可能快地渲染。
bool D3DRender::Initialize(int& screenWidth, int& screenHeight, bool vsync, HWND hwnd, bool fullScreen, float screenDepth, float screenNear)
{
    HRESULT result;
    IDXGIFactory* factory;
    IDXGIAdapter* adapter;
    IDXGIOutput* adapterOutput;
    unsigned int numModes, i, numerator, denominator;
    unsigned long long stringLength;
    DXGI_MODE_DESC* displayModeList;
    DXGI_ADAPTER_DESC adapterDesc;
    int error;
    DXGI_SWAP_CHAIN_DESC swapChainDesc;
    D3D_FEATURE_LEVEL featureLevel;
    ID3D11Texture2D* backBufferPtr;
    D3D11_TEXTURE2D_DESC depthBufferDesc;
    D3D11_DEPTH_STENCIL_DESC depthStencilDesc;
    D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;
    D3D11_RASTERIZER_DESC rasterDesc;
    float fieldOfView, screenAspect;

    m_vsync_enabled = vsync;

    //创建一个IDXGIFactory接口。这个接口将用于创建和管理Direct3D设备。
    result = CreateDXGIFactory(__uuidof(IDXGIFactory), (void**)&factory);
    if(FAILED(result))
    {
        return false;
    }

    //使用工厂为主图形接口（视频卡）创建适配器。
    result = factory->EnumAdapters(0, &adapter);
    if(FAILED(result))
    {
        return false;
    }

    //枚举主要适配器输出（监视器）。
    result = adapter->EnumOutputs(0, &adapterOutput);
    if(FAILED(result))
    {
        return false;
    }
    
    //获取适合适配器输出（监视器）的DXGI_FORMAT_R8G8B8A8_UNORM显示格式的模式数量。
    result = adapterOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &numModes, NULL);
    if(FAILED(result))
    {
        return false;
    }
    
    //创建一个列表来保存此监视器/显卡组合的所有可能的显示模式。
    displayModeList = new DXGI_MODE_DESC[numModes];
    if(!displayModeList)
    {
        return false;
    }
    
    //现在填充显示模式列表结构。
    result = adapterOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &numModes, displayModeList);
    if(FAILED(result))
    {
        return false;
    }
    

    //现在遍历所有显示模式，找到与屏幕宽度和高度匹配的显示模式。
    //当找到匹配项时，存储该监视器的刷新率的分子和分母。
    for(i=0; i<numModes; i++)
    {
        if(displayModeList[i].Width == (unsigned int)screenWidth)
        {
            if(displayModeList[i].Height == (unsigned int)screenHeight)
            {
                numerator = displayModeList[i].RefreshRate.Numerator; //刷新率分子
                denominator = displayModeList[i].RefreshRate.Denominator; //刷新率分母
            }
        }
    }

    //获取适配器描述。
    result = adapter->GetDesc(&adapterDesc);
    if(FAILED(result))
    {
        return false;
    }
    
    //将显卡内存从字节转换为MB。
    m_videoCardMemory = (int)(adapterDesc.DedicatedVideoMemory / 1024 / 1024);

    //将显卡描述字符串存储到m_videoCardDescription。 将显卡的名称转换为字符数组并存储它。
    error = wcstombs_s(&stringLength, m_videoCardDescription, 128, adapterDesc.Description, 128);
    if(error != 0)
    {
        return false;
    }

    //释放显示模式列表。
    delete [] displayModeList;
    displayModeList = 0;

    //释放适配器输出。
    adapterOutput->Release();
    adapterOutput = 0;
    
    //释放适配器。
    adapter->Release();
    adapter = 0;

    //释放工厂。
    factory->Release();
    factory = 0;

    //初始化DirectX，第一步就是设置交换链
    //设置交换链描述。 交换链是将图形绘制的前后缓冲区。
    //一般来说，你使用一个后面的缓冲区，对它做所有的绘图，然后把它交换到前面的缓冲区然后显示在用户的屏幕上。这就是为什么它被称为交换链。
    ZeroMemory(&swapChainDesc, sizeof(swapChainDesc));

    //设置为单个后缓冲区。
    swapChainDesc.BufferCount = 1;
    
    //设置后缓冲区的宽高
    swapChainDesc.BufferDesc.Width = screenWidth;
    swapChainDesc.BufferDesc.Height = screenHeight;

    //设置后缓冲区为DXGI_FORMAT_R8G8B8A8_UNORM格式。
    swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    
    if(m_vsync_enabled)
    {
        //如果垂直同步启用，则将刷新率设置为显示器的刷新率。
        swapChainDesc.BufferDesc.RefreshRate.Numerator = numerator;
        swapChainDesc.BufferDesc.RefreshRate.Denominator = denominator;
    }
    else
    {
        //如果垂直同步禁用，则将刷新率设置为0,这将不限制帧速率。
        swapChainDesc.BufferDesc.RefreshRate.Numerator = 0;
        swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
    }
    
    //设置后缓冲区为DXGI_USAGE_RENDER_TARGET_OUTPUT，这意味着后缓冲区将用于输出渲染。
    swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;

    //设置交换链的窗口句柄。
    swapChainDesc.OutputWindow = hwnd;

    //不进行多重采样。
    swapChainDesc.SampleDesc.Count = 1;
    swapChainDesc.SampleDesc.Quality = 0;

    if(fullScreen)
    {
        //如果全屏模式，则设置为全屏。
        swapChainDesc.Windowed = false;
    }
    else
    {
        //如果窗口模式，则设置为窗口。
        swapChainDesc.Windowed = true;
    }

    //设置交换链的扫描线顺序 将扫描线排序和缩放设置为未指定。
    swapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
    swapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

    //设置交换链的交换效果。 
    swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

    //设置交换链的标志。这通常是0。
    swapChainDesc.Flags = 0;

    //设置特征等级。
    //在设置交换链描述之后，我们还需要设置另一个称为特性级别的变量。
    //这个变量告诉DirectX我们计划使用哪个版本。这里我们将特性级别设置为11.0，即DirectX 11。
    //如果您计划支持多个版本或在低端硬件上运行，则可以将此设置为10或9以使用较低级别的DirectX版本。
    featureLevel = D3D_FEATURE_LEVEL_11_0;
    
    /*既然交换链描述和特性级别已经填写完毕，我们就可以创建交换链、Direct3D设备和Direct3D设备上下文。
    Direct3D设备和Direct3D设备上下文非常重要，它们是所有Direct3D功能的接口。
    从现在开始，我们将使用设备和设备上下文来处理几乎所有的事情。
    注意，如果用户没有DirectX 11显卡，这个函数调用将无法创建设备和设备上下文。
    此外，如果你正在测试DirectX 11功能，自己没有DirectX 11显卡，那么你可以用D3D_DRIVER_TYPE_REFERENCE替换D3D_DRIVER_TYPE_HARDWARE， DirectX将使用你的CPU来绘制而不是显卡硬件
    。请注意，这运行1/1000的速度，但它是很好的人谁没有DirectX 11视频卡尚未在所有的机器。 
    */
    result = D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, 0, &featureLevel, 1,
    D3D11_SDK_VERSION, &swapChainDesc, &m_swapChain, &m_device, NULL, &m_deviceContext);
    if(FAILED(result))
    {
        return false;
    }

    /*
    如果主显卡与DirectX 11不兼容，有时这个创建设备的调用将失败。
    有些机器可能将主卡作为DirectX 10视频卡，而将副卡作为DirectX 11视频卡。
    此外，一些混合显卡也是这样工作的，主要是低功率的英特尔显卡，次要是高功率的英伟达显卡。
    为了解决这个问题，你不需要使用默认设备，而是枚举机器中的所有显卡，让用户选择使用哪一张，然后在创建设备时指定那张卡。
    */

   /*
   现在我们有了一个交换链，我们需要获得一个指向后缓冲区的指针，然后将其附加到交换链上。我们将使用CreateRenderTargetView函数将后缓冲区附加到交换链上。
   */

    //获取指向后缓冲区的指针。
    result = m_swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&backBufferPtr);
    if(FAILED(result))
    {
        return false;
    }

    //使用CreateRenderTargetView函数将后缓冲区附加到交换链上。
    result = m_device->CreateRenderTargetView(backBufferPtr, NULL, &m_renderTargetView);
    if(FAILED(result))
    {
        return false;
    }
    
    //释放后缓冲区。
    backBufferPtr->Release();
    backBufferPtr = 0;

    //我们还需要设置一个深度缓冲区描述。我们将使用它来创建一个深度缓冲，以便我们的多边形可以在3D空间中正确渲染。
    //同时，我们将在深度缓冲区上附加一个模板缓冲区。模板缓冲可以用来实现运动模糊、体积阴影等效果。

    //初始化深度缓冲
    ZeroMemory(&depthBufferDesc, sizeof(depthBufferDesc));
    depthBufferDesc.Width = screenWidth;
    depthBufferDesc.Height = screenHeight;
    depthBufferDesc.MipLevels = 1;
    depthBufferDesc.ArraySize = 1;
    depthBufferDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    depthBufferDesc.SampleDesc.Count = 1;
    depthBufferDesc.SampleDesc.Quality = 0;
    depthBufferDesc.Usage = D3D11_USAGE_DEFAULT;
    depthBufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
    depthBufferDesc.CPUAccessFlags = 0;
    depthBufferDesc.MiscFlags = 0;

    //现在我们使用该描述创建深度/模板缓冲区。你会注意到我们使用CreateTexture2D函数来制作缓冲区，因此缓冲区只是一个2D纹理。
    //这样做的原因是，一旦你的多边形被排序，然后栅格化，它们就会在这个2D缓冲区中成为彩色像素。然后这个2D缓冲区被绘制到屏幕上。
    //创建深度缓冲区。
    result = m_device->CreateTexture2D(&depthBufferDesc, NULL, &m_depthStencilBuffer);
    if(FAILED(result))
    {
        return false;
    }

    //现在我们需要设置深度模板描述。这允许我们控制Direct3D将为每个像素做什么类型的深度测试。
    ZeroMemory(&depthStencilDesc, sizeof(depthStencilDesc));

    //启用深度测试。
    depthStencilDesc.DepthEnable = true;
    //设置深度缓冲区为只读。
    depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
    //设置深度测试的比较函数。
    depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;

    //启用模板测试。
    depthStencilDesc.StencilEnable = true;
    //设置模板读取掩码。
    depthStencilDesc.StencilReadMask = 0xFF;
    //设置模板写入掩码。
    depthStencilDesc.StencilWriteMask = 0xFF;
    //设置正面面片的模板失败操作。
    depthStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
    //设置正面面片的深度失败操作。
    depthStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
    //设置正面面片的模板通过操作。
    depthStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
    //设置正面面片的模板比较函数。
    depthStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

    //设置背面面片的模板失败操作。
    depthStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
    //设置背面面片的深度失败操作。
    depthStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
    //设置背面面片的模板通过操作。
    depthStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
    //设置背面面片的模板比较函数。
    depthStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

    //现在我们使用该描述创建深度/模板缓冲区。填好描述后，我们现在可以创建深度模板状态。
    result = m_device->CreateDepthStencilState(&depthStencilDesc, &m_depthStencilState);
    if(FAILED(result))
    {
        return false;
    }
    
    //有了创建的深度模板状态，我们现在可以设置它，使其生效。注意，我们使用设备上下文来设置它。
    m_deviceContext->OMSetDepthStencilState(m_depthStencilState, 1);
    
    //我们需要创建的下一件事是深度模板缓冲区的视图描述。我们这样做是为了让Direct3D知道使用深度缓冲作为深度模板纹理。
    //在填写完描述之后，我们调用CreateDepthStencilView函数来创建它。
    //初始化深度模板缓冲区视图描述。
    ZeroMemory(&depthStencilViewDesc, sizeof(depthStencilViewDesc));
    //设置深度模板缓冲区视图的格式。
    depthStencilViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    //设置深度模板缓冲区视图的维度。
    depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
    //设置深度模板缓冲区视图的Mip切片。
    depthStencilViewDesc.Texture2D.MipSlice = 0;

    //创建深度模板缓冲区视图。
    result = m_device->CreateDepthStencilView(m_depthStencilBuffer, &depthStencilViewDesc, &m_depthStencilView);
    if(FAILED(result))
    {
        return false;
    }

    //创建了这个之后，我们现在可以调用OMSetRenderTargets。
    //这将把渲染目标视图和深度模板缓冲区绑定到输出渲染管道。这样，管道渲染的图形将被绘制到我们之前创建的后缓冲中。
    //将图形写入后端缓冲区后，我们可以将其交换到前端，并在用户屏幕上显示图形。
    m_deviceContext->OMSetRenderTargets(1, &m_renderTargetView, m_depthStencilView);

    //现在渲染目标已经设置好了，我们可以继续一些额外的功能，这将使我们在未来的教程中对场景有更多的控制。
    //首先我们会创建一个光栅化状态。这将使我们能够控制多边形的渲染方式。我们可以做一些事情，比如让我们的场景在线框模式下渲染，或者让DirectX绘制多边形的正面和背面。
    //默认情况下，DirectX已经设置了一个栅格化状态，并且与下面的栅格化状态完全相同，但是你无法控制它，除非你自己设置一个。

    //初始化光栅化描述。
    rasterDesc.AntialiasedLineEnable = false;
    //设置光栅化状态的背面剔除模式。
    rasterDesc.CullMode = D3D11_CULL_BACK;
    //设置光栅化状态的深度偏置。
    rasterDesc.DepthBias = 0;
    //设置光栅化状态的深度偏置钳位。
    rasterDesc.DepthBiasClamp = 0.0f;
    //设置光栅化状态的深度剪裁。
    rasterDesc.DepthClipEnable = true;
    //设置光栅化状态的填充模式。
    rasterDesc.FillMode = D3D11_FILL_SOLID;
    //设置光栅化状态的前向剔除模式。
    rasterDesc.FrontCounterClockwise = false;
    //设置光栅化状态的多重采样。
    rasterDesc.MultisampleEnable = false;
    //设置光栅化状态的剪裁。
    rasterDesc.ScissorEnable = false;
    //设置光栅化状态的斜率缩放深度偏置。
    rasterDesc.SlopeScaledDepthBias = 0.0f;

    //现在我们使用该描述创建光栅化状态。
    result = m_device->CreateRasterizerState(&rasterDesc, &m_rasterState);
    if(FAILED(result))
    {
        return false;
    }
    //将光栅化状态设置为当前光栅化状态。
    m_deviceContext->RSSetState(m_rasterState);

    //现在我们有了一个光栅化状态，我们可以设置视口。视口是我们在Direct3D中看到的区域。
    //它基本上是我们的渲染目标，我们将在其中绘制我们的图形。
    //初始化视口。
    m_viewport.Width = (float)screenWidth;
    m_viewport.Height = (float)screenHeight;

    //设置视口。
    m_deviceContext->RSSetViewports(1, &m_viewport);

    //设置投影矩阵。
    fieldOfView = 3.141592654f / 4.0f;
    screenAspect = (float)screenWidth / (float)screenHeight;

    //创建投影矩阵。
    m_projectionMatrix = XMMatrixPerspectiveFovLH(fieldOfView, screenAspect, screenNear, screenDepth);

    //初始化世界矩阵为单位矩阵。
    m_worldMatrix = XMMatrixIdentity();

    //创建正交投影矩阵。
    m_orthoMatrix = XMMatrixOrthographicLH((float)screenWidth, (float)screenHeight, screenNear, screenDepth);

    return true;

}

void D3DRender::Shutdown()
{
	// Before shutting down set to windowed mode or when you release the swap chain it will throw an exception.
	if(m_swapChain)
	{
		m_swapChain->SetFullscreenState(false, NULL);
	}

	if(m_rasterState)
	{
		m_rasterState->Release();
		m_rasterState = 0;
	}

	if(m_depthStencilView)
	{
		m_depthStencilView->Release();
		m_depthStencilView = 0;
	}

	if(m_depthStencilState)
	{
		m_depthStencilState->Release();
		m_depthStencilState = 0;
	}

	if(m_depthStencilBuffer)
	{
		m_depthStencilBuffer->Release();
		m_depthStencilBuffer = 0;
	}

	if(m_renderTargetView)
	{
		m_renderTargetView->Release();
		m_renderTargetView = 0;
	}

	if(m_deviceContext)
	{
		m_deviceContext->Release();
		m_deviceContext = 0;
	}

	if(m_device)
	{
		m_device->Release();
		m_device = 0;
	}

	if(m_swapChain)
	{
		m_swapChain->Release();
		m_swapChain = 0;
	}

	return;
}

void D3DRender::BeginScene(float red, float green, float blue, float alpha)
{
	float color[4];


	// Setup the color to clear the buffer to.
	color[0] = red;
	color[1] = green;
	color[2] = blue;
	color[3] = alpha;

	// Clear the back buffer.
	m_deviceContext->ClearRenderTargetView(m_renderTargetView, color);
    
	// Clear the depth buffer.
	m_deviceContext->ClearDepthStencilView(m_depthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);

	return;
}


void D3DRender::EndScene()
{
	// Present the back buffer to the screen since rendering is complete.
	if(m_vsync_enabled)
	{
		// Lock to screen refresh rate.
		m_swapChain->Present(1, 0);
	}
	else
	{
		// Present as fast as possible.
		m_swapChain->Present(0, 0);
	}

	return;
}

ID3D11Device* D3DRender::GetDevice()
{
	return m_device;
}


ID3D11DeviceContext* D3DRender::GetDeviceContext()
{
	return m_deviceContext;
}

void D3DRender::GetProjectionMatrix(XMMATRIX& projectionMatrix)
{
	projectionMatrix = m_projectionMatrix;
	return;
}


void D3DRender::GetWorldMatrix(XMMATRIX& worldMatrix)
{
	worldMatrix = m_worldMatrix;
	return;
}


void D3DRender::GetOrthoMatrix(XMMATRIX& orthoMatrix)
{
	orthoMatrix = m_orthoMatrix;
	return;
}

void D3DRender::GetVideoCardInfo(char* cardName, int& memory)
{
	strcpy_s(cardName, 128, m_videoCardDescription);
	memory = m_videoCardMemory;
	return;
}

void D3DRender::SetBackBufferRenderTarget()
{
	// Bind the render target view and depth stencil buffer to the output render pipeline.
	m_deviceContext->OMSetRenderTargets(1, &m_renderTargetView, m_depthStencilView);

	return;
}


void D3DRender::ResetViewport()
{
	// Set the viewport.
	m_deviceContext->RSSetViewports(1, &m_viewport);

	return;
}



