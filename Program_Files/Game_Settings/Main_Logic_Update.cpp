/*==============================================================================

	Update Manager Title And Main UI [Main_UI_Setting.cpp]

	Author : Choi HyungJoon

==============================================================================*/
#include "Main_Logic_Update.h"
#include "Fade.h"
#include "Title.h"     
#include "Game.h"
#include "Mash_Field.h"
#include "Main_Menu.h"
#include "Setting.h"
#include "Select_Game_Mode.h"
#include "In_Game_Menu.h"

void Main_Logic_Initialize()
{
	Title_Initialize();
	Main_Menu_Initialize();
	Setting_Initialize();
	Select_Game_Mode_Initialize();
    In_Game_Menu_Initialize();
}

void Main_Logic_Finalize()
{
	Select_Game_Mode_Finalize();
	Setting_Finalize();
	Main_Menu_Finalize();
	Title_Finalize();
}

void Main_Logic_Update(double elapsed_time)
{
    Game_Manager::GetInstance()->Apply_Screen_Changes();

    Main_Screen M_State = Game_Manager::GetInstance()->Get_Current_Main_Screen();
    Sub_Screen S_State = Game_Manager::GetInstance()->Get_Current_Sub_Screen();
    Game_Select_Screen G_State = Game_Manager::GetInstance()->Get_Current_Game_Select_Screen();

    Main_Logic_Menu(M_State, elapsed_time);
    Main_Logic_Sub(S_State, elapsed_time);
    Main_Logic_Game(G_State, elapsed_time);
}

void Main_Logic_Menu(Main_Screen State, double elapsed_time)
{
    switch (State)
    {
    case Main_Screen::M_WAIT:
    case Main_Screen::MAIN:
        Title_Update(elapsed_time);
        break;

    case Main_Screen::MENU_SELECT:
        Main_Menu_Update(elapsed_time);
        break;

    case Main_Screen::SELECT_SETTINGS:
    case Main_Screen::SELECT_GAME:
    case Main_Screen::EXIT:
        break;
    }
}

void Main_Logic_Sub(Sub_Screen State, double elapsed_time)
{
    switch (State)
    {
    case Sub_Screen::S_WAIT:
    case Sub_Screen::S_DONE:
        break;

    case Sub_Screen::SETTINGS:
        Setting_Update(elapsed_time);
        break;
    }
}

void Main_Logic_Game(Game_Select_Screen State, double elapsed_time)
{
    switch (State)
    {
    case Game_Select_Screen::G_WAIT:
    case Game_Select_Screen::G_DONE:
        break;

    case Game_Select_Screen::GAME_MENU_SELECT:
        Select_Game_Mode_Update(elapsed_time);
        break;

    case Game_Select_Screen::GAME_PLAYING:
        Game_Update(elapsed_time);
        break;

    case Game_Select_Screen::GAME_IN_GAME_MENU:
        In_Game_Menu_Update(elapsed_time);
        break;

    case Game_Select_Screen::GAME_SETTING:
        Setting_Update(elapsed_time);
        break;
    }
}