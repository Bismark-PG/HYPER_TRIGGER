/*==============================================================================

   Effect Draw [Effect.h]

   Author : Choi HyungJoon

================================================z==============================*/
#include "Effect.h"
#include <DirectXMath.h>
using namespace DirectX;
#include "Texture_Manager.h"
#include "Sprite_Animation.h"
#include "Game_Texture.h"

struct Effect
{
	XMFLOAT2 POS;
	// XMFLOAT2 velocity;
	// double lifeTime;
	int Sprite_Ani_ID;
	bool IS_Enable;
};

static constexpr int EFFECT_MAX = 256;
static Effect Effects[EFFECT_MAX]{};
// If You Have Many Effect, Make Matrix
static int EffectTexID = -1;
static int AniPatternID = -1;

void Effect_Initialize()
{
	for (Effect& Eff : Effects)
	{
		Eff.IS_Enable = false;
	}

	AniPatternID = SpriteAni_Get_Pattern_Info(Texture_Manager::GetInstance()->
		GetID("Effect_White"), 16, 4, 0.01, {256, 256}, {0, 0}, false);
}

void Effect_Finalize()
{

}

void Effect_Update(double elapsed_time)
{
	float dt = static_cast<float>(elapsed_time);

	for (Effect& Eff : Effects)
	{
		if (!Eff.IS_Enable)	continue;

		if (SpriteAni_IsStopped(Eff.Sprite_Ani_ID))
		{
			SpriteAni_DestroyPlayer(Eff.Sprite_Ani_ID);
			Eff.IS_Enable = false;
		}
	}
}

void Effect_Draw()
{
	for (Effect& Eff : Effects)
	{
		if (!Eff.IS_Enable)	continue;

		//SpriteAni_Draw(Eff.Sprite_Ani_ID, Eff.position.x, Eff.position.y, ENEMY_WIDTH, ENEMY_HEIGHT, 0.0f);
	}
}

void Effect_Create(const XMFLOAT2& position)
{
	for (Effect& Eff : Effects)
	{
		if (Eff.IS_Enable) continue;

		Eff.IS_Enable = true;
		Eff.POS = position;
		Eff.Sprite_Ani_ID = SpriteAni_CreatePlayer(AniPatternID);
		break;
	}
}
