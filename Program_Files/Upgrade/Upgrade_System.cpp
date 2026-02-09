/*==============================================================================

    Manage Player Upgrade System [Upgrade_System.cpp]

    Author : Choi HyungJoon

==============================================================================*/
#include "Upgrade_System.h"
#include "Texture_Manager.h"
#include "Sprite.h"
#include "KeyLogger.h"
#include "direct3d.h"
#include "Audio_Manager.h"
#include "Random_Heapler_Logic.h"
#include "debug_ostream.h"
#include "Palette.h"
#include "Card_List.h"
using namespace DirectX;
using namespace PALETTE;

//----------------UI Texture----------------//
static int BG_ID = -1;

//----------------------POS----------------------//
static float BG_X, BG_Y, BG_W, BG_H;
static float L_Card_Left, M_Card_Left, R_Card_Left;
static float Card_Y, Card_W, Card_H;

//----------------State & Data----------------//
Card_Buffer C_Buffer = Card_Buffer::NONE;

// Mouse Info
static Mouse_Info Mouse_Upgrade_Menu;
static bool Is_Mouse_Left_Clicked_Prev = false;

void Upgrade_System::Init()
{
    Reset();
    BG_ID = Texture_Manager::GetInstance()->GetID("UI_Background");

    float Screen_W = static_cast<float>(Direct3D_GetBackBufferWidth());
    float Screen_H = static_cast<float>(Direct3D_GetBackBufferHeight());

    BG_W = Screen_W * 0.8f;
    BG_H = Screen_H * 0.8f;
    BG_X = (Screen_W - BG_W) * 0.5f;
    BG_Y = (Screen_H - BG_H) * 0.5f;

    Mouse_Upgrade_Menu.Size = Screen_H * 0.05f;
    Mouse_Upgrade_Menu.Prev_X = Screen_W * 0.5f;
    Mouse_Upgrade_Menu.Prev_Y = Screen_H * 0.5f;

    Card_W = BG_W * 0.25f;
    Card_H = BG_H * 0.6f;
    Card_Y = BG_Y + (BG_H * 0.5f) - (Card_H * 0.5f);

    float Total_Gap_Space = BG_W - (Card_W * 3.0f);
    float One_Gap = Total_Gap_Space / 4.0f;

    L_Card_Left = BG_X + One_Gap;

    M_Card_Left = L_Card_Left + Card_W + One_Gap;
    R_Card_Left = M_Card_Left + Card_W + One_Gap;
}

void Upgrade_System::Final()
{
    m_CurrentCards.clear();
}

void Upgrade_System::Reset()
{
    Card_List::Init();

    m_IsActive = false;
    m_LockedWeaponType = -1; // Not locked
    m_CurrentCards.clear();

    C_Buffer = Card_Buffer::NONE;
    Is_Mouse_Left_Clicked_Prev = false;
}

void Upgrade_System::Update()
{
    if (!m_IsActive) return;

    Mouse_State State = Mouse_Get_Prev_State(Mouse_Upgrade_Menu);
    bool Mouse_Moved = Is_Mouse_Moved();

    bool Click_Trigger = (State.leftButton && !Is_Mouse_Left_Clicked_Prev);
    bool Enter_Trigger = (KeyLogger_IsTrigger(KK_ENTER) || XKeyLogger_IsPadTrigger(XINPUT_GAMEPAD_A));
    bool Confirm = (Click_Trigger || Enter_Trigger);

    Is_Mouse_Left_Clicked_Prev = State.leftButton;

    if (Mouse_Moved)
    {
        if (Is_Mouse_In_RECT(Mouse_Upgrade_Menu.X, Mouse_Upgrade_Menu.Y, L_Card_Left, Card_Y, Card_W, Card_H))
        {
            if (C_Buffer != Card_Buffer::CARD_L)
            {
                Set_Card_Buffer(Card_Buffer::CARD_L);
                Audio_Manager::GetInstance()->Play_SFX("Upgrade_Buffer");
            }
        }
        else if (Is_Mouse_In_RECT(Mouse_Upgrade_Menu.X, Mouse_Upgrade_Menu.Y, M_Card_Left, Card_Y, Card_W, Card_H))
        {
            if (C_Buffer != Card_Buffer::CARD_M)
            {
                Set_Card_Buffer(Card_Buffer::CARD_M);
                Audio_Manager::GetInstance()->Play_SFX("Upgrade_Buffer");
            }
        }
        else if (Is_Mouse_In_RECT(Mouse_Upgrade_Menu.X, Mouse_Upgrade_Menu.Y, R_Card_Left, Card_Y, Card_W, Card_H))
        {
            if (C_Buffer != Card_Buffer::CARD_R)
            {
                Set_Card_Buffer(Card_Buffer::CARD_R);
                Audio_Manager::GetInstance()->Play_SFX("Upgrade_Buffer");
            }
        }
        else
        {
            Set_Card_Buffer(Card_Buffer::WAIT);
        }
    }
    if (KeyLogger_IsTrigger(KK_A) || KeyLogger_IsTrigger(KK_LEFT) || XKeyLogger_IsPadTrigger(XINPUT_GAMEPAD_DPAD_LEFT))
    {
        if (Get_Card_Buffer() == Card_Buffer::NONE || Get_Card_Buffer() == Card_Buffer::WAIT)
        {
            Audio_Manager::GetInstance()->Play_SFX("Upgrade_Buffer");
            Set_Card_Buffer(Card_Buffer::CARD_L);
        }
        else
        {
            if (Get_Card_Buffer() == Card_Buffer::CARD_M)
            {
                Audio_Manager::GetInstance()->Play_SFX("Upgrade_Buffer");
                Set_Card_Buffer(Card_Buffer::CARD_L);
            }
            else if (Get_Card_Buffer() == Card_Buffer::CARD_R)
            {
                Audio_Manager::GetInstance()->Play_SFX("Upgrade_Buffer");
                Set_Card_Buffer(Card_Buffer::CARD_M);
            }
        }
    }
    else if (KeyLogger_IsTrigger(KK_D) || KeyLogger_IsTrigger(KK_RIGHT) || XKeyLogger_IsPadTrigger(XINPUT_GAMEPAD_DPAD_RIGHT))
    {
        if (Get_Card_Buffer() == Card_Buffer::NONE || Get_Card_Buffer() == Card_Buffer::WAIT)
        {
            Audio_Manager::GetInstance()->Play_SFX("Upgrade_Buffer");
            Set_Card_Buffer(Card_Buffer::CARD_R);
        }
        else
        {
            if (Get_Card_Buffer() == Card_Buffer::CARD_L)
            {
                Audio_Manager::GetInstance()->Play_SFX("Upgrade_Buffer");
                Set_Card_Buffer(Card_Buffer::CARD_M);
            }
            else if (Get_Card_Buffer() == Card_Buffer::CARD_M)
            {
                Audio_Manager::GetInstance()->Play_SFX("Upgrade_Buffer");
                Set_Card_Buffer(Card_Buffer::CARD_R);
            }
        }
    }

    if (Confirm)
    {
        if (m_CurrentCards.size() < 3) return;

        switch (C_Buffer)
        {
        case Card_Buffer::CARD_L:
            Audio_Manager::GetInstance()->Play_SFX("Upgrade_Select");
            Apply_Upgrade(m_CurrentCards[0]);
            break;

        case Card_Buffer::CARD_M:
            Audio_Manager::GetInstance()->Play_SFX("Upgrade_Select");
            Apply_Upgrade(m_CurrentCards[1]);
            break;

        case Card_Buffer::CARD_R:
            Audio_Manager::GetInstance()->Play_SFX("Upgrade_Select");
            Apply_Upgrade(m_CurrentCards[2]);
            break;
        }
    }
}

