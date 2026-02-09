/*==============================================================================

	Manage Game Audio Resource [Game_Audio.h]

	Author : Choi HyungJoon

--------------------------------------------------------------------------------
	- Reference

		¡ BGM ¡

		Ÿ DOVA-SYNDROME
		> https://dova-s.jp/

		¡ SFX ¡

		Ÿ  Sound Effect Lab (Œø‰Ê‰¹ƒ‰ƒ{)
		> https://soundeffect-lab.info/

==============================================================================*/
#include "Game_Audio.h"
#include "Audio_Manager.h"

Audio_Manager* Sound_M;

void Game_Audio_Initialize()
{
	Sound_M = Audio_Manager::GetInstance();

	Game_Audio_BGM();
	Game_Audio_SFX();
}

void Game_Audio_Finalize()
{
}

void Game_Audio_BGM()
{

	//------------------------------Debug Texture------------------------------//
	Sound_M->Load_BGM("Bass", "Resource/BGM/Assignment_Bass.wav");
	Sound_M->Load_BGM("Guitar", "Resource/BGM/Assignment_Guitar.wav");
	Sound_M->Load_BGM("Kick", "Resource/BGM/Assignment_Kick.wav");
	Sound_M->Load_BGM("Snare", "Resource/BGM/Assignment_Snare.wav");
	Sound_M->Load_BGM("Synth_1", "Resource/BGM/Assignment_Synth_1.wav");
	Sound_M->Load_BGM("Synth_2", "Resource/BGM/Assignment_Synth_2.wav");
}

void Game_Audio_SFX()
{
	//--------------------------------Logo SFX--------------------------------//
	Sound_M->Load_SFX("Logo_Draw", "Resource/SFX/Logo_Draw_Fixed.wav");

	//-------------------------------Buffer SFX-------------------------------//
	Sound_M->Load_SFX("Buffer_Move", "Resource/SFX/Buffer_Move.wav");
	Sound_M->Load_SFX("Buffer_Select", "Resource/SFX/Buffer_Select.wav");
	Sound_M->Load_SFX("Buffer_Back", "Resource/SFX/Buffer_Back.wav");
	Sound_M->Load_SFX("Buffer_Denied", "Resource/SFX/Buffer_Denied.wav");
	Sound_M->Load_SFX("Controller_Alert", "Resource/SFX/Controller_Alert.wav");

	//--------------------------------Enemy SFX--------------------------------//
	Sound_M->Load_SFX("Enemy_Dead", "Resource/SFX/Enemy_Dead.wav");
	Sound_M->Load_SFX("Enemy_Fire", "Resource/SFX/Enemy_Fire.wav");

	//-------------------------------Player SFX-------------------------------//
	Sound_M->Load_SFX("Player_Hit", "Resource/SFX/Player_Hit.wav");
	Sound_M->Load_SFX("Player_Walk", "Resource/SFX/Player_Walk.wav");
	Sound_M->Load_SFX("Player_Level_UP", "Resource/SFX/Player_Level_UP.wav");
	Sound_M->Load_SFX("Player_Get_HP", "Resource/SFX/Player_Get_Lives.wav"); // Need Other
	Sound_M->Load_SFX("Player_Get_EXP", "Resource/SFX/Player_Get_EXP.wav");

	//-------------------------------Weopon SFX-------------------------------//
	Sound_M->Load_SFX("HG_Fire", "Resource/SFX/Weapon/HG_Fire.wav");
	Sound_M->Load_SFX("HG_Reload_Start", "Resource/SFX/Weapon/HG_Reload_Start.wav");
	Sound_M->Load_SFX("HG_Reload_Done" , "Resource/SFX/Weapon/HG_Reload_Done.wav");

	Sound_M->Load_SFX("AR_Fire", "Resource/SFX/Weapon/AR_Fire.wav");
	Sound_M->Load_SFX("AR_Fire_End", "Resource/SFX/Weapon/AR_Fire_End.wav");
	Sound_M->Load_SFX("AR_Reload_Start", "Resource/SFX/Weapon/AR_Reload_Start.wav");
	Sound_M->Load_SFX("AR_Reload_Ing",   "Resource/SFX/Weapon/AR_Reload_Ing.wav");
	Sound_M->Load_SFX("AR_Reload_Done",  "Resource/SFX/Weapon/AR_Reload_Done.wav");

	Sound_M->Load_SFX("MG_Fire", "Resource/SFX/Weapon/MG_Fire.wav");
	Sound_M->Load_SFX("MG_Fire_End", "Resource/SFX/Weapon/MG_Fire_End.wav");
	Sound_M->Load_SFX("MG_Reload_Start", "Resource/SFX/Weapon/MG_Reload_Start.wav");
	Sound_M->Load_SFX("MG_Reload_Ing",	 "Resource/SFX/Weapon/MG_Reload_Ing.wav");
	Sound_M->Load_SFX("MG_Reload_Done",  "Resource/SFX/Weapon/MG_Reload_Done.wav");
	
	Sound_M->Load_SFX("Grenade_Drow", "Resource/SFX/Weapon/Grenade_Drow.wav");
	Sound_M->Load_SFX("Grenade_Explosion", "Resource/SFX/Weapon/Grenade_Explosion.wav");
	Sound_M->Load_SFX("Grenade_Next", "Resource/SFX/Weapon/Grenade_Next.wav");

	Sound_M->Load_SFX("Weapon_Change", "Resource/SFX/Weapon/Weapon_Change.wav");
	Sound_M->Load_SFX("Weapon_Pick_UP", "Resource/SFX/Weapon/Weapon_Pick_UP.wav");
	
	//---------------------------Upgrade System SFX---------------------------//
	Sound_M->Load_SFX("Upgrade_Start",	"Resource/SFX/Upgrade_Start.wav");
	Sound_M->Load_SFX("Upgrade_Buffer", "Resource/SFX/Upgrade_Buffer.wav");
	Sound_M->Load_SFX("Upgrade_Select", "Resource/SFX/Upgrade_Select.wav");
}