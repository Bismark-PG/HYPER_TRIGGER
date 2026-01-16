/*==============================================================================
   
   Manage Enemy Spawning Logic [Enemy_Spawner.cpp]
   
   Author : Choi HyungJoon

==============================================================================*/
#include "Enemy_Spawner.h"
#include "Enemy_Manager.h"
#include "Player.h"
#include "Mash_Field.h"
#include "debug_ostream.h"
#include "Random_Heapler_Logic.h"
using namespace DirectX;

void Enemy_Spawner::Init()
{
    m_GameTime = 0.0f;

    // Timer Reset count >> Wave Count
    m_WaveTimers.clear();
    m_WaveTimers.resize(Wave_Data_Count, 0.0f);
}

void Enemy_Spawner::Reset()
{
    Init();
}

void Enemy_Spawner::Update(double dt)
{
    float fdt = static_cast<float>(dt);
    m_GameTime += fdt;

    // Check Wave Time
    for (int i = 0; i < Wave_Data_Count; ++i)
    {
        const WaveInfo& wave = Wave_Data[i];

        // Is Now Wave Time?
        if (m_GameTime >= wave.StartTime && m_GameTime < wave.EndTime)
        {
            // Timer Reset
            m_WaveTimers[i] -= fdt;

            // Spawn
            if (m_WaveTimers[i] <= 0.0f)
            {
                // Timer Reset For Next Spawn
                m_WaveTimers[i] = wave.SpawnInterval;

                // Spawn For Count
                for (int count = 0; count < wave.BatchCount; ++count)
                {
                    XMFLOAT3 spawnPos = Get_Random_Spawn_Position();

                    // Check Y POS?
                    Enemy_Manager::GetInstance().Spawn(spawnPos, wave.Type);
                }
            }
        }
    }
}

DirectX::XMFLOAT3 Enemy_Spawner::Get_Random_Spawn_Position()
{
    // Get Player POS
    XMFLOAT3 playerPos = Player_Get_POS();

    // 1. Random Radian
    float angle = RandomFloat(0.0f, XM_2PI);

    // 2. Random Distance
    float distance = RandomFloat(20.0f, 30.0f);

    // 3. Get POS
    float spawnX = playerPos.x + cosf(angle) * distance;
    float spawnZ = playerPos.z + sinf(angle) * distance;

    // 4. Restore Y Axis
    float spawnY = Mash_Field_Get_Height(spawnX, spawnZ);

    // If Out Of Map?
    if (spawnY < -500.0f)
    {
        spawnY = playerPos.y + 10.0f; // Air Drop
    }
    else
    {
        spawnY += 1.0f; // Upside Of Ground
    }

    return XMFLOAT3(spawnX, spawnY, spawnZ);
}