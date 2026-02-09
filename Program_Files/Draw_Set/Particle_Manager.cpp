/*==============================================================================
   
   Manage Visual Effects (Particles) [Particle_Manager.cpp]
    
    Author : Choi HyungJoon

==============================================================================*/
#include "Particle_Manager.h"
#include "Texture_Manager.h" 
#include "Billboard.h"       
#include "Shader_Manager.h"
#include "debug_ostream.h"
#include "system_timer.h"
#include "Random_Heapler_Logic.h"

using namespace DirectX;


void Particle_Manager::Init()
{
    m_ObjectPool.clear();
    m_ObjectPool.resize(MAX_OBJECT_POOL);
    for (auto& p : m_ObjectPool) p.Active = false;

    m_SparkPool.clear();
    m_SparkPool.resize(MAX_SPARK_POOL);
    for (auto& p : m_SparkPool) p.Active = false;

    srand(static_cast<unsigned int>(SystemTimer_GetAbsoluteTime() * 10000.0));

    m_TexID_Exp = Texture_Manager::GetInstance()->GetID("EXP");
    m_TexID_Hp = Texture_Manager::GetInstance()->GetID("HP");
    m_TexID_Bullet = Texture_Manager::GetInstance()->GetID("Bullet");
    m_TexID_Spark = Texture_Manager::GetInstance()->GetID("Pixel_Withe");

    if (m_TexID_Exp == -1) Debug::D_Out << "[Particle] EXP Texture Load Failed!" << std::endl;
    if (m_TexID_Hp == -1) Debug::D_Out << "[Particle] HP Texture Load Failed!" << std::endl;
    if (m_TexID_Bullet == -1) Debug::D_Out << "[Particle] Bullet Texture Load Failed!" << std::endl;
    if (m_TexID_Spark == -1) Debug::D_Out << "[Particle] Spark Texture Load Failed!" << std::endl;
}

void Particle_Manager::Final()
{
    m_ObjectPool.clear();
    m_SparkPool.clear();
}

void Particle_Manager::Update(double dt)
{
    float fdt = static_cast<float>(dt);

    auto UpdateList = [&](std::vector<Particle>& list)
        {
            for (auto& p : list)
            {
                if (!p.Active) continue;

                p.LifeTime -= fdt;

                if (p.LifeTime <= 0.0f)
                {
                    p.Active = false;
                    continue;
                }

                XMVECTOR vPos = XMLoadFloat3(&p.Position);
                XMVECTOR vVel = XMLoadFloat3(&p.Velocity);
                vPos += vVel * fdt;
                XMStoreFloat3(&p.Position, vPos);
            }
        };


    UpdateList(m_ObjectPool);
    UpdateList(m_SparkPool);
}

void Particle_Manager::Draw()
{
    Shader_Manager::GetInstance()->SetAlphaBlend(true);

    auto DrawList = [&](const std::vector<Particle>& list)
        {
            for (const auto& p : list)
            {
                if (!p.Active) continue;
                Billboard_Draw(p.TexID, p.Position, p.Size, p.Size, { 0.5f, 0.5f }, p.Color);
            }
        };

    DrawList(m_ObjectPool);
    DrawList(m_SparkPool);
}

void Particle_Manager::Spawn(const DirectX::XMFLOAT3& pos, const DirectX::XMFLOAT3& vel,
    const DirectX::XMFLOAT4& color, float lifeTime, float size, Particle_Type type)
{
    int texID = -1;
    std::vector<Particle>* targetPool = nullptr;

    switch (type)
    {
    case Particle_Type::EXP:
        texID = m_TexID_Exp;
        targetPool = &m_ObjectPool;
        break;
    case Particle_Type::HP:
        texID = m_TexID_Hp;
        targetPool = &m_ObjectPool;
        break;
    case Particle_Type::BULLET:
        texID = m_TexID_Bullet;
        targetPool = &m_ObjectPool;
        break;
    case Particle_Type::SPARK:
        texID = m_TexID_Spark;
        targetPool = &m_SparkPool;
        break;
    }

    if (texID == -1 || targetPool == nullptr) return;

    for (auto& p : *targetPool)
    {
        if (!p.Active)
        {
            p.Active = true;
            p.Position = pos;
            p.Velocity = vel;
            p.Color = color;
            p.LifeTime = lifeTime;
            p.MaxLifeTime = lifeTime;
            p.Size = size;
            p.TexID = texID;
            return;
        }
    }
}

void Particle_Manager::Spawn_Spark(const DirectX::XMFLOAT3& pos)
{
    // Particle Count
    int count = 5 + rand() % 4;

    for (int i = 0; i < count; ++i)
    {
        // 1. Random POS
        XMFLOAT3 vel = {};
        vel.x = RandomFloatMinus1To1();
        vel.y = RandomFloatMinus1To1();
        vel.z = RandomFloatMinus1To1();

        XMVECTOR Vec_Vel = XMLoadFloat3(&vel);
        Vec_Vel = XMVector3Normalize(Vec_Vel);

        // 2. Random Speed (10 ~ 20)
        float Speed = 10.0f + RandomFloat() * 10.0f;
        Vec_Vel *= Speed;
        XMStoreFloat3(&vel, Vec_Vel);

        // 3. Random Color (White ~ Yellow)
        float Blue = 0.5f + RandomFloat() * 0.5f;
        XMFLOAT4 Color = { 1.0f, 1.0f, Blue, 1.0f };

        // 4. Random Life Time (0.1 ~ 0.2)
        float Life = 0.1f + RandomFloat() * 0.1f;

        // 5. Random Size (0.1 ~ 0.2)
        float Size = 0.1f + RandomFloat() * 0.1f;

        Spawn(pos, vel, Color, Life, Size, Particle_Type::SPARK);
    }
}