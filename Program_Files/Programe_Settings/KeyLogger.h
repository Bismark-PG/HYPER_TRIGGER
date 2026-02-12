/*==============================================================================

	Keyboard Input Set [KeyLogger.h]

	Author : Choi HyungJoon

==============================================================================*/

#ifndef KEY_LOGGER_H
#define KEY_LOGGER_H
#include "mouse.h"
#include "keyboard.h"
#include <Xinput.h>
#include <DirectXMath.h>
#include "Frame.h"

enum Mouse_Button
{
    LEFT,
    RIGHT,
    MIDDLE
};

void KeyLogger_Initialize(HWND window);

void KeyLogger_Update();

bool KeyLogger_IsPressed(Keyboard_Keys key);
bool KeyLogger_IsTrigger(Keyboard_Keys key);
bool KeyLogger_IsReleased(Keyboard_Keys key);

bool KeyLogger_IsAnyKeyTriggered();
bool KeyLogger_IsAnyKeyReleased();
bool KeyLogger_IsKeyInput();

int KeyLogger_GetMouse_MoveX();
int KeyLogger_GetMouse_MoveY();

bool KeyLogger_IsMousePressed(Mouse_Button button);
bool KeyLogger_IsMouseTrigger(Mouse_Button button);
bool KeyLogger_IsMouseReleased(Mouse_Button button);

bool XKeyLogger_IsControllerInput();
bool XKeyLogger_GetControllerConnected();
bool XKeyLogger_GetControllerDisconnected();

bool XKeyLogger_IsPadPressed(WORD button);
bool XKeyLogger_IsPadTrigger(WORD button);
bool XKeyLogger_IsPadReleased(WORD button);

DirectX::XMFLOAT2 XKeyLogger_GetLeftStick();
DirectX::XMFLOAT2 XKeyLogger_GetRightStick();
BYTE XKeyLogger_GetLeftTrigger();
BYTE XKeyLogger_GetRightTrigger();

bool XKeyLogger_IsAnyPadReleased();

void KeyLogger_Reset();

#endif // KEY_LOGGER_H