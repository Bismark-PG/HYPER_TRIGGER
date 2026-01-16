/*==============================================================================

    Select Game Mode [Select_Game_Mode.h]

    Author : Choi HyungJoon

==============================================================================*/
#ifndef SELECT_GAME_MODE_H
#define SELECT_GAME_MODE_H

#include "mouse.h"

enum class SELECT_MODE_BUFFER
{
    NONE,
    WAIT,
    STORY_MODE,
    ENDLESS_MODE,
    BACK,
};

enum class GAME_MODE_BUFFER
{
    NONE,
    STORY_MODE,
    ENDLESS_MODE
};

void Select_Game_Mode_Initialize();
void Select_Game_Mode_Finalize();

void Select_Game_Mode_Update(double elapsed_time);
void Select_Game_Mode_Draw();

SELECT_MODE_BUFFER Get_Select_Menu_Buffer();
void Set_Select_Menu_Buffer(SELECT_MODE_BUFFER Buffer);

Mouse_Info Get_Select_Mode_Mouse_POS();

GAME_MODE_BUFFER Get_Game_Mode();
void Set_Game_Mode(GAME_MODE_BUFFER Mode);

void Select_Game_Menu_Texture();

#endif // SELECT_GAME_MODE_H