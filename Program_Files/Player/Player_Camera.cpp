/*==============================================================================

	Make Camera [Camera.h]

	Author : Choi HyungJoon

==============================================================================*/
#include "Player_Camera.h"
#include <DirectXMath.h>
#include "Shader_Manager.h"
#include "direct3d.h"
#include "Player.h"       
#include "KeyLogger.h"
#include <algorithm>
#include <debug_ostream.h>
#include "Resource_Manager.h"

using namespace DirectX;

static float Camera_Yaw = 0.0f;
static float Camera_Pitch = 0.0f;
static float Camera_Near_Z = 0.1f;
static float Camera_Far_z = 100.0f;
static float Camera_FOV = XMConvertToRadians(70.0f);
static XMFLOAT4X4 Camera_View_mtx = {};

static float Camera_Distance = 3.5f;
static float Camera_Height = 2.5f;
static XMFLOAT3 Camera_POS   = {};
static XMFLOAT3 Camera_Front = {};
static Player_Sights Current_Sights = {};
static float Camera_Sights_Offset = {};
static float Is_Camera_Sights_Changed = false;

static XMFLOAT3 Current_Camera_Pos = {};
static float Mouse_Sensitivity = 0.01f;

static bool Is_Menu_Mode = false;
static XMFLOAT3 Menu_Cam_Pos = { 0.0f, 0.0f, 0.0f };
static XMFLOAT3 Menu_Cam_Target = { 0.0f, 0.0f, 0.0f };

void Player_Camera_Initialize()
{
    Camera_POS = { 0.0f, 0.0f, 0.0f };
    Camera_Front = { 0.0f, 0.0f, 1.0f };
    Current_Sights = Player_Sights::Right;
    Camera_Sights_Offset = 1.5f;

    Current_Camera_Pos = { 0.0f, 0.0f, 0.0f };
    Mouse_Sensitivity = 0.01f;
}

void Player_Camera_Finalize()
{
}

void Player_Camera_Update(double elapsed_time)
{
    float dt = static_cast<float>(elapsed_time);

    if (Is_Menu_Mode)
    {
        XMVECTOR P = XMLoadFloat3(&Menu_Cam_Pos);
        XMVECTOR T = XMLoadFloat3(&Menu_Cam_Target);
        XMVECTOR U = { 0.0f, 1.0f, 0.0f };

        // Make Matrix View
        XMMATRIX View = XMMatrixLookAtLH(P, T, U);
        XMStoreFloat4x4(&Camera_View_mtx, View);

        // Shader Update
        Shader_Manager::GetInstance()->SetViewMatrix3D(View);

        // Camera POS
        Camera_POS = Menu_Cam_Pos;
        XMStoreFloat3(&Camera_Front, XMVector3Normalize(T - P));

        // Set Shader Projection
        float Aspect = (float)Direct3D_GetBackBufferWidth() / (float)Direct3D_GetBackBufferHeight();
        XMMATRIX Proj = XMMatrixPerspectiveFovLH(Camera_FOV, Aspect, Camera_Near_Z, Camera_Far_z);
        Shader_Manager::GetInstance()->SetProjectionMatrix3D(Proj);

        return;
    }

    if (Is_Camera_Sights_Changed)
    {
        Player_Camera_Update_Sights();
        Is_Camera_Sights_Changed = false;
    }

    // Get Mouse Movement
    float mouseMoveX = KeyLogger_GetMouse_MoveX() * Mouse_Sensitivity;
    float mouseMoveY = KeyLogger_GetMouse_MoveY() * Mouse_Sensitivity;

    // Add Angle
    Camera_Yaw += mouseMoveX;
    Camera_Pitch += mouseMoveY;

    // Limited Pitch 
    Camera_Pitch = std::max(-XM_PIDIV2 * 0.8f, std::min(Camera_Pitch, XM_PIDIV2 * 0.8f)); // -80 ~ +80

    // Make Rotation
    XMMATRIX rotation = XMMatrixRotationRollPitchYaw(Camera_Pitch, Camera_Yaw, 0);

    // Get Player POS
    XMVECTOR playerPos = XMLoadFloat3(&Player_Get_POS());

    // Set Camera FOV, POS
    XMVECTOR baseTargetPos = playerPos + XMVectorSet(0.0f, Camera_Height, 0.0f, 0.0f);
    XMVECTOR offset = XMVectorSet(0.0f, 0.0f, -Camera_Distance, 0.0f);
    XMVECTOR baseCameraPos = XMVector3Transform(offset, rotation) + baseTargetPos;

    // Get Camera Right Vector
    XMMATRIX yawRotation = XMMatrixRotationY(Camera_Yaw);
    XMVECTOR horizontalRightVec = XMVector3Transform(XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f), yawRotation);

    // Get Final Camera POS
    XMVECTOR shiftVector = horizontalRightVec * Camera_Sights_Offset;

    XMVECTOR finalCameraPos = baseCameraPos + shiftVector;
    XMVECTOR finalTargetPos = baseTargetPos + shiftVector;

    // Make View Matrix
    XMMATRIX mtxView = XMMatrixLookAtLH(finalCameraPos, finalTargetPos, { 0.0f,1.0f,0.0f });
    XMStoreFloat4x4(&Camera_View_mtx, mtxView);
    Shader_Manager::GetInstance()->SetViewMatrix3D(mtxView);
    XMStoreFloat3(&Current_Camera_Pos, finalCameraPos);

    // Get Camera Front Vector
    XMVECTOR front = XMVector3Transform(XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f), rotation);
    XMStoreFloat3(&Camera_Front, front);

    //--- Projection Matrix ---
	float Ratio = static_cast<float>(Direct3D_GetBackBufferWidth()) / Direct3D_GetBackBufferHeight();
	XMMATRIX mtxPerspective = XMMatrixPerspectiveFovLH(Camera_FOV, Ratio, Camera_Near_Z, Camera_Far_z);

    Shader_Manager::GetInstance()->SetProjectionMatrix3D(mtxPerspective);
}

