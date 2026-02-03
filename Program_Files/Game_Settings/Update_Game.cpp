/*==============================================================================

	Update Game Screen [Update_Game.cpp]

	Author : Choi HyungJoon

==============================================================================*/
#include "Update_Game.h"
#include "debug_ostream.h"
#include "System_Timer.h"
#include "Fade.h"
#include "Title.h"     
#include "Game.h"
#include "Main_Menu.h"
#include "Setting.h"
#include "Select_Game_Mode.h"
#include "In_Game_Menu.h"

void Main_Game_Screen_Update()
{
	Main_Screen current_screen = Game_Manager::GetInstance()->Get_Current_Main_Screen();

	switch (current_screen)
	{
	case Main_Screen::M_WAIT:
		Fade_Start(1.5f, false);
		Mouse_SetMode(MOUSE_POSITION_MODE_ABSOLUTE);
		Game_Manager::GetInstance()->Update_Main_Screen(Main_Screen::MAIN);
		break;

	case Main_Screen::MAIN:
		Title_Draw();
		break;

	case Main_Screen::MENU_SELECT:
		Main_Menu_Draw();
		Mouse_UI_Draw(Get_Main_Menu_Mouse_POS());
		break;

	case Main_Screen::SELECT_GAME:
		if (Game_Manager::GetInstance()->Get_Current_Game_Select_Screen() == Game_Select_Screen::GAME_MENU_SELECT)
		{
			Main_Menu_Draw();
		}
		Game_Select_Screen_Update();
		break;

	case Main_Screen::SELECT_SETTINGS:
		Main_Menu_Draw();
		Sub_Game_Screen_Update();
		break;

	case Main_Screen::EXIT:
		Game_Manager::GetInstance()->Update_Main_Screen(Main_Screen::M_DONE);
		Main_Menu_Draw();
		Debug::D_Out << "Exiting Game..." << std::endl;
		break;

	case Main_Screen::M_DONE:
		Main_Menu_Draw();
		Debug::D_Out << "Programme Done" << std::endl;
		break;
	}
}

void Sub_Game_Screen_Update()
{
	Sub_Screen current_screen = Game_Manager::GetInstance()->Get_Current_Sub_Screen();

	switch (current_screen)
	{
	case Sub_Screen::S_WAIT:
		break;

	case Sub_Screen::SETTINGS:
		Setting_Draw();
		Mouse_UI_Draw(Get_Setting_Menu_Mouse_POS());
		break;

	case Sub_Screen::S_DONE:
		Game_Manager::GetInstance()->Update_Main_Screen(Main_Screen::MENU_SELECT);
		Game_Manager::GetInstance()->Update_Sub_Screen(Sub_Screen::S_WAIT);
		break;
	}
}

void Game_Select_Screen_Update()
{
	Game_Select_Screen current_screen = Game_Manager::GetInstance()->Get_Current_Game_Select_Screen();

	switch (current_screen)
	{
	case Game_Select_Screen::G_WAIT:
		break;

	case Game_Select_Screen::GAME_MENU_SELECT:
		Main_Menu_BG_Draw();
		Select_Game_Mode_Draw();
		Mouse_UI_Draw(Get_Select_Mode_Mouse_POS());
		break;

	case Game_Select_Screen::GAME_PLAYING:
		Game_Draw();
		break;

	case Game_Select_Screen::GAME_IN_GAME_MENU:
		Game_Draw();
		In_Game_Menu_Draw();
		Mouse_UI_Draw(Get_In_Game_Mouse_POS());
		break;

	case Game_Select_Screen::GAME_SETTING:
		Game_Draw();
		Setting_Draw();
		Mouse_UI_Draw(Get_Setting_Menu_Mouse_POS());
		break;

	case Game_Select_Screen::G_DONE:
		Game_Manager::GetInstance()->Update_Main_Screen(Main_Screen::MENU_SELECT);
		Game_Manager::GetInstance()->Update_Game_Select_Screen(Game_Select_Screen::G_WAIT);
		break;
	}
}