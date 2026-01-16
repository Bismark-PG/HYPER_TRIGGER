/*==============================================================================

	Make Billboard [Billboard.h]

	Author : Choi HyungJoon

==============================================================================*/
#ifndef BILLBOARD_H
#define BILLBOARD_H
#include <DirectXMath.h>
#include <Shader_Manager.h>

class Animation;

void Billboard_Initialize();
void Billboard_Finalize();

void Billboard_Draw_Internal(int Tex_ID, const DirectX::XMFLOAT3& POS, float Scale_X, float Scale_Y,
	const DirectX::XMFLOAT2& Pivot, const UV_Parameter& uvParam, const DirectX::XMFLOAT4& Color);

void Billboard_Draw(int Tex_ID, const DirectX::XMFLOAT3& POS, float Scale_X, float Scale_Y,
	const DirectX::XMFLOAT2& Pivot, const DirectX::XMFLOAT4& Color = { 1.0f, 1.0f, 1.0f, 1.0f });

void Billboard_Draw_Animation(int PlayID, const DirectX::XMFLOAT3& POS, float Scale_X, float Scale_Y);

#endif // BILLBOARD_H
