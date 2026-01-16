/*==============================================================================

    Manage Visual Effects [Particle_Manager.h]

    Author : Choi HyungJoon

==============================================================================*/
#ifndef PARTICLE_MANAGER_H
#define PARTICLE_MANAGER_H

#include <vector>
#include <DirectXMath.h>

enum class Particle_Type
{
    EXP,   
    HP,    
    BULLET,
    SPARK  
};

// Particle Info
struct Particle
{
    bool                Active;
    DirectX::XMFLOAT3   Position;
    DirectX::XMFLOAT3   Velocity;
    DirectX::XMFLOAT4   Color;
    float               LifeTime;
    float               MaxLifeTime;
    float               Size;
    int                 TexID;
};

class Particle_Manager
{
public:
    static Particle_Manager& GetInstance()
    {
        static Particle_Manager instance;
        return instance;
    }

    void Init();
    void Final();
    void Update(double dt);
    void Draw();

    void Spawn(const DirectX::XMFLOAT3& pos, const DirectX::XMFLOAT3& vel,
        const DirectX::XMFLOAT4& color, float lifeTime, float size, Particle_Type type);

    void Spawn_Spark(const DirectX::XMFLOAT3& pos);

private:
    Particle_Manager() = default;
    ~Particle_Manager() = default;
    Particle_Manager(const Particle_Manager&) = delete;
    Particle_Manager& operator=(const Particle_Manager&) = delete;

    static const int MAX_OBJECT_POOL = 4000;
    static const int MAX_SPARK_POOL = 100;

    std::vector<Particle> m_ObjectPool;
    std::vector<Particle> m_SparkPool;

    int m_TexID_Exp = -1;
    int m_TexID_Hp = -1;
    int m_TexID_Bullet = -1;
    int m_TexID_Spark = -1;// "Pixel_Withe"
};

#endif // PARTICLE_MANAGER_H