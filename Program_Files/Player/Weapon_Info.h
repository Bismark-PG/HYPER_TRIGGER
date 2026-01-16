/*==============================================================================

    Manage Weapon Info [Weapon_Info.h]

    Author : Choi HyungJoon

==============================================================================*/
#ifndef WEAPON_INFO_H
#define WEAPON_INFO_H
#include "Bullet.h" 

enum class WeaponType
{
    HANDGUN,
    ASSAULT_RIFLE,
    GRENADE
};

struct WeaponInfo
{
    WeaponType  Type;
    BulletType  B_Type;
    float       FireRate;
    bool        IsAutomatic;
    int         Damage;
};

static const WeaponInfo INFO_HANDGUN =
{
    WeaponType::HANDGUN,
    BulletType::RAY,
    0.1f,
    false,
    20
};

static const WeaponInfo INFO_RIFLE =
{
    WeaponType::ASSAULT_RIFLE,
    BulletType::RAY,
    0.05f,
    true,
    15
};

static const WeaponInfo INFO_GRENADE =
{
    WeaponType::GRENADE,
    BulletType::GRENADE,
    1.0f,
    false,
    50
};

#endif // WEAPON_INFO_H