/*==============================================================================

    Vertex Shader For Skin Mash(Skinnig) [shader_vertex_skinning.hlsl]

    Author : Choi HyungJoon

==============================================================================*/

// b0: World, b1: View, b2: Proj Is Same To 3D Vertex Shader
cbuffer VS_CONSTANT_BUFFER : register(b0)
{
    float4x4 world;
};

cbuffer VS_CONSTANT_BUFFER : register(b1)
{
    float4x4 view;
};

cbuffer VS_CONSTANT_BUFFER : register(b2)
{
    float4x4 proj;
};

// b3: Bone Matrices (Max 256)
cbuffer VS_SKIN_BUFFER : register(b3)
{
    float4x4 boneTransforms[256];
};

struct VS_IN
{
    float4 posL : POSITION0;
    float4 normalL : NORMAL0;
    float4 color : COLOR0;
    float2 texcoord : TEXCOORD0;
    // Skinning Data
    uint4 boneIndices : BLENDINDICES0; // Bone Index
    float4 boneWeights : BLENDWEIGHT0; 
};

struct VS_OUT
{
    float4 posH : SV_POSITION;
    float4 posW : POSITION0;
    float4 normalW : NORMAL0;
    float4 color : COLOR0;
    float2 texcoord : TEXCOORD0;
};

VS_OUT main(VS_IN vi)
{
    VS_OUT vo;

    // 1. Skinning Calculate
    // Init
    float4 posAccum = float4(0.0f, 0.0f, 0.0f, 0.0f);
    float4 normalAccum = float4(0.0f, 0.0f, 0.0f, 0.0f);
    
    // Bone 1
    float4x4 boneMtx = boneTransforms[vi.boneIndices.x];
    posAccum += mul(vi.posL, boneMtx) * vi.boneWeights.x;
    normalAccum += mul(float4(vi.normalL.xyz, 0.0f), boneMtx) * vi.boneWeights.x;

    // Bone 2
    boneMtx = boneTransforms[vi.boneIndices.y];
    posAccum += mul(vi.posL, boneMtx) * vi.boneWeights.y;
    normalAccum += mul(float4(vi.normalL.xyz, 0.0f), boneMtx) * vi.boneWeights.y;

    // Bone 3
    boneMtx = boneTransforms[vi.boneIndices.z];
    posAccum += mul(vi.posL, boneMtx) * vi.boneWeights.z;
    normalAccum += mul(float4(vi.normalL.xyz, 0.0f), boneMtx) * vi.boneWeights.z;

    // Bone 4
    boneMtx = boneTransforms[vi.boneIndices.w];
    posAccum += mul(vi.posL, boneMtx) * vi.boneWeights.w;
    normalAccum += mul(float4(vi.normalL.xyz, 0.0f), boneMtx) * vi.boneWeights.w;

    // Reset W (POS : 1, Vector : 0)
    posAccum.w = 1.0f;

    // 2. Chage POS (Same To 3D V.S)
    // World >> View >> Proj

    float4x4 mtxWV = mul(world, view);
    float4x4 mtxWVP = mul(mtxWV, proj);

    vo.posH = mul(posAccum, mtxWVP); // Final POS
    vo.posW = mul(posAccum, world);  // World POS
    
    // Change Normal (Set World Rot)
    float4 normalW = mul(float4(normalAccum.xyz, 0.0f), world);
    vo.normalW = normalize(normalW);

    vo.color = vi.color;
    vo.texcoord = vi.texcoord;

    return vo;
}