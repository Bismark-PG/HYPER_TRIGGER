/*==============================================================================
    
    Game Setting [Setting.cpp]

    Author : Choi HyungJoon

==============================================================================*/
#include "Setting.h"
#include "Main_Menu.h"
#include "Audio_Manager.h"
#include "Texture_Manager.h"
#include "KeyLogger.h"
#include "Controller_Input.h"
#include "Shader_Manager.h"
#include "Game_Screen_Manager.h"
#include "Player_Camera.h"
#include "Palette.h"
#include "Debug_Ostream.h"
using namespace DirectX;
using namespace PALETTE;

//----------------UI Texture----------------//
static int Num_Arr[11];
static int NOW_BGM_TexID;
static int NOW_SFX_TexID;

static int UI_Button_L = -1;
static int UI_Button_R = -1;
static int UI_BGM = -1;
static int UI_SFX = -1;
static int UI_Sens_Text = -1;
static int UI_Back = -1;
static int UI_Background = -1;
static int UI_Pixel_White = -1;

//----------------------POS----------------------//
static float BG_X, BG_Y, BG_W, BG_H;

static float UI_X, UI_H, UI_W;
static float UI_Sens_Y;
static float UI_BGM_Y;
static float UI_SFX_Y;

static float UI_Back_X;
static float UI_Back_Y;

static float UI_Value_X;   

static float NUM_X;
static float L_BUTTON_X;
static float R_BUTTON_X;

static float Slider_Bar_X, Slider_Bar_Y, Slider_Bar_W, Slider_Bar_H;
static float Slider_Box_X, Slider_Box_Y, Slider_Box_Size;

static float NUM_WIDTH;
static float NUM_HEIGHT;
static float NUM_MAX_WIDTH;

//----------------State & Data----------------//
// State Info
static SETTING_BUFFER       S_Buffer = SETTING_BUFFER::SETTING_WAIT;
static SOUND_SETTING_STATE  S_State = SOUND_SETTING_STATE::NONE;

static SOUND_SCALE_BUFFER  BGM_Volume_Level = SOUND_SCALE_BUFFER::FIVE;
static SOUND_SCALE_BUFFER  SFX_Volume_Level = SOUND_SCALE_BUFFER::FIVE;

// Sensitivity
static float Current_Sensitivity;
static const float Sens_Scale = 0.0005f;
static const float Sens_Min = 0.001f;
static const float Sens_Max = 0.1f;
static bool Is_Slider_Dragging = false;

static bool Reset_KeyLogger = false;

// Mouse Info
Mouse_Info Mouse_Setting;
static bool Is_Mouse_Left_Clicked_Prev = false;

// Hleper Logic : To Do >> Get Texture ID For Sound Level
static int Get_Volume_Texture_ID(SOUND_SCALE_BUFFER level);
static int Get_Volume_Texture_ID(SOUND_SCALE_BUFFER level);

