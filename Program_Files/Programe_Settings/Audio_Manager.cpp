/*==============================================================================

    Manage Audio with Singleton pattern [Audio_Manager.cpp]

    Author : Choi HyungJoon

    - Note
        > Make With Singleton Pattern
        > Function Type  : Pointer

==============================================================================*/
#include "Audio_Manager.h"
#include <assert.h>
#include <mmsystem.h>
#include <algorithm>
#include "Debug_ostream.h"
// SFX Lifetime Manager
// If SFX Play Done, Destroy Voice
class Audio_Manager::Voice_Callback : public IXAudio2VoiceCallback
{
public:
    Voice_Callback(Audio_Manager* pManager, std::vector<IXAudio2SourceVoice*>& activeVoices, std::mutex& mutex)
        : m_pManager(pManager), m_ActiveVoices(activeVoices), m_Mutex(mutex)
    {
        Last_Loop_Time = std::chrono::high_resolution_clock::now();
    }
    ~Voice_Callback() {}

    void OnStreamEnd() override {}
    void OnVoiceProcessingPassEnd() override {}
    void OnVoiceProcessingPassStart(UINT32 BytesRequired) override {}
    void OnBufferStart(void* pBufferContext) override {}
    void OnVoiceError(void* pBufferContext, HRESULT Error) override {}
    void OnLoopEnd(void* pBufferContext) override
    {
        if (m_pManager)
        {
            auto Now = std::chrono::high_resolution_clock::now();

            std::chrono::duration<float> duration = Now - m_pManager->Last_Loop_Check_Time;

            if (duration.count() > 5.0f)
            {
                m_pManager->Is_Loop_Flag = true;
                m_pManager->m_CurrentLoopCount++;

                m_pManager->Last_Loop_Check_Time = Now;

                Debug::D_Out << "[Audio] Loop Count : " << m_pManager->m_CurrentLoopCount << std::endl;
            }
        }
    }

    void OnBufferEnd(void* pBufferContext) override
    {
        IXAudio2SourceVoice* pSourceVoice = (IXAudio2SourceVoice*)pBufferContext;

        std::lock_guard<std::mutex> lock(m_Mutex);

        auto Find = std::find(m_ActiveVoices.begin(), m_ActiveVoices.end(), pSourceVoice);

        if (Find != m_ActiveVoices.end())
        {
            m_ActiveVoices.erase(Find);
            pSourceVoice->DestroyVoice();
        }
    }

private:
    Audio_Manager* m_pManager; // Manager Pointer
    std::vector<IXAudio2SourceVoice*>& m_ActiveVoices;
    std::mutex& m_Mutex;
    std::chrono::steady_clock::time_point Last_Loop_Time;
};

// Singleton Instance Accessor
Audio_Manager* Audio_Manager::GetInstance()
{
    static Audio_Manager instance;
    return &instance;
}

// Creator / Extinction
Audio_Manager::Audio_Manager()
    : X_Audio(nullptr), m_pMasteringVoice(nullptr),
	Current_BGM_Volume(0.5f), Target_BGM_Volume(0.5f), Target_SFX_Volume(0.5f), // Default Volume : 50%
    Voice_Call_back(nullptr), Now_Playing_BGM_Name("")
{


}

Audio_Manager::~Audio_Manager()
{
    // Safety Device Code
    if (X_Audio)
        Final();
}

// Initialize
void Audio_Manager::Init()
{
    // Prevent Multiple Initialization
    XAudio2Create(&X_Audio, 0);
    assert(X_Audio);
    X_Audio->CreateMasteringVoice(&m_pMasteringVoice);
    assert(m_pMasteringVoice);

    Voice_Call_back = new Voice_Callback(this, Active_SFX_Voices, Voice_Mutex);

    Target_BGM_Volume = 0.5f;
    Current_BGM_Volume = 0.5f;
    Target_SFX_Volume = 0.5f;

    Active_BGM_Layers.clear();
    Is_Loop_Flag = false;
}

