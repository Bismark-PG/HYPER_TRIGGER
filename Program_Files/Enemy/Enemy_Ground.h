/*==============================================================================

    Manage Enemy Ground Type [Enemy_Ground.h]

    Author : Choi HyungJoon

==============================================================================*/
#ifndef ENEMY_GROUND_H
#define ENEMY_GROUND_H
#include "Enemy.h"
#include "Mash_Field.h"
#include <Player.h>

class Enemy_Ground : public Enemy
{
public:
    Enemy_Ground(const DirectX::XMFLOAT3& pos) : Enemy(pos) {}

    virtual void Update(double elapsed_time) override
    {
        float dt = static_cast<float>(elapsed_time);

        // --- Gravity Logic ---
        m_Gravity += -9.8f * dt;
        Position.y += m_Gravity * dt;

        float groundY = Mash_Field_Get_Height(Position.x, Position.z);
        if (groundY > -1000.0f)
        {
            float feetPos = Position.y - m_BottomOffset;
            if (feetPos < groundY)
            {
                Position.y = groundY + m_BottomOffset;
                m_Gravity = 0.0f;
            }
        }

        // --- AI Logic ---
        DirectX::XMFLOAT3 P_POS = Player_Get_POS();

        if (m_Info.Type == EnemyType::GROUND_NORMAL || m_Info.Type == EnemyType::GROUND_TANKER)
        {
            MoveToTarget(P_POS, m_Info.Speed, dt);
        }
        else if (m_Info.Type == EnemyType::GROUND_DASHER)
        {
            Update_Dasher_Logic(P_POS, dt);
        }

        // --- Collisions ---
        Enemy_Collision_Enemy(dt);
        Enemy_Collision_Player(dt); // Player Damege Logic?
        Enemy_Collision_Map(dt);
    }

private:
    void Update_Dasher_Logic(const DirectX::XMFLOAT3& PlayerPos, float dt)
    {
        using namespace DirectX;

        // Set Distance
        float distSq = static_cast<float>(pow(PlayerPos.x - Position.x, 2) + pow(PlayerPos.z - Position.z, 2));
        float rangeSq = static_cast<float>(pow(m_Info.Fire_Range, 2)); // Fire Range == Dash Range

        switch (m_State)
        {
        case EnemyState::CHASE:
            MoveToTarget(PlayerPos, m_Info.Speed, dt);

            // Charging
            if (distSq <= rangeSq)
            {
                m_State = EnemyState::ATTACK_PREP;
                m_StateTimer = 1.0f; // 1 Sec Wait
            }
            break;

        case EnemyState::ATTACK_PREP:
            // Only Rotation
            LookAt(PlayerPos);

            m_StateTimer -= dt;
            if (m_StateTimer <= 0.0f)
            {
                // Dash!
                m_TargetPos = PlayerPos;
                m_State = EnemyState::ATTACK_ACT;
            }
            break;

        case EnemyState::ATTACK_ACT:
            // Dash For Target POS
            if (MoveToTarget(m_TargetPos, m_Info.Dash_Speed, dt))
            {
                m_State = EnemyState::COOLDOWN;
                m_StateTimer = 1.0f; // 1 Sec Cooldown
            }
            break;

        case EnemyState::COOLDOWN:
            // 1 sec Wait
            m_StateTimer -= dt;
            if (m_StateTimer <= 0.0f)
            {
                m_State = EnemyState::CHASE;
            }
            break;
        }
    }

    void LookAt(const DirectX::XMFLOAT3& target)
    {
        using namespace DirectX;
        XMVECTOR Target = XMLoadFloat3(&target);
        XMVECTOR Pos = XMLoadFloat3(&Position);
        XMVECTOR Dir = Target - Pos;
        Dir = XMVectorSetY(Dir, 0.0f);
        Dir = XMVector3Normalize(Dir);

        DirectX::XMFLOAT3 dirFloat;
        XMStoreFloat3(&dirFloat, Dir);
        Rotation.y = atan2f(dirFloat.x, dirFloat.z) + XM_PI;
    }

    bool MoveToTarget(const DirectX::XMFLOAT3& target, float speed, float dt)
    {
        using namespace DirectX;
        XMVECTOR Target = XMLoadFloat3(&target);
        XMVECTOR Pos = XMLoadFloat3(&Position);
        XMVECTOR Dir = Target - Pos;

        Dir = XMVectorSetY(Dir, 0.0f); // Do Not Toucth Y Axis
        float dist = XMVectorGetX(XMVector3Length(Dir));

        // In Target POS, Return
        if (dist < 0.2f)
        {
            return true;
        }

        Dir = XMVector3Normalize(Dir);
        Pos += Dir * speed * dt;

        // Set Rotation
        DirectX::XMFLOAT3 dirFloat;
        XMStoreFloat3(&dirFloat, Dir);
        Rotation.y = atan2f(dirFloat.x, dirFloat.z) + XM_PI;

        XMStoreFloat3(&Position, Pos);
        return false;
    }
};
#endif