#include "ColorShader.h"

ColorShader::ColorShader()
{
    m_vertexShader = 0;
    m_pixelShader = 0;
    m_layout = 0;
    m_matrixBuffer = 0;
}

ColorShader::ColorShader(const ColorShader& other)
{
}

ColorShader::~ColorShader()
{
}

bool ColorShader::Initialize(ID3D11Device* device, HWND hwnd)
{
    bool result;
    wchar_t vs_filename[128];
    wchar_t ps_filename[128];

    int error = wcscpy_s(vs_filename, L"../shaders/color.vs");
    if(error != 0)
    {
        return false;
    }
    error = wcscpy_s(ps_filename, L"../shaders/color.ps");
    if(error != 0)
    {
        return false;
    }
    result = InitializeShader(device, hwnd, vs_filename, ps_filename);
    if(!result)
    {
        return false;
    }
    return true;
}

void ColorShader::Shutdown()
{
    ShutdownShader();
}

//渲染将首先使用SetShaderParameters函数设置着色器内部的参数。一旦设置好参数，它就会调用RenderShader来使用HLSL着色器绘制绿色三角形。
bool ColorShader::Render(ID3D11DeviceContext* deviceContext, int indexCount, XMMATRIX worldMatrix, XMMATRIX viewMatrix, XMMATRIX projectionMatrix)
{
    bool result = SetShaderParameters(deviceContext, worldMatrix, viewMatrix, projectionMatrix);
    if(!result)
    {
        return false;
    }
    RenderShader(deviceContext, indexCount);
    return true;
}

