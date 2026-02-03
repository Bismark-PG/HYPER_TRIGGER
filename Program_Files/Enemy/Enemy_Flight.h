/*==============================================================================

    Manage Enemy Flight Type [Enemy_Flight.h]

    Author : Choi HyungJoon

==============================================================================*/
#ifndef ENEMY_FLITGHT_H
#define ENEMY_FLITGHT_H
#include "Enemy.h"
#include "Player.h"
#include "Bullet_Manager.h"
#include "Audio_Manager.h"

constexpr float FLIGHT_AIM_OFFSET = 1.0f;
constexpr float FLIGHT_HOVERING_OFFSET = 3.5f;

class Enemy_Flight : public Enemy
{
public:
    Enemy_Flight(const DirectX::XMFLOAT3& pos) : Enemy(pos) {}

    virtual void Update(double elapsed_time) override
    {
        float dt = static_cast<float>(elapsed_time);

        DirectX::XMFLOAT3 P_POS = Player_Get_POS();
        P_POS.y += FLIGHT_AIM_OFFSET; // Aiming Player Head

        // --- Distance Check ---
        float distSq = static_cast<float>(pow(P_POS.x - Position.x, 2) +
            pow(P_POS.y - Position.y, 2) +
            pow(P_POS.z - Position.z, 2));

        float rangeSq = static_cast<float>(pow(m_Info.Fire_Range, 2));

        // --- AI Logic ---
        LookAt(P_POS); // Look Player Always

        if (m_State == EnemyState::CHASE)
        {
            if (distSq > rangeSq)
            {
                MoveTo(P_POS, m_Info.Speed, dt);
            }
            else
            {
                // Resct Y Axis Whit Hovering Offset
                if (Position.y < P_POS.y + FLIGHT_HOVERING_OFFSET - 1.0f)
                {
                    Position.y += m_Info.Speed * dt * 0.5f;
                }

                m_StateTimer = 0.0f;
                m_State = EnemyState::ATTACK_PREP;
            }
        }
        else if (m_State == EnemyState::ATTACK_PREP)
        {
            m_StateTimer += dt;
            if (m_StateTimer >= 0.5f)
            {
                m_StateTimer = 0.0f;
                m_State = EnemyState::ATTACK_ACT;
            }
        }
        else if (m_State == EnemyState::ATTACK_ACT)
        {
            // Enemy Fire
            Shoot_Bullet(P_POS);

            // After Attack, Return To Chase
            m_StateTimer = 0.0f;
            m_State = EnemyState::CHASE;
        }
        else if (m_State == EnemyState::COOLDOWN)
        {
            m_StateTimer += dt;
            if (m_StateTimer >= m_Info.Fire_Interval)
            {
                m_StateTimer = 0.0f;
                m_State = EnemyState::CHASE;
            }
        }

        // Ground Collision Prevention
        float groundY = Mash_Field_Get_Height(Position.x, Position.z);
        if (groundY > -1000.0f)
        {
            float feetPos = Position.y - m_BottomOffset;
            if (feetPos < groundY)
            {
                Position.y = groundY + m_BottomOffset + FLIGHT_HOVERING_OFFSET;
            }
        }

        Enemy_Collision_Enemy(dt);
        Enemy_Collision_Player(dt);
        Enemy_Collision_Map(dt);
    }

private:
    void LookAt(const DirectX::XMFLOAT3& target)
    {
        using namespace DirectX;
        XMVECTOR Target = XMLoadFloat3(&target);
        XMVECTOR Pos = XMLoadFloat3(&Position);
        XMVECTOR Dir = XMVector3Normalize(Target - Pos);

        DirectX::XMFLOAT3 dirFloat; XMStoreFloat3(&dirFloat, Dir);
        Rotation.y = atan2f(dirFloat.x, dirFloat.z) + XM_PI;
    }

    void MoveTo(const DirectX::XMFLOAT3& target, float speed, float dt)
    {
        using namespace DirectX;
        XMVECTOR Target = XMLoadFloat3(&target);
        XMVECTOR Pos = XMLoadFloat3(&Position);
        XMVECTOR Dir = Target - Pos;

        if (XMVectorGetX(XMVector3Length(Dir)) < 0.1f) return;

        Dir = XMVector3Normalize(Dir);
        Pos += Dir * speed * dt;
        XMStoreFloat3(&Position, Pos);
    }

    void Shoot_Bullet(const DirectX::XMFLOAT3& targetPos)
    {
        using namespace DirectX;

        // Fire POS
        XMFLOAT3 MyPos = Position;

        // Get Direction
        XMVECTOR vPos = XMLoadFloat3(&Position);
        XMVECTOR vTarget = XMLoadFloat3(&targetPos);
        XMVECTOR vDir = XMVector3Normalize(vTarget - vPos);

        XMFLOAT3 Dir;
        XMStoreFloat3(&Dir, vDir);

        // Fire, Use Bullet Ray
        Bullet_Manager::Instance().Fire(BulletType::RAY, MyPos, targetPos, Dir, m_Info.Bullet_Damage, BulletOwner::ENEMY);

        Audio_Manager::GetInstance()->Play_SFX("Enemy_Fire");
    }
};
#endif