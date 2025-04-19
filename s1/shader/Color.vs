cbuff MatrixBuff
{
    matrix worldMatrix;
    matrix viewMatrix;
    matrix projectionMatrix;
}

struct VertexInputType
{
    float4 position : POSITION;
    float4 color : COLOR;
}

struct PixelInputType
{
    float4 position : SV_POSITION;
    float4 color : COLOR;
}

//当GPU处理发送给它的顶点缓冲区的数据时，它会调用顶点着色器。这个我命名为ColorVertexShader的顶点着色器将被顶点缓冲区中的每个顶点调用。
//顶点着色器的输入必须与顶点缓冲区中的数据格式以及着色器源文件中的类型定义相匹配，在这种情况下是VertexInputType。
//顶点着色器的输出将被发送到像素着色器。在这种情况下，输出类型称为PixelInputType，它也是上面定义的。
//记住这一点，你会看到顶点着色器创建了一个PixelInputType类型的输出变量。
//然后，它获取输入顶点的位置，并将其乘以世界、视图和投影矩阵。
//这将把顶点放置在正确的位置，以便根据我们的视图在3D空间中渲染，然后在2D屏幕上渲染。之后，输出变量获取输入颜色的副本，然后返回输出，这将被用作像素着色器的输入。
//还要注意，我确实将输入位置的W值设置为1.0，否则它是未定义的，因为我们只读取位置的XYZ向量。

PixelInputType ColorVertexShader(VertexInputType input)
{
    PixelInputType output;
    //将位置向量更改为4个单位以进行适当的矩阵计算。
    input.position.w = 1.0f;

    //根据世界、视图和投影矩阵计算顶点的位置。
    output.position = mul(input.position, worldMatrix);
    output.position = mul(output.position, viewMatrix);
    output.position = mul(output.position, projectionMatrix);

    output.color = input.color;

    return output;

}


