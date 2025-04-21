#include "Model.h"

Model::Model()
{
    m_vertexBuffer = 0;
    m_indexBuffer = 0;
}

Model::~Model()
{

}

Model::Model(const Model& other)
{

}

//Initialize函数将调用顶点和索引缓冲区的初始化函数。
bool Model::Initialize(ID3D11Device* device)
{
    return InitializeBuffers(device);
}

//Shutdown函数将调用顶点和索引缓冲区的关闭函数。
void Model::Shutdown()
{
    ShutdownBuffers();
}

//Render函数将调用顶点和索引缓冲区的渲染函数。
void Model::Render(ID3D11DeviceContext* deviceContext)
{
    RenderBuffers(deviceContext);
}


int Model::GetIndexCount()
{
    return m_indexCount;
}

//InitializeBuffers函数是处理创建顶点和索引缓冲区的地方。通常，您将读入模型并从该数据文件创建缓冲区。在本教程中，我们将手动设置顶点和索引缓冲区中的点，因为它只是一个三角形。
bool Model::InitializeBuffers(ID3D11Device* device)
{
    VertexType* vertices;
    unsigned long* indices;
    D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
    D3D11_SUBRESOURCE_DATA vertexData, indexData;
    HRESULT result;

    //设置顶点数组大小，并分配内存。
    m_vertexCount = 3;
    m_indexCount = 3;

    vertices = new VertexType[m_vertexCount];
    if(!vertices)
    {
        return false;
    }
    //首先创建两个临时数组来保存稍后将用于填充最终缓冲区的顶点和索引数据。
    indices = new unsigned long[m_indexCount];  
    if(!indices)
    {
        return false;
    }

    //填充顶点数组与三角形数据。
    indices = new unsigned long[m_indexCount];
    if(!indices)
    {
        return false;
    }
    
    //现在用三角形的三个点以及每个点的索引来填充顶点和索引数组。
    //请注意，我是按顺时针顺序创建这些点的。如果你逆时针这样做，它会认为三角形是面向相反的方向，而不画它，因为背面剔除。
    //始终记住，将顶点发送到GPU的顺序是非常重要的。颜色也是在这里设置的，因为它是顶点描述的一部分。我将颜色设置为绿色。
    vertices[0].position = XMFLOAT3(-1.0f, -1.0f, 0.0f);  // Bottom left.
    vertices[0].color = XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f);

    vertices[1].position = XMFLOAT3(0.0f, 1.0f, 0.0f);  // Top middle.
    vertices[1].color = XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f);

    vertices[2].position = XMFLOAT3(1.0f, -1.0f, 0.0f);  // Bottom right.
    vertices[2].color = XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f);

    //填充索引数组。
    indices[0] = 0;  // Bottom left.
    indices[1] = 1;  // Top middle.
    indices[2] = 2;  // Bottom right.

    //填好顶点数组和索引数组后，我们现在可以使用它们来创建顶点缓冲区和索引缓冲区。
    //以相同的方式创建两个缓冲区。首先填写缓冲区的描述。在描述中，ByteWidth（缓冲区的大小）和BindFlags（缓冲区的类型）是您需要确保正确填写的内容。
    //在描述被填写之后，你还需要填写一个子资源指针，它将指向你之前创建的顶点或索引数组。有了描述和子资源指针，你可以使用D3D设备调用CreateBuffer，它将返回一个指向新缓冲区的指针。
    vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
    vertexBufferDesc.ByteWidth = sizeof(VertexType) * m_vertexCount;
    vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    vertexBufferDesc.CPUAccessFlags = 0;
    vertexBufferDesc.MiscFlags = 0;
    vertexBufferDesc.StructureByteStride = 0;

    //现在填写子资源数据。
    vertexData.pSysMem = vertices;
    vertexData.SysMemPitch = 0;
    vertexData.SysMemSlicePitch = 0;

    //创建顶点缓冲区。
    result = device->CreateBuffer(&vertexBufferDesc, &vertexData, &m_vertexBuffer);
    if(FAILED(result))
    {
        return false;
    }

    //现在填写索引缓冲区的描述。
    indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
    indexBufferDesc.ByteWidth = sizeof(unsigned long) * m_indexCount;
    indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
    indexBufferDesc.CPUAccessFlags = 0;
    indexBufferDesc.MiscFlags = 0;
    indexBufferDesc.StructureByteStride = 0;
    
    //现在填写子资源数据。
    indexData.pSysMem = indices;
    indexData.SysMemPitch = 0;
    indexData.SysMemSlicePitch = 0;

    //创建索引缓冲区。
    result = device->CreateBuffer(&indexBufferDesc, &indexData, &m_indexBuffer);
    if(FAILED(result))
    {
        return false;
    }

    //创建顶点缓冲区和索引缓冲区之后，可以删除顶点和索引数组，因为数据已经复制到缓冲区中，不再需要它们了。
    delete [] vertices;
    vertices = 0;
    delete [] indices;
    indices = 0;

    return true;
}

//ShutdownBuffers函数只是释放在InitializeBuffers函数中创建的顶点缓冲区和索引缓冲区。
void Model::ShutdownBuffers()
{
    //首先检查是否已经存在缓冲区。
    if(m_indexBuffer)
    {
        m_indexBuffer->Release();
        m_indexBuffer = 0;
    }

    if(m_vertexBuffer)
    {
        m_vertexBuffer->Release();
        m_vertexBuffer = 0;
    }

    return;
}

//RenderBuffers是从Render函数调用的。这个函数的目的是在GPU的输入汇编器上设置顶点缓冲区和索引缓冲区为活动状态
//一旦GPU有一个活动的顶点缓冲区，它就可以使用着色器来渲染该缓冲区。
//这个函数还定义了如何绘制这些缓冲区，比如三角形、线条、扇形等等。
//在本教程中，我们将顶点缓冲区和索引缓冲区设置为输入汇编器上的活动，并告诉GPU应该使用IASetPrimitiveTopology DirectX函数将缓冲区绘制为三角形。
void Model::RenderBuffers(ID3D11DeviceContext* deviceContext)
{
    unsigned int stride;
    unsigned int offset;

    //设置顶点缓冲区为活动状态。
    stride = sizeof(VertexType);
    offset = 0;

    //设置顶点缓冲区为活动状态。
    deviceContext->IASetVertexBuffers(0, 1, &m_vertexBuffer, &stride, &offset);

    //设置索引缓冲区为活动状态。    
    deviceContext->IASetIndexBuffer(m_indexBuffer, DXGI_FORMAT_R32_UINT, 0);

    //设置图元拓扑。
    deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

}





