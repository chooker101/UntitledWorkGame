// UntitledWorkGame.cpp : Defines the entry point for the application.
//
#include <Windows.h>
#include <includes.hpp>
#include <Halcyonic.hpp>

struct Vertex
{
	struct Position
	{
		float x = 0.0f, y = 0.0f, z = 0.0f;
	} position;
	Vertex(float inx, float iny, float inz) : position.x(inx), position.y(iny), position.z(inz) {}
};

HWND mWindow;

void HandleWinMessages(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_CLOSE:
		DestroyWindow(hWnd);
		PostQuitMessage(0);
		break;
	case WM_PAINT:
		ValidateRect(mWindow, NULL);
		break;
	case WM_KEYDOWN:
		break;
	case WM_KEYUP:
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
		break;
	case WM_ENTERSIZEMOVE:
		break;
	case WM_EXITSIZEMOVE:
		break;
	}
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	HandleWinMessages(hWnd, uMsg, wParam, lParam);
	return (DefWindowProc(hWnd, uMsg, wParam, lParam));
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR pCmdLine, int nCmdShow)
{
	uint32_t width = 1600;
	uint32_t height = 900;
	CoInitialize(nullptr);

	auto mInstance = hInstance;
	std::string mWindowName = "Test";

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
	wndClass.hInstance = hInstance;
	wndClass.hIcon = LoadIcon(nullptr, IDI_APPLICATION);
	wndClass.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wndClass.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	wndClass.lpszMenuName = nullptr;
#ifdef _UNICODE
	wndClass.lpszClassName = wideWindowName.c_str();
#else
	wndClass.lpszClassName = mWindowName.c_str();
#endif // _UNICODE
	wndClass.hIconSm = LoadIcon(nullptr, IDI_APPLICATION);

	if (!RegisterClassEx(&wndClass))
	{
		//std::cout << "Could not register window class!\n"; pass to log
		return 1;
	}

	const int screenWidth = GetSystemMetrics(SM_CXSCREEN);
	const int screenHeight = GetSystemMetrics(SM_CYSCREEN);

	DWORD dwExStyle;
	DWORD dwStyle;


	dwExStyle = WS_EX_APPWINDOW | WS_EX_WINDOWEDGE;
	dwStyle = WS_OVERLAPPEDWINDOW | WS_CLIPSIBLINGS | WS_CLIPCHILDREN;


	RECT windowRect = { 0, 0, (long)width,  (long)height };

	auto mWindowWidth = (uint32_t)windowRect.right;
	auto mWindowHeight = (uint32_t)windowRect.bottom;

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
		mWindowName.c_str(),
		mWindowName.c_str(),
		dwStyle | WS_CLIPSIBLINGS | WS_CLIPCHILDREN,
		0,
		0,
		windowRect.right - windowRect.left,
		windowRect.bottom - windowRect.top,
		nullptr,
		nullptr,
		hInstance,
		nullptr);
