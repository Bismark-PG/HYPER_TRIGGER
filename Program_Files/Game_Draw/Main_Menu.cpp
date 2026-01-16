/*==============================================================================

	Game Main Menu [Main_Menu.cpp]

	Author : Choi HyungJoon

==============================================================================*/
#include "Main_Menu.h"
#include "Map_System.h"
#include "Fade.h"
#include "Audio_Manager.h"
#include "Game_Screen_Manager.h"
#include "Shader_Manager.h"
#include "Sky.h"
#include "Texture_Manager.h"
#include "KeyLogger.h"
#include "Player_Camera.h"
#include "Controller_Input.h"
#include "Light_Manager.h"
#include "BGM_Mixer.h"
using namespace DirectX;

//----------------UI Texture----------------//
static int Main_BG = -1;
static int Main_Title = -1;
static int UI_Start = -1;
static int UI_Set = -1;
static int UI_Exit = -1;

//----------------------POS----------------------/
static float BG_W, BG_H;
static float Title_X, Title_Y, Title_W, Title_H;

static float UI_X;
static float Start_Y;
static float Settings_Y;
static float Exit_Y;

static float UI_Width;
static float UI_Height;

//----------------State & Data----------------//
// State Info
Main_Select_Buffer M_Buffer = Main_Select_Buffer::None;

// Sound Scale Info
static float Current_Volume = 0.0f;
static double BGM_Fade_Timer = 0.0;
constexpr double BGM_FADE_TIME = 3.0;
bool Is_BGM_Playing;
bool Is_BGM_Fading_In;

// Mouse Info
Mouse_Info Mouse_Menu;
static bool Is_Mouse_Left_Clicked_Prev = false;

// Fade Info
static bool EXIT_STATE = false;
static constexpr double FADE_OUT_TIME = 2.0;
static double Fade_Out_Timer = 0.0;

// Bool Info
static bool Menu_Selected = false;
static bool Controller_Alert = false;
static bool Wait_For_Release = false;

void Main_Menu_Initialize()
{
	Main_Menu_Texture();
	Mouse_SetVisible(false);

	BG_W = static_cast<float>(Direct3D_GetBackBufferWidth());
	BG_H = static_cast<float>(Direct3D_GetBackBufferHeight());

	Title_W = BG_W * 0.3f;
	Title_H = BG_H * 0.3f;
	Title_X = (BG_W * 0.9f) - Title_W;
	Title_Y = BG_H * 0.15f;

	Mouse_Menu.Size = BG_H * 0.05f;
	Mouse_Menu.Prev_X = BG_W * 0.5f;
	Mouse_Menu.Prev_Y = BG_H * 0.5f;

	UI_Width	= BG_W * 0.175f;
	UI_Height	= BG_H * 0.075f;
	UI_X		= BG_W * 0.2f - UI_Width * 0.5f;

	Start_Y		= BG_H * 0.55f - UI_Height * 0.5f;
	Settings_Y	= BG_H * 0.7f  - UI_Height * 0.5f;
	Exit_Y		= BG_H * 0.85f - UI_Height * 0.5f;

	Set_Main_Menu_Buffer(Main_Select_Buffer::None);

	Is_BGM_Playing = false;
	Is_BGM_Fading_In = false;

	Main_Menu_BG_Camera_Set();
}

void Main_Menu_Finalize()
{
}

