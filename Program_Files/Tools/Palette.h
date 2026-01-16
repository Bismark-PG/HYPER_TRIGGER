/*==============================================================================

	Color List [Palette.h]

	Author : Choi HyungJoon

==============================================================================*/

#ifndef COLOR_LIST_H
#define COLOR_LIST_H

#include <DirectXMath.h>

namespace PALETTE
{
	using Palette = DirectX::XMFLOAT4;

	//------------------------//    Number    //------------------------//

	// Radius
	static constexpr float R_Quarter_Pie		= 45.0f;
	static constexpr float R_Half_Pie			= 90.0f;
	static constexpr float R_One_Pie			= 180.0f;
	static constexpr float R_One_And_Half_Pie	= 270.0f;
	static constexpr float R_Two_Pie			= 360.0f;

	// Alpha
	static constexpr float A_Zero		   = 0.0f;
	static constexpr float A_Zero_One	   = 0.01f;
	static constexpr float A_Zero_O_Five   = 0.015f;
	static constexpr float A_Zero_Two	   = 0.02f;
	static constexpr float A_Zero_Quarter  = 0.025f;
	static constexpr float A_Zero_Four	   = 0.04f;
	static constexpr float A_Zero_Five	   = 0.05f;
	static constexpr float A_Z_T_Quarters  = 0.075f;
	static constexpr float A_One_Tenth	   = 0.1f;
	static constexpr float A_Point_Fifteen = 0.15f;
	static constexpr float A_Fifth		   = 0.2f;
	static constexpr float A_Quarter	   = 0.25f;
	static constexpr float A_Point_Three   = 0.3f;
	static constexpr float A_Third		   = 1.0f / 3.0f;
	static constexpr float A_P_Three_Five  = 0.35f;
	static constexpr float A_Two_Fifth	   = 0.4f;
	static constexpr float A_Four_Five	   = 0.45f;
	static constexpr float A_Half		   = 0.5f;
	static constexpr float A_Half_Half	   = 0.55f;
	static constexpr float A_Six_Tenths	   = 0.6f;
	static constexpr float A_Six_Five	   = 0.65f;
	static constexpr float A_Point_Seven   = 0.7f;
	static constexpr float A_T_Quarters    = 0.75f;
	static constexpr float A_F_Fifths      = 0.8f;
	static constexpr float A_P_Eight_Five  = 0.85f;
	static constexpr float A_Point_Nine    = 0.9f;

	// Bigger Than One
	static constexpr float A_Origin		      = 1.0f;
	static constexpr float A_One_A_Quarter    = 1.25f;
	static constexpr float A_One_And_Half     = 1.5f;
	static constexpr float A_One_A_T_Quarters = 1.75f;
	static constexpr float A_Double		      = 2.0f;
	static constexpr float A_Double_And_Half  = 2.5f;
	static constexpr float A_Triple		      = 3.0f;
	static constexpr float A_Quintuple		  = 5.0f;
	static constexpr float A_Decuple		  = 10.0f;

	//------------------------//    Colors    //------------------------//

	// Basic Colors
	static constexpr Palette White	= { 1.0f, 1.0f, 1.0f, 1.0f };
	static constexpr Palette Red	= { 1.0f, 0.0f, 0.0f, 1.0f };
	static constexpr Palette Green	= { 0.0f, 1.0f, 0.0f, 1.0f };
	static constexpr Palette Blue	= { 0.0f, 0.0f, 1.0f, 1.0f };

	static constexpr Palette Cyan	 = { 0.0f, 1.0f, 1.0f, 1.0f };
	static constexpr Palette Magenta = { 1.0f, 0.0f, 1.0f, 1.0f };
	static constexpr Palette Yellow	 = { 1.0f, 1.0f, 0.0f, 1.0f };
	static constexpr Palette Black	 = { 0.0f, 0.0f, 0.0f, 1.0f };

	static constexpr Palette ORANGE	= { 1.0f, 0.5f, 0.0f, 1.0f };
	static constexpr Palette PURPLE	= { 0.5f, 0.0f, 0.5f, 1.0f };
	static constexpr Palette BROWN	= { 0.6f, 0.3f, 0.1f, 1.0f };
	static constexpr Palette PINK	= { 1.0f, 0.0f, 0.5f, 1.0f };

	static constexpr Palette Light_Green	= { 0.5f, 0.8f, 0.3f, 1.0f };
	static constexpr Palette Half_Magenta	= { 1.0f, 0.0f, 1.0f, 0.5f };

	// Gray Scale
    static constexpr Palette Light_Gray  = { 211 / 255.f, 211 / 255.f, 211 / 255.f, 1.f };
    static constexpr Palette Gray		= { 128 / 255.f, 128 / 255.f, 128 / 255.f, 1.f };
    static constexpr Palette Dark_Gray	= { 64  / 255.f, 64  / 255.f, 64  / 255.f, 1.f };

	// Alpha Scale
	static constexpr Palette Alpha_Invisible	= { 0.0f, 0.0f, 0.0f, 0.0f };
	static constexpr Palette Alpha_Translucent  = { 0.0f, 0.0f, 0.0f, 0.5f };
	static constexpr Palette Alpha_Origin		= { 1.0f, 1.0f, 1.0f, 1.0f };
	static constexpr Palette Alpha_T_Quarter	= { 1.0f, 1.0f, 1.0f, 0.75f };
	static constexpr Palette Alpha_Half			= { 1.0f, 1.0f, 1.0f, 0.5f };
	static constexpr Palette Alpha_Quarter		= { 1.0f, 1.0f, 1.0f, 0.25f };
} 
#endif // COLOR_LIST_H