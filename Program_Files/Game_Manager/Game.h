/*==============================================================================

	Game [Game.h]

	Author : Choi HyungJoon

==============================================================================*/
#ifndef GAME_H
#define GAME_H
#include <d3d11.h>

void Game_Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
void Game_Finalize();

void Game_Update(double elapsed_time);
void Game_Draw();

bool Get_Debug_Mode_State();
void Set_Debug_Mode_State(bool State);

void Game_Info_Reset();
bool Game_Check_Is_Resetting();
#endif // GAME_H