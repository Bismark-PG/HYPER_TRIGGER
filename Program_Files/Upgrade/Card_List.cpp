/*==============================================================================

    Manage All Upgrade Cards Data & Logic [Card_List.cpp]

    Author : Choi HyungJoon

==============================================================================*/
#include "Card_List.h"
#include "Texture_Manager.h"
#include "Upgrade_System.h"   
#include "Player.h"           
#include "Weapon_System.h"    
#include "Resource_Manager.h" 
#include "debug_ostream.h"
#include <map>
#include <algorithm> 
#include <random>    
#include "Random_Heapler_Logic.h"

using namespace std;

static std::map<Card_Type, Card_Data> Card_Database;

struct Prob_Set
{
    float N; // Normal
    float R; // Rare
    float L; // Legendary
};

//----------------Weapon Card----------------//
static int HG_Ammo = -1, HG_Lock = -1, AR_Ammo = -1, AR_Lock = -1;
static int MG_Ammo = -1, MG_Lock = -1, G_Ammo  = -1, G_Lock  = -1;

//----------------Stat Card----------------//
static int ATK_UP_S = -1, ATK_UP_B = -1;
static int EXP_UP_S = -1, EXP_UP_B = -1;
static int HP_UP = -1, Seepd_UP = -1;

//----------------Helper Logic----------------//
static void Build_Database();
static void Card_Texture_Load();
static bool Is_Weapon_Lock_Card(Card_Type type);
static Prob_Set Get_Next_Probabilities(const std::vector<Card_Rarity>& history);

void Card_List::Init()
{
    Card_Database.clear();
    Build_Database();
}

std::vector<Card_Data> Card_List::Get_Random_Cards(int count)
{
    std::vector<Card_Data> result;
    std::vector<Card_Rarity> history;

    // 1. Prepare Pools (Normal, Rare, Legendary)
    std::vector<Card_Data> Pool_N, Pool_R, Pool_L;

    // [Trigger Check] 
    // If Player ALREADY has a locked weapon, exclude all Lock cards.
    bool Is_Already_Locked = (Upgrade_System::GetInstance().Get_Weapon_Lock_State() != -1);

    for (auto const& Card : Card_Database)
    {
        const Card_Data& data = Card.second;

        // Filter: If Weapon Locked, exclude Lock Cards
        if (Is_Already_Locked && Is_Weapon_Lock_Card(data.Type))
        {
            continue;
        }

        if (data.Rarity == Card_Rarity::Normal) 
        {
            Pool_N.push_back(data);
        }
        else if (data.Rarity == Card_Rarity::Rare) 
        {
            Pool_R.push_back(data);
        }
        else if (data.Rarity == Card_Rarity::Legendary) 
        {
            Pool_L.push_back(data);
        }
    }

    // 2. Pick Cards Loop
    for (int i = 0; i < count; ++i)
    {
        // A. Get Probabilities based on History
        Prob_Set Prob = Get_Next_Probabilities(history);

        // B. Roll Dice
        float dice = GetRandomFloat(0.0f, 1.0f);
        Card_Rarity targetRarity = {};

        if (dice < Prob.N)
        {
            targetRarity = Card_Rarity::Normal;
        }
        else if (dice < (Prob.N + Prob.R))
        {
            targetRarity = Card_Rarity::Rare;
        }
        else
        {
            targetRarity = Card_Rarity::Legendary;
        }

        // C. Select Card from Pool (Handle Empty Pool Fallback)
        std::vector<Card_Data>* TargetPool = nullptr;

        // Safety: If target pool is empty, downgrade rarity
        if (targetRarity == Card_Rarity::Legendary && Pool_L.empty())
        {
            targetRarity = Card_Rarity::Rare;
        }

        if (targetRarity == Card_Rarity::Rare && Pool_R.empty())
        {
            targetRarity = Card_Rarity::Normal;
        }

        // Save Rarity It Card Pool
        if (targetRarity == Card_Rarity::Legendary) 
        {
            TargetPool = &Pool_L;
        }
        else if (targetRarity == Card_Rarity::Rare)
        {
            TargetPool = &Pool_R;
        }
        else 
        {
            TargetPool = &Pool_N;
        }

        // No cards left
        if (TargetPool->empty()) 
        {
            break; 
        }

        // Pick Random
        int randIdx = rand() % TargetPool->size();
        Card_Data picked = (*TargetPool)[randIdx];

        result.push_back(picked);
        history.push_back(picked.Rarity);

        // Remove from pool to prevent duplicates in same hand
        TargetPool->erase(TargetPool->begin() + randIdx);

        // [Trigger Check 2]
        // If the picked card IS a Weapon Lock card, remove ALL other Lock cards from pools immediately!
        // (So 2nd and 3rd card cannot be Lock cards)
        if (Is_Weapon_Lock_Card(picked.Type))
        {
            auto Remover = [](const Card_Data& c)
                { 
                    return Is_Weapon_Lock_Card(c.Type);
                };

            Pool_N.erase(std::remove_if(Pool_N.begin(), Pool_N.end(), Remover), Pool_N.end());
            Pool_R.erase(std::remove_if(Pool_R.begin(), Pool_R.end(), Remover), Pool_R.end());
            Pool_L.erase(std::remove_if(Pool_L.begin(), Pool_L.end(), Remover), Pool_L.end());
        }
    }

    return result;
}

