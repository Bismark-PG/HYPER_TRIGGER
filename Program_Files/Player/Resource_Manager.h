/*==============================================================================

    Manage Game Resources [Resource_Manager.h]

    Author : Choi HyungJoon

==============================================================================*/
#ifndef RESOURCE_MANAGER_H
#define RESOURCE_MANAGER_H

#include <vector>
#include <DirectXMath.h>
#include "Weapon_Info.h"
#include "Collision.h"
#include "Billboard_Manager.h"

enum class Resource_Type
{
    EXP_ORB,
    HEALTH_POT,
    WEAPON_BOX
};

struct ResourceItem
{
    bool                Active;
    Resource_Type       Type;
    DirectX::XMFLOAT3   Position;
    float               Value;     
    float               Speed;     
    float               LifeTime;

	// For Weapon Box
    WeaponType          W_Type;
    Billboard_Object*   Drop_Box_Icon_Link = nullptr;
};

class Resource_Manager
{
public:
    static Resource_Manager& GetInstance()
    {
        static Resource_Manager instance;
        return instance;
    }

    void Init();
    void Final();
    void Clear();

    void Update(double elapsed_time);
    void Draw();
    void Reset();

    // --- Field Operations (Drop & Collect) ---
    void Spawn_Resource(const DirectX::XMFLOAT3& pos, Resource_Type type, float value, WeaponType wType = WeaponType::HANDGUN);
    ResourceItem* Get_Nearest_Weapon_In_View(const DirectX::XMFLOAT3& pos, const DirectX::XMFLOAT3& dir, float range = 2.0f);

    // --- Account Operations (Stats) ---
    int   Get_Level() const { return m_Level; }
    float Get_Current_Exp() const { return m_CurrentExp; }
    float Get_Max_Exp() const { return m_MaxExp; }
    float Get_Exp_Ratio() const; // UI

	// --- Level Bonus Operations ---
    void Add_Exp_Bonus(float ratio);

private:
    Resource_Manager() = default;
    ~Resource_Manager() = default;
    Resource_Manager(const Resource_Manager&) = delete;
    Resource_Manager& operator=(const Resource_Manager&) = delete;

    // --- Helper Functions ---
    void Apply_Resource_Effect(const ResourceItem& item);
    void Check_Level_Up();

    // --- Field Data ---
    std::vector<ResourceItem> m_Items;
    float m_MagnetRange = 5.0f;       
    float m_CollectRange = 1.0f;      

    // --- Player Stats Data ---
    int   m_Level = 1;
    float m_CurrentExp = 0.0f;
    float m_MaxExp = 100.0f;
    const float m_Exp_Multiplier = 1.25f;

	// --- Level Bonus Data ---
    float m_Exp_Bonus_Ratio = 0.0f;
};

#endif // RESOURCE_MANAGER_H