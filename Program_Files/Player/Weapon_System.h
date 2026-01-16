/*==============================================================================

    Manage Weapon Logic and Inventory [Weapon_System.h]

    Author : Choi HyungJoon

==============================================================================*/
#ifndef WEAPON_SYSTEM_H
#define WEAPON_SYSTEM_H
#include <DirectXMath.h>
#include "Weapon_Info.h" 

class Weapon_System
{
public:
    static Weapon_System& GetInstance()
    {
        static Weapon_System instance;
        return instance;
    }

    void Init();
    void Update(double dt);

    bool Fire(const DirectX::XMFLOAT3& visualPos, const DirectX::XMFLOAT3& logicalPos, const DirectX::XMFLOAT3& dir);

    void SwitchWeapon(WeaponType type);

    const WeaponInfo& GetCurrentWeaponInfo() const;

private:
    Weapon_System();
    ~Weapon_System();
    Weapon_System(const Weapon_System&) = delete;
    Weapon_System& operator=(const Weapon_System&) = delete;

    WeaponInfo m_CurrentWeapon;
    float      m_CoolDownTimer = 0.0f;
};

#endif // WEAPON_SYSTEM_H