void Setting_Initialize()
{
    Setting_Menu_Texture();

    Update_Setting_Buffer(SETTING_BUFFER::SETTING_NONE);
    Update_Sound_Setting_State(SOUND_SETTING_STATE::NONE);
    Update_BGM_Scale_Buffer(SOUND_SCALE_BUFFER::FIVE);
    Update_SFX_Scale_Buffer(SOUND_SCALE_BUFFER::FIVE);

    float Screen_W = static_cast<float>(Direct3D_GetBackBufferWidth());
    float Screen_H = static_cast<float>(Direct3D_GetBackBufferHeight());

    Mouse_Setting.Size = Screen_H * 0.05f;
    Mouse_Setting.Prev_X = Screen_W * 0.5f;
    Mouse_Setting.Prev_Y = Screen_H * 0.5f;

    BG_W = Screen_W * 0.8f;
    BG_H = Screen_H * 0.8f;
    BG_X = (Screen_W - BG_W) * 0.5f;
    BG_Y = (Screen_H - BG_H) * 0.5f;

    UI_X = BG_X + BG_W * 0.15f;
    UI_H = BG_H * 0.1f;
    UI_W = UI_H * 4.0f;
    UI_Back_X = (Screen_W * 0.5f) - (UI_W * 0.5f);
    UI_Value_X = BG_X + BG_W * 0.65f;

    UI_Sens_Y = BG_Y + BG_H * 0.2f;
    UI_BGM_Y = BG_Y + BG_H * 0.4f;
    UI_SFX_Y = BG_Y + BG_H * 0.6f;
    UI_Back_Y = BG_Y + BG_H * 0.8f;

    NUM_X = UI_Value_X - (UI_H * 0.5f);
    float Arrow_Gap = UI_H * 1.5f;
    L_BUTTON_X = UI_Value_X - Arrow_Gap - (UI_H * 0.5f);
    R_BUTTON_X = UI_Value_X + Arrow_Gap - (UI_H * 0.5f);

    // Slider Setup
    Slider_Bar_W = BG_W * 0.35f;
    Slider_Bar_H = UI_H * 0.2f;
    Slider_Bar_X = UI_Value_X - (Slider_Bar_W * 0.5f);
    Slider_Bar_Y = UI_Sens_Y + (UI_H * 0.4f);

    Slider_Box_Size = UI_H * 0.5f;
    Slider_Box_X = Slider_Bar_X + (Slider_Box_Size * 0.5f);
    Slider_Box_Y = Slider_Bar_Y + (Slider_Bar_H - Slider_Box_Size) * 0.5f;

    Reset_KeyLogger = false;

    Current_Sensitivity = Get_Mouse_Sensitivity();
    Is_Slider_Dragging = false;
}

void Setting_Finalize()
{
}

