struct ObjectData
{
    float4x4 model;
    float4x4 normal;
};

struct VPMatrix
{
    float4x4 VP;
};

struct VSInput
{
    float3 pos : ATTRIB0;
    float3 normal : ATTRIB1;
    //float3 tangent : ATTRIB2;
    //float3 bitangent : ATTRIB3;
    float2 uv : ATTRIB2;
};

struct PSInput
{
    float4 pos : SV_POSITION;
    float4 wpos : WORLD_POS;
    float3 normal : NORMAL;
    float2 uv : TEX_COORD;
    //nointerpolation uint matIdx : MAT_IDX;
};

StructuredBuffer<ObjectData> objs_data;
ConstantBuffer<VPMatrix> VP;

void main(in VSInput VSIn, out PSInput PSIn, uint instId : SV_InstanceID)
{
    ObjectData data = objs_data[instId];
    float4x4 MVP = data.model * VP.VP;
    PSIn.wpos = mul(float4(VSIn.pos.xyz, 1.0), data.model);
    PSIn.pos = mul(float4(VSIn.pos.xyz, 1.0), MVP);
    PSIn.normal = mul(float4(VSIn.normal.xyz, 1.0), VP.VP).xyz;
    PSIn.uv = float2(instId,instId);
    //PSIn.matIdx = geomConst.matIdx;
}