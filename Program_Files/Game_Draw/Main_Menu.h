/*==============================================================================

	Game Main Menu [Main_Menu.h]

	Author : Choi HyungJoon

==============================================================================*/
#ifndef MAIN_MENU_H
#define MAIN_MENU_H
#include <DirectXMath.h>
#include "mouse.h"    

enum class Main_Select_Buffer
{
	None,
	Wait,
	Start,
	Setting,
	Exit,
	Done
};

struct Button_RECT
{
	float x, y, w, h;
};

void Main_Menu_Initialize();
void Main_Menu_Finalize();

void Main_Menu_Update(double elapsed_time);
void Main_Menu_Draw();
void Main_Menu_BG_Draw();
void Main_Menu_UI_Draw();

Main_Select_Buffer Get_Main_Menu_Buffer();
void Set_Main_Menu_Buffer(Main_Select_Buffer Buffer);

bool IF_IS_Game_Done();
Mouse_Info Get_Main_Menu_Mouse_POS();

void Main_Menu_BG_Camera_Set();
void Main_Menu_BG_Updater(double elapsed_time);
void Main_Menu_Texture();

#endif // MAIN_MENU_H