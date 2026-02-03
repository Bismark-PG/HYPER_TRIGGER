/*==============================================================================

	Manage Draw Resource For Game UI [Game_UI.h]

	Author : Choi HyungJoon

==============================================================================*/
#include "Game_UI.h"
#include "Sprite.h"
#include "Texture_Manager.h"
#include <direct3d.h>
#include "Player.h"          
#include "Resource_Manager.h"
#include "Shader_Manager.h"
#include "Palette.h"
#include "Weapon_System.h"
#include <string>
#include <vector>
#include <cmath>
using namespace PALETTE;

// --- Texture ID ---
static int UI_Aim = -1, UI_Damage_Red = -1;
static int UI_HP_Bar = -1, UI_HP_Fill = -1, UI_EXP_Bar = -1, UI_EXP_Fill = -1;

// Weapon
static int UI_Weapon_BG = -1, UI_Current_Weapon = -1, UI_Next_Weapon_1 = -1, UI_Next_Weapon_2 = -1;
static int UI_Ammo_BG = -1, UI_Icon_HG = -1, UI_Icon_AR = -1, UI_Icon_MG = -1, UI_Icon_G = -1;
static int UI_Reload = -1;

// Numbers
static int UI_Num[10] = { -1 }, UI_Num_Slash = -1;

// --- POS ---
// Aim
static float Aim_X, Aim_Y, Aim_Size, Aim_Radian;
constexpr float Aim_Rotation_Amount = 0.0f;

// UI Bar Layout
static float Bar_W, Bar_H, Bar_Size;

// HP Bar Area
static float HP_BG_X, HP_BG_Y;
static float HP_Min_X, HP_Max_X;

// EXP Bar Area
static float EXP_BG_X, EXP_BG_Y;
static float EXP_Min_X, EXP_Max_X;

// Ratios
static float Current_HP_Ratio = 1.0f, Current_EXP_Ratio = 0.0f;

// Reload
static float Reload_Rotation = 0.0f;
constexpr float Reload_Speed_Min = DirectX::XM_2PI; // Min Speed : 1 Lap For 1 Sec
constexpr float Reload_Speed_Max = DirectX::XM_2PI * A_Double; // Max Speed : 2 Lap For 1 Sec

// Ammo
static const std::deque<WeaponState>* P_Inventory = nullptr;
static float Ammo_Box_X, Ammo_Box_Y, Ammo_Box_W, Ammo_Box_H;
static float Cur_Weapon_X, Cur_Weapon_Y, Cur_Weapon_Size;
static float Next_Weapon_1_X, Next_Weapon_1_Y, Next_Weapon_Size, Next_Weapon_Offset;
static float Next_Weapon_2_X, Next_Weapon_2_Y;

// Damaged
static float Damage_Effect_Timer = 0.0f;
static bool  Is_Damage_Effect_On = false;
static float Damage_Alpha = 0.0f;
constexpr float Max_Damage_Alpha = 0.1f; 

void Draw_Number_String(const std::string& str, float startX, float startY, float width, float height);
static int Get_Weapon_Icon_ID(WeaponType type);