//现在我们将从本教程中更重要的函数之一InitializeShader开始。
//这个函数实际上是加载着色器文件，并使其可用于DirectX和GPU。
//您还将看到布局的设置以及顶点缓冲区数据将如何在GPU中的图形管道上显示。
//布局将需要匹配modelclass.h文件中的VertexType以及在color.vs文件中定义的那个。
bool ColorShader::InitializeShader(ID3D11Device* device, HWND hwnd, WCHAR* vsFilename, WCHAR* psFilename)
{
    HRESULT result;
    ID3D10Blob* errorMessage = 0;
    ID3D10Blob* vertexShaderBuffer = 0;
    ID3D10Blob* pixelShaderBuffer = 0;
    D3D11_INPUT_ELEMENT_DESC polygonLayout[2];
    unsigned int numElements;
    D3D11_BUFFER_DESC matrixBufferDesc;

    //这里是我们将着色器程序编译到缓冲区的地方。
    //我们给它着色器文件的名称，着色器的名称，着色器版本（DirectX 11中的5.0），以及编译着色器的缓冲区。
    //如果编译着色器失败，它将在errorMessage字符串中放置一个错误消息，我们将该消息发送给另一个函数以写出错误。
    //如果它仍然失败并且没有errorMessage字符串，那么它意味着它找不到着色器文件，在这种情况下，我们弹出一个对话框。
    result = D3DCompileFromFile(vsFilename, NULL, NULL, "ColorVertexShader", "vs_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, &vertexShaderBuffer, &errorMessage);
    if(FAILED(result))
    {
        if(errorMessage)
        {
            OutputShaderErrorMessage(errorMessage, hwnd, vsFilename);
        }
        else
        {
            MessageBox(hwnd, vsFilename, L"Missing Shader File", MB_OK);
        }
        return false;
    }

    //接下来，我们编译像素着色器。
    result = D3DCompileFromFile(psFilename, NULL, NULL, "ColorPixelShader", "ps_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, &pixelShaderBuffer, &errorMessage);
    if(FAILED(result))
    {
        if(errorMessage)
        {
            OutputShaderErrorMessage(errorMessage, hwnd, psFilename);
        }
        else
        {
            MessageBox(hwnd, psFilename, L"Missing Shader File", MB_OK);
        }   
        return false;
    }

    
    //一旦顶点着色器和像素着色器代码成功编译成缓冲区，我们就可以使用这些缓冲区来创建着色器对象本身。从这一点开始，我们将使用这些指针来连接顶点和像素着色器。
    //现在我们创建顶点着色器。
    result = device->CreateVertexShader(vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), NULL, &m_vertexShader);
    if(FAILED(result))
    {
        return false;
    }

    //现在我们创建像素着色器。
    result = device->CreatePixelShader(pixelShaderBuffer->GetBufferPointer(), pixelShaderBuffer->GetBufferSize(), NULL, &m_pixelShader);
    if(FAILED(result))
    {
        return false;
    }

    //下一步是创建将由着色器处理的顶点数据的布局。由于这个着色器使用位置和颜色矢量，我们需要在布局中创建它们，并指定它们的大小
    //语义名称是在布局中填写的第一件事，这决定着色器确定布局中该元素的使用。
    //由于我们有两个不同的元素，第一个使用POSITION，第二个使用COLOR。
    //对于位置向量我们使用DXGI_FORMAT_R32G32B32_FLOAT，对于颜色我们使用DXGI_FORMAT_R32G32B32A32_FLOAT。
    //您需要注意的最后一件事是AlignedByteOffset，它指示数据如何在缓冲区中间隔。
    //对于这个布局，我们告诉它前12个字节是位置，接下来的16个字节是颜色，AlignedByteOffset显示每个元素开始的位置。
    //你可以使用D3D11_APPEND_ALIGNED_ELEMENT而不是把你自己的值放在AlignedByteOffset中，它会为你计算出间距。其他设置我现在默认设置，因为它们在本教程中不需要。
    //因为只会向顶点着色器传递数据，所以这里只需要填写一个布局。
    //现在我们填写布局。
    polygonLayout[0].SemanticName = "POSITION";
    //语义索引是0，因为我们只使用一个POSITION。
    polygonLayout[0].SemanticIndex = 0;
    //我们使用DXGI_FORMAT_R32G32B32_FLOAT，因为我们使用3个浮点值来表示位置。
    polygonLayout[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
    //输入槽是0，因为我们只使用一个输入槽。
    polygonLayout[0].InputSlot = 0;
    //AlignedByteOffset是0，因为我们从缓冲区开始位置开始。
    polygonLayout[0].AlignedByteOffset = 0;
    //输入槽类是D3D11_INPUT_PER_VERTEX_DATA，因为我们正在处理顶点数据。
    polygonLayout[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
    //InstanceDataStepRate是0，因为我们不使用实例化。
    polygonLayout[0].InstanceDataStepRate = 0;

    polygonLayout[1].SemanticName = "COLOR";
    //语义索引是0，因为我们只使用一个COLOR。
    polygonLayout[1].SemanticIndex = 0;
    //我们使用DXGI_FORMAT_R32G32B32A32_FLOAT，因为我们使用4个浮点值来表示颜色。
    polygonLayout[1].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
    //输入槽是0，因为我们只使用一个输入槽。
    polygonLayout[1].InputSlot = 0;
    //AlignedByteOffset是D3D11_APPEND_ALIGNED_ELEMENT，因为我们使用D3D11_APPEND_ALIGNED_ELEMENT来计算间距。
    polygonLayout[1].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
    //输入槽类是D3D11_INPUT_PER_VERTEX_DATA，因为我们正在处理顶点数据。
    polygonLayout[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
    //InstanceDataStepRate是0，因为我们不使用实例化。
    polygonLayout[1].InstanceDataStepRate = 0;
    
    //一旦布局描述已经设置好，我们就可以得到它的大小，然后使用D3D设备创建输入布局。同时释放顶点和像素着色器缓冲区，因为一旦布局被创建，它们就不再需要了。
    numElements = sizeof(polygonLayout) / sizeof(polygonLayout[0]);
    result = device->CreateInputLayout(polygonLayout, numElements, vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), &m_layout);
    if(FAILED(result))
    {
        return false;
    }
    
    //释放顶点和像素着色器缓冲区。
    vertexShaderBuffer->Release();
    vertexShaderBuffer = 0;
    pixelShaderBuffer->Release();
    pixelShaderBuffer = 0;

    //为了利用着色器，最后需要设置的是常量缓冲。正如你在顶点着色器中看到的，我们目前只有一个常量缓冲，所以我们只需要在这里设置一个，这样我们就可以与着色器进行交互。
    //缓冲区使用需要设置为动态，因为我们将每帧更新它。绑定标志表明这个缓冲区将是一个常量缓冲区。CPU访问标志需要与使用量相匹配，因此将其设置为D3D11_CPU_ACCESS_WRITE。
    //一旦我们填写了描述，我们就可以创建常量缓冲接口，然后使用SetShaderParameters函数访问着色器中的内部变量。
    matrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
    matrixBufferDesc.ByteWidth = sizeof(MatrixBufferType);
    matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    matrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    matrixBufferDesc.MiscFlags = 0;
    matrixBufferDesc.StructureByteStride = 0;

    //现在我们创建常量缓冲区。
    result = device->CreateBuffer(&matrixBufferDesc, NULL, &m_matrixBuffer);
    if(FAILED(result))
    {
        return false;
    }
    return true; 
    
}

void ColorShader::ShutdownShader()
{
	// Release the matrix constant buffer.
	if(m_matrixBuffer)
	{
		m_matrixBuffer->Release();
		m_matrixBuffer = 0;
	}

	// Release the layout.
	if(m_layout)
	{
		m_layout->Release();
		m_layout = 0;
	}

	// Release the pixel shader.
	if(m_pixelShader)
	{
		m_pixelShader->Release();
		m_pixelShader = 0;
	}

	// Release the vertex shader.
	if(m_vertexShader)
	{
		m_vertexShader->Release();
		m_vertexShader = 0;
	}

	return;
}

//OutputShaderErrorMessage将在编译顶点着色器或像素着色器时生成的错误信息写出来。
void ColorShader::OutputShaderErrorMessage(ID3D10Blob* errorMessage, HWND hwnd, WCHAR* shaderFilename)
{
	char* compileErrors;
	unsigned long bufferSize, i;
	ofstream fout;  

	//获取错误信息的大小。
	bufferSize = errorMessage->GetBufferSize();

	//分配一个字符串来存储错误信息。
	compileErrors = (char*)(errorMessage->GetBufferPointer());  
	
	//打开一个文件来写入错误信息。
	fout.open("shader-error.txt");

	//写入错误信息。
	for(i=0; i<bufferSize; i++)
	{
		fout << compileErrors[i];
	}   

	//关闭文件。
	fout.close();

	//释放错误信息。
	errorMessage->Release();    

	//显示一个错误对话框。
	MessageBox(hwnd, L"Error compiling shader.  Check shader-error.txt for message.", shaderFilename, MB_OK);

	return;
}

//SetShaderVariables函数的存在使得在着色器中设置全局变量更容易。
//在这个函数中使用的矩阵是在ApplicationClass中创建的，之后这个函数被调用，在渲染函数调用期间将它们从那里发送到顶点着色器。
bool ColorShader::SetShaderParameters(ID3D11DeviceContext* deviceContext, XMMATRIX worldMatrix, XMMATRIX viewMatrix, XMMATRIX projectionMatrix)
{
    HRESULT result;
    D3D11_MAPPED_SUBRESOURCE mappedResource;
    MatrixBufferType* dataPtr;
    unsigned int bufferNumber;

    //确保在将它们发送到着色器之前调换矩阵，这是DirectX 11的要求。
    worldMatrix = XMMatrixTranspose(worldMatrix);
    viewMatrix = XMMatrixTranspose(viewMatrix);
    projectionMatrix = XMMatrixTranspose(projectionMatrix);

    //锁定m_matrixBuffer，在其中设置新的矩阵，然后解锁它。
    result = deviceContext->Map(m_matrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
    if(FAILED(result))
    {
        return false;
    }

    //获取矩阵缓冲区的数据指针。
    dataPtr = (MatrixBufferType*)mappedResource.pData;

    //将矩阵复制到缓冲区。
    dataPtr->world = worldMatrix;
    dataPtr->view = viewMatrix;
    dataPtr->projection = projectionMatrix; 

    //解锁缓冲区。
    deviceContext->Unmap(m_matrixBuffer, 0);

    //现在在HLSL顶点着色器中设置更新后的矩阵缓冲。
    //在顶点着色器中设置常量缓冲区的位置。
    bufferNumber = 0;
    
    //最后，设置缓冲区在着色器中的位置。
    deviceContext->VSSetConstantBuffers(bufferNumber, 1, &m_matrixBuffer);

    return true;
}
