/*==============================================================================
	
	Make Mash Field [Mash_Field.cpp]
	
	Author : Choi HyungJoon

==============================================================================*/
#include "Mash_Field.h"
#include "direct3d.h"
using namespace DirectX;
#include "Texture_Manager.h"
#include "Palette.h"
#include "Shader_Manager.h"
using namespace PALETTE;

static constexpr float FIELD_UNIT_SIZE = 5.0f;  // Field Size
static constexpr int FIELD_H_COUNT = 30; // Horizon Count
static constexpr int FIELD_V_COUNT = 30; // Vertical Count

static constexpr int FIELD_H_VERTEX_COUNT = FIELD_H_COUNT + 1;
static constexpr int FIELD_V_VERTEX_COUNT = FIELD_V_COUNT + 1;

static constexpr int NUM_VERTEX = FIELD_H_VERTEX_COUNT * FIELD_V_VERTEX_COUNT;
static constexpr int NUM_INDEX = 3 * 2 * FIELD_H_COUNT * FIELD_V_COUNT;

static ID3D11Buffer* Vertex_Buffer = nullptr; // 頂点バッファ
static ID3D11Buffer* Index_Buffer = nullptr; // 頂点バッファ

// 注意！初期化で外部から設定されるもの。Release不要。
static ID3D11Device* g_pDevice = nullptr;
static ID3D11DeviceContext* g_pContext = nullptr;

static float Offset_X = -FIELD_H_VERTEX_COUNT * FIELD_UNIT_SIZE * 0.5f;
static float Offset_Z = -FIELD_V_VERTEX_COUNT * FIELD_UNIT_SIZE * 0.5f;

static float Total_Size_X = FIELD_H_COUNT * FIELD_UNIT_SIZE;
static float Total_Size_Z = FIELD_V_COUNT * FIELD_UNIT_SIZE;

static int Field_TexID = -1;

struct Vertex3D
{
	XMFLOAT3 POS;
	XMFLOAT3 Normal;
	XMFLOAT4 Color;
	XMFLOAT2 TexCord;
};

static Vertex3D Mash_Field_Vertex[NUM_VERTEX];

static unsigned short Mash_Field_Index[NUM_INDEX]
{

};

void Mash_Field_Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	// デバイスとデバイスコンテキストの保存
	g_pDevice = pDevice;
	g_pContext = pContext;

	// Set Offset
	Offset_X = -FIELD_H_COUNT * FIELD_UNIT_SIZE * 0.5f;
	Offset_Z = -FIELD_V_COUNT * FIELD_UNIT_SIZE * 0.5f;
	Total_Size_X = FIELD_H_COUNT * FIELD_UNIT_SIZE;
	Total_Size_Z = FIELD_V_COUNT * FIELD_UNIT_SIZE;

	Field_TexID = Texture_Manager::GetInstance()->GetID("Floor");

	Vertex3D* vertices = new Vertex3D[NUM_VERTEX];
	// Make Vertex Index
	for (int z = 0; z < FIELD_V_VERTEX_COUNT; z++)
	{
		for (int x = 0; x < FIELD_H_VERTEX_COUNT; x++)
		{
			int idx = z * FIELD_H_VERTEX_COUNT + x;

			float px = x * FIELD_UNIT_SIZE + Offset_X;
			float pz = z * FIELD_UNIT_SIZE + Offset_Z;
			float py = 0.0f;

			vertices[idx].POS = XMFLOAT3(px, py, pz);
			vertices[idx].Normal = XMFLOAT3(0.0f, 1.0f, 0.0f);
			vertices[idx].Color = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
			vertices[idx].TexCord = XMFLOAT2(px / 10.0f, pz / 10.0f);
		}
	}

	// 頂点バッファ生成
	D3D11_BUFFER_DESC bd = {};
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(Vertex3D) * NUM_VERTEX; // sizeof(Mash_Field_Vertex);
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = 0;

	D3D11_SUBRESOURCE_DATA sd{};
	sd.pSysMem = vertices;

	g_pDevice->CreateBuffer(&bd, &sd, &Vertex_Buffer);

	delete[] vertices;

	// Make Index Buffer
	unsigned int* Indices = new unsigned int[NUM_INDEX];
	int k = 0;
	for (int z = 0; z < FIELD_V_COUNT; z++)
	{
		for (int x = 0; x < FIELD_H_COUNT; x++)
		{
			Indices[k++] = (z + 0) * FIELD_H_VERTEX_COUNT + (x + 0);
			Indices[k++] = (z + 1) * FIELD_H_VERTEX_COUNT + (x + 0);
			Indices[k++] = (z + 0) * FIELD_H_VERTEX_COUNT + (x + 1);

			Indices[k++] = (z + 0) * FIELD_H_VERTEX_COUNT + (x + 1);
			Indices[k++] = (z + 1) * FIELD_H_VERTEX_COUNT + (x + 0);
			Indices[k++] = (z + 1) * FIELD_H_VERTEX_COUNT + (x + 1);
		}
	}

	// Creat Index Buffer
	bd.ByteWidth = sizeof(unsigned int) * NUM_INDEX; // if Arr >> "sizeof(Cube_Index)" is OK
	bd.BindFlags = D3D11_BIND_INDEX_BUFFER;

	sd.pSysMem = Indices;
	g_pDevice->CreateBuffer(&bd, &sd, &Index_Buffer);

	delete[] Indices;
}

