/*==============================================================================

    Managed Billboard Object Type [Billboard_Object.cpp]

    Author : Choi HyungJoon

==============================================================================*/
#include "Billboard_Object.h"

Billboard_Object::Billboard_Object(int texID, const DirectX::XMFLOAT3& pos, float scaleX, float scaleY)
    : m_TexID(texID), m_Position(pos), m_ScaleX(scaleX), m_ScaleY(scaleY), m_IsActive(false)
{
}

Billboard_Object::~Billboard_Object()
{
}

void Billboard_Object::Activate(const DirectX::XMFLOAT3& pos)
{
    m_IsActive = true;
    m_Position = pos;
}

void Billboard_Object::Deactivate()
{
    m_IsActive = false;
}

void Billboard_Object::Update(double dt)
{
    if (m_LifeTime > 0.0f)
    {
        m_LifeTime -= static_cast<float>(dt);

        if (m_LifeTime <= 0.0f)
        {
            Deactivate();
        }
    }
}

void Billboard_Object::Draw()
{
    if (!m_IsActive)
    {
        return;
    }

    if (m_LifeTime > 0.0f && m_LifeTime <= 5.0f)
    {
        int blink = static_cast<int>(m_LifeTime / 0.25f);

        if (blink % 2 == 0)
        {
            return;
        }
    }

    DirectX::XMFLOAT3 DrawPos = m_Position;
    DrawPos.x -= m_ScaleX * 0.5f;
    DrawPos.y -= m_ScaleY * 0.5f;

    Billboard_Draw(m_TexID, DrawPos, m_ScaleX, m_ScaleY, m_Pivot);
}

AABB Billboard_Object::GetAABB() const
{
    float w = m_ScaleX * 0.5f;
    float h = m_ScaleY;
    float d = 0.1f;

    return AABB
    {
        { m_Position.x + w, m_Position.y + h, m_Position.z + d },
        { m_Position.x - w, m_Position.y    , m_Position.z - d } 
    };
}