// Finalize
void Audio_Manager::Final()
{
    if (!X_Audio) return;

    Stop_All_SFX();
    Stop_BGM();

    // Unload All BGM
    for (auto const& Find : BGMs)
    {
        if (Find.second.Source)
        {
            Find.second.Source->DestroyVoice();
        }

        delete[] Find.second.Data;
    }

    BGMs.clear();
    Now_Playing_BGM_Name = "";

    // Unload All SFX
    for (auto const& Find : SFXs)
    {
        delete[] Find.second.Data;
    }

    SFXs.clear();

    if (m_pMasteringVoice)
    {
        m_pMasteringVoice->DestroyVoice();
        m_pMasteringVoice = nullptr;
    }

    if (X_Audio)
    {
        X_Audio->Release();
        X_Audio = nullptr;
    }

    delete Voice_Call_back;
    Voice_Call_back = nullptr;

    CoUninitialize();
}

// Get Sound Data for Audio file
bool Audio_Manager::Load_BGM(const std::string& name, const char* pFilePath)
{
    // Prevent Multiple Loading
    if (BGMs.find(name) != BGMs.end())
    {
        return false;
    }

    Sound_Data New_BGMData;
    if (Load_Wave_File(pFilePath, New_BGMData))
    {
        // For BGM, Create Permanent Use.
        X_Audio->CreateSourceVoice(&New_BGMData.Source, &New_BGMData.WAVE_Format);
        assert(New_BGMData.Source);
        BGMs[name] = New_BGMData;
        return true;
    }
    return false;
}

bool Audio_Manager::Load_SFX(const std::string& name, const char* pFilePath)
{
    // Prevent Multiple Loading
    if (SFXs.find(name) != SFXs.end())
    {
        return false;
    }

    Sound_Data New_SFXData;
    if (Load_Wave_File(pFilePath, New_SFXData))
    {
        SFXs[name] = New_SFXData;
        return true;
    }
    return false;
}

// Unload Sound Data
void Audio_Manager::Unload_BGM(const std::string& name)
{
    auto Find = BGMs.find(name);
    if (Find != BGMs.end())
    {
        if (Find->second.Source)
        {
            Find->second.Source->Stop();
            Find->second.Source->DestroyVoice();
        }
        delete[] Find->second.Data;
        BGMs.erase(Find);

        if (name == Now_Playing_BGM_Name)
            Now_Playing_BGM_Name = "";
    }
}

void Audio_Manager::Unload_SFX(const std::string& name)
{
    auto Find = SFXs.find(name);
    if (Find != SFXs.end())
    {
        delete[] Find->second.Data;
        SFXs.erase(Find);
    }
}

// Manage Sound Player
void Audio_Manager::Play_BGM(const std::string& name, bool bLoop)
{
    std::vector<std::string> single;
    single.push_back(name);

    Play_Layered_BGM(single, bLoop);

    Set_Layer_Volume(name, 1.0f);
}

void Audio_Manager::Pause_BGM(bool isPause)
{
    for (auto& layer : Active_BGM_Layers)
    {
        if (layer.Source)
        {
            if (isPause)
            {
                layer.Source->Stop();
            }
            else
            {
                layer.Source->Start();
            }
        }
    }
}

