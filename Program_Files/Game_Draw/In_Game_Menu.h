/*==============================================================================

    Manafe In Game Menu [In_Game_Menu.h]

    Author : Choi HyungJoon

==============================================================================*/
#ifndef IN_GAME_MENU_H
#define IN_GAME_MENU_H
#include "mouse.h"

enum class IN_GAME_BUFFER
{
    NONE,
    WAIT,
    RESUME,  
    SETTING, 
    MAIN_MENU
};

void In_Game_Menu_Initialize();
void In_Game_Menu_Finalize();

void In_Game_Menu_Update(double elapsed_time);
void In_Game_Menu_Draw();

void In_Game_Menu_Reset();

Mouse_Info Get_In_Game_Mouse_POS();

void In_Game_Menu_Texture();

#endif // IN_GAME_MENU_H