void Card_List::Apply_Card_Effect(Card_Type type)
{
    Debug::D_Out << "[Card] Apply Effect ID: " << (int)type << std::endl;

    switch (type)
    {
        // --- Weapon Lock ---
    case Card_Type::LOCK_HANDGUN:   
        Upgrade_System::GetInstance().Set_Weapon_Lock(WeaponType::HANDGUN); 
        break;
    case Card_Type::LOCK_RIFLE:     
        Upgrade_System::GetInstance().Set_Weapon_Lock(WeaponType::ASSAULT_RIFLE);
        break;
    case Card_Type::LOCK_MACHINEGUN: 
        Upgrade_System::GetInstance().Set_Weapon_Lock(WeaponType::MACHINE_GUN);
        break;
    case Card_Type::LOCK_GRENADE:   
        Upgrade_System::GetInstance().Set_Weapon_Lock(WeaponType::GRENADE); 
        break;

        // --- Attack ---
    case Card_Type::ATK_UP_SMALL:
        Player_Add_Damage_Bonus(0.2f); // 20%
        break;

    case Card_Type::ATK_UP_BIG:
        Player_Add_Damage_Bonus(0.5f); // 50%
        break;

        // --- Ammo ---
    case Card_Type::AMMO_UP_HG:     
        Weapon_System::GetInstance().Add_Ammo_Bonus(WeaponType::HANDGUN, 3, 9);
        break;
    case Card_Type::AMMO_UP_AR:      
        Weapon_System::GetInstance().Add_Ammo_Bonus(WeaponType::ASSAULT_RIFLE, 15, 45); 
        break;
    case Card_Type::AMMO_UP_MG:     
        Weapon_System::GetInstance().Add_Ammo_Bonus(WeaponType::MACHINE_GUN, 30, 60); 
        break;
    case Card_Type::AMMO_UP_GRENADE:
        Weapon_System::GetInstance().Add_Ammo_Bonus(WeaponType::GRENADE, 0, 2);
        break;

        // --- Stats ---
    case Card_Type::EXP_UP_SMALL:
        Resource_Manager::GetInstance().Add_Exp_Bonus(0.1f); // 10%
        break;

    case Card_Type::EXP_UP_BIG:
        Resource_Manager::GetInstance().Add_Exp_Bonus(0.25f); // 25%
        break;

    case Card_Type::SPEED_UP:
        Player_Add_Speed_Bonus(0.2f); // 20%
        break;

    case Card_Type::HP_UP:
        Player_Add_HP_Bonus(0.2f); // 20%
        break;
    }
}

static void Build_Database()
{
    Card_Texture_Load();

    // 1. Weapon Lock
    Card_Database[Card_Type::LOCK_HANDGUN]      = { Card_Type::LOCK_HANDGUN,    Card_Rarity::Legendary, "Lock_HG",  HG_Lock };
    Card_Database[Card_Type::LOCK_RIFLE]        = { Card_Type::LOCK_RIFLE,      Card_Rarity::Legendary, "Lock_AR",  AR_Lock };
    Card_Database[Card_Type::LOCK_MACHINEGUN]   = { Card_Type::LOCK_MACHINEGUN, Card_Rarity::Legendary, "Lock_MG",  MG_Lock };
    Card_Database[Card_Type::LOCK_GRENADE]      = { Card_Type::LOCK_GRENADE,    Card_Rarity::Legendary, "Lock_G",   G_Lock };

    // 2. Ammo                                                                           
    Card_Database[Card_Type::AMMO_UP_HG]        = { Card_Type::AMMO_UP_HG,      Card_Rarity::Rare,      "HG_Ammo",  HG_Ammo };
    Card_Database[Card_Type::AMMO_UP_AR]        = { Card_Type::AMMO_UP_AR,      Card_Rarity::Rare,      "AR_Ammo",  AR_Ammo };
    Card_Database[Card_Type::AMMO_UP_MG]        = { Card_Type::AMMO_UP_MG,      Card_Rarity::Rare,      "MG_Ammo",  MG_Ammo };
    Card_Database[Card_Type::AMMO_UP_GRENADE]   = { Card_Type::AMMO_UP_GRENADE, Card_Rarity::Rare,      "G_Ammo",   G_Ammo };

    // 3. Attack                                                                         
    Card_Database[Card_Type::ATK_UP_SMALL]      = { Card_Type::ATK_UP_SMALL,    Card_Rarity::Normal,    "ATK_Up_S", ATK_UP_S };
    Card_Database[Card_Type::ATK_UP_BIG]        = { Card_Type::ATK_UP_BIG,      Card_Rarity::Rare,      "ATK_Up_B", ATK_UP_B };

    // 4. Stats                                                                      
    Card_Database[Card_Type::EXP_UP_SMALL]      = { Card_Type::EXP_UP_SMALL,    Card_Rarity::Normal,    "EXP_Up_S", EXP_UP_S };
    Card_Database[Card_Type::EXP_UP_BIG]        = { Card_Type::EXP_UP_BIG,      Card_Rarity::Rare,      "EXP_Up_B", EXP_UP_B };

    Card_Database[Card_Type::HP_UP]             = { Card_Type::HP_UP,           Card_Rarity::Normal,    "HP_Up",    HP_UP };
    Card_Database[Card_Type::SPEED_UP]          = { Card_Type::SPEED_UP,        Card_Rarity::Normal,    "Speed_Up", Seepd_UP };
}

