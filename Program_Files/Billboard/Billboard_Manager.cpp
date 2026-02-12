/*==============================================================================

	Managed Billboard System [Billboard_Manager.cpp]

	Author : Choi HyungJoon

==============================================================================*/
#include "Billboard_Manager.h"

#include "Texture_Manager.h"
#include <debug_ostream.h>
using namespace DirectX;

constexpr float Box_Icon_LifeTime = 15.0f;

void Billboard_Manager::Init()
{
    m_ObjectPool.clear();
    m_EffectPool.clear();

    for (int i = 0; i < MAX_OBJECT_POOL; ++i)
    {
        m_ObjectPool.push_back(new Billboard_Object(-1, { 0,0,0 }, 1.0f, 1.0f));
    }

    for (int i = 0; i < MAX_EFFECT_POOL; ++i)
    {
        m_EffectPool.push_back(new Billboard_Effect());
    }

    int texID = Texture_Manager::GetInstance()->GetID("Enemy_Real_Explosion");
    if (texID != -1)
    {
        XMUINT2 patternSize = { 64, 64 };
        m_ExplosionPatternID = SpriteAni_Get_Pattern_Info(texID, 16, 4, 0.05, patternSize, { 0, 0 }, false);
    }
    else
    {
        Debug::D_Out << "[Billboard Manager] Explosion Texture Not Found!" << std::endl;
    }
}

void Billboard_Manager::Final()
{
    for (auto* obj : m_ObjectPool) delete obj;
    m_ObjectPool.clear();

    for (auto* effect : m_EffectPool) delete effect;
    m_EffectPool.clear();
}

void Billboard_Manager::Reset()
{
    // Return All Objects
    for (auto* obj : m_ObjectPool)
    {
        if (obj->IsActive())
        {
            obj->Deactivate();
        }
    }

    // Reset Effects
    for (auto* effect : m_EffectPool)
    {
        if (effect->IsActive())
        {
            effect->Deactivate();
        }
    }

    Debug::D_Out << "[Billboard] All Objects Reset to Pool." << std::endl;
}

void Billboard_Manager::Update(double dt)
{
    for (auto* obj : m_ObjectPool)
    {
        if (obj->IsActive())
        {
            obj->Update(dt);
        }
    }

    for (auto* effect : m_EffectPool)
    {
        if (effect->IsActive())
        {
            effect->Update(dt);
        }
    }
}

void Billboard_Manager::Draw()
{
    for (auto* obj : m_ObjectPool)
    {
        if (obj->IsActive())
        {
            obj->Draw();
        }
    }

    for (auto* effect : m_EffectPool)
    {
        if (effect->IsActive())
        {
            effect->Draw();
        }
    }
}

void Billboard_Manager::Create(const XMFLOAT3& pos, Billboard_Type Type)
{
    int texID = -1;
    float scale = 1.0f;

    switch (Type)
    {
    case Billboard_Type::TARGET:
        texID = Texture_Manager::GetInstance()->GetID("Target");
        scale = 1.0f;
        break;

    case Billboard_Type::OBJECT:
        texID = Texture_Manager::GetInstance()->GetID("Object");
        scale = 5.0f;
        break;
    }

    if (texID == -1) return;

    for (auto* obj : m_ObjectPool)
    {
        if (!obj->IsActive())
        {
            // Found Inactive Object -> Reuse
            obj->Reset_State(texID, pos, scale, scale);
            obj->Activate(pos);

            // Note : Need On Hit?
            return;
        }
    }

    Debug::D_Out << "[Billboard] Object Pool Full!" << std::endl;
}

void Billboard_Manager::Create_Weapon(const DirectX::XMFLOAT3& pos, WeaponType wType, Billboard_Object** Box_Icon)
{
    std::string Tex_Name = "";
    float Scale = 2.0f;

    switch (wType)
    {
    case WeaponType::HANDGUN:      
        Tex_Name = "UI_Drop_Box_HG";
        break;

    case WeaponType::ASSAULT_RIFLE: 
        Tex_Name = "UI_Drop_Box_AR";
        break;

    case WeaponType::MACHINE_GUN:  
        Tex_Name = "UI_Drop_Box_MG";
        break;

    case WeaponType::GRENADE:     
        Tex_Name = "UI_Drop_Box_G";
        break;

    default: 
        return;
    }

    int Box_Tex_ID = Texture_Manager::GetInstance()->GetID(Tex_Name);

    if (Box_Tex_ID == -1)
    {
        Debug::D_Out << "[Billboard] Weapon Box Texture Not Found : " << Tex_Name << std::endl;
        return;
    }

    DirectX::XMFLOAT3 Spawn_Pos = pos;
    Spawn_Pos.y += 3.0f; // Over The Box

    // Object Pooling
    for (auto* obj : m_ObjectPool)
    {
        // Find Inactive
        if (!obj->IsActive())
        {
            // Reuse & Set Properties
            obj->Reset_State(Box_Tex_ID, Spawn_Pos, Scale, Scale);
            obj->SetLifeTime(Box_Icon_LifeTime);
            obj->Activate(Spawn_Pos);

            // Link Pointer
            if (Box_Icon)
            {
                *Box_Icon = obj;
            }
            return;
        }
    }

    Debug::D_Out << "[Billboard] Weapon Box Pool Full!" << std::endl;
}

void Billboard_Manager::Create_Effect(const XMFLOAT3& pos, int patternID, float scale, Effect_Type Type)
{
    int finalPatternID = patternID;

    if (Type == Effect_Type::EXPLOSION && m_ExplosionPatternID != -1)
        finalPatternID = m_ExplosionPatternID;

    if (finalPatternID == -1) return;

    float finalScale = scale * 0.8f;

    for (auto* effect : m_EffectPool)
    {
        if (!effect->IsActive())
        {
            effect->Reset(finalPatternID, pos, finalScale);
            return;
        }
    }

    Debug::D_Out << "Effect Pool Full!" << std::endl;
}

Billboard_Target* Billboard_Manager::Check_Collision(const AABB& box)
{
    for (auto* obj : m_ObjectPool)
    {
        if (!obj->IsActive())
        {
            continue;
        }

        Billboard_Target* target = dynamic_cast<Billboard_Target*>(obj);

        if (target)
        {
            if (Collision_Is_Hit_AABB(box, target->GetAABB()).Is_Hit)
            {
                return target;
            }
        }
    }
    return nullptr;
}