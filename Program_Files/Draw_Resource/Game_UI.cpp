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
using namespace PALETTE;

// --- Texture ID ---
static int UI_Aim = -1;
static int UI_HP_Bar = -1;
static int UI_HP_Fill = -1;
static int UI_EXP_Bar = -1;
static int UI_EXP_Fill = -1;
static int UI_Damage_Red = -1;

static int UI_Gum = -1;
static int UI_Ammo = -1;

// --- POS & SIZE ---
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
static float Current_HP_Ratio = 1.0f;
static float Current_EXP_Ratio = 0.0f;

// Damaged
static float Damage_Effect_Timer = 0.0f;
static bool  Is_Damage_Effect_On = false;
static float Damage_Alpha = 0.0f;
constexpr float Damage_Duration = 1.0f;
constexpr float Max_Damage_Alpha = 0.3f; 

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

    // 3. HP Bar (Left Bottom)
    HP_BG_X = 20.0f;
    HP_BG_Y = Screen_H - Bar_H - 20.0f;

    // HP: [Big Pad] [Fill ----->] [Small Pad]
    HP_Min_X = HP_BG_X + (Bar_W * Padding_Big);
    HP_Max_X = HP_BG_X + (Bar_W * (1.0f - Padding_Small));

    // 4. EXP Bar (Right Bottom)
    EXP_BG_X = Screen_W - Bar_W - 20.0f;
    EXP_BG_Y = HP_BG_Y;

    // EXP: [Small Pad] [<----- Fill] [Big Pad]
    EXP_Min_X = EXP_BG_X + (Bar_W * Padding_Small);
    EXP_Max_X = EXP_BG_X + (Bar_W * (1.0f - Padding_Big));

    // 5. Damaged Overlay
    Is_Damage_Effect_On = false;
    Damage_Effect_Timer = 0.0f;
    Damage_Alpha = 0.0f;
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
    Current_HP_Ratio = std::max(0.0f, std::min(Current_HP_Ratio, 1.0f));
    Current_EXP_Ratio = std::max(0.0f, std::min(Current_EXP_Ratio, 1.0f));

    Aim_Radian += Aim_Rotation_Amount * dt;

    if (Is_Damage_Effect_On)
    {
        Damage_Effect_Timer += dt;

        if (Damage_Effect_Timer <= 0.5f)
        {
            float ratio = Damage_Effect_Timer / 0.5f;
            Damage_Alpha = ratio * Max_Damage_Alpha;
        }
        else if (Damage_Effect_Timer <= 1.0f)
        {
            float ratio = (Damage_Effect_Timer - 0.5f) / 0.5f;
            Damage_Alpha = Max_Damage_Alpha * (1.0f - ratio);
        }
        else
        {
            Is_Damage_Effect_On = false;
            Damage_Alpha = 0.0f;
        }
    }
}

void Game_UI_Draw()
{
    Direct3D_SetDepthEnable(false);
    Shader_Manager::GetInstance()->Begin2D();

    // 1. Aim
    Sprite_Draw(UI_Aim, Aim_X, Aim_Y, Aim_Size, Aim_Size, Aim_Radian, Alpha_T_Quarter);

    // -----------------------------------------------------------
    // 2. HP Bar (Left -> Right)
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
    // 3. EXP Bar (Right -> Left)
    // -----------------------------------------------------------
    Sprite_Draw(UI_EXP_Bar, EXP_BG_X, EXP_BG_Y, Bar_W, Bar_H);

    if (Current_EXP_Ratio > 0.0f)
    {
        float Tex_W = static_cast<float>(Texture_Manager::GetInstance()->Get_Width(UI_EXP_Fill));
        float Tex_H = static_cast<float>(Texture_Manager::GetInstance()->Get_Height(UI_EXP_Fill));

        float Draw_Width = (EXP_Max_X - EXP_Min_X) * Current_EXP_Ratio;

        float Draw_X = EXP_Max_X - Draw_Width;

        float UV_Cut_Width = Tex_W * Current_EXP_Ratio;
        float UV_Start_X = Tex_W - UV_Cut_Width;      

        Sprite_UV_Draw(UI_EXP_Fill, Draw_X, EXP_BG_Y, Draw_Width, Bar_H, UV_Start_X, A_Zero, UV_Cut_Width, Tex_H);
    }

    // -----------------------------------------------------------
    // 4. Damager Overlay
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
	//----------------Aim Texture----------------//
	UI_Aim = Texture_Manager::GetInstance()->GetID("Aim");

	//----------------Bar Texture----------------//
	UI_HP_Bar = Texture_Manager::GetInstance()->GetID("HP_Bar");
	UI_HP_Fill = Texture_Manager::GetInstance()->GetID("HP_Fill");
	UI_EXP_Bar = Texture_Manager::GetInstance()->GetID("EXP_Bar");
	UI_EXP_Fill = Texture_Manager::GetInstance()->GetID("EXP_Fill");
}

void Game_UI_Trigger_Damage()
{
    Is_Damage_Effect_On = true;
    Damage_Effect_Timer = 0.0f;
}