/*==============================================================================

   Direct3D11用 デバックテキスト表示 [debug_text.cpp]
														 Author : Youhei Sato
														 Date   : 2025/06/15
												   Edited by : Choi HyungJoon
												 Edited Date : 2025/08/20
	- Edit Note
		> Make Resize Function for Window Recreate
		> Make Resize Function for Text Size Change
		> Use Texture Manager

--------------------------------------------------------------------------------

==============================================================================*/
#include "debug_text.h"
using namespace DirectX;
#include <D3Dcompiler.h>
using namespace Microsoft::WRL;
#include "Texture_Manager.h"

namespace Text
{
	DebugText::DebugText(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, int fontTextureID, UINT screenWidth, UINT screenHeight)
		: m_pDevice(pDevice), m_pContext(pContext), m_Scale(1.0f), m_OffsetX(0.0f), m_OffsetY(0.0f)
	{
		m_pTextureView = Texture_Manager::GetInstance()->Get_Shader_Resource_View(fontTextureID);
		if (!m_pTextureView)
		{
			MessageBoxW(nullptr, L"Font Texture View Load Failed.", L"DebugText Error", MB_OK | MB_ICONERROR);
			return;
		}
		m_TextureWidth = Texture_Manager::GetInstance()->Get_Width(fontTextureID);
		m_TextureHeight = Texture_Manager::GetInstance()->Get_Height(fontTextureID);

		m_TextLines.emplace_back();

		// ブレンドステート設定
		D3D11_BLEND_DESC blend_desc{};
		blend_desc.RenderTarget[0].BlendEnable = TRUE;
		blend_desc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
		blend_desc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
		blend_desc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
		blend_desc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_SRC_ALPHA;
		blend_desc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_INV_SRC_ALPHA;
		blend_desc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
		blend_desc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
		m_pDevice->CreateBlendState(&blend_desc, m_pBlendState.GetAddressOf());

		// 深度ステンシルステート設定
		D3D11_DEPTH_STENCIL_DESC depth_stencil_desc{};
		depth_stencil_desc.DepthFunc = D3D11_COMPARISON_LESS;
		depth_stencil_desc.StencilEnable = FALSE;
		depth_stencil_desc.DepthEnable = FALSE;
		depth_stencil_desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
		m_pDevice->CreateDepthStencilState(&depth_stencil_desc, m_pDepthStencilState.GetAddressOf());


		// ラスタライザーステート設定
		D3D11_RASTERIZER_DESC rasterizer_desc{};
		rasterizer_desc.FillMode = D3D11_FILL_SOLID;
		rasterizer_desc.CullMode = D3D11_CULL_BACK;
		rasterizer_desc.DepthClipEnable = TRUE;
		m_pDevice->CreateRasterizerState(&rasterizer_desc, m_pRasterizerState.GetAddressOf());

		// 頂点シェーダーの作成
		static const char* vs_text = R"(
			cbuffer ConstantBuffer : register(b0)
			{
				float4x4 mtx;
			}

			struct VS_IN
			{
				float3 pos : POSITION;
				float4 col : COLOR;
				float2 uv : TEXCOORD;
			};

			struct PS_IN
			{
				float4 pos : SV_POSITION;
				float4 col : COLOR;
				float2 uv : TEXCOORD;
			};

			PS_IN main(VS_IN input)
			{
				PS_IN output;
				output.pos = mul(float4(input.pos, 1.0f), mtx);
				output.col = input.col;
				output.uv = input.uv;
				return output;
			})";

		ComPtr<ID3DBlob> vsBlob;
		D3DCompile(vs_text, strlen(vs_text), nullptr, nullptr, nullptr, "main", "vs_5_0", 0, 0, &vsBlob, nullptr);
		m_pDevice->CreateVertexShader(vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(), nullptr, &m_pVertexShader);

