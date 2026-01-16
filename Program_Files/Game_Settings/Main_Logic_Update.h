/*==============================================================================

	Update Manager Title And Main UI [Main_UI_Setting.h]

	Author : Choi HyungJoon

==============================================================================*/
#ifndef MAIN_LOGIC_H
#define MAIN_LOGIC_H
#include <d3d11.h>
#include "Game_Screen_Manager.h"

void Main_Logic_Initialize();
void Main_Logic_Finalize();

void Main_Logic_Update(double elapsed_time);

void Main_Logic_Menu(Main_Screen State, double elapsed_time);
void Main_Logic_Sub(Sub_Screen State, double elapsed_time);
void Main_Logic_Game(Game_Select_Screen State, double elapsed_time);

#endif // MAIN_LOGIC_H