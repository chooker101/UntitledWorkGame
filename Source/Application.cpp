#include "precompiled.hpp"
#include "Keys.hpp"
#include "Camera.hpp"
#include "Graphics.hpp"
#include "Application.hpp"

application_ptr Application::s_Instance = nullptr;

LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	Application::Instance()->HandleWinMessages(hWnd, uMsg, wParam, lParam);
	return (DefWindowProc(hWnd, uMsg, wParam, lParam));
}

inline bool Application::IsWindowForeground()
{
	return (GetForegroundWindow() == mWindow);
}

void Application::CreateInstance()
{
	if (s_Instance == nullptr)
	{
		s_Instance = static_cast<application_ptr>(new Application());
	}
}

const application_ptr& Application::Instance()
{
	assert(s_Instance != nullptr);
	return s_Instance;
}

void Application::DestroyInstance()
{
	s_Instance.release();
	s_Instance = nullptr;
}

void Application::HandleWinMessages(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_CLOSE:
		isRunning = false;
		windowPrepared = false;
		DestroyWindow(hWnd);
		PostQuitMessage(0);
		break;
	case WM_PAINT:
		ValidateRect(mWindow, NULL);
		break;
	case WM_KEYDOWN:
		switch (wParam)
		{
		case VK_ESCAPE:
		{
			isRunning = false;
			windowPrepared = false;
			DestroyWindow(hWnd);
			PostQuitMessage(0);
			break;
		}
		case KEYS::W:
		{
			Graphics::Instance()->GetMainCamera().MoveLocalZ(-0.2f);
			break;
		}
		case KEYS::S:
		{
			Graphics::Instance()->GetMainCamera().MoveLocalZ(0.2f);
			break;
		}
		case KEYS::A:
		{
			Graphics::Instance()->GetMainCamera().MoveLocalX(-0.2f);
			break;
		}
		case KEYS::D:
		{
			Graphics::Instance()->GetMainCamera().MoveLocalX(0.2f);
			break;
		}
		case KEYS::Q:
		{
			Graphics::Instance()->GetMainCamera().MoveLocalY(0.2f);
			break;
		}
		case KEYS::E:
		{
			Graphics::Instance()->GetMainCamera().MoveLocalY(-0.2f);
			break;
		}
		}
		break;
	case WM_KEYUP:
		switch (wParam)
		{
		case KEYS::W:
		{
			//Input::Instance()->EndCallInputFunctions(KEYS::W);
			break;
		}
		case KEYS::S:
		{
			//Input::Instance()->EndCallInputFunctions(KEYS::S);
			break;
		}
		case KEYS::A:
		{
			//Input::Instance()->EndCallInputFunctions(KEYS::A);
			break;
		}
		case KEYS::D:
		{
			//Input::Instance()->EndCallInputFunctions(KEYS::D);
			break;
		}
		case KEYS::Q:
		{
			//Input::Instance()->EndCallInputFunctions(KEYS::E);
			break;
		}
		case KEYS::E:
		{
			//Input::Instance()->EndCallInputFunctions(KEYS::E);
			break;
		}
		}
		break;
	case WM_RBUTTONDOWN:
	case WM_LBUTTONDOWN:
	case WM_MBUTTONDOWN:
	case WM_MOUSEWHEEL:
	{
		break;
	}
	case WM_MOUSEMOVE:
		break;
	case WM_SIZE:
		if ((windowPrepared) && (wParam != SIZE_MINIMIZED))
		{
			if ((windowResizing) || ((wParam == SIZE_MAXIMIZED) || (wParam == SIZE_RESTORED)))
			{
				WindowResize(LOWORD(lParam), HIWORD(lParam));
			}
		}
		break;
	case WM_ENTERSIZEMOVE:
		windowResizing = true;
		break;
	case WM_EXITSIZEMOVE:
		windowResizing = false;
		break;
	}
}

