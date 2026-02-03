/*==============================================================================

	Helper Logic For Random [Random_Heapler_Logic.h]

	Author : Choi HyungJoon

==============================================================================*/
#include <random>
#ifndef RANDOM_LOGIC_H
#define RANDOM_LOGIC_H

// -----------------------------------------------------------
// Core Logic For Random Logic 
// -----------------------------------------------------------

// [Core] Global Random Engine
inline std::mt19937& Get_Random_Engine()
{
	static std::random_device rd;
	static std::mt19937 gen(rd());
	return gen;
}

// Float Random (min ~ max)
inline float Get_Random_Float_Core(float min, float max)
{
	std::uniform_real_distribution<float> dis(min, max);
	return dis(Get_Random_Engine());
}

// Int Random (min ~ max)
inline int Get_Random_Int_Core(int min, int max)
{
	std::uniform_int_distribution<int> dis(min, max);
	return dis(Get_Random_Engine());
}

// -----------------------------------------------------------
// Logic For Resource Manager
// -----------------------------------------------------------

static float RandomFloatRange(float min, float max)
{
	return Get_Random_Float_Core(min, max);
}

// -----------------------------------------------------------
// Logic For Enemy Spawner
// -----------------------------------------------------------

static float RandomFloat(float min, float max)
{
	return Get_Random_Float_Core(min, max);
}

// -----------------------------------------------------------
// Logic For Enemy Manager
// -----------------------------------------------------------

static float GetRandomFloat(float min, float max)
{
	return Get_Random_Float_Core(min, max);
}

// -----------------------------------------------------------
// Logic For Particle
// -----------------------------------------------------------

static float RandomFloatMinus1To1() // (-1.0 ~ 1.0)
{
	return Get_Random_Float_Core(-1.0f, 1.0f);
}

static float RandomFloat()			// (0.0 ~ 1.0)
{
	return Get_Random_Float_Core(0.0f, 1.0f);
}

// -----------------------------------------------------------

#endif // RANDOM_LOGIC_H