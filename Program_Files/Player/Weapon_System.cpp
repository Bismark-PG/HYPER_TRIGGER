/*==============================================================================

    Manage Weapon Logic and Inventory [Weapon_System.cpp]

    Author : Choi HyungJoon

==============================================================================*/
#include "Weapon_System.h"
#include "Bullet_Manager.h"
#include "Audio_Manager.h"
#include "BGM_Mixer.h"
#include "KeyLogger.h" 

void Weapon_System::Init()
{
    Clear();
}

void Weapon_System::Update(double elapsed_time)
{
    float dt = static_cast<float>(elapsed_time);

    if (m_CoolDownTimer > 0.0f)
    {
        m_CoolDownTimer -= dt;
    }

    if (m_IsReloading)
    {
        m_ReloadTimer -= dt;
        if (m_ReloadTimer <= 0.0f)
        {
            Ammo_Reload();
        }
    }
}

void Weapon_System::Clear()
{
    m_WeaponQueue.clear();
    m_CoolDownTimer = 0.0f;

    AddWeapon(WeaponType::HANDGUN);

    for (int i = 0; i < 4; i++)
    {
        m_Current_Ammo_Bonus[i] = 0;
        m_Reserve_Ammo_Bonus[i] = 0;
    }
}

bool Weapon_System::Fire(const DirectX::XMFLOAT3& visualPos, const DirectX::XMFLOAT3& logicalPos, const DirectX::XMFLOAT3& dir, int damage)
{
    if (m_WeaponQueue.empty()) return false;
    if (m_CoolDownTimer > 0.0f) return false;
    if (m_IsReloading) return false;

    WeaponState& weapon = m_WeaponQueue.front();

    if (weapon.CurrentAmmo <= 0)
    {
        if (weapon.ReserveAmmo > 0)
        {
            Reload();
            return false;
        }
        else
        {
            switch (weapon.Spec.Type) // Need Sound
            {
            case WeaponType::HANDGUN:
                Audio_Manager::GetInstance()->Play_SFX("Attack_1");
                break;
            case WeaponType::ASSAULT_RIFLE:
                Audio_Manager::GetInstance()->Play_SFX("Attack_2");
                break;
            case WeaponType::MACHINE_GUN:
                Audio_Manager::GetInstance()->Play_SFX("Attack_3");
                break;
            case WeaponType::GRENADE:
                Audio_Manager::GetInstance()->Play_SFX("Attack_4");
                break;
            }
            return false;
        }
    }

    m_CoolDownTimer = weapon.Spec.FireRate;
    weapon.CurrentAmmo--;

    Bullet_Manager::Instance().Fire(weapon.Spec.B_Type, visualPos, logicalPos, dir, damage, BulletOwner::PLAYER);

	// Check Ammo And Switch If Needed
    CheckAmmoAndSwitch();

    return true;
}

void Weapon_System::Reload()
{
    if (m_WeaponQueue.empty()) return;
    if (m_IsReloading) return;

    WeaponState& weapon = m_WeaponQueue.front();

    if (weapon.CurrentAmmo >= weapon.Spec.MaxMagazine || weapon.ReserveAmmo <= 0)
    {
        return;
    }

    if (weapon.Spec.ReloadTime <= 0.0f)
    {
        Ammo_Reload();
        return;
    }

    m_IsReloading = true;
    m_ReloadMaxTime = weapon.Spec.ReloadTime;
    m_ReloadTimer = m_ReloadMaxTime;

    switch (weapon.Spec.Type) // Need Sound
    {
    case WeaponType::HANDGUN:
        Audio_Manager::GetInstance()->Play_SFX("Reload_Start_1");
        break;
    case WeaponType::ASSAULT_RIFLE:
        Audio_Manager::GetInstance()->Play_SFX("Reload_Start_2");
        break;
    case WeaponType::MACHINE_GUN:
        Audio_Manager::GetInstance()->Play_SFX("Reload_Start_3");
        break;
    }
}

