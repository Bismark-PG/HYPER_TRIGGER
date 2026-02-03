/*==============================================================================

	Manage Player Upgrade System [Upgrade_System.h]

	Author : Choi HyungJoon

==============================================================================*/
#ifndef UPGRADE_SYSTEM
#define UPGRADE_SYSTEM
#include <vector>
#include <string>
#include "Weapon_Info.h"
#include "Card_List.h"

// Upgrade Card Buffer State (Mouse/Keyboard Selection)
enum class Card_Buffer
{
    NONE,
    WAIT,
    CARD_L,
    CARD_M,
    CARD_R
};

class Upgrade_System
{
public:
    static Upgrade_System& GetInstance()
    {
        static Upgrade_System instance;
        return instance;
    }

    void Init();
    void Final();
	void Reset();

    void Update();
    void Draw();

	// If Level UP, Call This Function
    void Show_Upgrade_Select();

	// Upgrade System Active State
    bool Is_Active() const;

	// IF Weapon Is Locked, Get Locked Weapon Type
    void Set_Weapon_Lock(WeaponType type);
    int Get_Weapon_Lock_State() const;

    void Set_Card_Buffer(Card_Buffer Buffer);
    Card_Buffer Get_Card_Buffer() const;

private:
    Upgrade_System() = default;
    ~Upgrade_System() = default;

    void Apply_Upgrade(const Card_Data& card);

    bool m_IsActive = false;
    int  m_LockedWeaponType = -1; // -1: None

	// 3 Current Upgrade Cards
    std::vector<Card_Data> m_CurrentCards;
};

#endif // UPGRADE_SYSTEM