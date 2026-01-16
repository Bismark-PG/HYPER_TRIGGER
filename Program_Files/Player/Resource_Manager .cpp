/*==============================================================================

    Manage Game Resources [Resource_Manager.cpp]

    Author : Choi HyungJoon

==============================================================================*/
#include "Resource_Manager.h"
#include "Particle_Manager.h"
#include "Player.h"     
#include "Game_Model.h" 
#include "Debug_Collision.h"
#include "debug_ostream.h"
#include "Random_Heapler_Logic.h"
#include "Audio_Manager.h"
#include "Shader_Manager.h"
using namespace DirectX;

void Resource_Manager::Init()
{
    m_Items.clear();
    m_Items.reserve(1000);

    m_Level = 1;
    m_CurrentExp = 0.0f;
    m_MaxExp = 100.0f;
}

void Resource_Manager::Final()
{
    m_Items.clear();
}

void Resource_Manager::Update(double dt)
{
    XMFLOAT3 pPos = Player_Get_POS();
    XMVECTOR vPlayerPos = XMLoadFloat3(&pPos);

    for (auto& item : m_Items)
    {
        if (!item.Active) continue;

        if (rand() % 10 < 3)
        {
            XMFLOAT3 particlePos = item.Position;
            particlePos.x += RandomFloatRange(-0.5f, 0.5f);
            particlePos.y += RandomFloatRange(0.0f, 0.5f);
            particlePos.z += RandomFloatRange(-0.5f, 0.5f);

            XMFLOAT3 particleVel = { 0.0f, RandomFloatRange(0.5f, 1.5f), 0.0f };
            float lifeTime = RandomFloatRange(0.5f, 1.0f);
            float size = RandomFloatRange(0.1f, 0.2f);

            Particle_Type pType = Particle_Type::EXP;
            XMFLOAT4 color = { 1.0f, 1.0f, 1.0f, 1.0f };

            if (item.Type == Resource_Type::EXP_ORB)
            {
                pType = Particle_Type::EXP;
            }
            else // HEALTH_POT
            {
                pType = Particle_Type::HP;
            }

            Particle_Manager::GetInstance().Spawn(particlePos, particleVel, color, lifeTime, size, pType);
        }

        XMVECTOR vItemPos = XMLoadFloat3(&item.Position);
        XMVECTOR vDist = XMVector3Length(vPlayerPos - vItemPos);
        float dist = XMVectorGetX(vDist);

        // Magnet Logic
        if (dist < m_MagnetRange)
        {
            XMVECTOR vDir = XMVector3Normalize(vPlayerPos - vItemPos);

            float moveSpeed = 10.0f + (m_MagnetRange - dist) * 5.0f;

            vItemPos += vDir * moveSpeed * static_cast<float>(dt);
            XMStoreFloat3(&item.Position, vItemPos);
        }

        // Collection Logic
        if (dist < m_CollectRange)
        {
            Apply_Resource_Effect(item);
            item.Active = false; // Invisiable
        }
    }
}

void Resource_Manager::Draw()
{
    Particle_Manager::GetInstance().Draw();

    MODEL* pModel = Model_Manager::GetInstance()->GetModel("Ball");
    if (!pModel) return;

    for (const auto& item : m_Items)
    {
        if (!item.Active) continue;

        XMFLOAT4 color = { 1.0f, 1.0f, 1.0f, 1.0f };
        if (item.Type == Resource_Type::EXP_ORB)
        {
            color = { 0.2f, 1.0f, 0.2f, 1.0f };
        }
        else if (item.Type == Resource_Type::HEALTH_POT)
        {
            color = { 1.0f, 0.2f, 0.2f, 1.0f };
        }

        Shader_Manager::GetInstance()->SetDiffuseColor(color);

        XMMATRIX world = XMMatrixScaling(0.3f, 0.3f, 0.3f) * XMMatrixTranslation(item.Position.x, item.Position.y, item.Position.z);

        ModelDraw(pModel, world);
    }

    Shader_Manager::GetInstance()->SetDiffuseColor(XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));
}

void Resource_Manager::Reset()
{
    m_Items.clear();

    Init();
}

void Resource_Manager::Spawn_Resource(const DirectX::XMFLOAT3& pos, Resource_Type type, float value)
{
    // Object Pooling
    for (auto& item : m_Items)
    {
        if (!item.Active)
        {
            item.Active = true;
            item.Position = pos;
            item.Type = type;
            item.Value = value;
            return;
        }
    }

    ResourceItem newItem;
    newItem.Active = true;
    newItem.Position = pos;
    newItem.Type = type;
    newItem.Value = value;
    m_Items.push_back(newItem);
}

void Resource_Manager::Apply_Resource_Effect(const ResourceItem& item)
{
    switch (item.Type)
    {
    case Resource_Type::EXP_ORB:
        m_CurrentExp += item.Value;
        Audio_Manager::GetInstance()->Play_SFX("Player_Get_EXP");
        Check_Level_Up();
        break;

    case Resource_Type::HEALTH_POT:
        Audio_Manager::GetInstance()->Play_SFX("Player_Get_HP");
        Player_Heal(static_cast<int>(item.Value));
        break;
    }
}

void Resource_Manager::Check_Level_Up()
{
    while (m_CurrentExp >= m_MaxExp)
    {
        m_CurrentExp -= m_MaxExp;
        m_Level++;
        m_MaxExp *= 1.2f;
        
        // Stat UP
        // When Update Game, Need To Change Logic
        Player_LevelUp();
        Debug::D_Out << "Level Up! Current Level: " << m_Level << std::endl;
    }
}

float Resource_Manager::Get_Exp_Ratio() const
{
    if (m_MaxExp <= 0.0f) return 0.0f;
    return m_CurrentExp / m_MaxExp;
}