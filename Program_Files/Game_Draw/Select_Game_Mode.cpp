/*==============================================================================

    Select Game Mode [Select_Game_Mode.h]

    Author : Choi HyungJoon

==============================================================================*/
#include "Select_Game_Mode.h"
#include "Texture_Manager.h"
#include "Game_Screen_Manager.h"
#include "Audio_Manager.h"
#include "KeyLogger.h"
#include "Controller_Input.h"
#include "Shader_Manager.h"
#include "Palette.h"
#include "Fade.h"
#include "Main_Menu.h"
#include "Player_Camera.h"
#include "Game.h"
using namespace DirectX;
using namespace PALETTE;

// --- Texture ID ---
static int UI_Wave_Mode = -1;
static int UI_Endless_Mode = -1;
static int UI_Back = -1;

// --- POS ---
static float UI_X = 0.0f;
static float UI_Width = 0.0f;
static float UI_Height = 0.0f;

static float Story_Y = 0.0f;
static float Endless_Y = 0.0f;
static float Back_Y = 0.0f;

// --- Mouse State ---
static SELECT_MODE_BUFFER Select_Buffer = SELECT_MODE_BUFFER::NONE;
static GAME_MODE_BUFFER Game_Mode = GAME_MODE_BUFFER::NONE;
static Mouse_Info Mouse_Select;
static bool Is_Mouse_Left_Clicked_Prev = false;
static float Input_Lock_Timer = 0.0f;
static constexpr float INPUT_LOCK_TIME = 0.5f;

// --- Fade ---
static bool Is_Game_Starting = false;

void Select_Game_Mode_Initialize()
{
    Select_Game_Menu_Texture();

    float Screen_W = static_cast<float>(Direct3D_GetBackBufferWidth());
    float Screen_H = static_cast<float>(Direct3D_GetBackBufferHeight());

    Mouse_Select.Size = Screen_H * 0.05f;
    Mouse_Select.Prev_X = Screen_W * 0.5f;
    Mouse_Select.Prev_Y = Screen_H * 0.5f;

    UI_Width  = Screen_W * 0.175f;
    UI_Height = Screen_H * 0.075f;
    UI_X      = Screen_W * 0.2f - UI_Width * 0.5f;

    Story_Y   = Screen_H * 0.55f - UI_Height * 0.5f;
    Endless_Y = Screen_H * 0.7f  - UI_Height * 0.5f;
    Back_Y    = Screen_H * 0.85f - UI_Height * 0.5f;

    Set_Select_Menu_Buffer(SELECT_MODE_BUFFER::NONE);
    Is_Game_Starting = false;

    Input_Lock_Timer = 0.0;

    Is_Mouse_Left_Clicked_Prev = Mouse_State_Reset();
}

void Select_Game_Mode_Finalize()
{
}

