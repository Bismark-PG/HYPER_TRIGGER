/*==============================================================================

	Managed Billboard System [Billboard_Manager.cpp]

	Author : Choi HyungJoon

==============================================================================*/
#include "Billboard_Manager.h"
#include "Billboard_Object.h"
#include "Billboard_Target.h"
#include "Billboard_Effect.h"
#include "Texture_Manager.h"
#include <debug_ostream.h>
using namespace DirectX;

void Billboard_Manager::Init()
{
    m_ObjList.clear();
    m_EffectPool.clear();

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
    for (auto* obj : m_ObjList) delete obj;
    m_ObjList.clear();

    for (auto* effect : m_EffectPool) delete effect;
    m_EffectPool.clear();
}

void Billboard_Manager::Update(double dt)
{
    for (auto it = m_ObjList.begin(); it != m_ObjList.end(); )
    {
        Billboard_Object* obj = *it;
        obj->Update(dt);
        if (!obj->IsActive())
        {
            delete obj;
            it = m_ObjList.erase(it);
        }
        else
        {
            ++it;
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
    for (auto* obj : m_ObjList) obj->Draw();

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

    switch (Type)
    {
    case Billboard_Type::TARGET:
    {
        texID = Texture_Manager::GetInstance()->GetID("Target");

        if (texID == -1)
        {
            Debug::D_Out << "Texture Load Failed" << std::endl;
            return;
        }

        Billboard_Target* New_T = new Billboard_Target(texID, pos, 1.0f, 1.0f);
        New_T->Activate(pos);
        m_ObjList.push_back(New_T);
        break;
    }

    case Billboard_Type::OBJECT:
    {
        texID = Texture_Manager::GetInstance()->GetID("Object");

        if (texID == -1)
        {
            Debug::D_Out << "Texture Load Failed" << std::endl;
            return;
        }

        Billboard_Object* New_O = new Billboard_Object(texID, pos, 5.0f, 5.0f);
        New_O->Activate(pos);
        m_ObjList.push_back(New_O);
        break;
    }
    }
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
    for (auto* obj : m_ObjList)
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