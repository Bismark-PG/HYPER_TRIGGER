/*==============================================================================

	Helper Logic For Random [Random_Heapler_Logic.h]

	Author : Choi HyungJoon

==============================================================================*/
#include <cmath>
#ifndef RANDOM_LOGIC_H
#define RANDOM_LOGIC_H

// Logic For Resource Manager
static float RandomFloatRange(float min, float max)
{
	return min + static_cast<float>(rand()) / static_cast<float>(RAND_MAX) * (max - min);
}

// Logic For Particle
static float RandomFloatMinus1To1()
{
	return (static_cast<float>(rand()) / static_cast<float>(RAND_MAX)) * 2.0f - 1.0f;
}

static float RandomFloat()
{
	return static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
}

// Logic For Enemy Spawner
static float RandomFloat(float min, float max)
{
	return min + static_cast<float>(rand()) / static_cast<float>(RAND_MAX) * (max - min);
}

// Logic For Enemy Manager
static float GetRandomFloat(float min, float max)
{
	return min + (static_cast<float>(rand()) / static_cast<float>(RAND_MAX)) * (max - min);
}

#endif // RANDOM_LOGIC_H