void Setting_Update(double elapsed_time)
{
    if (Reset_KeyLogger)
    {
        if (KeyLogger_IsAnyKeyReleased() || XKeyLogger_IsAnyPadReleased())
            Reset_KeyLogger = false;
        return;
    }

    // Get Mouse Update
    Mouse_State State = Mouse_Get_Prev_State(Mouse_Setting);
    bool Mouse_Movement = Is_Mouse_Moved();
    float Mouse_X = Mouse_Setting.X;
    float Mouse_Y = Mouse_Setting.Y;

    // Slider Box POS Check
    float ratio = (Get_Mouse_Sensitivity() - Sens_Min) / (Sens_Max - Sens_Min);
    ratio = std::max(0.0f, std::min(ratio, 1.0f));
    Slider_Box_X = Slider_Bar_X + (Slider_Bar_W * ratio) - (Slider_Box_Size * 0.5f);

    bool Click_Trigger = (State.leftButton && !Is_Mouse_Left_Clicked_Prev);
    bool Enter_Trigger = (KeyLogger_IsTrigger(KK_ENTER) || XKeyLogger_IsPadTrigger(XINPUT_GAMEPAD_A));
    bool Confirm = (Click_Trigger || Enter_Trigger);

    Is_Mouse_Left_Clicked_Prev = State.leftButton;

    if (Get_Setting_State() == SOUND_SETTING_STATE::NONE)
    {
        if (Mouse_Movement)
        {
            if ((Is_Mouse_In_RECT(Mouse_Setting.X, Mouse_Setting.Y, UI_X, UI_Sens_Y, UI_W, UI_H)) ||
                (Is_Mouse_In_RECT(Mouse_Setting.X, Mouse_Setting.Y, Slider_Bar_X, Slider_Bar_Y, Slider_Bar_W, Slider_Bar_H)))
            {
                if (Get_Setting_Buffer() != SETTING_BUFFER::SENSITIVITY)
                {
                    Audio_Manager::GetInstance()->Play_SFX("Buffer_Move");
                    Update_Setting_Buffer(SETTING_BUFFER::SENSITIVITY);
                }
            }
            else if (Is_Mouse_In_RECT(Mouse_Setting.X, Mouse_Setting.Y, UI_X, UI_BGM_Y, UI_W, UI_H))
            {
                if (Get_Setting_Buffer() != SETTING_BUFFER::BGM)
                {
                    Audio_Manager::GetInstance()->Play_SFX("Buffer_Move");
                    Update_Setting_Buffer(SETTING_BUFFER::BGM);
                }
            }
            else if (Is_Mouse_In_RECT(Mouse_Setting.X, Mouse_Setting.Y, UI_X, UI_SFX_Y, UI_W, UI_H))
            {
                if (Get_Setting_Buffer() != SETTING_BUFFER::SFX)
                {
                    Audio_Manager::GetInstance()->Play_SFX("Buffer_Move");
                    Update_Setting_Buffer(SETTING_BUFFER::SFX);
                }
            }
            else if (Is_Mouse_In_RECT(Mouse_Setting.X, Mouse_Setting.Y, UI_Back_X, UI_Back_Y, UI_W, UI_H))
            {
                if (Get_Setting_Buffer() != SETTING_BUFFER::SETTING_BACK)
                {
                    Audio_Manager::GetInstance()->Play_SFX("Buffer_Move");
                    Update_Setting_Buffer(SETTING_BUFFER::SETTING_BACK);
                }
            }
            else
            {
                Update_Setting_Buffer(SETTING_BUFFER::SETTING_WAIT);
            }
        }

        if (KeyLogger_IsTrigger(KK_W) || KeyLogger_IsTrigger(KK_UP) || XKeyLogger_IsPadTrigger(XINPUT_GAMEPAD_DPAD_UP))
        {
            if (Get_Setting_Buffer() == SETTING_BUFFER::SETTING_NONE || Get_Setting_Buffer() == SETTING_BUFFER::SETTING_WAIT)
            {
                Audio_Manager::GetInstance()->Play_SFX("Buffer_Move");
                Update_Setting_Buffer(SETTING_BUFFER::SENSITIVITY);
            }
            else
            {
                if (Get_Setting_Buffer() == SETTING_BUFFER::BGM)
                {
                    Audio_Manager::GetInstance()->Play_SFX("Buffer_Move");
                    Update_Setting_Buffer(SETTING_BUFFER::SENSITIVITY);
                }
                else if (Get_Setting_Buffer() == SETTING_BUFFER::SFX)
                {
                    Audio_Manager::GetInstance()->Play_SFX("Buffer_Move");
                    Update_Setting_Buffer(SETTING_BUFFER::BGM);
                }
                else if (Get_Setting_Buffer() == SETTING_BUFFER::SETTING_BACK)
                {
                    Audio_Manager::GetInstance()->Play_SFX("Buffer_Move");
                    Update_Setting_Buffer(SETTING_BUFFER::SFX);
                }
            }
        }
        else if (KeyLogger_IsTrigger(KK_S) || KeyLogger_IsTrigger(KK_DOWN) || XKeyLogger_IsPadTrigger(XINPUT_GAMEPAD_DPAD_DOWN))
        {
            if (Get_Setting_Buffer() == SETTING_BUFFER::SETTING_NONE || Get_Setting_Buffer() == SETTING_BUFFER::SETTING_WAIT)
            {
                Audio_Manager::GetInstance()->Play_SFX("Buffer_Move");
                Update_Setting_Buffer(SETTING_BUFFER::SETTING_BACK);
            }
            else
            {
                if (Get_Setting_Buffer() == SETTING_BUFFER::SENSITIVITY)
                {
                    Audio_Manager::GetInstance()->Play_SFX("Buffer_Move");
                    Update_Setting_Buffer(SETTING_BUFFER::BGM);
                }
                else if (Get_Setting_Buffer() == SETTING_BUFFER::BGM)
                {
                    Audio_Manager::GetInstance()->Play_SFX("Buffer_Move");
                    Update_Setting_Buffer(SETTING_BUFFER::SFX);
                }
                else if (Get_Setting_Buffer() == SETTING_BUFFER::SFX)
                {
                    Audio_Manager::GetInstance()->Play_SFX("Buffer_Move");
                    Update_Setting_Buffer(SETTING_BUFFER::SETTING_BACK);
                }
            }
        }

        if (Confirm)
        {
            bool Is_Valid_Selection = (Get_Setting_Buffer() != SETTING_BUFFER::SETTING_NONE && Get_Setting_Buffer() != SETTING_BUFFER::SETTING_WAIT);

            if (Is_Valid_Selection)
            {
                Audio_Manager::GetInstance()->Play_SFX("Buffer_Select");

                switch (Get_Setting_Buffer())
                {
                case SETTING_BUFFER::SENSITIVITY:
                    Update_Sound_Setting_State(SOUND_SETTING_STATE::SENSITIVITY_SETTING);
                    break;

                case SETTING_BUFFER::BGM:
                    Update_Sound_Setting_State(SOUND_SETTING_STATE::BGM_SETTING);
                    break;

                case SETTING_BUFFER::SFX:
                    Update_Sound_Setting_State(SOUND_SETTING_STATE::SFX_SETTING);
                    break;

                case SETTING_BUFFER::SETTING_BACK:
                    if (Game_Manager::GetInstance()->Get_Current_Main_Screen() == Main_Screen::SELECT_GAME)
                    {
                        Game_Manager::GetInstance()->Update_Game_Select_Screen(Game_Select_Screen::GAME_IN_GAME_MENU);
                    }
                    else
                    {
                        Game_Manager::GetInstance()->Update_Sub_Screen(Sub_Screen::S_DONE);
                    }

                    Update_Setting_Buffer(SETTING_BUFFER::SETTING_NONE);
                    break;
                }
            }
        }
    }
   else if (Get_Setting_State() == SOUND_SETTING_STATE::SENSITIVITY_SETTING)
   {
       if (KeyLogger_IsTrigger(KK_BACK) || XKeyLogger_IsPadTrigger(XINPUT_GAMEPAD_B) || State.rightButton || Confirm)
       {
           bool Clicked_Slider = false;

           if (Click_Trigger)
           {
               if (Is_Mouse_In_RECT(Mouse_X, Mouse_Y, Slider_Bar_X - 10, Slider_Bar_Y - 20, Slider_Bar_W + 20, UI_H))
               {
                   Clicked_Slider = true;
               }
           }

           if (!Clicked_Slider)
           {
               Audio_Manager::GetInstance()->Play_SFX("Buffer_Select");
               Update_Sound_Setting_State(SOUND_SETTING_STATE::NONE);
               Is_Slider_Dragging = false;
               return;
           }
       }

       // Set Sensitivity (Keyboard)
       if (KeyLogger_IsTrigger(KK_A) || KeyLogger_IsTrigger(KK_LEFT) || XKeyLogger_IsPadTrigger(XINPUT_GAMEPAD_DPAD_LEFT))
       {
           Current_Sensitivity -= Sens_Scale;
       }
       else if (KeyLogger_IsTrigger(KK_D) || KeyLogger_IsTrigger(KK_RIGHT) || XKeyLogger_IsPadTrigger(XINPUT_GAMEPAD_DPAD_RIGHT))
       {
           Current_Sensitivity += Sens_Scale;
       }

       // Cheaack Mouse Trigger
       if (Click_Trigger && Is_Mouse_In_RECT(Mouse_X, Mouse_Y, Slider_Bar_X - 10, Slider_Bar_Y - 20, Slider_Bar_W + 20, UI_H))
       {
           Is_Slider_Dragging = true;
       }

       // Set Sensitivity
       if (Is_Slider_Dragging && KeyLogger_IsMousePressed(Mouse_Button::LEFT))
       {
           float newRatio = (Mouse_X - Slider_Bar_X) / Slider_Bar_W;
           newRatio = std::max(0.0f, std::min(newRatio, 1.0f));
           Current_Sensitivity = Sens_Min + newRatio * (Sens_Max - Sens_Min);
       }
       else
       {
           Is_Slider_Dragging = false;
       }

       // Set Min Max
       Current_Sensitivity = std::max(Sens_Min, std::min(Current_Sensitivity, Sens_Max));
       Set_Mouse_Sensitivity(Current_Sensitivity);

       Debug::D_Out << "[Setting} Now Mouse Sensitivity : " << Current_Sensitivity << std::endl;
    }
   else if (Get_Setting_State() == SOUND_SETTING_STATE::BGM_SETTING || Get_Setting_State() == SOUND_SETTING_STATE::SFX_SETTING)
   {
        // 1. Get Now Mode
        bool Is_BGM_Mode = (Get_Setting_State() == SOUND_SETTING_STATE::BGM_SETTING);

        // 2. Data Set
        int currentVal = static_cast<int>(Is_BGM_Mode ? Get_BGM_Scale_Buffer() : Get_SFX_Scale_Buffer());
        float Target_UI_Y = Is_BGM_Mode ? UI_BGM_Y : UI_SFX_Y;

        // --- Back / Cancel Logic ---
        if (KeyLogger_IsTrigger(KK_BACK) || XKeyLogger_IsPadTrigger(XINPUT_GAMEPAD_B) || State.rightButton || Confirm)
        {
            bool Clicked_Arrow = false;
            if (Click_Trigger)
            {
                if (Is_Mouse_In_RECT(Mouse_X, Mouse_Y, L_BUTTON_X, Target_UI_Y, UI_H, UI_H))
                {
                    Clicked_Arrow = true;
                }
                else if (Is_Mouse_In_RECT(Mouse_X, Mouse_Y, R_BUTTON_X, Target_UI_Y, UI_H, UI_H))
                {
                    Clicked_Arrow = true;
                }
            }

            if (!Clicked_Arrow)
            {
                Audio_Manager::GetInstance()->Play_SFX("Buffer_Select");
                Update_Sound_Setting_State(SOUND_SETTING_STATE::NONE);
                return;
            }
        }

        // --- Value Change Logic ---
        bool Changed = false;

        // Keyboard Input
        if (KeyLogger_IsTrigger(KK_A) || KeyLogger_IsTrigger(KK_LEFT) || XKeyLogger_IsPadTrigger(XINPUT_GAMEPAD_DPAD_LEFT))
        {
            currentVal--;
            Changed = true;
            Audio_Manager::GetInstance()->Play_SFX("Buffer_Move");
        }
        else if (KeyLogger_IsTrigger(KK_D) || KeyLogger_IsTrigger(KK_RIGHT) || XKeyLogger_IsPadTrigger(XINPUT_GAMEPAD_DPAD_RIGHT))
        {
            currentVal++;
            Changed = true;
            Audio_Manager::GetInstance()->Play_SFX("Buffer_Move");
        }

        // Mouse Input
        if (Click_Trigger)
        {
            if (Is_Mouse_In_RECT(Mouse_X, Mouse_Y, L_BUTTON_X, Target_UI_Y, UI_H, UI_H)) // Left Arrow
            {
                currentVal--;
                Changed = true;
                Audio_Manager::GetInstance()->Play_SFX("Buffer_Move");
            }
            else if (Is_Mouse_In_RECT(Mouse_X, Mouse_Y, R_BUTTON_X, Target_UI_Y, UI_H, UI_H)) // Right Arrow
            {
                currentVal++;
                Changed = true;
                Audio_Manager::GetInstance()->Play_SFX("Buffer_Move");
            }
        }

        // --- Apply Changes ---
        if (Changed)
        {
            // Set Volume Range (0 ~ 10)
            currentVal = std::max(0, std::min(currentVal, 10));

            // Get Volume (0.0 ~ 1.0)
            float vol = static_cast<float>(currentVal) / 10.0f;

            // Call Audio Set Logic
            if (Is_BGM_Mode)
            {
                Update_BGM_Scale_Buffer(static_cast<SOUND_SCALE_BUFFER>(currentVal));
                Audio_Manager::GetInstance()->Set_Target_BGM_Volume(vol);
            }
            else
            {
                Update_SFX_Scale_Buffer(static_cast<SOUND_SCALE_BUFFER>(currentVal));
                Audio_Manager::GetInstance()->Set_Target_SFX_Volume(vol);
            }
        }
    }
}

