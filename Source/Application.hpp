#pragma once

class Application;
typedef std::unique_ptr<Application> application_ptr;
class Application
{
private:
	std::string mWindowName;
	static application_ptr s_Instance;

	HINSTANCE mInstance;
	HWND mWindow;
	MSG msg;
	POINT mCurrPoint;
	POINT mStartPoint;

	uint32_t mWindowWidth;
	uint32_t mWindowHeight;

	bool mMouseVisible = true;
	bool mInputOn = true;

	bool mEnableVSync;
	bool mEnableValidation;

	bool windowPrepared;
	bool windowResizing;

	bool isRunning = false;
	bool isFocus = false;

	Application() = default;
	inline bool IsWindowForeground();
public:
	static void CreateInstance();
	static const application_ptr& Instance();
	static void DestroyInstance();

	void HandleWinMessages(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	void InitializeWindow(const std::string windowName, HINSTANCE hinstance, uint32_t width, uint32_t height, bool fullscreen);

	void WindowResize(uint32_t targetWidth, uint32_t targetHeight);

	uint32_t GetWidth() { return mWindowWidth; }
	uint32_t GetHeight() { return mWindowHeight; }

	const HINSTANCE& GetInstanceHandle() { return mInstance; }
	const HWND& GetWindowHandle() { return mWindow; }

	void Update();

	bool IsApplicationRunning() { return isRunning; }
};