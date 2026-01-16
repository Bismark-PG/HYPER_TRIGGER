/*==============================================================================
 
    Game Title And Logo [Title.cpp]

	Author : Choi HyungJoon

==============================================================================*/
#include "Title.h"
#include "Game_Screen_Manager.h"
#include "Texture_Manager.h"
#include "Sprite.h"
#include "direct3d.h"    
#include "System_Timer.h"
#include "Fade.h"
#include "BGM_Mixer.h"
#include "Audio_Manager.h"
#include "Shader_Manager.h"
using namespace DirectX;

static int Intro_BG		  = -1;
static int Intro_Logo	  = -1;
static int Intro_Name	  = -1;
static int Intro_Pixel_W  = -1;
static int Intro_Pixel_B  = -1;

static float ScreenW = 0.0f;
static float ScreenH = 0.0f;

static float Logo_Size = 0.0f;
static float Logo_X = 0.0f;
static float Logo_Y = 0.0f;

static float Name_W = 0.0f;
static float Name_H = 0.0f;
static float Name_X = 0.0f;
static float Name_Y = 0.0f;

static float Bar_W = 0.0f;     
static float Bar_H = 0.0f;     
static float Bar_X = 0.0f;     
static float Bar_Y = 0.0f;     
static float Bar_Max_H = 0.0f;

static INTRO_STATE Current_State = INTRO_STATE::FADE_IN_START;
static float Timer = 0.0f;  
static float Bar_Move_Timer = 0.0f;
static float Bar_Expend_Speed = 200.0f;

static const float NAME_REVEAL_TIME = 1.5f;
static const float FADE_DURATION = 1.0f;

static bool Is_Logo_SFX_Playing = false;

static const float FADE_TIME = 1.5f;

void Title_Initialize()
{
	Title_Texture();

	ScreenW = static_cast<float>(Direct3D_GetBackBufferWidth());
	ScreenH = static_cast<float>(Direct3D_GetBackBufferHeight());

	Logo_Size = ScreenH * 0.15f;
	Logo_X = (ScreenW * 0.5f) - (Logo_Size * 0.5f);
	Logo_Y = (ScreenH * 0.4f) - (Logo_Size * 0.5f);

	Name_H = Logo_Size * 0.5f;
	Name_W = Logo_Size * 2.5f;
	Name_X = (ScreenW * 0.5f) - (Name_W * 0.5f);
	Name_Y = (ScreenH * 0.6f) - (Name_H * 0.5f);

	Bar_W = Name_W * 0.01f;
    Bar_Max_H = Name_H;
    Bar_H = 0.0f;
    Bar_X = Name_X;
    Bar_Y = Name_Y + (Name_H * 0.5f);

	Current_State = INTRO_STATE::FADE_IN_START;
    Timer = 0.0f;
    Bar_Move_Timer = 0.0f;
	Is_Logo_SFX_Playing = false;
}

void Title_Finalize()
{

}

