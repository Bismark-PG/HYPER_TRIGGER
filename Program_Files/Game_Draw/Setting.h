/*==============================================================================
    
    Game Setting [Setting.h]
    
    Author : Choi HyungJoon

==============================================================================*/
#ifndef SETTING_H
#define SETTING_H
#include <direct3d.h>
#include <DirectXMath.h>
#include "mouse.h"

enum class SETTING_BUFFER
{
	SETTING_NONE,
	SETTING_WAIT,
	SENSITIVITY,
	SPRINT_TOGGLE,
	SHOULDER_VIEW,
	BGM_SETTING,
	SFX_SETTING,
	SETTING_BACK,
	SETTING_DONE,
	DONE
};

enum class SOUND_SETTING_STATE
{
	NONE,
	SENSITIVITY_SETTING,
	SPRINT_SETTING,
	VIEW_SETTING,
	BGM_SETTING,
	SFX_SETTING,
	SET_DONE
};

enum SOUND_SCALE_BUFFER : int
{
	SOUND_MIN,
	ONE, TWO, THREE, FOUR, FIVE, SIX, SEVEN, EIGHT, NINE,
	SOUND_MAX
};

void Setting_Initialize();
void Setting_Finalize();
void Setting_Update(double elapsed_time);
void Setting_Draw();

void Update_Setting_Buffer(SETTING_BUFFER Buffer);
SETTING_BUFFER Get_Setting_Buffer();

void Update_Sound_Setting_State(SOUND_SETTING_STATE State);
SOUND_SETTING_STATE Get_Setting_State();

void Update_BGM_Scale_Buffer(SOUND_SCALE_BUFFER BGM_Scale);
SOUND_SCALE_BUFFER Get_BGM_Scale_Buffer();
void Update_SFX_Scale_Buffer(SOUND_SCALE_BUFFER SFX_Scale);
SOUND_SCALE_BUFFER Get_SFX_Scale_Buffer();

bool Setting_Get_Sprint_Type(); // true: Toggle, false: Hold
bool Setting_Get_View_Type();   // true: Right, false: Left

Mouse_Info Get_Setting_Menu_Mouse_POS();

void Setting_Menu_Texture();

#endif // SETTING_H