/*==============================================================================

    Manage Audio with Singleton pattern [Audio_Manager.h]

    Author : Choi HyungJoon

==============================================================================*/
#ifndef AUDIO_MANAGER_H
#define AUDIO_MANAGER_H

#include <xaudio2.h>
#include <string>
#include <map>
#include <vector>
#include <mutex>
#include <chrono>

struct Sound_Data
{
    IXAudio2SourceVoice* Source{};
    WAVEFORMATEX         WAVE_Format{};
    BYTE*                Data{};
    int                  Length{};
    int                  Play_Length{};
};

struct BGM_Layer
{
    std::string          Name{};
    IXAudio2SourceVoice* Source{};
    float                Layer_Volume{};
};

class Audio_Manager
{
public:
    // Get Audio Manager Instance
    static Audio_Manager* GetInstance();

    // Singleton Safety
    Audio_Manager(const Audio_Manager&) = delete;
    Audio_Manager& operator=(const Audio_Manager&) = delete;

    // Reset Audio Manager
    void Init();
    void Final();

    // --- Loading ---
    bool Load_BGM(const std::string& name, const char* pFilePath);
    bool Load_SFX(const std::string& name, const char* pFilePath);

    void Unload_BGM(const std::string& name);
    void Unload_SFX(const std::string& name);

    // --- Playback Control ---

    // Play Only One BGM
    void Play_BGM(const std::string& name, bool bLoop = true);
    void Pause_BGM(bool isPause);

    // Play Layer BGMs
    void Play_Layered_BGM(const std::vector<std::string>& names, bool bLoop = true);

    // Stop Layer BGM
    void Stop_BGM();

    // Stop BGM With Name
    void Stop_BGM(const std::string& name);

    void Play_SFX(const std::string& name);
    void Stop_All_SFX();

    // --- Volume & Control ---

    // Master Volume Setting
    void Set_Target_BGM_Volume(float volume);
    void Set_Target_SFX_Volume(float volume);
    void Update_Current_BGM_Volume(float volume);

    // Set Volume For Some Layer
    void Set_Layer_Volume(const std::string& name, float volume);

    // Check Loop For Trigger
    bool Is_BGM_Loop_Just_Finished();
    int Get_Current_Loop_Count() const;
    void Reset_Loop_Count();

    // --- Getters ---
    float Get_Target_BGM_Volume() const;
    float Get_Current_BGM_Volume() const;
    float Get_Target_SFX_Volume() const;

    std::string Get_Playing_BGM_Name() const;

private:
    Audio_Manager();
    ~Audio_Manager();

    bool Load_Wave_File(const char* pFilePath, Sound_Data& soundData);

    IXAudio2* X_Audio = nullptr;
    IXAudio2MasteringVoice* m_pMasteringVoice = nullptr;

    std::map<std::string, Sound_Data> BGMs;
    std::map<std::string, Sound_Data> SFXs;

    // Callbacks & Voices
    class Voice_Callback; // Forward Declaration
    Voice_Callback* Voice_Call_back = nullptr;

    std::vector<IXAudio2SourceVoice*> Active_SFX_Voices;
    std::mutex                        Voice_Mutex;

    // Volume States
    float Current_BGM_Volume = 0.5f;
    float Target_BGM_Volume = 0.5f;
    float Target_SFX_Volume = 0.5f;

    // Save Playing BGM Info
    std::string Now_Playing_BGM_Name;

    // Layer System
    std::vector<BGM_Layer> Active_BGM_Layers;
    bool Is_Loop_Flag = false; // Check Loop

    int m_CurrentLoopCount = 0;
    std::chrono::high_resolution_clock::time_point Last_Loop_Check_Time;
    friend class Voice_Callback;
};

#endif // AUDIO_MANAGER_H