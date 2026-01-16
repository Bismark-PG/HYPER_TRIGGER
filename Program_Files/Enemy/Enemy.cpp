/*==============================================================================

	Manage Enemy [Enemy.h]

	Author : Choi HyungJoon

==============================================================================*/
#include "Enemy.h"
#include "Enemy_Manager.h"
#include "Enemy_Ground.h"
#include "Enemy_Flight.h"
#include "Game_Model.h"
#include "Player.h"
#include "Map_System.h"
#include "Debug_Collision.h"
#include <vector>
#include "Game.h"
#include "Resource_Manager.h"
#include "Billboard_Manager.h"
#include "Shader_Manager.h"
#include "Audio_Manager.h"
using namespace DirectX;

// Enemy Class Implementation
Enemy::Enemy(const XMFLOAT3& pos)
	: Position(pos)
	, m_Info{}
{
	m_pModel = Model_Manager::GetInstance()->GetModel("Ball");
	// Info Will Be Paste In Activate Logic
}

Enemy::~Enemy()
{
}

void Enemy::Activate(const XMFLOAT3& pos, EnemyType type)
{
	m_IsActive = true;
	Position = pos;
	m_State = EnemyState::CHASE;
	m_Gravity = 0.0f;
	m_StateTimer = 0.0f;

	switch (type)
	{
	case EnemyType::GROUND_NORMAL:
		m_Info = Ground_Normal_Info; 
		break;

	case EnemyType::GROUND_DASHER: 
		m_Info = Ground_Dasher_Info; 
		break;

	case EnemyType::GROUND_TANKER: 
		m_Info = Ground_Tanker_Info;
		break;

	case EnemyType::FLIGHT_NORMAL: 
		m_Info = Filght_Normal_Info;
		break;

	case EnemyType::FLIGHT_DASHER: 
		m_Info = Filght_Dasher_Info; 
		break;
	}

	m_HP = m_Info.MaxHP;

	if (m_pModel)
	{
		m_BottomOffset = fabs(m_pModel->Local_AABB.Min.y) * m_Info.Scale;
	}
}

void Enemy::Deactivate()
{
	if (!m_IsActive) return;

	m_IsActive = false;

	if (Game_Check_Is_Resetting())
	{
		return;
	}

	Audio_Manager::GetInstance()->Play_SFX("Enemy_Dead");

	if (m_Info.Type == EnemyType::GROUND_TANKER)
	{
		Resource_Manager::GetInstance().Spawn_Resource(Position, Resource_Type::HEALTH_POT, 25.0f);
	}
	Resource_Manager::GetInstance().Spawn_Resource(Position, Resource_Type::EXP_ORB, m_Info.EXP_Mount);

	float effectScale = m_Info.Scale * 0.6f;

	Billboard_Manager::Instance().Create_Effect(Position, 0, effectScale, Effect_Type::EXPLOSION);
}

void Enemy::Update(double elapsed_time)
{
}

void Enemy::Draw()
{
	if (!m_pModel) return;

	XMFLOAT4 Color = { 1.0f, 1.0f, 1.0f, 1.0f }; // Normal >> White

	switch (m_Info.Type)
	{
	case EnemyType::GROUND_NORMAL:
		Color = { 0.2f, 1.0f, 0.2f, 1.0f }; // Green
		break;
	case EnemyType::GROUND_DASHER:
		Color = { 1.0f, 0.2f, 0.2f, 1.0f }; // Red
		break;
	case EnemyType::GROUND_TANKER:
		Color = { 0.2f, 0.2f, 1.0f, 1.0f }; // Blue
		break;
	case EnemyType::FLIGHT_NORMAL:
		Color = { 1.0f, 1.0f, 0.0f, 1.0f }; // Yellow
		break;
	case EnemyType::FLIGHT_DASHER:
		Color = { 1.0f, 0.5f, 0.0f, 1.0f }; // Orange
		break;
	}

	Shader_Manager::GetInstance()->SetDiffuseColor(Color);

	XMMATRIX mtxScale = XMMatrixScaling(m_Info.Scale, m_Info.Scale, m_Info.Scale);
	XMMATRIX mtxRot = XMMatrixRotationY(Rotation.y);
	XMMATRIX mtxTrans = XMMatrixTranslation(Position.x, Position.y, Position.z);
	XMMATRIX mtxWorld = mtxScale * mtxRot * mtxTrans;

	ModelDraw(m_pModel, mtxWorld);

	Shader_Manager::GetInstance()->SetDiffuseColor(XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));

	if (Get_Debug_Mode_State())
	{
		Debug_Collision_Draw(GetAABB(), { 1.0f, 0.0f, 0.0f, 1.0f });
	}
}

