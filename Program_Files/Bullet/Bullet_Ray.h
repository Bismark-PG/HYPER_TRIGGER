/*==============================================================================

    Manage Bullet Ray-Marching Type [Bullet_Ray.h]

    Author : Choi HyungJoon

==============================================================================*/
#ifndef BULLET_RAY_H
#define BULLET_RAY_H
#include "Bullet.h"
#include "Enemy_Manager.h"
#include "Map_System.h"
#include "Game_Model.h"        
#include "Billboard_Manager.h" 
#include "Shader_Manager.h" 
#include "Mash_Field.h"
#include "Particle_Manager.h"
#include "Player.h"
#include "Random_Heapler_Logic.h"
#include "Player_Camera.h"
using namespace DirectX;

class Bullet_Ray : public Bullet
{
public:
    Bullet_Ray(const XMFLOAT3& visualPos, const XMFLOAT3& logicalPos, const XMFLOAT3& dir, BulletOwner owner, int damage)
        : Bullet(visualPos, dir, owner)
        , m_StartPosition(visualPos)
        , m_TargetPosition{}        
        , m_HitEnemy(false)
        , m_HitWall(false)
    {
        m_Damage = damage;

        m_MaxRange = 100.0f;    // Range of the Ray
        m_StepSize = 0.5f;      // Step Size for Ray Marching
        m_VisualSpeed = 5.0f;  // Enemy Bullet Speed

        m_pModel = Model_Manager::GetInstance()->GetModel("Ball");

        // If Fire Owner Is Player, Set Ray Mode
        if (m_Owner == BulletOwner::PLAYER)
        {
            m_VisualSpeed = 100.0f; // Bullet Speed
            Calculate_Hit_Point(logicalPos, dir);

            // Virtual Bullet
            XMVECTOR vStart = XMLoadFloat3(&m_Position); // m_Position == visualPos
            XMVECTOR vTarget = XMLoadFloat3(&m_TargetPosition);
            XMVECTOR vNewDir = XMVector3Normalize(vTarget - vStart);
            XMStoreFloat3(&m_Direction, vNewDir);
        }
        else // ENEMY
        {
            // Shoot To Player Ditection
            XMVECTOR vStart = XMLoadFloat3(&m_Position);
            XMVECTOR vDir = XMLoadFloat3(&m_Direction);
            XMVECTOR vEnd = vStart + (vDir * m_MaxRange);
            XMStoreFloat3(&m_TargetPosition, vEnd);
        }
    }

    virtual void Update(double elapsed_time) override
    {
        float dt = static_cast<float>(elapsed_time);

        // --- Bullet Movement ---
        XMVECTOR Current_Pos = XMLoadFloat3(&m_Position);
        XMVECTOR Dir = XMLoadFloat3(&m_Direction);

        Current_Pos += Dir * m_VisualSpeed * dt;
        XMStoreFloat3(&m_Position, Current_Pos);

        // Make Particle
        XMVECTOR vTrailDir = -Dir;

        float spread = 0.2f;
        XMFLOAT3 randomOffset = {
            RandomFloatRange(-spread, spread),
            RandomFloatRange(-spread, spread),
            RandomFloatRange(-spread, spread)
        };
        vTrailDir += XMLoadFloat3(&randomOffset);
        vTrailDir = XMVector3Normalize(vTrailDir);

  
        float trailSpeed = RandomFloatRange(1.0f, 3.0f);
        vTrailDir *= trailSpeed;

        XMFLOAT3 trailVel;
        XMStoreFloat3(&trailVel, vTrailDir);

        m_AliveTimer += dt;

        if (m_Owner == BulletOwner::ENEMY)
        {
            // Enemy Bullect Will Be Dead 10 Sec
            if (m_AliveTimer >= 10.0f)
            {
                m_IsDead = true;
            }
        }
        else // BulletOwner::PLAYER
        {
            // Get Player FOV
            XMFLOAT3 camPos = Player_Camera_Get_Current_POS();
            XMFLOAT3 camFront = Player_Camera_Get_Front();

            XMVECTOR vCamPos = XMLoadFloat3(&camPos);
            XMVECTOR vCamFront = XMLoadFloat3(&camFront);
            XMVECTOR vBulletPos = XMLoadFloat3(&m_Position);

            // Vector For Camera To Bullet
            XMVECTOR vToBullet = XMVector3Normalize(vBulletPos - vCamPos);

            float dot = XMVectorGetX(XMVector3Dot(vToBullet, vCamFront));

            // Out Of Screen, Bullect Dead
            if (dot < 0.2f) // 1.0f = Front(0), 0.0f = Horizon(90), -1.0 = Back(180)
            {
                m_IsDead = true;
            }
        }

        // Make Particle
        m_TrailTimer += dt;

        while (m_TrailTimer >= m_TrailInterval)
        {
            m_TrailTimer -= m_TrailInterval;

            // LifeTime : 0.2 ~ 0.3, Size : 0.1 ~ 0.2
            Particle_Manager::GetInstance().Spawn(
                m_Position,
                trailVel,
                { 1.0f, 1.0f, 1.0f, 1.0f },
                RandomFloatRange(0.2f, 0.3f),
                RandomFloatRange(0.1f, 0.2f),
                Particle_Type::BULLET
            );
        }

        // Collition Check
        if (m_Owner == BulletOwner::PLAYER)
        {
            XMVECTOR Target_Pos = XMLoadFloat3(&m_TargetPosition);
            // If Direction Is To Close For Tager, Hit
            float distToTarget = XMVectorGetX(XMVector3Length(Target_Pos - Current_Pos));

            // If Will Be Collied In Next Frame, HIt
            if (distToTarget <= (m_VisualSpeed * dt))
            {
                Spawn_Hit_Effect();
                m_IsDead = true;
                XMStoreFloat3(&m_Position, Target_Pos);
            }
        }
        else // BulletOwner::ENEMY
        {
            AABB BulletAABB =
            {
                { m_Position.x + 0.3f, m_Position.y + 0.3f, m_Position.z + 0.3f },
                { m_Position.x - 0.3f, m_Position.y - 0.3f, m_Position.z - 0.3f }
            };

            if (Collision_Is_Hit_AABB(BulletAABB, Player_Get_AABB()).Is_Hit)
            {
                // Player Hit!
                Player_OnDamage(m_Damage);

                m_IsDead = true;
                Spawn_Hit_Effect();// Effect
            }
        }

        // Check Floor Or Cude
        if (Map_System_Check_Collision_AABB(GetAABB()))
        {
            m_IsDead = true;
            Spawn_Hit_Effect();
        }
    }

