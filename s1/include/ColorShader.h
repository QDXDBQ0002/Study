#ifndef _COLORSHADER_H_
#define _COLORSHADER_H_

#include <d3d11.h>
#include <d3dcompiler.h>
#include <DirectXMath.h>
#include <fstream>

using namespace DirectX;
using namespace std;

class ColorShader
{

    //这里是cBuffer类型的定义，它将与顶点着色器一起使用。这个typedef必须与顶点着色器中的typedef完全相同，因为模型数据需要匹配着色器中的typepedef才能正确渲染。
    struct MatrixBufferType
    {
        XMMATRIX world;
        XMMATRIX view;
        XMMATRIX projection;
    };
    

    public:
        ColorShader();
        ColorShader(const ColorShader&);    
        ~ColorShader();
        //这里的函数处理着色器的初始化和关闭。渲染函数设置着色器参数，然后使用着色器绘制准备好的模型顶点。
        bool Initialize(ID3D11Device*, HWND);
        void Shutdown();
        bool Render(ID3D11DeviceContext*, int, XMMATRIX, XMMATRIX, XMMATRIX);
    private:
        ID3D11VertexShader* m_vertexShader;
        ID3D11PixelShader* m_pixelShader;
        ID3D11InputLayout* m_layout;
        ID3D11Buffer* m_matrixBuffer;
        bool InitializeShader(ID3D11Device*, HWND, WCHAR*, WCHAR*);
        void ShutdownShader();
        void OutputShaderErrorMessage(ID3D10Blob*, HWND, WCHAR*);
        bool SetShaderParameters(ID3D11DeviceContext*, XMMATRIX, XMMATRIX, XMMATRIX);
        void RenderShader(ID3D11DeviceContext*, int);
    
};

#endif