void Game_UI_Initialize()
{
	Game_UI_Texture();

	float Screen_W = static_cast<float>(Direct3D_GetBackBufferWidth());
	float Screen_H = static_cast<float>(Direct3D_GetBackBufferHeight());

    // 1. Aim
    Aim_Size = Screen_H * 0.125f;
    Aim_X = (Screen_W * 0.5f) - (Aim_Size * 0.5f);
    Aim_Y = (Screen_H * 0.5f) - (Aim_Size * 0.5f);
    Aim_Radian = 0.0f;

    // 2. Bar Dimensions
    Bar_Size = Screen_H * 0.05f;
    Bar_W = (Bar_Size * 10.0f);
    Bar_H = (Bar_Size * 1.5f);

    // Set Fill
    float Padding_Big = 0.025f;
    float Padding_Small = 0.01f;

    // 3. EXP Bar
    EXP_BG_X = Screen_W * 0.01f;
    EXP_BG_Y = Screen_H - Bar_H - EXP_BG_X;

    // 4. HP Bar
    HP_BG_X = EXP_BG_X;
    HP_BG_Y = EXP_BG_Y - (Bar_H * 1.2f);

    // HP: [Big Pad] [Fill ----->] [Small Pad]
    HP_Min_X = HP_BG_X + (Bar_W * Padding_Big);
    HP_Max_X = HP_BG_X + (Bar_W * (1.0f - Padding_Small));
    EXP_Min_X = EXP_BG_X + (Bar_W * Padding_Big);
    EXP_Max_X = EXP_BG_X + (Bar_W * (1.0f - Padding_Small));

    // If Want R To L Use This
    // EXP: [Small Pad] [<----- Fill] [Big Pad]
    // EXP_Min_X = EXP_BG_X + (Bar_W * Padding_Small);
    // EXP_Max_X = EXP_BG_X + (Bar_W * (1.0f - Padding_Big));

    // 5. Ammo
    Ammo_Box_W = Bar_W * 0.6f;
    Ammo_Box_H = EXP_BG_Y - HP_BG_Y; 
    Ammo_Box_X = Screen_W - EXP_BG_X - Ammo_Box_W;
    Ammo_Box_Y = EXP_BG_Y;

    // 6. Current Weapon
    Cur_Weapon_Size = Ammo_Box_H;
    Cur_Weapon_X = Ammo_Box_X - (Cur_Weapon_Size * 1.2f);
    Cur_Weapon_Y = Ammo_Box_Y;

    // 7. Next Weapon
    Next_Weapon_Size = Ammo_Box_H * 0.5f;
    Next_Weapon_Offset = (Ammo_Box_W * 0.5f) * 0.5f;

    // Next 1
    Next_Weapon_1_X = Ammo_Box_X + Next_Weapon_Offset - (Next_Weapon_Size * 0.5f);
    Next_Weapon_1_Y = Ammo_Box_Y - (Next_Weapon_Size * 1.2f);

    // Next 2
    Next_Weapon_2_X = Ammo_Box_X + (Next_Weapon_Offset * 1.5f) + (Next_Weapon_Size * 0.5f);;
    Next_Weapon_2_Y = Next_Weapon_1_Y;

    // 8. Damaged Overlay
    Is_Damage_Effect_On = false;
    Damage_Effect_Timer = 0.0f;
    Damage_Alpha = 0.0f;

    // 9. Clear Inventory Pointer
    P_Inventory = nullptr;
}

void Game_UI_Finalize()
{
}

