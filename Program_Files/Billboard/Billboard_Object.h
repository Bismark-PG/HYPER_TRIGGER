/*==============================================================================

	Managed Billboard Object Type [Billboard_Object.h]

	Author : Choi HyungJoon

==============================================================================*/
#ifndef BILLBOARD_OBJECT_H
#define BILLBOARD_OBJECT_H
#include <DirectXMath.h>
#include "Collision.h"
#include "Billboard.h"

class Billboard_Object
{
public:
    Billboard_Object(int texID, const DirectX::XMFLOAT3& pos, float scaleX, float scaleY);
    virtual ~Billboard_Object();

    // --- Basic System ---
    virtual void Activate(const DirectX::XMFLOAT3& pos);
    virtual void Deactivate();
    bool IsActive() const { return m_IsActive; }

    // --- Main Logic ---
    virtual void Update(double dt);
    virtual void Draw();

    // --- Getters ---
    AABB GetAABB() const;
    DirectX::XMFLOAT3 GetPosition() const { return m_Position; }

protected:
    bool m_IsActive = false;
    int m_TexID = -1;

    // Transform
    DirectX::XMFLOAT3 m_Position;
    float m_ScaleX;
    float m_ScaleY;
    DirectX::XMFLOAT2 m_Pivot = { 0.5f, 0.5f };
};

#endif // BILLBOARD_OBJECT_H
