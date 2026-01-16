/*==============================================================================

	Manage Player Logic [Player.cpp]

	Author : Choi HyungJoon

==============================================================================*/
#include "Player.h"
#include "KeyLogger.h"
#include "model.h"
#include "Shader_Manager.h"
#include "Player_Camera.h" 
#include "Game_Model.h"
#include "Debug_Collision.h"
#include "Palette.h"
#include "Map_System.h"
#include "Game.h"
#include "Mash_Field.h"
#include "Weapon_System.h"
#include "Resource_Manager.h"
#include "Game_UI.h"
#include "Fade.h"   
#include "Game_Screen_Manager.h" 
#include "BGM_Mixer.h"
#include "Audio_Manager.h"
using namespace DirectX;
using namespace PALETTE;

constexpr float Player_drag = 5.0f;

static XMFLOAT3 Player_Pos	 = {};
static XMFLOAT3 Player_Start_POS = {};
static XMFLOAT3 Player_Front = { 0.0f, 0.0f, 1.0f };
static XMFLOAT3 Player_Vel	 = {};
static XMVECTOR Player_Speed = {};
static float Player_Height_Offset = 0.0f;

static XMVECTOR Gravity		= { 0.0f, -15.0f, 0.0f };
static XMVECTOR Walk_Speed	= { 35.f, 0.0, 35.f };
static XMVECTOR Run_Speed	= { 75.f, 0.0, 75.f };

static MODEL* Player_Model{nullptr};
static bool Is_Jump = false;
static bool Is_Run = false;
static bool Is_Run_Toggle = false;

// Player resource
static const int Player_Base_MaxHP = 100;
static int Player_HP = 100;
static int Player_MaxHP = 100;
static float Player_Damage_Coeff = 1.0f;

static bool Player_Is_Dead = false;
static bool Is_Game_Over_Sequence = false;

static bool  Is_Invincible = false;     
static float Invincible_Timer = 0.0f;   
constexpr float INVINCIBLE_DURATION = 1.0f;

static float Walk_Timer = 0.0f;

// static Player Update Logic
static void Player_Update_Teleport_System();
static XMVECTOR Player_Update_AABB_System(XMVECTOR Velocity);
static void Player_Update_Respawn_System(XMVECTOR Velocity);

// If Input Model, Can Change Chara
void Player_Initialize(const XMFLOAT3& First_POS, const XMFLOAT3& First_Front)
{
	Player_Pos = First_POS;
	Player_Start_POS = First_POS; 
	Player_Vel = { 0.0f, 0.0f, 0.0f };
	XMStoreFloat3(&Player_Front, XMVector3Normalize(XMLoadFloat3(&First_Front)));

	Player_Model = Model_Manager::GetInstance()->GetModel("Player");

	Is_Jump = false;
	Is_Run = false;
	Is_Run_Toggle = false;

	Player_HP = Player_Base_MaxHP;
	Player_MaxHP = Player_Base_MaxHP;
	Player_Damage_Coeff = 1.0f;

	Player_Is_Dead = false;
	Is_Game_Over_Sequence = false;

	Is_Invincible = false;
	Invincible_Timer = 0.0f;

	Weapon_System::GetInstance().Init();
}

void Player_Finalize()
{
	Player_Model = nullptr;
}

