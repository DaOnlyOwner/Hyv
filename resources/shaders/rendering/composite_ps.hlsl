
struct PSInput
{
    float4 Pos : SV_POSITION;
    float2 UV : TEX_COORD;
};

// struct View
// {
//     float4x4 VPInv;
//     float3 eyePos;
//     int showHitLocations;
//     int probeID;
//     float indirectInt, directInt,p4;
// };

// ConstantBuffer<View> view;
// ConstantBuffer<Sun> sun;
// Texture2D GBuffer_Albedo;
Texture2D GBuffer_Normal;
// Texture2D GBuffer_Depth;

// Texture2D rayTest;
// Texture2D rayHitLocations;


// https://github.com/DiligentGraphics/DiligentSamples/blob/master/Tutorials/Tutorial22_HybridRendering/assets/Utils.fxh
float3 ScreenPosToWorldPos(float2 ScreenSpaceUV, float Depth, float4x4 ViewProjInv)
{
	float4 PosClipSpace;
    PosClipSpace.xy = ScreenSpaceUV * float2(2.0, -2.0) + float2(-1.0, 1.0);
    PosClipSpace.z = Depth;
    PosClipSpace.w = 1.0;
    float4 WorldPos = mul(PosClipSpace, ViewProjInv);
    return WorldPos.xyz / WorldPos.w;
}

float4 main(in PSInput PSIn) : SV_TARGET
{
    uint2 dim;
    GBuffer_Normal.GetDimensions(dim.x, dim.y);
    int3 texelPos = int3(
        PSIn.UV.x * dim.x,
        (1-PSIn.UV.y) * dim.y,
        0
    );


    float3 normal = GBuffer_Normal.Load(texelPos).xyz;
    return float4(normal,0);
}