void Upgrade_System::Draw()
{
    if (!m_IsActive) return;

    Direct3D_SetDepthEnable(false);

    Sprite_Draw(BG_ID, BG_X, BG_Y, BG_W, BG_H);

    float Left_X_Arr[3] = { L_Card_Left, M_Card_Left, R_Card_Left };

    for (int i = 0; i < m_CurrentCards.size(); ++i)
    {
        float Current_W = Card_W, Current_H = Card_H;
        float Draw_X = Left_X_Arr[i], Draw_Y = Card_Y;

        bool Is_Hover = false;
        if (i == 0 && C_Buffer == Card_Buffer::CARD_L) 
        {
            Is_Hover = true;
        }
        if (i == 1 && C_Buffer == Card_Buffer::CARD_M)
        {
            Is_Hover = true;
        }
        if (i == 2 && C_Buffer == Card_Buffer::CARD_R) 
        {
            Is_Hover = true;
        }

        if (Is_Hover)
        {
            float Scale = 1.1f;

            float Scaled_W = Card_W * Scale;
            float Scaled_H = Card_H * Scale;

            Draw_X -= (Scaled_W - Card_W) * 0.5f;
            Draw_Y -= (Scaled_H - Card_H) * 0.5f;

            Current_W = Scaled_W;
            Current_H = Scaled_H;
        }

        if (m_CurrentCards[i].TextureID != -1)
        {
            Sprite_Draw(m_CurrentCards[i].TextureID, Draw_X, Draw_Y, Current_W, Current_H);
        }
    }

    Mouse_UI_Draw(Mouse_Upgrade_Menu);
}

// --------------------------------------------------------------
//					Upgrade System Private Logic
// --------------------------------------------------------------
void Upgrade_System::Show_Upgrade_Select()
{
    m_IsActive = true;
    m_CurrentCards.clear();

    auto drawnCards = Card_List::Get_Random_Cards(3);

    for (const auto& data : drawnCards)
    {
        Card_Data uiCard;
        uiCard.Type = data.Type;
        uiCard.Rarity = data.Rarity;
        uiCard.Title = data.Title;
        uiCard.TextureID = data.TextureID;

        m_CurrentCards.push_back(uiCard);
    }

    Mouse_SetMode(MOUSE_POSITION_MODE_ABSOLUTE);
    Audio_Manager::GetInstance()->Play_SFX("Upgrade_Start");
}

bool Upgrade_System::Is_Active() const
{
    return m_IsActive;
}

void Upgrade_System::Set_Weapon_Lock(WeaponType type)
{
    m_LockedWeaponType = static_cast<int>(type);
}

int Upgrade_System::Get_Weapon_Lock_State() const
{
    return m_LockedWeaponType;
}

void Upgrade_System::Set_Card_Buffer(Card_Buffer Buffer)
{
    C_Buffer = Buffer;
}

Card_Buffer Upgrade_System::Get_Card_Buffer() const
{
    return C_Buffer;
}

void Upgrade_System::Apply_Upgrade(const Card_Data& card)
{
    // Delegate to Card_List
    Card_List::Apply_Card_Effect(card.Type);

    // Close Menu
    m_IsActive = false;
    C_Buffer = Card_Buffer::NONE;
    Mouse_SetMode(MOUSE_POSITION_MODE_RELATIVE);

    Debug::D_Out << "[Upgrade] Selected : " << card.Title << std::endl;
}