void Setting_Draw()
{
    Direct3D_SetDepthEnable(false);
    Shader_Manager::GetInstance()->Begin2D();

    // Common Colors
    XMFLOAT4 Select_Color = Alpha_Origin;
    XMFLOAT4 Normal_Color = Alpha_Half;

    // Background Panel
    Sprite_Draw(UI_Background, BG_X, BG_Y, BG_W, BG_H, 0.0f, Alpha_Origin);

    // ------------------------------------------------------------
    // 1. Sensitivity Slider
    // ------------------------------------------------------------
    bool Is_Sens_Focus = (Get_Setting_Buffer() == SETTING_BUFFER::SENSITIVITY || Get_Setting_State() == SOUND_SETTING_STATE::SENSITIVITY_SETTING);
    Sprite_Draw(UI_Sens_Text, UI_X, UI_Sens_Y, UI_W, UI_H, 0.0f, Is_Sens_Focus ? Select_Color : Normal_Color);

    // Slider Bar & Box
    Sprite_Draw(UI_Pixel_White, Slider_Bar_X, Slider_Bar_Y, Slider_Bar_W, Slider_Bar_H, 0.0f, Is_Sens_Focus ? Select_Color : Normal_Color);
    Sprite_Draw(UI_Pixel_White, Slider_Box_X, Slider_Box_Y, Slider_Box_Size, Slider_Box_Size, 45.0f, Is_Sens_Focus ? Select_Color : Light_Gray);

    // ------------------------------------------------------------
    // 2. BGM Setting (UI + Arrows + Number)
    // ------------------------------------------------------------
    bool Is_BGM_Focus = (Get_Setting_Buffer() == SETTING_BUFFER::BGM || Get_Setting_State() == SOUND_SETTING_STATE::BGM_SETTING);
    XMFLOAT4 BGM_Color = Is_BGM_Focus ? Select_Color : Normal_Color;

    // Label "BGM"
    Sprite_Draw(UI_BGM, UI_X, UI_BGM_Y, UI_W, UI_H, 0.0f, BGM_Color);

    // Arrows (Left / Right)
    Sprite_Draw(UI_Button_L, L_BUTTON_X, UI_BGM_Y, UI_H, UI_H, 0.0f, BGM_Color);
    Sprite_Draw(UI_Button_R, R_BUTTON_X, UI_BGM_Y, UI_H, UI_H, 0.0f, BGM_Color);

    // Number (0 ~ 10)
    int bgmIdx = static_cast<int>(Get_BGM_Scale_Buffer()); // Enum -> Int
    if (bgmIdx >= 0 && bgmIdx <= 10)
    {
        Sprite_Draw(Num_Arr[bgmIdx], NUM_X, UI_BGM_Y, UI_H, UI_H, 0.0f, BGM_Color);
    }

    // ------------------------------------------------------------
    // 3. SFX Setting (UI + Arrows + Number)
    // ------------------------------------------------------------
    bool Is_SFX_Focus = (Get_Setting_Buffer() == SETTING_BUFFER::SFX || Get_Setting_State() == SOUND_SETTING_STATE::SFX_SETTING);
    XMFLOAT4 SFX_Color = Is_SFX_Focus ? Select_Color : Normal_Color;

    // Label "SFX"
    Sprite_Draw(UI_SFX, UI_X, UI_SFX_Y, UI_W, UI_H, 0.0f, SFX_Color);

    // Arrows
    Sprite_Draw(UI_Button_L, L_BUTTON_X, UI_SFX_Y, UI_H, UI_H, 0.0f, SFX_Color);
    Sprite_Draw(UI_Button_R, R_BUTTON_X, UI_SFX_Y, UI_H, UI_H, 0.0f, SFX_Color);

    // Number
    int sfxIdx = static_cast<int>(Get_SFX_Scale_Buffer());
    if (sfxIdx >= 0 && sfxIdx <= 10)
    {
        Sprite_Draw(Num_Arr[sfxIdx], NUM_X, UI_SFX_Y, UI_H, UI_H, 0.0f, SFX_Color);
    }

    // ------------------------------------------------------------
    // 4. Back Button
    // ------------------------------------------------------------
    bool Is_Back_Focus = (Get_Setting_Buffer() == SETTING_BUFFER::SETTING_BACK);
    Sprite_Draw(UI_Back, UI_Back_X, UI_Back_Y, UI_W, UI_H, 0.0f, Is_Back_Focus ? Select_Color : Normal_Color);
}