void Main_Menu_Update(double elapsed_time)
{
	Main_Menu_BG_Updater(elapsed_time);
	float dt = static_cast<float>(elapsed_time);

	bool Controller_Alert_Now = Controller_Set_UP();

	if (Controller_Alert_Now)
	{
		Controller_Alert = true;
		Wait_For_Release = true;
		return;
	}

	if (Wait_For_Release)
	{
		if (KeyLogger_IsAnyKeyReleased() || XKeyLogger_IsAnyPadReleased())
			Wait_For_Release = false;
		return;
	}

	Controller_Alert = false;

	Mixer_First_Game_Start(elapsed_time);

	Mouse_State State = Mouse_Get_Prev_State(Mouse_Menu);
	bool Mouse_Movement = Is_Mouse_Moved();

	bool Is_Mouse_Click = (State.leftButton);

	if (Mouse_Movement)
	{
		if (Is_Mouse_In_RECT(Mouse_Menu.X, Mouse_Menu.Y, UI_X, Start_Y, UI_Width, UI_Height))
		{
			if (Get_Main_Menu_Buffer() != Main_Select_Buffer::Start)
			{
				Set_Main_Menu_Buffer(Main_Select_Buffer::Start);
				Audio_Manager::GetInstance()->Play_SFX("Buffer_Move");
			}
		}
		else if (Is_Mouse_In_RECT(Mouse_Menu.X, Mouse_Menu.Y, UI_X, Settings_Y, UI_Width, UI_Height))
		{
			if (Get_Main_Menu_Buffer() != Main_Select_Buffer::Setting)
			{
				Set_Main_Menu_Buffer(Main_Select_Buffer::Setting);
				Audio_Manager::GetInstance()->Play_SFX("Buffer_Move");
			}
		}
		else if (Is_Mouse_In_RECT(Mouse_Menu.X, Mouse_Menu.Y, UI_X, Exit_Y, UI_Width, UI_Height))
		{
			if (Get_Main_Menu_Buffer() != Main_Select_Buffer::Exit)
			{
				Set_Main_Menu_Buffer(Main_Select_Buffer::Exit);
				Audio_Manager::GetInstance()->Play_SFX("Buffer_Move");
			}
		}
		else
		{
			Set_Main_Menu_Buffer(Main_Select_Buffer::Wait);
		}
	}

	if (KeyLogger_IsTrigger(KK_W) || KeyLogger_IsTrigger(KK_UP) || XKeyLogger_IsPadTrigger(XINPUT_GAMEPAD_DPAD_UP))
	{
		if (Get_Main_Menu_Buffer() == Main_Select_Buffer::None || Get_Main_Menu_Buffer() == Main_Select_Buffer::Wait)
		{
			Set_Main_Menu_Buffer(Main_Select_Buffer::Start);
			Audio_Manager::GetInstance()->Play_SFX("Buffer_Move");
		}
		else
		{
			if (Get_Main_Menu_Buffer() == Main_Select_Buffer::Setting)
			{
				Set_Main_Menu_Buffer(Main_Select_Buffer::Start);
				Audio_Manager::GetInstance()->Play_SFX("Buffer_Move");
			}
			else if (Get_Main_Menu_Buffer() == Main_Select_Buffer::Exit)
			{
				Set_Main_Menu_Buffer(Main_Select_Buffer::Setting);
				Audio_Manager::GetInstance()->Play_SFX("Buffer_Move");
			}
		}
	}
	else if (KeyLogger_IsTrigger(KK_S) || KeyLogger_IsTrigger(KK_DOWN) || XKeyLogger_IsPadTrigger(XINPUT_GAMEPAD_DPAD_DOWN))
	{
		if (Get_Main_Menu_Buffer() == Main_Select_Buffer::None || Get_Main_Menu_Buffer() == Main_Select_Buffer::Wait)
		{
			Set_Main_Menu_Buffer(Main_Select_Buffer::Start);
			Audio_Manager::GetInstance()->Play_SFX("Buffer_Move");
		}
		else
		{
			if (Get_Main_Menu_Buffer() == Main_Select_Buffer::Start)
			{
				Set_Main_Menu_Buffer(Main_Select_Buffer::Setting);
				Audio_Manager::GetInstance()->Play_SFX("Buffer_Move");
			}
			else if (Get_Main_Menu_Buffer() == Main_Select_Buffer::Setting)
			{
				Set_Main_Menu_Buffer(Main_Select_Buffer::Exit);
				Audio_Manager::GetInstance()->Play_SFX("Buffer_Move");
			}
		}
	}

	bool Confirm_Input = (KeyLogger_IsTrigger(KK_ENTER) || XKeyLogger_IsPadTrigger(XINPUT_GAMEPAD_A));

	if (State.leftButton && !Is_Mouse_Left_Clicked_Prev)
	{
		if (Get_Main_Menu_Buffer() == Main_Select_Buffer::Start ||
			Get_Main_Menu_Buffer() == Main_Select_Buffer::Setting ||
			Get_Main_Menu_Buffer() == Main_Select_Buffer::Exit)
		{
			Confirm_Input = true;
		}
	}
	Is_Mouse_Left_Clicked_Prev = State.leftButton;

	if (Confirm_Input)
	{
		if (Get_Main_Menu_Buffer() != Main_Select_Buffer::None && Get_Main_Menu_Buffer() != Main_Select_Buffer::Wait)
		{
			Audio_Manager::GetInstance()->Play_SFX("Buffer_Select");

			switch (Get_Main_Menu_Buffer())
			{
			case Main_Select_Buffer::Start:
				Game_Manager::GetInstance()->Update_Main_Screen(Main_Screen::SELECT_GAME);
				Game_Manager::GetInstance()->Update_Game_Select_Screen(Game_Select_Screen::GAME_MENU_SELECT);
				Set_Main_Menu_Buffer(Main_Select_Buffer::None);
				break;

			case Main_Select_Buffer::Setting:
				Game_Manager::GetInstance()->Update_Main_Screen(Main_Screen::SELECT_SETTINGS);
				Game_Manager::GetInstance()->Update_Sub_Screen(Sub_Screen::SETTINGS);
				Set_Main_Menu_Buffer(Main_Select_Buffer::None);
				break;

			case Main_Select_Buffer::Exit:
				Game_Manager::GetInstance()->Update_Main_Screen(Main_Screen::EXIT);
				Set_Main_Menu_Buffer(Main_Select_Buffer::Done);
				Fade_Start(FADE_OUT_TIME, true);
				EXIT_STATE = true;
				Fade_Out_Timer = 0.0;
				break;
			}
		}
	}
}