void Player_Camera_Set_Menu_Mode(const XMFLOAT3& pos, const XMFLOAT3& target)
{
    Is_Menu_Mode = true;
    Menu_Cam_Pos = pos;
    Menu_Cam_Target = target;

    Camera_POS = Menu_Cam_Pos;
}

void Player_Camera_Set_Game_Mode()
{
    Is_Menu_Mode = false;
}

const XMFLOAT3& Player_Camera_Get_POS()
{
    return Camera_POS;
}

const XMFLOAT3& Player_Camera_Get_Front()
{
    return Camera_Front;
}

const XMFLOAT3& Player_Camera_Get_Current_POS()
{
    return Current_Camera_Pos;
}
float Player_Camera_Get_Yaw()
{
    return Camera_Yaw;
}
float Player_Camera_Get_Pitch()
{
    return Camera_Pitch;
}

float Player_Camera_Get_Far_Z()
{
    return Camera_Far_z;
}

Player_Sights Player_Camera_Get_Now_Sights()
{
    return Current_Sights;
}

void Player_Camera_Set_Sights(Player_Sights sight)
{
    Current_Sights = sight;

    switch (Current_Sights)
    {
    case Player_Sights::Left:
        Camera_Sights_Offset = -1.5f;
        break;
    case Player_Sights::Middle:
        Camera_Sights_Offset = 0.0f;
        break;
    case Player_Sights::Right:
        Camera_Sights_Offset = 1.5f;
        break;
    }
}

void Player_Camera_Update_Sights()
{
    Camera_Sights_Offset *= -1.0f;

    if (Camera_Sights_Offset > 0.0f)
        Current_Sights = Player_Sights::Left;
    else if (Camera_Sights_Offset < 0.0f)
        Current_Sights = Player_Sights::Right;
}

void Player_Camera_Change_Sights()
{
    Is_Camera_Sights_Changed = true;
}

DirectX::XMFLOAT4X4& PlayerCamera_GetViewMatrix()
{
    return Camera_View_mtx;
}

void Set_Mouse_Sensitivity(float Sensitivity)
{
    Mouse_Sensitivity = Sensitivity;
}

float Get_Mouse_Sensitivity()
{
    return Mouse_Sensitivity;
}
