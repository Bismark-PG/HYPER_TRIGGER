/*==============================================================================

	Manage Debug Collision [Debug_Collision.cpp]

	Author : Choi HyungJoon

==============================================================================*/
#include "Debug_Collision.h"
#include "Texture_Manager.h"
#include "Cube.h"

bool Is_Debug_Box_Draw = true;
using namespace DirectX;

static ID3D11RasterizerState* Wire_Frame_State = nullptr;
static int Debug_Texture_ID = -1;

void Debug_Collision_Initialize(ID3D11Device* device)
{
	D3D11_RASTERIZER_DESC desc = {};
	desc.FillMode = D3D11_FILL_WIREFRAME;
	desc.CullMode = D3D11_CULL_NONE;     
	desc.DepthClipEnable = true;
	desc.MultisampleEnable = true;

	device->CreateRasterizerState(&desc, &Wire_Frame_State);

	Debug_Texture_ID = Texture_Manager::GetInstance()->GetID("TextSample");
}

void Debug_Collision_Finalize()
{
	SAFE_RELEASE(Wire_Frame_State);
}

void Debug_Collision_Draw(const AABB& Collision, const DirectX::XMFLOAT4& color)
{
	if (!Debug_Draw_Get_State())	return;

	ID3D11DeviceContext* Context = Direct3D_GetContext();
	if (!Context || !Wire_Frame_State)	 return;

	// Set AABB Scale 
	XMVECTOR Vec_Min = XMLoadFloat3(&Collision.Min);
	XMVECTOR Vec_Max = XMLoadFloat3(&Collision.Max);
	XMVECTOR Vec_Size = Vec_Max - Vec_Min;

	// Get Middle Of AABB
	XMVECTOR vCenter = (Vec_Min + Vec_Max) * 0.5f;

	// Scaling Cude Scale
	XMMATRIX mtxScale = XMMatrixScalingFromVector(Vec_Size);
	XMMATRIX mtxTrans = XMMatrixTranslationFromVector(vCenter);
	XMMATRIX world = mtxScale * mtxTrans;

	// Save Rasterizer State
	ID3D11RasterizerState* oldState = nullptr;
	Context->RSGetState(&oldState);

	// Change Rasterizer To Wire Frame
	Context->RSSetState(Wire_Frame_State);

	// Set Shader Color
	Shader_Manager::GetInstance()->SetDiffuseColor(color);
	Shader_Manager::GetInstance()->SetTexture3D(Texture_Manager::GetInstance()->
		Get_Shader_Resource_View(Debug_Texture_ID));

	// Begin Shader
	Shader_Manager::GetInstance()->Begin3D(Shader_Filter::MAG_MIP_POINT); 
	Shader_Manager::GetInstance()->SetDiffuseColor(color);
	Shader_Manager::GetInstance()->SetTexture3D(Texture_Manager::GetInstance()->
		Get_Shader_Resource_View(Debug_Texture_ID));

	Debug_Cube_Draw(world);

	// Rollback Rasterizer State
	Context->RSSetState(oldState);
	SAFE_RELEASE(oldState);
}

bool Debug_Draw_Get_State()
{
	return Is_Debug_Box_Draw;
}

void Debug_Draw_Get_State(bool State)
{
	Is_Debug_Box_Draw = State;
}
