/*==============================================================================

	Manage Draw Resource For Game UI [Game_UI.h]

	Author : Choi HyungJoon

==============================================================================*/
#ifndef GAME_UI_H
#define GAME_UI_H
#include <d3d11.h>

void Game_UI_Initialize();
void Game_UI_Finalize();

void Game_UI_Update(double elapsed_time);

void Game_UI_Draw();
void Game_UI_Texture();

void Game_UI_Trigger_Damage();

#endif // GAME_UI_H