#endif

	if (!mWindow)
	{
		//printf("Could not create window!\n"); //throw exception
		assert(mWindow);
		return 1;
	}


	// Center on screen
	uint32_t x = (GetSystemMetrics(SM_CXSCREEN) - windowRect.right) / 2;
	uint32_t y = (GetSystemMetrics(SM_CYSCREEN) - windowRect.bottom) / 2;
	SetWindowPos(mWindow, 0, x, y, 0, 0, SWP_NOZORDER | SWP_NOSIZE);

	POINT mStartPoint;
	mStartPoint.x = static_cast<LONG>((float)(width * 0.5f) + 0.5f);
	mStartPoint.y = static_cast<LONG>((float)(height * 0.5f) + 0.5f);

	ShowWindow(mWindow, SW_SHOW);

	hal::Render::CreateInstance();
	hal::RenderLayout renderLayout = hal::RenderLayout();
	hal::Render::Instance()->SetRenderLayout(&renderLayout);
	hal::SwapchainColourPlaceholder swapchainPlaceholder = hal::SwapchainColourPlaceholder(hal::Render::Instance()->GetColourFormat());
	hal::DepthStencilLayout depthStencilLayout = hal::DepthStencilLayout();
	std::vector<hal::AttachmentLayout*> attachments = { &swapchainPlaceholder, &depthStencilLayout };
	hal::DepthStencil depthStencil = hal::DepthStencil(&depthStencilLayout);
	hal::RenderPassLayout renderPassLayout = hal::RenderPassLayout(attachments);
	hal::RenderPass renderPass = hal::RenderPass(&renderPassLayout);

	hal::DescriptorSet uniform = hal::DescriptorSet(hal::ShaderStage::Vertex, hal::LayoutBindingDescriptor::UniformBuffer, 0);
	Vertex constantColour = Vertex(0.0f, 1.0f, 0.0f);
	hal::Buffer uniformBuffer = hal::Buffer(sizeof(Vertex), reinterpret_cast<uint8_t*>(&constantColour), hal::BufferType::UniformBuffer);
	std::vector<const hal::DescriptorSet*> descriptors = { &uniform };
	hal::InputAttributes inputAttributes = hal::InputAttributes(VK_FORMAT_R32G32B32_SFLOAT, hal::InputAttributes::CalculateOffset<Vertex, Vertex::Position>());

	std::vector<const hal::InputAttributes *> inputVector = { &inputAttributes };
	hal::ShaderInputLayout shaderInputLayout = hal::ShaderInputLayout(inputVector, sizeof(Vertex), descriptors);
	std::vector<const hal::ShaderInfo*> shaderInfos = { new hal::ShaderInfo{hal::ShaderStage::Vertex, std::string("./vertex.spirv")}, new hal::ShaderInfo{hal::ShaderStage::Fragment,  std::string("./frag.spirv")} };
	hal::PipelineLayout pipelineLayout = hal::PipelineLayout(shaderInfos, &shaderInputLayout);
	hal::Pipeline pipeline = hal::Pipeline(pipelineLayout);
	hal::DrawInfo drawInfo = hal::DrawInfo(&pipeline);
	std::vector<const hal::DrawInfo*> drawInfos = {};
	hal::DrawBuffer drawBuffer = hal::DrawBuffer();
	drawBuffer.StartDrawBuffer();
	drawBuffer.RecordVulkanCommands(vkCmdBindDescriptorSets, VK_PIPELINE_BIND_POINT_GRAPHICS, mPipeline->mVulkanPipelineLayout, 0, 1, &mPipeline->mVulkanDescriptorSet, 0, nullptr);

	// Bind the rendering pipeline
	// The pipeline (state object) contains all states of the rendering pipeline, binding it will set all the states specified at pipeline creation time
	drawBuffer.RecordVulkanCommands(vkCmdBindPipeline, VK_PIPELINE_BIND_POINT_GRAPHICS, mPipeline->mVulkanPipeline);

	// Bind vertex buffer (contains position and colors)
	VkDeviceSize offsets[1] = { 0 };
	drawBuffer.RecordVulkanCommands(vkCmdBindVertexBuffers, 0, 1, hal::DrawCommand::GetBufferPlaceholder(hal::BufferType::VertexBuffer, 0), offsets);

	// Bind index buffer
	drawBuffer.RecordVulkanCommands(vkCmdBindIndexBuffer, hal::DrawCommand::GetBufferPlaceholder(hal::BufferType::IndexBuffer, 0), 0, VK_INDEX_TYPE_UINT32);

	// Draw indexed
	drawBuffer.RecordVulkanCommands(vkCmdDrawIndexed, hal::DrawCommand::GetBufferPlaceholderLength(hal::BufferType::IndexBuffer, 0), 1, 0, 0, 0);

	std::vector<hal::DrawBuffer*> drawBuffers = {};
	hal::RenderInfo renderInfo = hal::RenderInfo(drawBuffers);

	return 0;
}