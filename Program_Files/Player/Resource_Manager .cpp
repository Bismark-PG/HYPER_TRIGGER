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
#include "Upgrade_System.h"
#include "Mash_Field.h"
using namespace DirectX;

void Resource_Manager::Init()
{
    Clear();

}

void Resource_Manager::Final()
{
    Clear();
}

void Resource_Manager::Clear()
{
    m_Items.clear();

    m_Items.reserve(1000);

    m_Level = 1;
    m_CurrentExp = 0.0f;
    m_MaxExp = 100.0f;

    m_Exp_Bonus_Ratio = 0.0f;
}

void Resource_Manager::Update(double elapsed_time)
{
    float dt = static_cast<float>(elapsed_time);

    XMFLOAT3 pPos = Player_Get_POS();
    XMVECTOR vPlayerPos = XMLoadFloat3(&pPos);

    float Hover_Height = 0.5f;

    for (auto& item : m_Items)
    {
        if (!item.Active) continue;

        // Count Life Time
        item.LifeTime -= dt;
        if (item.LifeTime <= 0.0f)
        {
            item.Active = false;

            // If Resource Type Is Weapom Bos, Delete Billboard
            if (item.Type == Resource_Type::WEAPON_BOX)
            {

                if (item.Drop_Box_Icon_Link)
                {
                    item.Drop_Box_Icon_Link->Deactivate();
                }
            }
            continue;
        }

        // Get Height
        float Ground_Y = Mash_Field_Get_Height(item.Position.x, item.Position.z);
        float Target_Y = Ground_Y + Hover_Height;

        // Set Gravity
        if (item.Position.y > Target_Y)
        {
            // Fall 10.0f For Sec
            item.Position.y -= 10.0f * dt;

            if (item.Position.y < Target_Y)
            {
                item.Position.y = Target_Y;
            }
        }
        // If Over The Ground, Get Hovering
        else
        {
            // Hovering
            float bobbing = sinf(static_cast<float>(GetTickCount64()) * 0.005f) * 0.1f;
            item.Position.y = Target_Y + bobbing;
        }

        if (item.Type == Resource_Type::WEAPON_BOX)
        {
            continue;
        }

        // Make Resources With Random Range
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

    // Check Player Current EXP
    Check_Level_Up();
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

void Resource_Manager::Spawn_Resource(const DirectX::XMFLOAT3& pos, Resource_Type type, float value, WeaponType wType)
{
    float lifeTime = {};

    switch (type)
    {
    case Resource_Type::EXP_ORB:
    case Resource_Type::HEALTH_POT:
        lifeTime = 45.0f;
        break;

    case Resource_Type::WEAPON_BOX:
        lifeTime = 15.0f;
        break;
    }

    // Object Pooling
    for (auto& item : m_Items)
    {
        if (!item.Active)
        {
            item.Active = true;
            item.Position = pos;
            item.Type = type;
            item.Value = value;
            item.W_Type = wType;
            item.LifeTime = lifeTime;

            // If Resource Type Is Weapom Bos, Get Billboard
            item.Drop_Box_Icon_Link = nullptr;
            if (type == Resource_Type::WEAPON_BOX)
            {
                Billboard_Manager::Instance().Create_Weapon(pos, wType, &item.Drop_Box_Icon_Link);
            }
            return;
        }
    }

    ResourceItem newItem = {};
    newItem.Active = true;
    newItem.Position = pos;
    newItem.Type = type;
    newItem.Value = value;
    newItem.W_Type = wType;
    newItem.LifeTime = lifeTime;

    // If Resource Type Is Weapom Bos, Get Billboard
    if (type == Resource_Type::WEAPON_BOX)
    {
        Billboard_Manager::Instance().Create_Weapon(pos, wType, &newItem.Drop_Box_Icon_Link);
    }
    m_Items.push_back(newItem);
}

ResourceItem* Resource_Manager::Get_Nearest_Weapon_In_View(const DirectX::XMFLOAT3& pos, const DirectX::XMFLOAT3& dir, float range)
{
    ResourceItem* Is_Weapon_Box = nullptr;
    float Max_Dot = -1.0f;

    XMVECTOR Eye_Pos = XMLoadFloat3(&pos);
    XMVECTOR Look_Dir = XMLoadFloat3(&dir);
    float Range_Sqrt = range * range; // Sprt For Optimization

    for (auto& item : m_Items)
    {
        if (!item.Active || item.Type != Resource_Type::WEAPON_BOX)
        {
            continue;
        }

        // ---------------------------------------------------------
        // Distance Check
        // ---------------------------------------------------------
        XMVECTOR Item_Pos = XMLoadFloat3(&item.Position);
        XMVECTOR Dist = Item_Pos - Eye_Pos;
        float Dist_Sqrt = XMVectorGetX(XMVector3LengthSq(Dist)); // Sprt For Optimization

        if (Dist_Sqrt > Range_Sqrt)
        {
            continue; // To Far
        }

        // ---------------------------------------------------------
        // View Angle Check
        // ---------------------------------------------------------
        XMVECTOR Target_Pos = {};

        // If Box Have Icon, Check Icon POS
        if (item.Drop_Box_Icon_Link && item.Drop_Box_Icon_Link->IsActive())
        {
            XMFLOAT3 POS = item.Drop_Box_Icon_Link->GetPosition();
            Target_Pos = XMLoadFloat3(&POS);
        }
        else
        {
            // If Box Don`t Have Icon, Just Aim To Box (For Safety)
            Target_Pos = Item_Pos;
        }

        // Vector For Eye To Icon(Box)
        XMVECTOR To_Target = Target_Pos - Eye_Pos;
        To_Target = XMVector3Normalize(To_Target);

        // Get Dot Product
        float Dot = XMVectorGetX(XMVector3Dot(Look_Dir, To_Target));

        // ---------------------------------------------------------
        // Judgement
        // Almost Front = 0.9f (Around Degree 25)
        // Maybe Front = 0.85f (Around Degree 30)
        // ---------------------------------------------------------
        if (Dot > 0.85f)
        {
            // If So Many Box, Get Front Box
            if (Dot > Max_Dot)
            {
                Max_Dot = Dot;
                Is_Weapon_Box = &item;
            }
        }
    }

    return Is_Weapon_Box;
}

void Resource_Manager::Apply_Resource_Effect(const ResourceItem& item)
{
    switch (item.Type)
    {
    case Resource_Type::EXP_ORB:
    {
        float finalExp = item.Value * (1.0f + m_Exp_Bonus_Ratio);
        m_CurrentExp += finalExp;
        Audio_Manager::GetInstance()->Play_SFX("Player_Get_EXP");
    }
        break;

    case Resource_Type::HEALTH_POT:
        Audio_Manager::GetInstance()->Play_SFX("Player_Get_HP");
        Player_Heal(static_cast<int>(item.Value));
        break;
    }
}

void Resource_Manager::Check_Level_Up()
{
    // If Player EXP Is Over Then Max EXP, Level UP
    if (m_CurrentExp >= m_MaxExp)
    {
        // Decrease Current EXP
        m_CurrentExp -= m_MaxExp;
        
        // Level UP, Increse Max EXP
        m_Level++;
        m_MaxExp *= 1.2f;

        // Level Up Gift Resource To Player
        Player_LevelUp();
        Debug::D_Out << "Level Up! Current Level: " << m_Level << std::endl;

        Upgrade_System::GetInstance().Show_Upgrade_Select();
    }
}

float Resource_Manager::Get_Exp_Ratio() const
{
    if (m_MaxExp <= 0.0f) return 0.0f;
    return m_CurrentExp / m_MaxExp;
}

void Resource_Manager::Add_Exp_Bonus(float ratio)
{
    m_Exp_Bonus_Ratio += ratio;

    Debug::D_Out << "[Resource] EXP Bonus Added! Current Bonus: " << m_Exp_Bonus_Ratio * 100.0f << "%" << std::endl;
}