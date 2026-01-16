/*==============================================================================

	Managed Billboard System [Billboard_Manager.h]

	Author : Choi HyungJoon

==============================================================================*/
#ifndef BILLBOARD_MANAGER_H
#define BILLBOARD_MANAGER_H

#include <vector>
#include <DirectXMath.h>
#include "Collision.h"

enum class Billboard_Type
{
	TARGET,
	OBJECT
};

enum class Effect_Type
{
	SMOKE,
	EXPLOSION
};

class Billboard_Object;
class Billboard_Target;

class Billboard_Manager
{
public:
    static Billboard_Manager& Instance()
    {
        static Billboard_Manager instance;
        return instance;
    }

    void Init();
    void Final();
    void Update(double dt);
    void Draw();

    void Create(const DirectX::XMFLOAT3& pos, Billboard_Type Type);
    void Create_Effect(const DirectX::XMFLOAT3& pos, int patternID, float scale, Effect_Type Type);

    Billboard_Target* Check_Collision(const AABB& box);

private:
    Billboard_Manager() = default;
    ~Billboard_Manager() = default;

    std::vector<Billboard_Object*> m_ObjList;

    static constexpr int MAX_EFFECT_POOL = 200;
    std::vector<class Billboard_Effect*> m_EffectPool;

    int m_ExplosionPatternID = -1;
};

#endif // BILLBOARD_MANAGER_H