    virtual void Draw() override
    {
        if (m_pModel)
        {
            XMMATRIX world = XMMatrixScaling(0.1f, 0.1f, 0.1f) * XMMatrixTranslation(m_Position.x, m_Position.y, m_Position.z);

            Shader_Manager::GetInstance()->SetDiffuseColor(XMFLOAT4(1.0f, 1.0f, 0.0f, 1.0f));

            ModelDraw(m_pModel, world);

            Shader_Manager::GetInstance()->SetDiffuseColor(XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));
        }
    }

private:
    float m_MaxRange;
    float m_StepSize;

    MODEL* m_pModel = nullptr;
    DirectX::XMFLOAT3 m_TargetPosition;
    DirectX::XMFLOAT3 m_StartPosition; 
    float m_VisualSpeed;

    bool m_HitEnemy = false;
    bool m_HitWall = false;

    float m_AliveTimer = 0.0f;
    float m_TrailTimer = 0.0f;
    const float m_TrailInterval = 0.05f;

    // --- Ray Maching ---
    void Calculate_Hit_Point(const XMFLOAT3& camPos, const XMFLOAT3& camDir)
    {
        // Ray Start POS == Player Camera POS
        XMVECTOR Ray_Pos = XMLoadFloat3(&camPos);
        XMVECTOR Ray_Dir = XMLoadFloat3(&camDir);
        float Now_Dist = 0.0f;

        m_HitEnemy = false;
        m_HitWall = false;

        // Default Target == Max Range
        XMVECTOR vEndPos = Ray_Pos + Ray_Dir * m_MaxRange;
        XMStoreFloat3(&m_TargetPosition, vEndPos);

        while (Now_Dist < m_MaxRange)
        {
            // Update Ray
            Ray_Pos += Ray_Dir * m_StepSize;
            Now_Dist += m_StepSize;

            XMFLOAT3 CheckPos;
            XMStoreFloat3(&CheckPos, Ray_Pos);

            // AABB Collision (Wall/Object)
            AABB RayAABB = {
                { CheckPos.x + 0.1f, CheckPos.y + 0.1f, CheckPos.z + 0.1f },
                { CheckPos.x - 0.1f, CheckPos.y - 0.1f, CheckPos.z - 0.1f }
            };

            if (Map_System_Check_Collision_AABB(RayAABB))
            {
                m_HitWall = true;
                XMStoreFloat3(&m_TargetPosition, Ray_Pos);
                break;
            }

            // Ground Collision
            float Ground_Y = Mash_Field_Get_Height(CheckPos.x, CheckPos.z);
            if (CheckPos.y < Ground_Y)
            {
                m_HitWall = true;
                XMStoreFloat3(&m_TargetPosition, Ray_Pos);
                break;
            }

            // Enemy Collision
            if (m_Owner == BulletOwner::PLAYER)
            {
                Enemy* HitEnemy = Enemy_Manager::GetInstance().Check_Collision_AABB(RayAABB);
                if (HitEnemy != nullptr)
                {
                    HitEnemy->OnDamage(m_Damage);
                    m_HitEnemy = true;
                    XMStoreFloat3(&m_TargetPosition, Ray_Pos);
                    break;
                }
            }
        }
    }

    void Spawn_Hit_Effect() const
    {
        if (m_HitEnemy)
        {
            Particle_Manager::GetInstance().Spawn_Spark(m_TargetPosition);
        }
        else if (m_HitWall)
        {
            Particle_Manager::GetInstance().Spawn_Spark(m_TargetPosition);
        }
        else
        {
        }
    }

    AABB GetAABB()
    {
        return {
           { m_Position.x + 0.2f, m_Position.y + 0.2f, m_Position.z + 0.2f },
           { m_Position.x - 0.2f, m_Position.y - 0.2f, m_Position.z - 0.2f }
        };
    }
};

#endif // BULLET_RAY_H