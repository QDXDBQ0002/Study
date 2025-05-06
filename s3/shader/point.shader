struct VOut
{
    float4 pos : SV_POSITION;
    float4 color : COLOR;
};

VOut main(float4 pos : POSITION, float4 color : COLOR)
{
    VOut output;
    output.pos = pos;
    output.color = color;
    return output;
}