void Select_Game_Mode_Update(double elapsed_time)
{
    Main_Menu_BG_Updater(elapsed_time);
    float dt = static_cast<float>(elapsed_time);

    if (Is_Game_Starting)
    {
        if (Fade_GetState() == FADE_STATE::FINISHED_OUT)
        {
            Game_Manager::GetInstance()->Update_Game_Select_Screen(Game_Select_Screen::GAME_PLAYING);
            Player_Camera_Set_Game_Mode();
            Mouse_SetMode(MOUSE_POSITION_MODE_RELATIVE);
            Fade_Start(1.0, false);
            Is_Game_Starting = false;
        }
        return;
    }

    if (Input_Lock_Timer < INPUT_LOCK_TIME)
    {
        Input_Lock_Timer += dt;

        Mouse_State DumpState = Mouse_Get_Prev_State(Mouse_Select);
        Is_Mouse_Left_Clicked_Prev = DumpState.leftButton;
        return;
    }

    Mouse_State State = Mouse_Get_Prev_State(Mouse_Select);
    bool Mouse_Movement = Is_Mouse_Moved();

    bool Click_Trigger = (State.leftButton && !Is_Mouse_Left_Clicked_Prev);
    bool Confirm_Input = (KeyLogger_IsTrigger(KK_ENTER) || XKeyLogger_IsPadTrigger(XINPUT_GAMEPAD_A) || Click_Trigger);

    Is_Mouse_Left_Clicked_Prev = State.leftButton;

    if (Mouse_Movement)
    {
        if (Is_Mouse_In_RECT(Mouse_Select.X, Mouse_Select.Y, UI_X, Story_Y, UI_Width, UI_Height))
        {
            if (Get_Select_Menu_Buffer() != SELECT_MODE_BUFFER::STORY_MODE)
            {
                Set_Select_Menu_Buffer(SELECT_MODE_BUFFER::STORY_MODE);
                Audio_Manager::GetInstance()->Play_SFX("Buffer_Move");
            }
        }
        else if (Is_Mouse_In_RECT(Mouse_Select.X, Mouse_Select.Y, UI_X, Endless_Y, UI_Width, UI_Height))
        {
            if (Get_Select_Menu_Buffer() != SELECT_MODE_BUFFER::ENDLESS_MODE)
            {
                Set_Select_Menu_Buffer(SELECT_MODE_BUFFER::ENDLESS_MODE);
                Audio_Manager::GetInstance()->Play_SFX("Buffer_Move");
            }
        }
        else if (Is_Mouse_In_RECT(Mouse_Select.X, Mouse_Select.Y, UI_X, Back_Y, UI_Width, UI_Height))
        {
            if (Get_Select_Menu_Buffer() != SELECT_MODE_BUFFER::BACK)
            {
                Set_Select_Menu_Buffer(SELECT_MODE_BUFFER::BACK);
                Audio_Manager::GetInstance()->Play_SFX("Buffer_Move");
            }
        }
        else
        {
            Set_Select_Menu_Buffer(SELECT_MODE_BUFFER::WAIT);
        }
    }

    if (KeyLogger_IsTrigger(KK_W) || KeyLogger_IsTrigger(KK_UP) || XKeyLogger_IsPadTrigger(XINPUT_GAMEPAD_DPAD_UP))
    {
        if (Get_Select_Menu_Buffer() == SELECT_MODE_BUFFER::NONE || Get_Select_Menu_Buffer() == SELECT_MODE_BUFFER::WAIT)
        {
            Set_Select_Menu_Buffer(SELECT_MODE_BUFFER::STORY_MODE);
            Audio_Manager::GetInstance()->Play_SFX("Buffer_Move");
        }
        else
        {
            if (Get_Select_Menu_Buffer() == SELECT_MODE_BUFFER::ENDLESS_MODE)
            {
                Set_Select_Menu_Buffer(SELECT_MODE_BUFFER::STORY_MODE);
                Audio_Manager::GetInstance()->Play_SFX("Buffer_Move");
            }
            else if (Get_Select_Menu_Buffer() == SELECT_MODE_BUFFER::BACK)
            {
                Set_Select_Menu_Buffer(SELECT_MODE_BUFFER::ENDLESS_MODE);
                Audio_Manager::GetInstance()->Play_SFX("Buffer_Move");
            }
        }
    }
    else if (KeyLogger_IsTrigger(KK_S) || KeyLogger_IsTrigger(KK_DOWN) || XKeyLogger_IsPadTrigger(XINPUT_GAMEPAD_DPAD_DOWN))
    {
        if (Get_Select_Menu_Buffer() == SELECT_MODE_BUFFER::NONE || Get_Select_Menu_Buffer() == SELECT_MODE_BUFFER::WAIT)
        {
            Set_Select_Menu_Buffer(SELECT_MODE_BUFFER::STORY_MODE);
            Audio_Manager::GetInstance()->Play_SFX("Buffer_Move");
        }
        else
        {
            if (Get_Select_Menu_Buffer() == SELECT_MODE_BUFFER::STORY_MODE)
            {
                Set_Select_Menu_Buffer(SELECT_MODE_BUFFER::ENDLESS_MODE);
                Audio_Manager::GetInstance()->Play_SFX("Buffer_Move");
            }
            else if (Get_Select_Menu_Buffer() == SELECT_MODE_BUFFER::ENDLESS_MODE)
            {
                Set_Select_Menu_Buffer(SELECT_MODE_BUFFER::BACK);
                Audio_Manager::GetInstance()->Play_SFX("Buffer_Move");
            }
        }
    }

    if (Click_Trigger)
    {
        if (Get_Select_Menu_Buffer() != SELECT_MODE_BUFFER::STORY_MODE &&
            Get_Select_Menu_Buffer() != SELECT_MODE_BUFFER::ENDLESS_MODE &&
            Get_Select_Menu_Buffer() != SELECT_MODE_BUFFER::BACK)
        {
            Confirm_Input = false;
        }
    }
    Is_Mouse_Left_Clicked_Prev = State.leftButton;

    if (Confirm_Input)
    {
        if (Get_Select_Menu_Buffer() != SELECT_MODE_BUFFER::NONE && Get_Select_Menu_Buffer() != SELECT_MODE_BUFFER::WAIT)
        {
            switch (Get_Select_Menu_Buffer())
            {
            case SELECT_MODE_BUFFER::STORY_MODE:
            case SELECT_MODE_BUFFER::ENDLESS_MODE:

                // Endless Mode Is Not Maed Now, Just Denined
                if (Get_Select_Menu_Buffer() == SELECT_MODE_BUFFER::ENDLESS_MODE)
                {
                    Audio_Manager::GetInstance()->Play_SFX("Buffer_Denied");
                    break;
                }

                Set_Game_Mode(Get_Select_Menu_Buffer() == SELECT_MODE_BUFFER::STORY_MODE ? GAME_MODE_BUFFER::STORY_MODE : GAME_MODE_BUFFER::ENDLESS_MODE);
                Audio_Manager::GetInstance()->Play_SFX("Buffer_Select");
                Game_Info_Reset();
                Fade_Start(1.0, true);
                Is_Game_Starting = true;
                break;

            case SELECT_MODE_BUFFER::BACK:
                Audio_Manager::GetInstance()->Play_SFX("Buffer_Back");
                Game_Manager::GetInstance()->Update_Main_Screen(Main_Screen::MENU_SELECT);

                Set_Select_Menu_Buffer(SELECT_MODE_BUFFER::NONE);
                Game_Manager::GetInstance()->Update_Game_Select_Screen(Game_Select_Screen::G_WAIT);
                break;
            }
        }
    }
}