		// 頂点シェーダーの入力レイアウトを定義
		D3D11_INPUT_ELEMENT_DESC layout[] =
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,    0,  0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "COLOR",    0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,       0, 28, D3D11_INPUT_PER_VERTEX_DATA, 0 }
		};
		m_pDevice->CreateInputLayout(layout, ARRAYSIZE(layout), vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(), &m_pInputLayout);

		// 定数バッファの作成
		D3D11_BUFFER_DESC cbDesc = {};
		cbDesc.ByteWidth = sizeof(DirectX::XMFLOAT4X4);
		cbDesc.Usage = D3D11_USAGE_DEFAULT;
		cbDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		m_pDevice->CreateBuffer(&cbDesc, nullptr, &m_pVSConstantBuffer);

		// Reset Draw Location
		Resize(screenWidth, screenHeight);

		// ピクセルシェーダーの作成
		static const char* ps_text = R"(
			Texture2D fontTexture : register(t0);
			SamplerState fontSampler : register(s0);
			struct PS_IN
			{
				float4 pos : SV_POSITION;
				float4 col : COLOR;
				float2 uv : TEXCOORD;
			};

			float4 main(PS_IN input) : SV_TARGET {
				float4 texColor = fontTexture.Sample(fontSampler, input.uv);
				return texColor * input.col;
			}
		)";

		ComPtr<ID3DBlob> psBlob;
		D3DCompile(ps_text, strlen(ps_text), nullptr, nullptr, nullptr, "main", "ps_5_0", 0, 0, &psBlob, nullptr);
		m_pDevice->CreatePixelShader(psBlob->GetBufferPointer(), psBlob->GetBufferSize(), nullptr, &m_pPixelShader);

		// サンプラーステートの作成
		D3D11_SAMPLER_DESC sampler_desc = {};
		sampler_desc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
		sampler_desc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
		sampler_desc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
		sampler_desc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
		sampler_desc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
		sampler_desc.MinLOD = 0.0f;
		sampler_desc.MaxLOD = D3D11_FLOAT32_MAX;
		m_pDevice->CreateSamplerState(&sampler_desc, m_pSamplerState.GetAddressOf());

		Resize(screenWidth, screenHeight);
	}

	void DebugText::Print(const std::string& text, const DirectX::XMFLOAT4& color)
	{
		m_TextLines.back().strings.emplace_back(color);

		for (char c : text)
		{
			if (c == '\n')
			{
				m_TextLines.emplace_back();
				m_TextLines.back().strings.emplace_back(color);
			}
			else if (c == '\r')
			{
				// Ignore
			}
			else if (c == '\t')
			{
				// Convert tab to spaces
				do
				{
					if (m_MaxCharactersPerLine && m_TextLines.back().characterCount >= m_MaxCharactersPerLine)
					{
						m_TextLines.emplace_back();
						m_TextLines.back().strings.emplace_back(color);
						break;
					}

					m_TextLines.back().strings.back().characters += ' ';
					m_TextLines.back().characterCount++;
					m_TextLines.back().spaceCount++;
				} while (m_TextLines.back().characterCount % 4 != 0);
			}
			else
			{
				if (m_MaxCharactersPerLine && m_TextLines.back().characterCount >= m_MaxCharactersPerLine)
				{
					m_TextLines.emplace_back();
					m_TextLines.back().strings.emplace_back(color);
				}

				if (c >= ' ' && c <= '~') {
					m_TextLines.back().strings.back().characters += c;

					if (c != ' ')
						m_CharacterCount++;
					else
						m_TextLines.back().spaceCount++;

					m_TextLines.back().characterCount++;
				}
				else
				{
					m_TextLines.back().strings.back().characters += '?';
					m_CharacterCount++;
					m_TextLines.back().characterCount++;
				}
			}
		}

		// [RESTORED] Remove old lines if max line limit is reached
		int last_line_count = m_TextLines.back().characterCount ? 0 : -1;
		while (m_MaxLine > 0 && m_TextLines.size() + last_line_count > m_MaxLine) {
			UINT remove_character_count = m_TextLines.front().characterCount - m_TextLines.front().spaceCount;
			m_CharacterCount -= remove_character_count;
			m_TextLines.pop_front();
		}
	}

	void DebugText::Draw()
	{
		if (m_CharacterCount == 0) return;

		if (!m_pVertexBuffer || m_CharacterCount > m_BufferAllocatedCount) {
			createBuffers(m_CharacterCount);
		}

		D3D11_MAPPED_SUBRESOURCE msr;
		m_pContext->Map(m_pVertexBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &msr);
		Vertex* pV = reinterpret_cast<Vertex*>(msr.pData);

		D3D11_MAPPED_SUBRESOURCE msrIdx;
		m_pContext->Map(m_pIndexBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &msrIdx);
		WORD* pI = reinterpret_cast<WORD*>(msrIdx.pData);

		const float charWidth = (m_TextureWidth / 16.0f);
		const float charHeight = (m_TextureHeight / 16.0f);

		UINT currentLine = 0;
		UINT charDrawn = 0;

		for (const auto& line : m_TextLines) {
			UINT currentColumn = 0;
			for (const auto& segment : line.strings) {
				for (char c : segment.characters) {
					if (c != ' ') {
						int index = c - ' ';
						float u0 = (index % 16) / 16.0f;
						float v0 = (index / 16) / 16.0f;
						float u1 = u0 + (1.0f / 16.0f);
						float v1 = v0 + (1.0f / 16.0f);

						float x = m_OffsetX + currentColumn * charWidth * m_Scale;
						float y = m_OffsetY + currentLine   * charHeight * m_Scale;

						UINT vertexIndex = charDrawn * 4;
						pV[vertexIndex + 0] = { {x, y, 0}, segment.color, {u0, v0} };
						pV[vertexIndex + 1] = { {x + charWidth * m_Scale, y, 0}, segment.color, {u1, v0} };
						pV[vertexIndex + 2] = { {x, y + charHeight * m_Scale, 0}, segment.color, {u0, v1} };
						pV[vertexIndex + 3] = { {x + charWidth * m_Scale, y + charHeight * m_Scale, 0}, segment.color, {u1, v1} };

						UINT indexStart = charDrawn * 6;
						pI[indexStart + 0] = static_cast<WORD>(vertexIndex + 0);
						pI[indexStart + 1] = static_cast<WORD>(vertexIndex + 1);
						pI[indexStart + 2] = static_cast<WORD>(vertexIndex + 2);

						pI[indexStart + 3] = static_cast<WORD>(vertexIndex + 2);
						pI[indexStart + 4] = static_cast<WORD>(vertexIndex + 1);
						pI[indexStart + 5] = static_cast<WORD>(vertexIndex + 3);

						charDrawn++;
					}
					currentColumn++;
				}
			}
			currentLine++;
		}

		// 頂点バッファと頂点インデックスのロックを解除
		m_pContext->Unmap(m_pVertexBuffer.Get(), 0);
		m_pContext->Unmap(m_pIndexBuffer.Get(), 0);

		// == Backup existing pipeline states ==
		ComPtr<ID3D11BlendState> pPreviousBlendState;
		float previous_blend_factor[4];
		UINT previous_sample_mask;
		m_pContext->OMGetBlendState(pPreviousBlendState.GetAddressOf(), previous_blend_factor, &previous_sample_mask);

		ComPtr<ID3D11DepthStencilState> pPreviousDepthStencilState;
		UINT previous_stencil_ref = 0;
		m_pContext->OMGetDepthStencilState(pPreviousDepthStencilState.GetAddressOf(), &previous_stencil_ref);

		ComPtr<ID3D11RasterizerState> pPreviousRasterizerState;
		m_pContext->RSGetState(pPreviousRasterizerState.GetAddressOf());

		// == Set pipeline states for text rendering ==
		// --- Input Assembler stage (IA) ---
		UINT stride = sizeof(Vertex);
		UINT offset = 0;
		m_pContext->IASetVertexBuffers(0, 1, m_pVertexBuffer.GetAddressOf(), &stride, &offset);
		m_pContext->IASetIndexBuffer(m_pIndexBuffer.Get(), DXGI_FORMAT_R16_UINT, 0);
		m_pContext->IASetInputLayout(m_pInputLayout.Get());
		m_pContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		// --- Vertex Shader stage (VS) ---
		m_pContext->VSSetShader(m_pVertexShader.Get(), nullptr, 0);
		m_pContext->VSSetConstantBuffers(0, 1, m_pVSConstantBuffer.GetAddressOf());

		// --- Pixel Shader stage (PS) ---
		m_pContext->PSSetShader(m_pPixelShader.Get(), nullptr, 0);
		m_pContext->PSSetShaderResources(0, 1, m_pTextureView.GetAddressOf());
		m_pContext->PSSetSamplers(0, 1, m_pSamplerState.GetAddressOf());

		// --- Output Merger stage (OM) ---
		float blend_factor[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
		m_pContext->OMSetBlendState(m_pBlendState.Get(), blend_factor, 0xffffffff);
		m_pContext->OMSetDepthStencilState(m_pDepthStencilState.Get(), 0);

		// --- Rasterizer stage (RS) ---
		m_pContext->RSSetState(m_pRasterizerState.Get());

		// == Draw the text ==
		m_pContext->DrawIndexed(charDrawn * 6, 0, 0);

		// == Restore previous pipeline states ==
		m_pContext->OMSetBlendState(pPreviousBlendState.Get(), previous_blend_factor, 0xffffffff);
		m_pContext->OMSetDepthStencilState(pPreviousDepthStencilState.Get(), previous_stencil_ref);
		m_pContext->RSSetState(pPreviousRasterizerState.Get());
	}

	void DebugText::Clear()
	{
		m_TextLines.clear();
		m_TextLines.emplace_back(); // 最初の行を再度追加
		m_CharacterCount = 0;
	}

	void DebugText::createBuffers(UINT characterCount)
	{
		if (characterCount == 0) return;
		m_BufferAllocatedCount = characterCount;

		D3D11_BUFFER_DESC bd = {};
		bd.Usage = D3D11_USAGE_DYNAMIC;
		bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

		bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		bd.ByteWidth = sizeof(Vertex) * m_BufferAllocatedCount * 4;
		m_pDevice->CreateBuffer(&bd, nullptr, m_pVertexBuffer.ReleaseAndGetAddressOf());

		bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
		bd.ByteWidth = sizeof(WORD) * m_BufferAllocatedCount * 6;
		m_pDevice->CreateBuffer(&bd, nullptr, m_pIndexBuffer.ReleaseAndGetAddressOf());
	}

	// Resize For Window Recreate
	void DebugText::Resize(UINT screenWidth, UINT screenHeight)
	{
		DirectX::XMFLOAT4X4 mtx;
		DirectX::XMStoreFloat4x4(&mtx, DirectX::XMMatrixTranspose(
			DirectX::XMMatrixOrthographicOffCenterLH(0.0f, (float)screenWidth, (float)screenHeight, 0.0f, 0.0f, 1.0f)));
		m_pContext->UpdateSubresource(m_pVSConstantBuffer.Get(), 0, nullptr, &mtx, 0, 0);
	}
}