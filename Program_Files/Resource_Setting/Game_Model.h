/*==============================================================================

    Management Model Resource [Game_Model.h]

    Author : Choi HyungJoon

==============================================================================*/
#ifndef GAME_MODEL_H
#define GAME_MODEL_H
#include <d3d11.h>
#include <DirectXMath.h>
#include <wrl/client.h>
#include <string>
#include <unordered_map>
#include "model.h"

class Model_Manager
{
public:
    static Model_Manager* GetInstance();

    // Singleton Safety
    Model_Manager(const Model_Manager&) = delete;
    Model_Manager& operator=(const Model_Manager&) = delete;

    static void Init();
    static void Final();

    void Load();
    static MODEL* GetModel(const std::string& name);

private:
    Model_Manager() = default;
    ~Model_Manager() = default;

    void Clear();

    std::unordered_map<std::string, MODEL*> m_ModelDB;
};
#endif // GAME_MODEL_H