void Game_UI_Update(double elapsed_time)
{
    float dt = static_cast<float>(elapsed_time);

    // HP Ratio Calculation
    int curHP = Player_Get_HP();
    int maxHP = Player_Get_MaxHP();
    if (maxHP > 0)
        Current_HP_Ratio = static_cast<float>(curHP) / static_cast<float>(maxHP);
    else
        Current_HP_Ratio = 0.0f;

    // EXP Ratio Calculation
    Current_EXP_Ratio = Resource_Manager::GetInstance().Get_Exp_Ratio();

    // Clamp (0.0 ~ 1.0)
    Current_HP_Ratio  = std::max(0.0f, std::min(Current_HP_Ratio, 1.0f));
    Current_EXP_Ratio = std::max(0.0f, std::min(Current_EXP_Ratio, 1.0f));

    // Get Weapon Info
    P_Inventory = &Weapon_System::GetInstance().GetInventory();

    if (P_Inventory != nullptr)
    {
        size_t size = P_Inventory->size();

        if (size > 0)
        {
            UI_Current_Weapon = Get_Weapon_Icon_ID(P_Inventory->at(0).Spec.Type);
        }
        else
        {
            UI_Current_Weapon = -1;
        }

        if (size > 1)
        {
            UI_Next_Weapon_1 = Get_Weapon_Icon_ID(P_Inventory->at(1).Spec.Type);
        }
        else
        {
            UI_Next_Weapon_1 = -1;
        }

        if (size > 2)
        {
            UI_Next_Weapon_2 = Get_Weapon_Icon_ID(P_Inventory->at(2).Spec.Type);
        }
        else
        {
            UI_Next_Weapon_2 = -1;
        }
    }

    if (Weapon_System::GetInstance().Is_Reloading())
    {
        // Get Reload Progress
        float Progress = Weapon_System::GetInstance().Get_Reload_State();

        // Increse Rotation Speed
        float Speed = Reload_Speed_Min + (Reload_Speed_Max - Reload_Speed_Min) * Progress;

        // Set Ratio
        Reload_Rotation += Speed * dt;
    }
    else
    {
        // Not Reload Now
        Reload_Rotation = 0.0f;
    }

    if (Is_Damage_Effect_On)
    {
        Damage_Effect_Timer += dt;

        if (Damage_Effect_Timer <= 0.0f)
        {
            float ratio = Damage_Effect_Timer / 0.5f;
            Damage_Alpha = ratio * Max_Damage_Alpha;
        }
        else if (Damage_Effect_Timer <= 0.5f)
        {
            float ratio = (Damage_Effect_Timer - 0.25f) / 0.5f;
            Damage_Alpha = Max_Damage_Alpha * (1.0f - ratio);
        }
        else
        {
            Is_Damage_Effect_On = false;
            Damage_Alpha = 0.0f;
        }
    }

    if (Weapon_System::GetInstance().Is_Reloading())
    {
        Reload_Rotation += dt* DirectX::XM_2PI * 2.0f;
    }
    else
    {
        Reload_Rotation = 0.0f;
    }
}