void Application::InitializeWindow(const std::string windowName, HINSTANCE hinstance, uint32_t width, uint32_t height, bool fullscreen)
{
	CoInitialize(nullptr);

	mInstance = hinstance;
	mWindowName = windowName;

#ifdef _UNICODE
	std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
	std::wstring wideWindowName = converter.from_bytes(windowName);

	WNDCLASSEXW wndClass;
	wndClass.cbSize = sizeof(WNDCLASSEXW);
#else
	WNDCLASSEXA wndClass;
	wndClass.cbSize = sizeof(WNDCLASSEXA);
#endif // _UNICODE
	wndClass.style = CS_HREDRAW | CS_VREDRAW;
	wndClass.lpfnWndProc = WndProc;
	wndClass.cbClsExtra = 0;
	wndClass.cbWndExtra = 0;
	wndClass.hInstance = hinstance;
	wndClass.hIcon = LoadIcon(nullptr, IDI_APPLICATION);
	wndClass.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wndClass.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	wndClass.lpszMenuName = nullptr;
#ifdef _UNICODE
	wndClass.lpszClassName = wideWindowName.c_str();
#else
	wndClass.lpszClassName = windowName.c_str();
#endif // _UNICODE
	wndClass.hIconSm = LoadIcon(nullptr, IDI_APPLICATION);

	if (!RegisterClassEx(&wndClass))
	{
		//std::cout << "Could not register window class!\n"; pass to log
		return;
	}

	const int screenWidth = GetSystemMetrics(SM_CXSCREEN);
	const int screenHeight = GetSystemMetrics(SM_CYSCREEN);

	if (fullscreen)
	{
		DEVMODE dmScreenSettings;
		memset(&dmScreenSettings, 0, sizeof(dmScreenSettings));
		dmScreenSettings.dmSize = sizeof(dmScreenSettings);
		dmScreenSettings.dmPelsWidth = screenWidth;
		dmScreenSettings.dmPelsHeight = screenHeight;
		dmScreenSettings.dmBitsPerPel = 32;
		dmScreenSettings.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;

		if ((width != screenWidth) && (height != screenHeight))
		{
			if (ChangeDisplaySettings(&dmScreenSettings, CDS_FULLSCREEN) != DISP_CHANGE_SUCCESSFUL)
			{
				if (MessageBoxA(NULL, "Fullscreen Mode not supported!\n Switch to window mode?", "Error", MB_YESNO | MB_ICONEXCLAMATION) == IDYES)
				{
					fullscreen = false;
				}
				else
				{
					return;
				}
			}
		}
	}

	DWORD dwExStyle;
	DWORD dwStyle;

	if (fullscreen)
	{
		dwExStyle = WS_EX_APPWINDOW;
		dwStyle = WS_POPUP | WS_CLIPSIBLINGS | WS_CLIPCHILDREN;
	}
	else
	{
		dwExStyle = WS_EX_APPWINDOW | WS_EX_WINDOWEDGE;
		dwStyle = WS_OVERLAPPEDWINDOW | WS_CLIPSIBLINGS | WS_CLIPCHILDREN;
	}

	RECT windowRect = { 0, 0, fullscreen ? (long)screenWidth : (long)width, fullscreen ? (long)screenHeight : (long)height };

	mWindowWidth = (uint32_t)windowRect.right;
	mWindowHeight = (uint32_t)windowRect.bottom;

	AdjustWindowRectEx(&windowRect, dwStyle, FALSE, dwExStyle);
#ifdef _UNICODE
	mWindow = CreateWindowExW(
		0,
		wideWindowName.c_str(),
		wideWindowName.c_str(),
		dwStyle | WS_CLIPSIBLINGS | WS_CLIPCHILDREN,
		0,
		0,
		windowRect.right - windowRect.left,
		windowRect.bottom - windowRect.top,
		nullptr,
		nullptr,
		hinstance,
		nullptr);
#else
	mWindow = CreateWindowExA(
		0,
		windowName.c_str(),
		windowName.c_str(),
		dwStyle | WS_CLIPSIBLINGS | WS_CLIPCHILDREN,
		0,
		0,
		windowRect.right - windowRect.left,
		windowRect.bottom - windowRect.top,
		nullptr,
		nullptr,
		hinstance,
		nullptr);
#endif

	if (!mWindow)
	{
		assert(mWindow);
		return;
	}

	if (!fullscreen)
	{
		// Center on screen
		uint32_t x = (GetSystemMetrics(SM_CXSCREEN) - windowRect.right) / 2;
		uint32_t y = (GetSystemMetrics(SM_CYSCREEN) - windowRect.bottom) / 2;
		SetWindowPos(mWindow, 0, x, y, 0, 0, SWP_NOZORDER | SWP_NOSIZE);
	}

	mStartPoint.x = static_cast<LONG>((float)(width * 0.5f) + 0.5f);
	mStartPoint.y = static_cast<LONG>((float)(height * 0.5f) + 0.5f);

	ShowWindow(mWindow, SW_SHOW);

	isRunning = true;
}

void Application::WindowResize(uint32_t targetWidth, uint32_t targetHeight)
{
}

void Application::Update()
{
	while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	if (msg.message == WM_QUIT)
	{
		isRunning = false;
		return;
	}

	bool lastVisible = mMouseVisible;
	isFocus = IsWindowForeground();
	if (isFocus)
	{
		mMouseVisible = false;

		if (mInputOn)
		{
			POINT lastPoint = mCurrPoint;
			GetCursorPos(&mCurrPoint);
			Graphics::Instance()->GetMainCamera().RotateYaw((static_cast<float>(mCurrPoint.x) - static_cast<float>(lastPoint.x)) * 0.01f);
			Graphics::Instance()->GetMainCamera().RotatePitch((static_cast<float>(mCurrPoint.y) - static_cast<float>(lastPoint.y)) * 0.01f);
			mInputOn = false;
		}
		else
		{
			SetCursorPos(mStartPoint.x, mStartPoint.y);
			GetCursorPos(&mCurrPoint);
			mInputOn = true;
		}
	}
	else
	{
		mMouseVisible = true;
	}

	if (lastVisible != mMouseVisible)
	{
		ShowCursor(mMouseVisible);
	}
}