void Update_Setting_Buffer(SETTING_BUFFER Buffer)
{
    S_Buffer = Buffer;
}

SETTING_BUFFER Get_Setting_Buffer()
{
    return S_Buffer;
}

void Update_Sound_Setting_State(SOUND_SETTING_STATE State)
{
    S_State = State;
}

SOUND_SETTING_STATE Get_Setting_State()
{
    return S_State;
}

void Update_BGM_Scale_Buffer(SOUND_SCALE_BUFFER BGM_Scale)
{
    BGM_Volume_Level = BGM_Scale;
}

SOUND_SCALE_BUFFER Get_BGM_Scale_Buffer()
{
    return static_cast<SOUND_SCALE_BUFFER>(BGM_Volume_Level);
}

void Update_SFX_Scale_Buffer(SOUND_SCALE_BUFFER SFX_Scale)
{
    SFX_Volume_Level = SFX_Scale;
}

SOUND_SCALE_BUFFER Get_SFX_Scale_Buffer()
{
    return static_cast<SOUND_SCALE_BUFFER>(SFX_Volume_Level);
}

Mouse_Info Get_Setting_Menu_Mouse_POS()
{
    return Mouse_Setting;
}

void Setting_Menu_Texture()
{
    //----------------Number Texture----------------//
    Num_Arr[0]  = Texture_Manager::GetInstance()->GetID("UI_Num_MIN");
    Num_Arr[1]  = Texture_Manager::GetInstance()->GetID("UI_Num_1");
    Num_Arr[2]  = Texture_Manager::GetInstance()->GetID("UI_Num_2");
    Num_Arr[3]  = Texture_Manager::GetInstance()->GetID("UI_Num_3");
    Num_Arr[4]  = Texture_Manager::GetInstance()->GetID("UI_Num_4");
    Num_Arr[5]  = Texture_Manager::GetInstance()->GetID("UI_Num_5");
    Num_Arr[6]  = Texture_Manager::GetInstance()->GetID("UI_Num_6");
    Num_Arr[7]  = Texture_Manager::GetInstance()->GetID("UI_Num_7");
    Num_Arr[8]  = Texture_Manager::GetInstance()->GetID("UI_Num_8");
    Num_Arr[9]  = Texture_Manager::GetInstance()->GetID("UI_Num_9");
    Num_Arr[10] = Texture_Manager::GetInstance()->GetID("UI_Num_MAX");
    UI_Button_L = Texture_Manager::GetInstance()->GetID("UI_Num_Button_L");
    UI_Button_R = Texture_Manager::GetInstance()->GetID("UI_Num_Button_R");

    //------------------UI Texture------------------//
    UI_BGM = Texture_Manager::GetInstance()->GetID("UI_BGM");
    UI_SFX = Texture_Manager::GetInstance()->GetID("UI_SFX");
    UI_Sens_Text = Texture_Manager::GetInstance()->GetID("UI_Sensitivity"); 
    UI_Back = Texture_Manager::GetInstance()->GetID("UI_Back");
    UI_Background = Texture_Manager::GetInstance()->GetID("UI_Background");
    UI_Pixel_White = Texture_Manager::GetInstance()->GetID("Pixel_Withe");
}

static int Get_Volume_Texture_ID(SOUND_SCALE_BUFFER level)
{
    int idx = static_cast<int>(level);

    if (idx < 0)
    {
        idx = 0;
    }

    if (idx > 10)
    {
        idx = 10;
    }

    return Num_Arr[idx];
}