/*==============================================================================

    Management Global Game Light [Light_Manager.h]

    Author : Choi HyungJoon

==============================================================================*/
#ifndef LIGHT_MANAGER_H
#define LIGHT_MANAGER_H

#include <d3d11.h>
#include <DirectXMath.h>

class Light_Manager
{
public:
    static Light_Manager& GetInstance();

    Light_Manager(const Light_Manager&) = delete;
    Light_Manager& operator=(const Light_Manager&) = delete;

    void Init();

    void Global_Light_Set_Up() const;

    void Global_Light_Update(double elapsed_time);

    // --- Setters ---
    void Set_Ambient_Color(const DirectX::XMFLOAT4& color);
    void Set_Directional_Light(const DirectX::XMFLOAT4& dir, const DirectX::XMFLOAT4& color);

	// --- Point Light Management ---
    void Set_Point_Light(int index, const DirectX::XMFLOAT3& pos, float range, const DirectX::XMFLOAT4& color);
    void Set_Point_Light_Active_Count(int count);

private:
    Light_Manager()
        : m_PointLightCount(0)
    {
        m_Ambient.Color = { 0.1f, 0.1f, 0.1f, 1.0f };
        m_Directional.Vector = { 0.0f, -1.0f, 0.0f, 0.0f };
        m_Directional.Color = { 1.0f, 1.0f, 1.0f, 1.0f };

        for (int i = 0; i < 4; ++i)
        {
            m_PointLights[i].Position = { 0.0f, 0.0f, 0.0f };
            m_PointLights[i].Range = 0.0f;
            m_PointLights[i].Color = { 0.0f, 0.0f, 0.0f, 0.0f };
        }
    }
    ~Light_Manager() = default;

    struct AmbientData
    {
        DirectX::XMFLOAT4 Color;
    };

    struct DirectionalData
    {
		DirectX::XMFLOAT4 Vector; // Direction
		DirectX::XMFLOAT4 Color;  // Value
    };

    struct PointLightData
    {
        DirectX::XMFLOAT3 Position;
        float Range;
        DirectX::XMFLOAT4 Color;
    };

    AmbientData      m_Ambient;
    DirectionalData  m_Directional;
    PointLightData   m_PointLights[4];
    int              m_PointLightCount = 0;
};

#endif // LIGHT_MANAGER_H