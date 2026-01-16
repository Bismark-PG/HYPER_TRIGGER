/*==============================================================================

    Manage Bullet Spawan System [Bullet_Manager.cpp]

    Author : Choi HyungJoon

==============================================================================*/
#include "Bullet_Manager.h"
#include "Bullet_Ray.h"
#include "Bullet_Grenade.h"
#include <algorithm>

void Bullet_Manager::Init()
{
    m_BulletList.clear();
}

void Bullet_Manager::Final()
{
    for (Bullet* b : m_BulletList)
    {
        delete b;
    }
    m_BulletList.clear();
}

void Bullet_Manager::Update(double elapsed_time)
{
    for (Bullet* b : m_BulletList)
    {
        b->Update(elapsed_time);
    }

    auto it = std::remove_if(m_BulletList.begin(), m_BulletList.end(),
        [](Bullet* b)
        {
            if (b->IsDead())
            {
                delete b;
                return true;
            }
            return false;
        });

    m_BulletList.erase(it, m_BulletList.end());
}

void Bullet_Manager::Draw()
{
    for (Bullet* b : m_BulletList)
    {
        b->Draw();
    }
}

void Bullet_Manager::Reset()
{
    for (Bullet* b : m_BulletList)
    {
        delete b;
    }

    m_BulletList.clear();
}

void Bullet_Manager::Fire(BulletType type, const DirectX::XMFLOAT3& visualPos, const DirectX::XMFLOAT3& logicalPos,
    const DirectX::XMFLOAT3& dir, BulletOwner owner)
{
    Bullet* N_Bullet = nullptr;

    switch (type)
    {
    case BulletType::RAY:
        N_Bullet = new Bullet_Ray(visualPos, logicalPos, dir, owner);
        break;

    case BulletType::GRENADE:
        N_Bullet = new Bullet_Grenade(visualPos, dir, owner);
        break;
    }

    if (N_Bullet)
    {
        m_BulletList.push_back(N_Bullet);
    }
}