void Player_Update(double elapsed_time)
{
	float dt = static_cast<float>(elapsed_time);

	// --- 1. Game Over And Invincible Logic ---
	if (Is_Game_Over_Sequence)
	{
		if (Fade_GetState() == FADE_STATE::FINISHED_OUT)
		{
			Mixer_Init();
			Game_Manager::GetInstance()->Update_Main_Screen(Main_Screen::MENU_SELECT);
			Game_Manager::GetInstance()->Update_Game_Select_Screen(Game_Select_Screen::G_WAIT);

			Mouse_SetMode(MOUSE_POSITION_MODE_ABSOLUTE);
			Fade_Start(1.5f, false);
		}
		return;
	}

	if (Player_Is_Dead) return;

	if (Is_Invincible)
	{
		Invincible_Timer -= dt;
		if (Invincible_Timer <= 0.0f)
		{
			Is_Invincible = false;
			Invincible_Timer = 0.0f;
		}
	}

	// --- 2. Input & Physics Calculation ---
	if (KeyLogger_IsTrigger(KK_F4)) // toggle mode << debug
		Is_Run_Toggle = !Is_Run_Toggle;

	if (Is_Run_Toggle)
	{
		if (KeyLogger_IsTrigger(KK_LEFTSHIFT))
			Is_Run = !Is_Run;
	}
	else
	{
		if ((KeyLogger_IsPressed(KK_LEFTSHIFT) && !Is_Run) || (KeyLogger_IsReleased(KK_LEFTSHIFT) && Is_Run))
			Is_Run = !Is_Run;
	}

	if (Is_Run)
		Player_Speed = Run_Speed;
	else
		Player_Speed = Walk_Speed;

	if (elapsed_time > 0.05)
	{
		elapsed_time = 0.05;
	}

	XMVECTOR POS = XMLoadFloat3(&Player_Pos);
	XMVECTOR Vel = XMLoadFloat3(&Player_Vel);

	// Jump Logic
	if (KeyLogger_IsTrigger(KK_SPACE) && !Is_Jump)
	{
		Is_Jump = true;
		Vel = XMVectorSetY(Vel, 0.0f);
		Vel += XMVECTOR{ 0.0f, 10.0f, 0.0f };
	}

	// Gravity Apply
	Vel += Gravity * dt;

	// Movement Logic
	XMVECTOR Dir{};
	XMVECTOR Front = XMLoadFloat3(&Player_Camera_Get_Front());
	XMVECTOR Flat_Front = XMVector3Normalize(XMVectorSetY(Front, 0.0f));
	XMVECTOR Flat_Right = XMVector3Cross({ 0.0f,1.0f,0.0f }, Flat_Front);

	bool Is_Input_Moving = false;

	if (KeyLogger_IsPressed(KK_W)) Dir += Flat_Front;
	if (KeyLogger_IsPressed(KK_S)) Dir -= Flat_Front;
	if (KeyLogger_IsPressed(KK_D)) Dir += Flat_Right;
	if (KeyLogger_IsPressed(KK_A)) Dir -= Flat_Right;

	Dir = XMVector3Normalize(Dir);

	Vel += Dir * Player_Speed * dt;
	
	if (Is_Input_Moving && !Is_Jump)
	{
		Walk_Timer += dt;

		if (Walk_Timer >= 1.0f)
		{
			Audio_Manager::GetInstance()->Play_SFX("Player_Walk");

			Walk_Timer = 0.0f;
		}
	}
	else
	{
		Walk_Timer = 0.0f;
	}

	// Friction
	XMVECTOR Horizontal_Vel = XMVectorMultiply(Vel, XMVectorSet(1.0f, 0.0f, 1.0f, 0.0f));
	Horizontal_Vel -= Horizontal_Vel * Player_drag * dt;
	Vel = XMVectorMultiply(Vel, XMVectorSet(0.0f, 1.0f, 0.0f, 1.0f)) + Horizontal_Vel;

	// Apply Velocity
	POS += Vel * dt;
	XMStoreFloat3(&Player_Pos, POS);

	// --- 3. Collision & Respawn (Physics Correction) ---
	Player_Update_Respawn_System(Vel);
	Vel = Player_Update_AABB_System(Vel);
	Player_Update_Teleport_System();

	// --- 4. Weapon System ---
	Weapon_System::GetInstance().Update(elapsed_time);

	// Shooting Logic
	bool TryFire = false;
	const WeaponInfo& currentWeapon = Weapon_System::GetInstance().GetCurrentWeaponInfo();

	// Mouse Left Button Check
	if (currentWeapon.IsAutomatic)
	{
		if (KeyLogger_IsMousePressed(Mouse_Button::LEFT))
		{
			TryFire = true;
		}
	}
	else
	{
		if (KeyLogger_IsMouseTrigger(Mouse_Button::LEFT))
		{
			TryFire = true;
		}
	}

	if (TryFire)
	{
		// Logical Fire POS
		XMFLOAT3 CamPos = Player_Camera_Get_Current_POS();
		XMFLOAT3 CamDir = Player_Camera_Get_Front();

		// Virtual Fire POS
		XMFLOAT3 PlayerCenter = Player_Get_POS();

		// In Now Hard-Coeding Logic, After Need Player Fire POS Logic.
		XMVECTOR vP_Pos = XMLoadFloat3(&PlayerCenter);
		XMVECTOR vP_Front = XMLoadFloat3(&Player_Get_Front());
		XMVECTOR vUp = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
		XMVECTOR vRight = XMVector3Cross(vUp, vP_Front); 
		XMVECTOR vGunPos = vP_Pos + (vUp * 1.5f) + (vRight * 0.5f) + (vP_Front * 0.5f);
		XMFLOAT3 GunPos;
		XMStoreFloat3(&GunPos, vGunPos);

		// Make Bullet, Fire!
		Weapon_System::GetInstance().Fire(GunPos, CamPos, CamDir);
		Audio_Manager::GetInstance()->Play_SFX("Player_Fire");
	}

	// Save Final Front & Vel
	XMStoreFloat3(&Player_Front, Flat_Front);
	XMStoreFloat3(&Player_Vel, Vel);
}

