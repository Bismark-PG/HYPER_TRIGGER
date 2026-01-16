/*==============================================================================

	Make Window [Game_Window.cpp]

	Author : Choi HyungJoon

==============================================================================*/
#include "Game_Window.h"
#include "direct3d.h"
#include "KeyLogger.h"
#include "mouse.h"
#include <algorithm>
#include "imgui.h"
#include "imgui_impl_win32.h"
#include "Game_Screen_Manager.h"

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

Window_Manager* Window_Manager::GetInstance()
{
	static Window_Manager instance;
	return &instance;
}

HWND Window_Manager::Init(HINSTANCE hInstance, int nCmdShow)
{
	m_hInstance = hInstance;

	WNDCLASSEX wcex{};
	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.lpfnWndProc = WndProc;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(hInstance, IDI_APPLICATION);
	wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = nullptr;
	wcex.lpszClassName = WINDOW_CLASS;
	wcex.hIconSm = LoadIcon(wcex.hInstance, IDI_APPLICATION);

	if (!RegisterClassEx(&wcex))
	{
#if defined(DEBUG) || defined(_DEBUG)
		MessageBox(nullptr, "Window Class Registration Failed!", "Error", MB_OK | MB_ICONERROR);
#else
		MessageBox(nullptr, L"Window Class Registration Failed!", L"Error", MB_OK | MB_ICONERROR);
#endif
		return nullptr;
	}

	RECT WindowRect = { 0, 0, (LONG)m_ScreenWidth, (LONG)m_ScreenHeight };
	DWORD WinStyle = WS_OVERLAPPEDWINDOW ^ (WS_MAXIMIZEBOX | WS_THICKFRAME);

	AdjustWindowRect(&WindowRect, WinStyle, FALSE);

	const int WINDOW_WIDTH = WindowRect.right - WindowRect.left;
	const int WINDOW_HEIGTH = WindowRect.bottom - WindowRect.top;

	int desktop_width = GetSystemMetrics(SM_CXSCREEN);
	int desktop_height = GetSystemMetrics(SM_CYSCREEN);

	const int windowX = std::max((desktop_width - WINDOW_WIDTH) / 2, 0);
	const int windowY = std::max((desktop_height - WINDOW_HEIGTH) / 2, 0);

	m_hWnd = CreateWindow(WINDOW_CLASS,
		TITLE,
		WinStyle,
		windowX,
		windowY,
		WINDOW_WIDTH,
		WINDOW_HEIGTH,
		nullptr, nullptr, hInstance, nullptr);

	if (!m_hWnd)
	{
#if defined(DEBUG) || defined(_DEBUG)
		MessageBox(nullptr, "Window Creation Failed!", "Error", MB_OK | MB_ICONERROR);
#else
		MessageBox(nullptr, L"Window Creation Failed!", L"Error", MB_OK | MB_ICONERROR);
#endif
		return nullptr;
	}

	ShowWindow(m_hWnd, nCmdShow);
	UpdateWindow(m_hWnd);

	return m_hWnd;
}

void Window_Manager::Final()
{
	if (m_hInstance)
	{
		UnregisterClass(WINDOW_CLASS, m_hInstance);
		m_hInstance = nullptr;
	}
	m_hWnd = nullptr;
}

void Window_Manager::HandleResize(UINT width, UINT height)
{
	if (width == 0 || height == 0) return;

	m_ScreenWidth = width;
	m_ScreenHeight = height;

	Direct3D_Re_Size(width, height);
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	if (ImGui_ImplWin32_WndProcHandler(hWnd, message, wParam, lParam))
		return true;

	bool Allow_Mouse_Input = true;
	Main_Screen Current_State = Game_Manager::GetInstance()->Get_Current_Main_Screen();

	if (Current_State == Main_Screen::M_WAIT || Current_State == Main_Screen::MAIN)
	{
		Allow_Mouse_Input = false;
	}

	if (Allow_Mouse_Input)
	{
		switch (message)
		{
		case WM_ACTIVATEAPP:
			Mouse_ProcessMessage(message, wParam, lParam);
			break;

		case WM_INPUT:
		case WM_MOUSEMOVE:
		case WM_LBUTTONDOWN:
		case WM_LBUTTONUP:
		case WM_RBUTTONDOWN:
		case WM_RBUTTONUP:
		case WM_MBUTTONDOWN:
		case WM_MBUTTONUP:
		case WM_MOUSEWHEEL:
		case WM_XBUTTONDOWN:
		case WM_XBUTTONUP:
		case WM_MOUSEHOVER:
			if (Allow_Mouse_Input)
				Mouse_ProcessMessage(message, wParam, lParam);
			break;
		}
	}

	switch (message)
	{
    case WM_ACTIVATEAPP:
        Keyboard_ProcessMessage(message, wParam, lParam);
        break;

	case WM_KEYDOWN:
		//if (wParam == VK_ESCAPE)
		//{
		//	SendMessage(hWnd, WM_CLOSE, 0, 0);
		//}
	case WM_SYSKEYDOWN:
	case WM_KEYUP:
	case WM_SYSKEYUP:
		Keyboard_ProcessMessage(message, wParam, lParam);
		break;

	case WM_SIZE:
		if (wParam != SIZE_MINIMIZED)
		{
			UINT Width = LOWORD(lParam);
			UINT Height = HIWORD(lParam);

			Window_Manager::GetInstance()->HandleResize(Width, Height);
		}
		break;

	case WM_CLOSE:
		Window_Manager::GetInstance()->SetMessageBoxActive(true);

#if defined(DEBUG) || defined(_DEBUG)
		if (MessageBox(hWnd, "Really Want Exit Game?", "WARNING", MB_OKCANCEL) == IDOK)
#else
		if (MessageBox(hWnd, L"Really Want Exit Game?", L"WARNING", MB_OKCANCEL) == IDOK)
#endif
			DestroyWindow(hWnd);

		Window_Manager::GetInstance()->SetMessageBoxActive(false);
		break;

	case WM_DESTROY:
		PostQuitMessage(0);
		break;

	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}