void Mash_Field_Finalize()
{
	SAFE_RELEASE(Vertex_Buffer);
	SAFE_RELEASE(Index_Buffer);
}

void Mash_Field_Draw()
{
	// シェーダーを描画パイプラインに設定
	Shader_Manager::GetInstance()->Begin3D_For_Field();

	// 頂点バッファを描画パイプラインに設定
	UINT stride = sizeof(Vertex3D);
	UINT offset = 0;
	g_pContext->IASetVertexBuffers(0, 1, &Vertex_Buffer, &stride, &offset);

	// Set Index Buffer
	// unsigned short = DXGI_FORMAT_R16_UINT
	// unsigned int	  = DXGI_FORMAT_R32_UINT
	g_pContext->IASetIndexBuffer(Index_Buffer, DXGI_FORMAT_R32_UINT, 0);

	// プリミティブトポロジ設定
	g_pContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// Texture_SetTexture(Cube_Tex_ID);
	ID3D11ShaderResourceView* texture = Texture_Manager::GetInstance()->Get_Shader_Resource_View(Field_TexID);
	Shader_Manager::GetInstance()->SetTexture3D(texture);

	// Just Draw
	// g_pContext->Draw(NUM_VERTEX, 0); 

	// Draw With Index, 9-Tile Rendering
	for (int z = -1; z <= 1; z++)
	{
		for (int x = -1; x <= 1; x++)
		{
			// 中央(0,0)を基準に、Total_Size分ずらして配置
			float moveX = x * Total_Size_X;
			float moveZ = z * Total_Size_Z;

			XMMATRIX world = XMMatrixTranslation(moveX, 0.0f, moveZ);
			Shader_Manager::GetInstance()->SetWorldMatrix3D(world);

			g_pContext->DrawIndexed(NUM_INDEX, 0, 0);
		}
	}
}

float Mash_Field_Get_Height(float worldX, float worldZ)
{
	// Legacy System For Get Ground Height For Enemy Y, Not Use Now 

	/*
	// World Index to Local Index
	// WorldX = (IndexX * Size) + OffsetX
	// IndexX = (WorldX - OffsetX) / Size
	float Local_X = (worldX - Offset_X) / FIELD_SIZE;
	float Local_Z = (worldZ - Offset_Z) / FIELD_SIZE;

	// Check Out of Map
	if (Local_X < 0.0f || Local_X >static_cast<float>(FIELD_H_VERTEX_COUNT) - 1 ||
		Local_Z < 0.0f || Local_Z >static_cast<float>(FIELD_V_VERTEX_COUNT) - 1)
	{
		// If Out of Map, return low height
		return -99999.0f;
	}

	// Get Height
	int Center_Index = FIELD_H_VERTEX_COUNT / 2;
	float Height = 0.0f;

	if (Local_X > static_cast<float>(Center_Index))
	{
		// Linear Interpolation
		float Ramp_Ratio = (Local_X - static_cast<float>(Center_Index))
			/ (static_cast<float>(FIELD_H_VERTEX_COUNT) -1 - Center_Index);
		Height = Ramp_Ratio * 10.0f;
	}

	return Height;
	*/

	// Just Return 0.0f (Ground Y)
	return 0.0f;
}

float Mash_Field_Get_Size_X()
{ 
	return Total_Size_X;
}

float Mash_Field_Get_Size_Z() 
{ 
	return Total_Size_Z; 
}

// --- Note ---
/*
int Index = 0;
for (int V = 0; V < FIELD_V_COUNT; V++)
{
	for (int H = 0; H < FIELD_H_COUNT; H++)
	{
		unsigned short Top_Left = H + V * FIELD_H_VERTEX_COUNT;
		unsigned short Top_Right = Top_Left + 1;
		unsigned short Bottom_Left = H + (V + 1) * FIELD_H_VERTEX_COUNT;
		unsigned short Bottom_Right = Bottom_Left + 1;

		//------------- Draw Type : \ -------------//
		// Draw First Triangle  (T.L -> B.L -> T.R)
		Mash_Field_Index[Index++] = Top_Left;
		Mash_Field_Index[Index++] = Bottom_Left;
		Mash_Field_Index[Index++] = Top_Right;

		// Draw Second Triangle (T.R -> B.R -> B.L)
		Mash_Field_Index[Index++] = Top_Right;
		Mash_Field_Index[Index++] = Bottom_Left;
		Mash_Field_Index[Index++] = Bottom_Right;

		//------------- Draw Type : / -------------//
		// Draw First Triangle  (T.L -> B.R -> T.R)
		Mash_Field_Index[Index++] = Top_Left;
		Mash_Field_Index[Index++] = Bottom_Right;
		Mash_Field_Index[Index++] = Top_Right;

		// Draw Second Triangle (T.L -> B.L -> B.R)
		Mash_Field_Index[Index++] = Top_Left;
		Mash_Field_Index[Index++] = Bottom_Left;
		Mash_Field_Index[Index++] = Bottom_Right;
	}
}
*/