void Player_Draw()
{
	if (!Player_Model)
		return;

	Shader_Manager::GetInstance()->SetLightSpecular(Player_Camera_Get_POS(), 164.0f, { 1.0f, 1.0f, 1.0f, 1.0f });

	// Get Movement
	XMMATRIX t = XMMatrixTranslation(Player_Pos.x, Player_Pos.y + Player_Height_Offset, Player_Pos.z);
	
	// Get Yaw
	float yaw = atan2f(Player_Front.x, Player_Front.z);

	// Reverse Player
	float modelYaw = yaw + XM_PI;
	XMMATRIX r = XMMatrixRotationY(modelYaw);
	XMMATRIX world = r * t;

	ModelDraw(Player_Model, world);

	if (Get_Debug_Mode_State())
	{
		Debug_Collision_Draw(Player_Get_AABB(), Red);
	}
}

void Player_Reset()
{
	// 1. Movement Reset
	Player_Pos = Player_Start_POS;
	Player_Vel = { 0.0f, 0.0f, 0.0f };
	Player_Speed = { 0.0f, 0.0f, 0.0f, 0.0f };

	// 2. Resource Reset
	Player_HP = Player_Base_MaxHP;
	Player_MaxHP = Player_Base_MaxHP;
	Player_Damage_Coeff = 1.0f;

	Player_Is_Dead = false;
	Is_Game_Over_Sequence = false;

	// Resource_Manager::GetInstance().Reset(); 
}

void Player_Set_POS(XMFLOAT3& POS)
{
	Player_Pos = POS;
}

const XMFLOAT3& Player_Get_POS()
{
	return Player_Pos;
}

const XMFLOAT3& Player_Get_Front()
{
	return Player_Front;
}

AABB Player_Get_AABB()
{
	float w = 0.25f;
	float h = 1.75f;
	float d = 0.25f;

	return AABB
	{
		{ Player_Pos.x + w, Player_Pos.y + h, Player_Pos.z + d }, // Max
		{ Player_Pos.x - w, Player_Pos.y	, Player_Pos.z - d }  // Min
	};
}

void Player_OnDamage(int damage)
{
	if (Player_Is_Dead || Is_Game_Over_Sequence) return;
	if (Is_Invincible) return;

	Audio_Manager::GetInstance()->Play_SFX("Player_Hit");

	Player_HP -= damage;

	Game_UI_Trigger_Damage();

	Is_Invincible = true;
	Invincible_Timer = INVINCIBLE_DURATION;

	if (Player_HP <= 0)
	{
		Player_HP = 0;
		Player_Is_Dead = true;

		Is_Game_Over_Sequence = true;
		Fade_Start(2.0f, true, { 1.0f, 0.0f, 0.0f, 1.0f });
	}
}

void Player_Heal(int amount)
{
	if (Player_Is_Dead) return;
	Player_HP += amount;
	if (Player_HP > Player_MaxHP) Player_HP = Player_MaxHP;
}

int Player_Get_HP()
{
	return Player_HP;
}

int Player_Get_MaxHP()
{
	return Player_MaxHP;
}

void Player_LevelUp()
{
	// 1. Damage UP
	Player_Damage_Coeff *= 1.25f;

	// 2. MAX HP UP
	int bonusMaxHP = static_cast<int>(Player_Base_MaxHP * 0.1f);
	Player_MaxHP += bonusMaxHP;

	// 3. Heal
	int healAmount = static_cast<int>(Player_MaxHP * 0.1f);
	Player_Heal(healAmount);

	Audio_Manager::GetInstance()->Play_SFX("Player_Level_UP");
}

float Player_Get_Damage_Coefficient()
{
	return Player_Damage_Coeff;
}

// Player Update Logic
static void Player_Update_Teleport_System()
{
	float Half_Size_X = Mash_Field_Get_Size_X() * 0.5f;
	float Half_Size_Z = Mash_Field_Get_Size_Z() * 0.5f;

	bool Teleport = false;

	if (Player_Pos.x > Half_Size_X)
	{
		Player_Pos.x -= Mash_Field_Get_Size_X();
		Teleport = true;
	}
	else if (Player_Pos.x < -Half_Size_X)
	{
		Player_Pos.x += Mash_Field_Get_Size_X();
		Teleport = true;
	}

	if (Player_Pos.z > Half_Size_Z)
	{
		Player_Pos.z -= Mash_Field_Get_Size_Z();
		Teleport = true;
	}
	else if (Player_Pos.z < -Half_Size_Z)
	{
		Player_Pos.z += Mash_Field_Get_Size_Z();
		Teleport = true;
	}
}