void Main_Menu_Draw()
{
	Main_Menu_BG_Draw();
	Main_Menu_UI_Draw();
}

void Main_Menu_BG_Draw()
{
	Sprite_Draw(Main_BG, A_Zero, A_Zero, BG_W, BG_H);
	Sprite_Draw(Main_Title, Title_X, Title_Y, Title_W, Title_H);

	/*
	Shader_Manager::GetInstance()->Begin3D();
	Light_Manager::GetInstance().Global_Light_Set_Up();
	
	Sky_Draw();                    

	XMMATRIX mtxIdentity = XMMatrixIdentity();
	Shader_Manager::GetInstance()->SetWorldMatrix3D(mtxIdentity);

	Map_System_Draw();
	*/
}

void Main_Menu_UI_Draw()
{
	Direct3D_SetDepthEnable(false);
	Shader_Manager::GetInstance()->Begin2D();

	XMFLOAT4 A_Origin = Alpha_Origin;
	XMFLOAT4 A_Half = Alpha_Half;

	Sprite_Draw(UI_Start, UI_X, Start_Y, UI_Width, UI_Height, A_Zero,
		(M_Buffer == Main_Select_Buffer::Start) ? A_Origin : A_Half);

	Sprite_Draw(UI_Set, UI_X, Settings_Y, UI_Width, UI_Height, A_Zero,
		(M_Buffer == Main_Select_Buffer::Setting) ? A_Origin : A_Half);

	Sprite_Draw(UI_Exit, UI_X, Exit_Y, UI_Width, UI_Height, A_Zero,
		(M_Buffer == Main_Select_Buffer::Exit) ? A_Origin : A_Half);
}

Main_Select_Buffer Get_Main_Menu_Buffer()
{
	return M_Buffer;
}

void Set_Main_Menu_Buffer(Main_Select_Buffer Buffer)
{
	M_Buffer = Buffer;
}

bool IF_IS_Game_Done()
{
	return EXIT_STATE;
}

Mouse_Info Get_Main_Menu_Mouse_POS()
{
	return Mouse_Menu;
}

void Main_Menu_BG_Camera_Set()
{
	XMFLOAT3 CamPos = { -15.0f, 8.0f, -15.0f };
	XMFLOAT3 CamTarget = { 0.0f, 2.0f, 20.0f };
	Player_Camera_Set_Menu_Mode(CamPos, CamTarget);
}

void Main_Menu_BG_Updater(double elapsed_time)
{
	Player_Camera_Update(elapsed_time);
	Sky_Update();
	Map_System_Update(elapsed_time);
}

void Main_Menu_Texture()
{
	//---------------Main Menu Texture---------------//
	Main_BG  = Texture_Manager::GetInstance()->GetID("BG_Menu");
	Main_Title  = Texture_Manager::GetInstance()->GetID("BG_Title");

	UI_Start = Texture_Manager::GetInstance()->GetID("Start");
	UI_Set	 = Texture_Manager::GetInstance()->GetID("Settings");
	UI_Exit  = Texture_Manager::GetInstance()->GetID("Exit");

	if (UI_Start == -1 || UI_Set == -1 || UI_Exit == -1)
	{
		Debug::D_Out << "[Main Menu] Texture Init Error" << std::endl;
	}
}