void Enemy::OnDamage(int damage)
{
	if (!m_IsActive)
	{
		return;
	}
	int FinalDamage = damage;

	if (FinalDamage < 1)
	{
		FinalDamage = 1;
	}

	m_HP -= FinalDamage;

	if (m_HP <= 0)
	{
		Deactivate();
	}
}

AABB Enemy::GetAABB() const
{
	if (!m_pModel) return AABB{ {0,0,0}, {0,0,0} };

	AABB worldAABB = {};

	XMVECTOR Min = XMLoadFloat3(&m_pModel->Local_AABB.Min) * m_Info.Scale;
	XMVECTOR Max = XMLoadFloat3(&m_pModel->Local_AABB.Max) * m_Info.Scale;
	XMVECTOR Pos = XMLoadFloat3(&Position);

	XMStoreFloat3(&worldAABB.Min, Min + Pos);
	XMStoreFloat3(&worldAABB.Max, Max + Pos);

	return worldAABB;
}

void Enemy::Enemy_Collision_Map(double dt)
{
	AABB E_AABB = GetAABB();
	int OBJ_Count = Map_System_Get_Object_Count();

	XMVECTOR Enemy_POS = XMLoadFloat3(&Position);

	for (int i = 0; i < OBJ_Count; i++)
	{
		const MapObject* OBJ = Map_System_Get_Object(i);

		if (OBJ->OBJ_ID == FIELD) continue;

		IsHit hit = Collision_Is_Hit_AABB(E_AABB, OBJ->Collision);

		if (hit.Is_Hit)
		{
			XMVECTOR vNormal = XMLoadFloat3(&hit.Normal);
			XMVECTOR vPush = vNormal * hit.Depth;

			Enemy_POS += vPush;

			XMStoreFloat3(&Position, Enemy_POS);
			E_AABB = GetAABB();
		}
	}
}

void Enemy::Enemy_Collision_Player(double dt)
{
	XMFLOAT3 P_Pos = Player_Get_POS();
	XMFLOAT3 E_Pos = Position;

	// 1. Set Y Axis And Lock
	XMVECTOR V_P_Pos = XMLoadFloat3(&P_Pos);
	XMVECTOR V_E_Pos = XMLoadFloat3(&E_Pos);

	V_P_Pos = XMVectorSetY(V_P_Pos, 0.0f);
	V_E_Pos = XMVectorSetY(V_E_Pos, 0.0f);

	// 2. Get Dist
	XMVECTOR V_Dist = XMVector3Length(V_E_Pos - V_P_Pos);
	float dist = XMVectorGetX(V_Dist);

	// 3. Set AABB Range
	AABB P_AABB = Player_Get_AABB();
	float P_Width = P_AABB.Max.x - P_AABB.Min.x;
	float P_Radius = P_Width * 0.5f;

	float E_Radius = m_Info.Scale * 0.5f;

	// Get Real Core
	float Collision_Radius = (P_Radius + E_Radius) * 0.5f;

	// 4. Collision Check And Push Back
	if (dist < Collision_Radius)
	{
		if (dist < 0.001f)
		{
			Position.x += 0.1f;
			return;
		}

		Player_OnDamage(5);

		XMVECTOR V_Dir = V_E_Pos - V_P_Pos;
		V_Dir = XMVector3Normalize(V_Dir);

		float pushDist = Collision_Radius - dist;
		XMVECTOR V_Current_Pos = XMLoadFloat3(&Position);
		V_Current_Pos += V_Dir * pushDist;

		XMStoreFloat3(&Position, V_Current_Pos);
	}
}

void Enemy::Enemy_Collision_Enemy(double dt)
{
	std::vector<Enemy*>& activeList = Enemy_Manager::GetInstance().Get_Active_List();

	AABB My_AABB = GetAABB();
	XMVECTOR My_Pos = XMLoadFloat3(&Position);

	for (const auto& other : activeList)
	{
		// Skip Self, Or Dead Enemy
		if (!other->IsActive() || other->IsDead())
		{
			continue;
		}

		IsHit Hit = Collision_Is_Hit_AABB(My_AABB, other->GetAABB());

		if (Hit.Is_Hit)
		{
			XMVECTOR vNormal = XMLoadFloat3(&Hit.Normal);
			XMVECTOR vDepth = XMLoadFloat(&Hit.Depth);

			// Each Enemy Push Out Half Distance
			My_Pos += vNormal * vDepth * 0.5f;
		}
	}
	XMStoreFloat3(&Position, My_Pos);
}