void Audio_Manager::Play_Layered_BGM(const std::vector<std::string>& names, bool bLoop)
{
    Stop_BGM();

    if (names.empty()) return;

    Now_Playing_BGM_Name = names[0];

    UINT32 operationSet = 1;

    for (const auto& name : names)
    {
        auto it = BGMs.find(name);
        if (it == BGMs.end()) continue;

        Sound_Data& data = it->second;
        IXAudio2SourceVoice* pSource = nullptr;

        if (FAILED(X_Audio->CreateSourceVoice(&pSource, &data.WAVE_Format, 0, XAUDIO2_DEFAULT_FREQ_RATIO, Voice_Call_back)))
            continue;

        XAUDIO2_BUFFER buffer = { 0 };
        buffer.pAudioData = data.Data;
        buffer.Flags = XAUDIO2_END_OF_STREAM;
        buffer.AudioBytes = data.Length;
        buffer.LoopCount = bLoop ? XAUDIO2_LOOP_INFINITE : 0;
        buffer.pContext = nullptr;

        pSource->SubmitSourceBuffer(&buffer);

        float startVol = 0.0f;
        pSource->SetVolume(startVol * Target_BGM_Volume);

        pSource->Start(0, operationSet);

        BGM_Layer layer;
        layer.Name = name;
        layer.Source = pSource;
        layer.Layer_Volume = startVol;
        Active_BGM_Layers.push_back(layer);
    }

    X_Audio->CommitChanges(operationSet);

    Is_Loop_Flag = false;
}

void Audio_Manager::Stop_BGM()
{
    for (auto& layer : Active_BGM_Layers)
    {
        if (layer.Source)
        {
            layer.Source->Stop();
            layer.Source->FlushSourceBuffers();
            layer.Source->DestroyVoice();
        }
    }

    Active_BGM_Layers.clear();

    Now_Playing_BGM_Name = ""; 
}

void Audio_Manager::Stop_BGM(const std::string& name)
{
    auto Find = BGMs.find(name);

    if (Find != BGMs.end() && Find->second.Source)
        Find->second.Source->Stop();

    if (name == Now_Playing_BGM_Name)
        Now_Playing_BGM_Name = "";
}

void Audio_Manager::Play_SFX(const std::string& name)
{
    auto Find = SFXs.find(name);

    if (Find == SFXs.end())
        return;

    Sound_Data& SFX_Data = Find->second;

    // For SFX, Create Temporary, Will be Destroyed After Playback.
    IXAudio2SourceVoice* Source_Voice{};
    X_Audio->CreateSourceVoice(&Source_Voice, &SFX_Data.WAVE_Format, 0, XAUDIO2_DEFAULT_FREQ_RATIO, Voice_Call_back);

    XAUDIO2_BUFFER Buffer{};
    Buffer.AudioBytes = SFX_Data.Length;
    Buffer.pAudioData = SFX_Data.Data;
    Buffer.PlayBegin = 0;
    Buffer.PlayLength = SFX_Data.Play_Length;
    Buffer.pContext = Source_Voice; // Deliver Voice Pointers to Callback

    Source_Voice->SubmitSourceBuffer(&Buffer, NULL);
    Source_Voice->SetVolume(Target_SFX_Volume);

    {
        std::lock_guard<std::mutex> lock(Voice_Mutex);
        Active_SFX_Voices.push_back(Source_Voice);
    }

    Source_Voice->Start();
}

void Audio_Manager::Stop_All_SFX()
{
    std::lock_guard<std::mutex> Lock(Voice_Mutex);
    for (auto Voice : Active_SFX_Voices)
    {
        Voice->Stop();
        Voice->DestroyVoice();
    }

    Active_SFX_Voices.clear();
}

// Manage Volume
void Audio_Manager::Set_Target_BGM_Volume(float volume)
{
    Target_BGM_Volume = std::max(0.0f, std::min(volume, 1.0f));
    Current_BGM_Volume = Target_BGM_Volume;

    for (auto& layer : Active_BGM_Layers)
    {
        if (layer.Source)
        {
            layer.Source->SetVolume(layer.Layer_Volume * Target_BGM_Volume);
        }
    }
}

void Audio_Manager::Set_Target_SFX_Volume(float volume)
{
    Target_SFX_Volume = std::max(0.0f, std::min(volume, 1.0f));
}

