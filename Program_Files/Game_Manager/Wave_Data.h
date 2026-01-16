/*==============================================================================
    
    Define Enemy Spawn Waves [Wave_Data.h]
    
    Author : Choi HyungJoon

==============================================================================*/
#ifndef WAVE_DATA_H
#define WAVE_DATA_H
#include "Enemy.h"
// For Prototype, Delete After
constexpr float Wave_End_Time = 240;

/*==============================================================================
// --- Wave Time Line ---
==============================================================================*/
struct WaveInfo
{
    float       StartTime;     
    float       EndTime;       
    float       SpawnInterval; 
    EnemyType   Type;          
    int         BatchCount;    
};

static const WaveInfo Wave_Data[] =
{
    // Start ~ 40 Sec
    // 00 ~ 10 Sec : Interval 2 Sec, Ground Normal, Count 2
    { 0.0f,  10.0f, 2.0f, EnemyType::GROUND_NORMAL, 2 },
    // 10 ~ 40 Sec : Interval 5 Sec, Filght Normal, Count 1
    { 10.0f, 40.0f, 5.0f, EnemyType::FLIGHT_NORMAL, 1 },

    // 30 Sec ~ 60 Sec
    // Interval 2 Sec, Ground Normal, Count 2
    { 30.0f, 60.0f, 2.0f, EnemyType::GROUND_NORMAL, 2 },
    // Interval 5 Sec, Ground Dasher, Count 1
    { 30.0f, 60.0f, 5.0f, EnemyType::GROUND_DASHER, 1 },

    // 60 Sec ~ 120 Sec
    // Interval 2 Sec, Ground Normal, Count 2
    { 60.0f, 120.0f, 2.0f,  EnemyType::GROUND_NORMAL, 2 },
    // Interval 5 Sec, Ground Dasher, Count 3
    { 60.0f, 120.0f, 5.0f,  EnemyType::GROUND_DASHER, 3 },
    // Interval 20 Sec, Ground Tanker, Count 2
    { 60.0f, 120.0f, 20.0f, EnemyType::GROUND_TANKER, 2 },
    // Interval 3 Sec, Filght Normal, Count 1
    { 60.0f, 120.0f, 3.0f,  EnemyType::FLIGHT_NORMAL, 1 },
    // Interval 5 Sec, Ground Dasher, Count 2
    { 60.0f, 120.0f, 5.0f,  EnemyType::FLIGHT_DASHER, 2 },

    // 120 Sec ~ Wave_End_Time
    // Interval 2 Sec, Ground Normal, Count 3
    { 120.0f, Wave_End_Time, 2.0f, EnemyType::GROUND_NORMAL, 3 },
    // Interval 1.5 Sec, Ground Dasher, Count 2
    { 120.0f, Wave_End_Time, 1.5f, EnemyType::GROUND_DASHER, 2 },
    // Interval 7.5 Sec, Ground Tanker, Count 1
    { 120.0f, Wave_End_Time, 7.5f, EnemyType::GROUND_TANKER, 1 },
    // Interval 3 Sec, Filght Normal, Count 2
    { 120.0f, Wave_End_Time, 3.0f, EnemyType::FLIGHT_NORMAL, 2 },
    // Interval 5 Sec, Ground Dasher, Count 3
    { 120.0f, Wave_End_Time, 5.0f, EnemyType::FLIGHT_DASHER, 3 },
};
static const int Wave_Data_Count = sizeof(Wave_Data) / sizeof(WaveInfo);

/*==============================================================================
// --- DJing System ---
==============================================================================*/
struct Music_Trigger_Info
{
    int         Loop_Index; // Trigger Loop Count
    const char* L_Name;     // Part Name
    bool        Is_On;      // true : (ON), false : (OFF)
    bool        Is_Set = false; // Trigger Check
};

static const Music_Trigger_Info DJMAX[] =
{
    // Phase 1
    { 0,   "Snare",   true },
    { 1,   "Kick",    true },

    // Phase 2
    { 3,  "Guitar",  true },
    { 3,  "Kick",    false },
    { 3,  "Snare",   false },

    // Phase 3
    { 5,  "Guitar",  false },
    { 5,  "Kick",    true },
    { 5,  "Snare",   true },

    // Phase 4
    { 7,  "Bass",    false },
    { 7,  "Kick",    false },
    { 7,  "Snare",   false },
    { 7,  "Synth_1", true },

    // Phase 5
    { 8,  "Synth_2", true },

    { 9,  "Synth_1", false },
    { 9,  "Kick",    true },
    { 9,  "Snare",   true },

    // Phase 6
    { 10,  "Guitar",  true },

    { 12, "Guitar",  false },
    { 12, "Bass",    true },

    // Final Phase
    { 13, "Guitar",  true },
    { 13, "Synth_1", true },

    // END SIGNAL
    { 25, "END_SIGNAL", false }
};

static const int DJ_Count = sizeof(DJMAX) / sizeof(Music_Trigger_Info);

#endif // WAVE_DATA_H