void Select_Game_Mode_Draw()
{
    Direct3D_SetDepthEnable(false);
    Shader_Manager::GetInstance()->Begin2D();

    XMFLOAT4 XMF4_A_Origin = Alpha_Origin;
    XMFLOAT4 XMF4_A_Half   = Alpha_Half;

    Sprite_Draw(UI_Wave_Mode, UI_X, Story_Y, UI_Width, UI_Height, A_Zero,
        (Get_Select_Menu_Buffer() == SELECT_MODE_BUFFER::STORY_MODE) ? XMF4_A_Origin : XMF4_A_Half);

    Sprite_Draw(UI_Endless_Mode, UI_X, Endless_Y, UI_Width, UI_Height, A_Zero,
        (Get_Select_Menu_Buffer() == SELECT_MODE_BUFFER::ENDLESS_MODE) ? XMF4_A_Origin : XMF4_A_Half);

    Sprite_Draw(UI_Back, UI_X, Back_Y, UI_Width, UI_Height, A_Zero,
        (Get_Select_Menu_Buffer() == SELECT_MODE_BUFFER::BACK) ? XMF4_A_Origin : XMF4_A_Half);
}

SELECT_MODE_BUFFER Get_Select_Menu_Buffer()
{
    return Select_Buffer;
}

void Set_Select_Menu_Buffer(SELECT_MODE_BUFFER Buffer)
{
    Select_Buffer = Buffer;
}

Mouse_Info Get_Select_Mode_Mouse_POS()
{
    return Mouse_Select;
}

GAME_MODE_BUFFER Get_Game_Mode()
{
    return Game_Mode;
}

void Set_Game_Mode(GAME_MODE_BUFFER Mode)
{
    Game_Mode = Mode;
}

void Select_Game_Menu_Texture()
{
    //----------------UI Texture----------------//
    UI_Wave_Mode = Texture_Manager::GetInstance()->GetID("Wave");
    UI_Endless_Mode = Texture_Manager::GetInstance()->GetID("Endless");
    UI_Back = Texture_Manager::GetInstance()->GetID("UI_Back");
}