void Audio_Manager::Update_Current_BGM_Volume(float volume)
{
    Current_BGM_Volume = volume;

    for (auto& layer : Active_BGM_Layers)
    {
        if (layer.Source)
        {
            layer.Source->SetVolume(layer.Layer_Volume * Current_BGM_Volume);
        }
    }
}

void Audio_Manager::Set_Layer_Volume(const std::string& name, float volume)
{
    volume = std::max(0.0f, std::min(volume, 1.0f));

    for (auto& layer : Active_BGM_Layers)
    {
        if (layer.Name == name)
        {
            layer.Layer_Volume = volume;

            if (layer.Source)
            {
                layer.Source->SetVolume(layer.Layer_Volume * Target_BGM_Volume);
            }
            return;
        }
    }
}
bool Audio_Manager::Is_BGM_Loop_Just_Finished()
{
    if (Is_Loop_Flag)
    {
        Is_Loop_Flag = false;
        return true;
    }
    return false;
}

int Audio_Manager::Get_Current_Loop_Count() const
{
    return m_CurrentLoopCount;
}

void Audio_Manager::Reset_Loop_Count()
{
    m_CurrentLoopCount = 0;
}

float Audio_Manager::Get_Target_BGM_Volume() const
{
    return Target_BGM_Volume;
}

float Audio_Manager::Get_Current_BGM_Volume() const
{
    return Current_BGM_Volume;
}

float Audio_Manager::Get_Target_SFX_Volume() const
{
    return Target_SFX_Volume;
}

std::string Audio_Manager::Get_Playing_BGM_Name() const
{
    return Now_Playing_BGM_Name;
}

// Private Helper Function
bool Audio_Manager::Load_Wave_File(const char* pFilePath, Sound_Data& soundData)
{
    // Open 'wav' File
    HMMIO HMMIO_Open = mmioOpenA((LPSTR)pFilePath, NULL, MMIO_READ);
    if (!HMMIO_Open) return false;

    // Going down to 'RIFF' chunk
    MMCKINFO RIFF_Chunk = {};
    RIFF_Chunk.fccType = mmioFOURCC('W', 'A', 'V', 'E');
    if (mmioDescend(HMMIO_Open, &RIFF_Chunk, NULL, MMIO_FINDRIFF) != MMSYSERR_NOERROR)
    {
        mmioClose(HMMIO_Open, 0);
        return false;
    }

    // Going down to 'FMT' chunk
    MMCKINFO FMT_Chunk = {};
    FMT_Chunk.ckid = mmioFOURCC('f', 'm', 't', ' ');
    if (mmioDescend(HMMIO_Open, &FMT_Chunk, &RIFF_Chunk, MMIO_FINDCHUNK) != MMSYSERR_NOERROR)
    {
        mmioClose(HMMIO_Open, 0);
        return false;
    }

    // Read Format Info
    mmioRead(HMMIO_Open, (HPSTR)&soundData.WAVE_Format, sizeof(soundData.WAVE_Format));
    mmioAscend(HMMIO_Open, &FMT_Chunk, 0);

    // Going down to 'data' chunk
    MMCKINFO DTAT_Chunk = {};
    DTAT_Chunk.ckid = mmioFOURCC('d', 'a', 't', 'a');
    if (mmioDescend(HMMIO_Open, &DTAT_Chunk, &RIFF_Chunk, MMIO_FINDCHUNK) != MMSYSERR_NOERROR)
    {
        mmioClose(HMMIO_Open, 0);
        return false;
    }

    // Read Sound Data
    UINT32 Buffer_Len = DTAT_Chunk.cksize;
    soundData.Data = new BYTE[Buffer_Len];
    mmioRead(HMMIO_Open, (HPSTR)soundData.Data, Buffer_Len);

    soundData.Length = Buffer_Len;
    soundData.Play_Length = Buffer_Len / soundData.WAVE_Format.nBlockAlign;

    mmioClose(HMMIO_Open, 0);
    return true;
}