void Card_Texture_Load()
{
    //----------------Weapon Card----------------//
    HG_Ammo = Texture_Manager::GetInstance()->GetID("Card_HG_Ammo");
    HG_Lock = Texture_Manager::GetInstance()->GetID("Card_HG_Lock");

    AR_Ammo = Texture_Manager::GetInstance()->GetID("Card_AR_Ammo");
    AR_Lock = Texture_Manager::GetInstance()->GetID("Card_AR_Lock");

    MG_Ammo = Texture_Manager::GetInstance()->GetID("Card_MG_Ammo");
    MG_Lock = Texture_Manager::GetInstance()->GetID("Card_MG_Lock");

    G_Ammo = Texture_Manager::GetInstance()->GetID("Card_G_Ammo");
    G_Lock = Texture_Manager::GetInstance()->GetID("Card_G_Lock");

    //----------------Stat Card----------------//
    HP_UP = Texture_Manager::GetInstance()->GetID("Card_HP_UP");

    ATK_UP_S = Texture_Manager::GetInstance()->GetID("Card_ATK_UP_Small");
    ATK_UP_B = Texture_Manager::GetInstance()->GetID("Card_ATK_UP_Big");

    EXP_UP_S = Texture_Manager::GetInstance()->GetID("Card_EXP_UP_Small");
    EXP_UP_B = Texture_Manager::GetInstance()->GetID("Card_EXP_UP_Big");
 
    Seepd_UP = Texture_Manager::GetInstance()->GetID("Card_Speed_UP");
}

static bool Is_Weapon_Lock_Card(Card_Type type)
{
    return (type == Card_Type::LOCK_HANDGUN || type == Card_Type::LOCK_RIFLE ||
            type == Card_Type::LOCK_MACHINEGUN || type == Card_Type::LOCK_GRENADE);
}

// --- Probability Logic Implementation ---
static Prob_Set Get_Next_Probabilities(const std::vector<Card_Rarity>& history)
{
    // Default (0 Cards picked)
    if (history.empty())
        return { 0.5f, 0.3f, 0.2f };

    // 1 Card Picked (Check 1st Card)
    if (history.size() == 1)
    {
        switch (history[0])
        {
        case Card_Rarity::Normal:    return { 0.5f, 0.25f, 0.25f };
        case Card_Rarity::Rare:      return { 0.6f, 0.20f, 0.20f };
        case Card_Rarity::Legendary: return { 0.7f, 0.20f, 0.10f };
        }
    }

    // 2 Cards Picked (Check 1st & 2nd)
    if (history.size() == 2)
    {
        Card_Rarity c1 = history[0];
        Card_Rarity c2 = history[1];

        // 1: Normal
        if (c1 == Card_Rarity::Normal)
        {
            if (c2 == Card_Rarity::Normal)    return { 0.40f, 0.30f, 0.30f }; // N, N
            if (c2 == Card_Rarity::Rare)      return { 0.70f, 0.15f, 0.15f }; // N, R
            if (c2 == Card_Rarity::Legendary) return { 0.80f, 0.10f, 0.10f }; // N, L
        }
        // 1: Rare
        else if (c1 == Card_Rarity::Rare)
        {
            if (c2 == Card_Rarity::Normal)    return { 0.40f, 0.35f, 0.25f }; // R, N
            if (c2 == Card_Rarity::Rare)      return { 0.80f, 0.175f, 0.125f }; // R, R
            if (c2 == Card_Rarity::Legendary) return { 0.80f, 0.15f, 0.05f }; // R, L
        }
        // 1: Legendary
        else if (c1 == Card_Rarity::Legendary)
        {
            if (c2 == Card_Rarity::Normal)    return { 0.75f, 0.15f, 0.10f }; // L, N
            if (c2 == Card_Rarity::Rare)      return { 0.80f, 0.15f, 0.05f }; // L, R 
            if (c2 == Card_Rarity::Legendary) return { 0.90f, 0.07f, 0.03f }; // L, L
        }
    }

    return { 1.0f, 0.0f, 0.0f }; // Fallback
}