/*==============================================================================

	Manage Bullet And Bullet Type [Bullet.h]

	Author : Choi HyungJoon

==============================================================================*/
#include "Bullet.h"
#include "Bullet_Ray.h"
#include "Bullet_Grenade.h"
#include <vector>
#include <algorithm>

static std::vector<Bullet*> g_BulletList;

void Bullet_System_Initialize()
{
    g_BulletList.clear();
}

void Bullet_System_Finalize()
{
    for (Bullet* b : g_BulletList) delete b;
    g_BulletList.clear();
}

void Bullet_System_Update(double elapsed_time)
{
    for (Bullet* b : g_BulletList)
    {
        b->Update(elapsed_time);
    }

	// Delete Dead Bullets
    auto it = std::remove_if(g_BulletList.begin(), g_BulletList.end(),
        [](Bullet* b)
        {
            if (b->IsDead())
            {
                delete b;
                return true;
            }
            return false;
        });
    g_BulletList.erase(it, g_BulletList.end());
}

void Bullet_System_Draw()
{
    for (Bullet* b : g_BulletList)
    {
        b->Draw();
    }
}
void Bullet_Fire(BulletType type, const DirectX::XMFLOAT3& pos, const DirectX::XMFLOAT3& dir)
{
    Bullet* N_Bullet = nullptr;

    switch (type)
    {
    case BulletType::RAY:
        N_Bullet = new Bullet_Ray(pos, pos, dir, BulletOwner::PLAYER); // Old Ver Logic... Delect..? Upgrade...?
        break;

    case BulletType::GRENADE:
        N_Bullet = new Bullet_Grenade(pos, dir, BulletOwner::PLAYER);
        break;
    }

    if (N_Bullet)
    {
        g_BulletList.push_back(N_Bullet);
    }
}