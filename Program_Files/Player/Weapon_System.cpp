/*==============================================================================

    Manage Weapon Logic and Inventory [Weapon_System.cpp]

    Author : Choi HyungJoon

==============================================================================*/
#include "Weapon_System.h"
#include "Bullet_Manager.h"
#include "KeyLogger.h" 

Weapon_System::Weapon_System()
{
    m_CurrentWeapon = INFO_HANDGUN;
    m_CoolDownTimer = 0.0f;
}

Weapon_System::~Weapon_System()
{
}

void Weapon_System::Init()
{
    m_CurrentWeapon = INFO_HANDGUN;
    m_CoolDownTimer = 0.0f;
}

void Weapon_System::Update(double dt)
{
    if (m_CoolDownTimer > 0.0f)
    {
        m_CoolDownTimer -= static_cast<float>(dt);
    }
}

bool Weapon_System::Fire(const DirectX::XMFLOAT3& visualPos, const DirectX::XMFLOAT3& logicalPos, const DirectX::XMFLOAT3& dir)
{
    if (m_CoolDownTimer > 0.0f)
    {
        return false;
    }

    m_CoolDownTimer = m_CurrentWeapon.FireRate;

    Bullet_Manager::Instance().Fire(m_CurrentWeapon.B_Type, visualPos, logicalPos, dir, BulletOwner::PLAYER);

    return true;
}

void Weapon_System::SwitchWeapon(WeaponType type)
{
    switch (type)
    {
    case WeaponType::HANDGUN:
        m_CurrentWeapon = INFO_HANDGUN;
        break;
    case WeaponType::ASSAULT_RIFLE:
        m_CurrentWeapon = INFO_RIFLE;
        break;
    case WeaponType::GRENADE:
        m_CurrentWeapon = INFO_GRENADE;
        break;
    }

    m_CoolDownTimer = 0.5f;
}

const WeaponInfo& Weapon_System::GetCurrentWeaponInfo() const
{
    return m_CurrentWeapon;
}