void Game_UI_Draw()
{
    Direct3D_SetDepthEnable(false);
    Shader_Manager::GetInstance()->Begin2D();

    // -----------------------------------------------------------
    // 1. HP Bar (Left -> Right)
    // -----------------------------------------------------------
    Sprite_Draw(UI_HP_Bar, HP_BG_X, HP_BG_Y, Bar_W, Bar_H);

    if (Current_HP_Ratio > 0.0f)
    {
        float Tex_W = static_cast<float>(Texture_Manager::GetInstance()->Get_Width(UI_HP_Fill));
        float Tex_H = static_cast<float>(Texture_Manager::GetInstance()->Get_Height(UI_HP_Fill));

        float Draw_Width = (HP_Max_X - HP_Min_X) * Current_HP_Ratio;
        float UV_Width = Tex_W * Current_HP_Ratio;

        Sprite_UV_Draw(UI_HP_Fill, HP_Min_X, HP_BG_Y, Draw_Width, Bar_H, A_Zero, A_Zero, UV_Width, Tex_H);
    }

    // -----------------------------------------------------------
    // 2. EXP Bar (Left -> Right)
    // -----------------------------------------------------------
    Sprite_Draw(UI_EXP_Bar, EXP_BG_X, EXP_BG_Y, Bar_W, Bar_H);

    if (Current_EXP_Ratio > 0.0f)
    {
        float Tex_W = static_cast<float>(Texture_Manager::GetInstance()->Get_Width(UI_EXP_Fill));
        float Tex_H = static_cast<float>(Texture_Manager::GetInstance()->Get_Height(UI_EXP_Fill));

        float Draw_Width = (EXP_Max_X - EXP_Min_X) * Current_EXP_Ratio;
        float UV_Width = Tex_W * Current_EXP_Ratio;

        Sprite_UV_Draw(UI_EXP_Fill, EXP_Min_X, EXP_BG_Y, Draw_Width, Bar_H, A_Zero, A_Zero, UV_Width, Tex_H);
    }

    // -----------------------------------------------------------
    // 3. Weapon UI (Right Side)
    // -----------------------------------------------------------

    // Backgrounds (Always Draw Boxes)
    // Ammo Box
    Sprite_Draw(UI_Ammo_BG, Ammo_Box_X, Ammo_Box_Y, Ammo_Box_W, Ammo_Box_H);
    // Cur Weapon Box
    Sprite_Draw(UI_Weapon_BG, Cur_Weapon_X, Cur_Weapon_Y, Cur_Weapon_Size, Cur_Weapon_Size);
    // Next Weapon Boxes
    Sprite_Draw(UI_Weapon_BG, Next_Weapon_1_X, Next_Weapon_1_Y, Next_Weapon_Size, Next_Weapon_Size);
    Sprite_Draw(UI_Weapon_BG, Next_Weapon_2_X, Next_Weapon_2_Y, Next_Weapon_Size, Next_Weapon_Size);

    if (Weapon_System::GetInstance().HasWeapon())
    {
        // A. Current Weapon (Index 0)
        if (P_Inventory != nullptr && !P_Inventory->empty())
        {
            if (UI_Current_Weapon != -1)
            {
                Sprite_Draw(UI_Current_Weapon, Cur_Weapon_X, Cur_Weapon_Y, Cur_Weapon_Size, Cur_Weapon_Size);

                // Ammo Numbers (000 / 000)
                const auto& C_Weapon = P_Inventory->at(0);
                char AmmoBuffer[32];
                sprintf_s(AmmoBuffer, "%03d / %03d", C_Weapon.CurrentAmmo, C_Weapon.ReserveAmmo);

                // Draw Numbers inside Ammo Box
                float Num_W = Ammo_Box_W / 9.0f;
                float Num_H = Ammo_Box_H * 0.6f;
                float Num_Y = Ammo_Box_Y + (Ammo_Box_H - Num_H) * 0.5f; // Center Y

                // Get Middle X POS
                float Box_Center_X = Ammo_Box_X + (Ammo_Box_W * 0.5f);

                // Get Total_Text_Width
                // (Count) * (Wifth) * (0.8)
                float Total_Text_Width = 9.0f * (Num_W * 0.8f);

                // Get Start POS.X = Middle - (Hlaf Of Total Text Width) + (Little Bit)
                float Start_X = Box_Center_X - (Total_Text_Width * 0.5f);

                Draw_Number_String(AmmoBuffer, Start_X, Num_Y, Num_W, Num_H);
            }

            // B. Next Weapon 1 (Index 1)
            if (UI_Next_Weapon_1 != -1)
            {
                Sprite_Draw(UI_Next_Weapon_1, Next_Weapon_1_X, Next_Weapon_1_Y, Next_Weapon_Size, Next_Weapon_Size);
            }

            // C. Next Weapon 2 (Index 2)
            if (UI_Next_Weapon_2 != -1)
            {
                Sprite_Draw(UI_Next_Weapon_2, Next_Weapon_2_X, Next_Weapon_2_Y, Next_Weapon_Size, Next_Weapon_Size);
            }
        }
    }

    // -----------------------------------------------------------
    // 4. Reload And Aim
    // -----------------------------------------------------------
    if (Weapon_System::GetInstance().Is_Reloading())
    {
        Sprite_Draw(UI_Reload, Aim_X, Aim_Y, Aim_Size, Aim_Size, Reload_Rotation);
    }

    Sprite_Draw(UI_Aim, Aim_X, Aim_Y, Aim_Size, Aim_Size, Aim_Radian, Alpha_T_Quarter);

    // -----------------------------------------------------------
    // 5. Damager Overlay
    // -----------------------------------------------------------
    if (Is_Damage_Effect_On && Damage_Alpha > 0.0f)
    {
        float Screen_W = static_cast<float>(Direct3D_GetBackBufferWidth());
        float Screen_H = static_cast<float>(Direct3D_GetBackBufferHeight());

        Palette RedColor = { 1.0f, 0.0f, 0.0f, Damage_Alpha };
        Sprite_Draw(UI_Damage_Red, 0.0f, 0.0f, Screen_W, Screen_H, 0.0f, RedColor);
    }
}