static XMVECTOR Player_Update_AABB_System(XMVECTOR Velocity)
{
	XMVECTOR Vel = Velocity;

	// Check Collision With Map Objects
	int OBJ_Count = Map_System_Get_Object_Count();
	float skinWidth = 0.05f;

	// Get Player AABB
	AABB Vertical_AABB = Player_Get_AABB();
	AABB Horizontal_AABB = Player_Get_AABB();

	// Get Flat Front
	Vertical_AABB.Min.x += skinWidth; Vertical_AABB.Max.x -= skinWidth;
	Vertical_AABB.Min.z += skinWidth; Vertical_AABB.Max.z -= skinWidth;

	for (int i = 0; i < OBJ_Count; i++)
	{
		const MapObject* OBJ = Map_System_Get_Object(i);
		if (OBJ->OBJ_ID == FIELD)
			continue;

		// Y Axis Collision
		IsHit Hit = Collision_Is_Hit_AABB(Vertical_AABB, OBJ->Collision);

		if (Hit.Is_Hit && Hit.Normal.y != 0.0f)
		{
			// Y Push Out
			XMVECTOR vNormal = XMLoadFloat3(&Hit.Normal);
			XMVECTOR vPush = vNormal * Hit.Depth;
			XMVECTOR vCurrentPos = XMLoadFloat3(&Player_Pos);
			vCurrentPos += vPush;
			XMStoreFloat3(&Player_Pos, vCurrentPos);

			// Landed on Object
			if (Hit.Normal.y > 0.5f)
			{
				Vel = XMVectorSetY(Vel, 0.0f);
				Player_Vel.y = 0.0f;
				Is_Jump = false;
			}
			// Hit Head
			else if (Hit.Normal.y < -0.5f)
			{
				Vel = XMVectorSetY(Vel, 0.0f);
				Player_Vel.y = 0.0f;
			}

			// Re-calc AABB
			Vertical_AABB = Player_Get_AABB();
			Vertical_AABB.Min.x += skinWidth; Vertical_AABB.Max.x -= skinWidth;
			Vertical_AABB.Min.z += skinWidth; Vertical_AABB.Max.z -= skinWidth;
		}
	}

	// XZ Axis Collision
	for (int i = 0; i < OBJ_Count; i++)
	{
		const MapObject* OBJ = Map_System_Get_Object(i);
		if (OBJ->OBJ_ID == FIELD)
			continue;

		IsHit Hit = Collision_Is_Hit_AABB(Horizontal_AABB, OBJ->Collision);

		// XZ Axis Collision
		if (Hit.Is_Hit && Hit.Normal.y == 0.0f)
		{
			// Push Out
			XMVECTOR vNormal = XMLoadFloat3(&Hit.Normal);
			XMVECTOR vPush = vNormal * Hit.Depth;
			XMVECTOR vCurrentPos = XMLoadFloat3(&Player_Pos);
			vCurrentPos += vPush;
			XMStoreFloat3(&Player_Pos, vCurrentPos);

			// Wall Sliding
			float velocityDot = XMVectorGetX(XMVector3Dot(Vel, vNormal));
			if (velocityDot < 0.0f)
			{
				Vel -= vNormal * velocityDot;
			}

			// AABB Update
			Horizontal_AABB = Player_Get_AABB();
		}
	}

	return Vel;
}

void Player_Update_Respawn_System(XMVECTOR Velocity)
{
	XMVECTOR Vel = Velocity;

	// Ground Collision Check
	float Ground_Y = Mash_Field_Get_Height(Player_Pos.x, Player_Pos.z);

	if (Ground_Y > -1000.0f)
	{
		if (Player_Pos.y < Ground_Y && XMVectorGetY(Vel) <= 0.0f)
		{
			Player_Pos.y = Ground_Y;       
			Vel = XMVectorSetY(Vel, 0.0f); 
			Player_Vel.y = 0.0f;           

			Is_Jump = false; 
		}
	}

	if (Player_Pos.y < -100.0f)
	{
		Player_Pos = Player_Start_POS;

		Player_Vel = { 0.0f, 0.0f, 0.0f };
	}
}