/*==============================================================================

    Manage Bullet Spawan System [Bullet_Manager.h]

    Author : Choi HyungJoon

==============================================================================*/
#ifndef BULLET_MANAGER_H
#define BULLET_MANAGER_H

#include <vector>
#include <DirectXMath.h>
#include "Bullet.h"

// --- Bullet System Manager ---
class Bullet_Manager
{
public:
    static Bullet_Manager& Instance()
    {
        static Bullet_Manager instance;
        return instance;
    }

    void Init();
    void Final();

    void Update(double dt);
    void Draw();

    void Reset();

    void Fire(BulletType type, const DirectX::XMFLOAT3& visualPos, const DirectX::XMFLOAT3& logicalPos,
        const DirectX::XMFLOAT3& dir, int damage, BulletOwner owner = BulletOwner::PLAYER);

private:
    Bullet_Manager() = default;
    ~Bullet_Manager() = default;

    Bullet_Manager(const Bullet_Manager&) = delete;
    Bullet_Manager& operator=(const Bullet_Manager&) = delete;

    std::vector<Bullet*> m_BulletList;
};

#endif // BULLET_MANAGER_H