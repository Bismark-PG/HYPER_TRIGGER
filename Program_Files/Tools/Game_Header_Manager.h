/*==============================================================================

	Manage Header file  [Game_Header_Manager.h]

	Author : Choi HyungJoon

==============================================================================*/
#ifndef HEADER_MANAGER_H
#define HEADER_MANAGER_H

//==============================================================================//
// Include all necessary headers for the game manager
// Program Set
#include "Game_Window.h"
#include "direct3d.h"
#include "system_timer.h"
#include "KeyLogger.h"
#include "Audio_Manager.h"

// Shader Set
#include "Shader_Manager.h"
#include "Light_Manager.h"

// Draw Set
#include "Sprite.h"
#include "Sprite_Animation.h"
#include "Texture_Manager.h"
#include "Billboard.h"
#include "Billboard_Manager.h"

// Resource Set
#include "Game_Texture.h"
#include "Game_Model.h"
#include "Game_Audio.h"

// Game Set
#include "Game.h"
#include "Player.h"
#include "Player_Camera.h"
#include "Mash_Field.h"
#include "Game_Screen_Manager.h"
#include "Update_Game.h"
#include "Main_Menu.h"
#include "Setting.h"
#include "Select_Game_Mode.h"
#include "Map_System.h"

// Debug Set
#include "debug_text.h"
#include "Debug_Camera.h"
#include "GUI_Manager.h"
#include "Debug_Collision.h"

// Tools
#include "Controller_Input.h"
#include "Fade.h"
#include "Main_Logic_Update.h"

// Other
#include <sstream>
#include "Palette.h"

//==============================================================================//

#endif // HEADER_MANAGER_H