bool Weapon_System::AddWeapon(WeaponType type)
{
    if (m_WeaponQueue.size() >= m_MaxInventorySize)
    {
        return false;
    }

    WeaponState newState;
    switch (type)
    {
    case WeaponType::HANDGUN: 
        newState.Spec = SPEC_HANDGUN;
        break;

    case WeaponType::ASSAULT_RIFLE:
        newState.Spec = SPEC_RIFLE; 
        break;

    case WeaponType::MACHINE_GUN:  
        newState.Spec = SPEC_MACHINEGUN; 
        break;

    case WeaponType::GRENADE:     
        newState.Spec = SPEC_GRENADE;
        break;
    }

    int Index = static_cast<int>(type);
    int C_Bonus = m_Current_Ammo_Bonus[Index];
    int R_Bonus = m_Reserve_Ammo_Bonus[Index];

    newState.CurrentAmmo = newState.Spec.MaxMagazine + C_Bonus;
    newState.ReserveAmmo = newState.Spec.MaxReserve  + R_Bonus;

    m_WeaponQueue.push_back(newState);

    Audio_Manager::GetInstance()->Play_SFX("Weapon_PickUp");  // Need Sound
    SyncBGM();

    return true;
}

bool Weapon_System::HasWeapon() const
{
    return !m_WeaponQueue.empty();
}

const WeaponState& Weapon_System::GetCurrentWeapon() const
{
    return m_WeaponQueue.front();
}

const std::deque<WeaponState>& Weapon_System::GetInventory() const
{
    return m_WeaponQueue;
}

float Weapon_System::Get_Reload_State() const
{
    if (!m_IsReloading || m_ReloadMaxTime <= 0.0f)
        return 0.0f;

    // 0.0 (Start) -> 1.0 (Done)
    float Progress = 1.0f - (m_ReloadTimer / m_ReloadMaxTime);

    // Safety Clamping (0 ~ 1)
    if (Progress < 0.0f)
    {
        Progress = 0.0f;
    }
    if (Progress > 1.0f)
    {
        Progress = 1.0f;
    }

    return Progress;
}

// ------------------------------------------------------------
//                Weapon_System Private Logic
// ------------------------------------------------------------
Weapon_System::Weapon_System()
{
	Clear();
}

Weapon_System::~Weapon_System()
{
}

void Weapon_System::CheckAmmoAndSwitch()
{
    if (m_WeaponQueue.empty()) return;

    WeaponState& weapon = m_WeaponQueue.front();

    if (weapon.CurrentAmmo <= 0 && weapon.ReserveAmmo <= 0)
    {
        m_WeaponQueue.pop_front();
        Audio_Manager::GetInstance()->Play_SFX("change"); // Need Sound
    }
}

void Weapon_System::Ammo_Reload()
{
    m_IsReloading = false;
    m_ReloadTimer = 0.0f;

    if (m_WeaponQueue.empty()) return;

    // Get Weapon
    WeaponState& weapon = m_WeaponQueue.front();

    // Get Need Ammo ( Max - Now)
    int Ammo_Need = weapon.Spec.MaxMagazine - weapon.CurrentAmmo;
    int Ammo_Amount = (weapon.ReserveAmmo >= Ammo_Need) ? Ammo_Need : weapon.ReserveAmmo;

    weapon.CurrentAmmo += Ammo_Amount;
    weapon.ReserveAmmo -= Ammo_Amount;

    // Need Sound
    // Reload End Sound. Need??
}

void Weapon_System::SyncBGM()
{
    // 1. Weapon Layer Flags
    bool P_Kick = false;
    bool P_Guitar = false;
    bool P_Synth1 = false;
    bool P_Synth2 = false;

    // 2. Check Current Weapon Queue
    for (const auto& weapon : m_WeaponQueue)
    {
        std::string inst = weapon.Spec.InstrumentName;

        if (inst == "Kick") 
        {
            P_Kick = true;
        }
        else if (inst == "Guitar")
        {
            P_Guitar = true;
        }
        else if (inst == "Synth_1") 
        {
            P_Synth1 = true;
        }
        else if (inst == "Synth_2") 
        {
            P_Synth2 = true;
        }
    }

    // 3. Set Mixer Control Layer
	// Will Be Game Start, Snare Layer On 
    Mixer_Control_Layer("Snare", true);

	// Weapon Layers Set
    Mixer_Control_Layer("Kick",     P_Kick);
    Mixer_Control_Layer("Guitar",   P_Guitar);
    Mixer_Control_Layer("Synth_1",  P_Synth1);
    Mixer_Control_Layer("Synth_2",  P_Synth2);
}


void Weapon_System::Add_Ammo_Bonus(WeaponType type, int magBonus, int reserveBonus)
{
    int idx = static_cast<int>(type);
    if (idx >= 0 && idx < 4)
    {
        m_Current_Ammo_Bonus[idx] += magBonus;
        m_Reserve_Ammo_Bonus[idx] += reserveBonus;
    }
}