void Title_Update(double elapsed_time)
{
    float dt = (float)elapsed_time;

    switch (Current_State)
    {
    case INTRO_STATE::FADE_IN_START:

        if (Fade_GetState() == FADE_STATE::FINISHED_IN)
        {
            Current_State = INTRO_STATE::BAR_EXPAND;

            if (!Is_Logo_SFX_Playing)
            {
                Debug::D_Out << "[Title] Logo Draw Start" << std::endl;
                Audio_Manager::GetInstance()->Play_SFX("Logo_Draw");
                Is_Logo_SFX_Playing = true;
            }
        }
        break;

    case INTRO_STATE::BAR_EXPAND:
        Bar_H += Bar_Expend_Speed * dt;
        Bar_Y = Name_Y + (Name_H * 0.5f) - (Bar_H * 0.5f);

        if (Bar_H >= Bar_Max_H)
        {
            Bar_H = Bar_Max_H;
            Bar_Y = Name_Y;
            Current_State = INTRO_STATE::NAME_REVEAL;
            Bar_Move_Timer = 0.0f;
        }
        break;

    case INTRO_STATE::NAME_REVEAL:
    {
        Bar_Move_Timer += dt;

        float t = Bar_Move_Timer / NAME_REVEAL_TIME;

        if (t >= 1.0f)
        {
            t = 1.0f;
            Current_State = INTRO_STATE::BAR_SHRINK;
        }

        Bar_X = Name_X + (Name_W * t);
    }
    break;

    case INTRO_STATE::BAR_SHRINK:
        Bar_H -= Bar_Expend_Speed * dt;
        Bar_Y = Name_Y + (Name_H * 0.5f) - (Bar_H * 0.5f);

        if (Bar_H <= 0.0f)
        {
            Bar_H = 0.0f;
            Current_State = INTRO_STATE::WAIT_LOGO;
            Timer = 0.0f;
        }
        break;

    case INTRO_STATE::WAIT_LOGO:
        Timer += dt;
        if (Timer >= 1.0f)
        {
            Current_State = INTRO_STATE::FADE_OUT_END;
            Mixer_Init();
            Fade_Start(FADE_DURATION, true);
        }
        break;

    case INTRO_STATE::FADE_OUT_END:
        if (Fade_GetState() == FADE_STATE::FINISHED_OUT)
        {
            Direct3D_SetDepthEnable(true);
            Shader_Manager::GetInstance()->Begin3D();
            Fade_Start(FADE_DURATION, false);
            Debug::D_Out << "[Title] Logo Draw Done" << std::endl;
            Current_State = INTRO_STATE::DONE;
        }
        break;

    case INTRO_STATE::DONE:
        Game_Manager::GetInstance()->Update_Main_Screen(Main_Screen::MENU_SELECT);
        break;
    }
}

void Title_Draw()
{
	Logo_Draw();
}

void Logo_Draw()
{
    if (Current_State == INTRO_STATE::DONE) return;

    if (Intro_BG == -1 || Intro_Logo == -1 || Intro_Name == -1 || Intro_Pixel_B == -1) return;

    Direct3D_SetDepthEnable(false);

    Sprite_Draw(Intro_BG, A_Zero, A_Zero, ScreenW, ScreenH, A_Zero);
    Sprite_Draw(Intro_Logo, Logo_X, Logo_Y, Logo_Size, Logo_Size, A_Zero);

    if (Current_State >= INTRO_STATE::BAR_EXPAND)
    {
        float Current_Width = Bar_X - Name_X;

        if (Current_Width > 0.0f)
        {
            float TexW = (float)Texture_Manager::GetInstance()->Get_Width(Intro_Name);
            float TexH = (float)Texture_Manager::GetInstance()->Get_Height(Intro_Name);

            float Ratio = Current_Width / Name_W;
            if (Ratio > 1.0f)
                Ratio = 1.0f;

            float px = 0.0f;
            float py = 0.0f;
            float pw = TexW * Ratio;
            float ph = TexH;

            Sprite_UV_Draw(Intro_Name, Name_X, Name_Y, Current_Width, Name_H,
                px, py, pw, ph, A_Zero);
        }
    }
    
    Sprite_Draw(Intro_Pixel_W, Bar_X, Bar_Y, Bar_W, Bar_H, A_Zero);
    //Sprite_Draw(Intro_Pixel_W, 0, 0, ScreenW, ScreenH, A_Zero);
}

void Title_Texture()
{
	//---------------Intro Logo Texture---------------//
	Intro_BG		= Texture_Manager::GetInstance()->GetID("K");
	Intro_Logo		= Texture_Manager::GetInstance()->GetID("Bismark_LOGO");
	Intro_Name		= Texture_Manager::GetInstance()->GetID("Bismark_Name");
	Intro_Pixel_W	= Texture_Manager::GetInstance()->GetID("Pixel_Withe");
	Intro_Pixel_B	= Texture_Manager::GetInstance()->GetID("Pixel_Black");
}