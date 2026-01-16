/*==============================================================================

	Make Billboard [Billboard.cpp]

	Author : Choi HyungJoon

==============================================================================*/
#include "Billboard.h"
#include "direct3d.h"
#include <DirectXMath.h>
#include "Texture_Manager.h"
#include "Shader_Manager.h"
#include <Player_Camera.h>
#include <wrl/client.h>
#include "Sprite_Animation.h"
#include "Palette.h"
using namespace DirectX;
using namespace PALETTE;

static constexpr int NUM_VERTEX = 4;

static Microsoft::WRL::ComPtr<ID3D11Buffer> Vertex_Buffer = nullptr;

struct Vertex_Billboard
{
	XMFLOAT3 position;
	XMFLOAT4 color;   
	XMFLOAT2 texcord; 
};

void Billboard_Initialize()
{
	static Vertex_Billboard Billboard[]
	{
		{{ 0.0f, 1.0f, 0.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, {0.0f, 0.0f}}, // 0 : LU
		{{ 1.0f, 1.0f, 0.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, {1.0f, 0.0f}}, // 1 : RU
		{{ 0.0f, 0.0f, 0.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, {0.0f, 1.0f}}, // 2 : LD
		{{ 1.0f, 0.0f, 0.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, {1.0f, 1.0f}}  // 3 : RD
	};

	D3D11_BUFFER_DESC bd = {};
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(Vertex_Billboard) * NUM_VERTEX;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = 0;

	D3D11_SUBRESOURCE_DATA sd{};
	sd.pSysMem = Billboard;

	Direct3D_GetDevice()->CreateBuffer(&bd, &sd, Vertex_Buffer.GetAddressOf());
}

void Billboard_Finalize()
{
	Vertex_Buffer.Reset();
}

void Billboard_Draw_Internal(int Tex_ID, const XMFLOAT3& POS, float Scale_X, float Scale_Y,
	const XMFLOAT2& Pivot, const UV_Parameter& uvParam, const XMFLOAT4& Color)
{
	// Set UV
	Shader_Manager::GetInstance()->SetUVParameter(uvParam);
	Shader_Manager::GetInstance()->Begin_Billboard();

	// Set Color
	Shader_Manager::GetInstance()->SetDiffuseColor(Color);

	// Set Texture
	ID3D11ShaderResourceView* SRV = Texture_Manager::GetInstance()->Get_Shader_Resource_View(Tex_ID);
	if (SRV)
	{
		Direct3D_GetContext()->PSSetShaderResources(0, 1, &SRV);
	}

	// Set Vertex Buffer
	UINT stride = sizeof(Vertex_Billboard);
	UINT offset = 0;
	Direct3D_GetContext()->IASetVertexBuffers(0, 1, Vertex_Buffer.GetAddressOf(), &stride, &offset);

	// Set Topology
	Direct3D_GetContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	// Set Offset
	XMMATRIX Pivot_Offset = XMMatrixTranslation(-Pivot.x, -Pivot.y, 0.0f);
	// Set Scale Matrix
	XMMATRIX Scale = XMMatrixScaling(Scale_X, Scale_Y, 1.0f);

	// Get Camera Rotation
	XMFLOAT4X4 view4x4 = PlayerCamera_GetViewMatrix();

	// Remove Translation
	view4x4._41 = 0.0f; view4x4._42 = 0.0f; view4x4._43 = 0.0f;

	// Get Inverse_Matrix
	XMMATRIX Inverse_mtx = XMMatrixTranspose(XMLoadFloat4x4(&view4x4));

	// Get Scale And Translation
	XMMATRIX Trans = XMMatrixTranslation(POS.x, POS.y, POS.z);

	// Get Final mtx
	XMMATRIX mtxWorld = Pivot_Offset * Scale * Inverse_mtx * Trans;

	// Set World mtx
	Shader_Manager::GetInstance()->SetWorldMatrix3D(mtxWorld);

	// Draw
	Direct3D_GetContext()->Draw(NUM_VERTEX, 0);
}

void Billboard_Draw(int Tex_ID, const DirectX::XMFLOAT3& POS, float Scale_X, float Scale_Y,
	const DirectX::XMFLOAT2& Pivot, const DirectX::XMFLOAT4& Color)
{
	// Set UV
	UV_Parameter UV = {};
	UV.scale = { 1.0f, 1.0f };
	UV.translation = { 0.0f, 0.0f };

	Billboard_Draw_Internal(Tex_ID, POS, Scale_X, Scale_Y, Pivot, UV, Color);
}

void Billboard_Draw_Animation(int PlayID, const DirectX::XMFLOAT3& POS, float Scale_X, float Scale_Y)
{
	int texID = -1;
	UV_Parameter UV = {};

	if (SpriteAni_Get_Current_UV(PlayID, texID, UV.scale, UV.translation))
	{
		Billboard_Draw_Internal(texID, POS, Scale_X, Scale_Y, { 0.5f, 0.5f }, UV, White);
	}
}