#ifndef _MODEL_H_
#define _MODEL_H_

#include <d3d11.h>
#include <DirectXColors.h>

using namespace DirectX;

class Model
{

    private:
        //顶点结构。下面是我们的顶点类型的定义，它将与这个ModelClass中的顶点缓冲区一起使用。还要注意的是，这个typedef必须与ColorShaderClass中的布局相匹配，这将在后面的教程中看到。
        struct VertexType
        {
            XMFLOAT3 position;
            XMFLOAT4 color;
        };
        
        
    public:
        Model();
        Model(const Model&);
        ~Model();
        //这里的函数处理模型顶点和索引缓冲区的初始化和关闭。渲染函数把模型的几何图形放到显卡上，准备用颜色着色器绘制。
        bool Initialize(ID3D11Device* device);
        void Shutdown();
        void Render(ID3D11DeviceContext* deviceContext);
        //这个函数返回索引缓冲区中的顶点数量。
        int GetIndexCount();

    private:
        bool InitializeBuffers(ID3D11Device* device);
        void ShutdownBuffers();
        void RenderBuffers(ID3D11DeviceContext* deviceContext);

        //这里声明了两个缓冲区，一个用于顶点，一个用于索引。
        //ModelClass中的私有变量是顶点和索引缓冲区，以及两个整数，用于跟踪每个缓冲区的大小。请注意，所有DirectX 11缓冲区通常使用通用的ID3D11Buffer类型，并且在第一次创建时通过缓冲区描述更清楚地识别。
        ID3D11Buffer* m_vertexBuffer;
        ID3D11Buffer* m_indexBuffer;
        int m_vertexCount;
        int m_indexCount;
};

#endif
