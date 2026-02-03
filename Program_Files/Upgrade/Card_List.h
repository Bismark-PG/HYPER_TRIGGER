/*==============================================================================

    Manage All Upgrade Cards Data & Logic [Card_List.h]

    Author : Choi HyungJoon

==============================================================================*/
#ifndef CARD_LIST_H
#define CARD_LIST_H

#include <string>
#include <vector>
#include "Weapon_Info.h"

// Card ID
enum class Card_Type
{
    // --- Weapon Lock ---
    LOCK_HANDGUN,
    LOCK_RIFLE,
    LOCK_MACHINEGUN,
    LOCK_GRENADE,

    // --- Stat Boosts ---
    ATK_UP_SMALL,       // ATK + 20%
    ATK_UP_BIG,         // ATK + 50%

    // --- Ammo ---
    AMMO_UP_HG,         // HG Ammo +3
    AMMO_UP_AR,         // AR Ammo +15
    AMMO_UP_MG,         // MG Ammo +30
    AMMO_UP_GRENADE,    // Grenade +2

    // --- Stat Boosts (Utill/Survaive) ---
    EXP_UP_SMALL,       // EXP + 10%
    EXP_UP_BIG,         // EXP + 25%
    SPEED_UP,           // SPEED + 20%
    HP_UP,              // HP + 20%

	COUNT // All Card Types
};

enum class Card_Rarity
{
	Normal,
	Rare,
	Legendary
};

struct Card_Data
{
    Card_Type   Type = Card_Type::ATK_UP_SMALL;
	Card_Rarity Rarity = Card_Rarity::Normal;
    std::string Title = "";
    int         TextureID = -1;
};

namespace Card_List
{
    void Init();
    std::vector<Card_Data> Get_Random_Cards(int count);
    void Apply_Card_Effect(Card_Type type);
}

#endif // CARD_LIST_H