void Game_UI_Texture()
{
	//----------------BG Texture----------------//
    UI_Damage_Red = Texture_Manager::GetInstance()->GetID("R");

	//----------------Aim Texture----------------//
	UI_Aim = Texture_Manager::GetInstance()->GetID("Aim");

	//----------------Bar Texture----------------//
	UI_HP_Bar   = Texture_Manager::GetInstance()->GetID("HP_Bar");
	UI_HP_Fill  = Texture_Manager::GetInstance()->GetID("HP_Fill");
	UI_EXP_Bar  = Texture_Manager::GetInstance()->GetID("EXP_Bar");
	UI_EXP_Fill = Texture_Manager::GetInstance()->GetID("EXP_Fill");

	//----------------Weapon Texture----------------//
    UI_Weapon_BG = Texture_Manager::GetInstance()->GetID("UI_Weapon_Box");
    UI_Ammo_BG = Texture_Manager::GetInstance()->GetID("UI_Ammo_Box");
    UI_Icon_HG = Texture_Manager::GetInstance()->GetID("UI_HG");
    UI_Icon_AR = Texture_Manager::GetInstance()->GetID("UI_AR");
    UI_Icon_MG = Texture_Manager::GetInstance()->GetID("UI_MG");
    UI_Icon_G  = Texture_Manager::GetInstance()->GetID("UI_Grenade");
    UI_Reload = Texture_Manager::GetInstance()->GetID("Reload");

	//----------------Numbers Texture----------------//
    UI_Num[0] = Texture_Manager::GetInstance()->GetID("UI_Num_MIN");
    UI_Num[1] = Texture_Manager::GetInstance()->GetID("UI_Num_1");
    UI_Num[2] = Texture_Manager::GetInstance()->GetID("UI_Num_2");
    UI_Num[3] = Texture_Manager::GetInstance()->GetID("UI_Num_3");
    UI_Num[4] = Texture_Manager::GetInstance()->GetID("UI_Num_4");
    UI_Num[5] = Texture_Manager::GetInstance()->GetID("UI_Num_5");
    UI_Num[6] = Texture_Manager::GetInstance()->GetID("UI_Num_6");
    UI_Num[7] = Texture_Manager::GetInstance()->GetID("UI_Num_7");
    UI_Num[8] = Texture_Manager::GetInstance()->GetID("UI_Num_8");
    UI_Num[9] = Texture_Manager::GetInstance()->GetID("UI_Num_9");
    UI_Num_Slash = Texture_Manager::GetInstance()->GetID("UI_Num_Slash");
}

void Game_UI_Trigger_Damage()
{
    Is_Damage_Effect_On = true;
    Damage_Effect_Timer = 0.0f;
}

void Draw_Number_String(const std::string& str, float startX, float startY, float width, float height)
{
    float Current_X = startX;

    for (char Num : str)
    {
        if (Num >= '0' && Num <= '9')
        {
            int Digit = Num - '0';
            if (UI_Num[Digit] != -1)
            {
                Sprite_Draw(UI_Num[Digit], Current_X, startY, width, height);
            }
        }
        else if (Num == '/')
        {
            Sprite_Draw(UI_Num_Slash, Current_X, startY, width, height);
        }

        Current_X += width * 0.8f;
    }
}

static int Get_Weapon_Icon_ID(WeaponType type)
{
    switch (type)
    {
    case WeaponType::HANDGUN:      
        return UI_Icon_HG;

    case WeaponType::ASSAULT_RIFLE: 
        return UI_Icon_AR;

    case WeaponType::MACHINE_GUN:  
        return UI_Icon_MG;

    case WeaponType::GRENADE:     
        return UI_Icon_G;
    }
    return -1;
}