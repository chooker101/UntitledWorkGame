// UntitledWorkGame.cpp : Defines the entry point for the application.
//
#include "precompiled.hpp"
#include <includes.hpp>
#include <halcyonic_renderer.hpp>
#include "Vector3.hpp"
#include "Application.hpp"
#include "Camera.hpp"
#include "Graphics.hpp"
#include "RenderVertex.hpp"
#include "RenderObject.hpp"

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR pCmdLine, int nCmdShow)
{
	Application::CreateInstance();
	Application::Instance()->InitializeWindow("Untitled", hInstance, 1920, 1080, false);
	Graphics::CreateInstance();
	Camera mainCamera = Camera();
	Graphics::Instance()->SetMainCamera(&mainCamera);

	std::vector<RenderVertex> vertexBuffer = {
		RenderVertex(Vector3(-1.0f, -1.0f, 1.0f), Vector3(132.0f / 255.0f, 192.0f / 255.0f, 122.0f / 255.0f)),		//0 FBL
		RenderVertex(Vector3(-1.0f, 1.0f, 1.0f), Vector3(36.0f / 255.0f, 65.0f / 255.0f, 233.0f / 255.0f)),			//1 FTL
		RenderVertex(Vector3(1.0f, 1.0f, 1.0f), Vector3(99.0f / 255.0f, 137.0f / 255.0f, 199.0f / 255.0f)),			//2 FTR
		RenderVertex(Vector3(1.0f, -1.0f, 1.0f), Vector3(159.0f / 255.0f, 229.0f / 255.0f, 221.0f / 255.0f)),		//3 FBR
		RenderVertex(Vector3(-1.0f, -1.0f, -1.0f), Vector3(222.0f / 255.0f, 196.0f / 255.0f, 1.0f / 255.0f)),		//4 BBL
		RenderVertex(Vector3(-1.0f, 1.0f, -1.0f), Vector3(203.0f / 255.0f, 143.0f / 255.0f, 156.0f / 255.0f)),		//5 BTL
		RenderVertex(Vector3(1.0f, 1.0f, -1.0f), Vector3(3.0f / 255.0f, 72.0f / 255.0f, 61.0f / 255.0f)),			//6 BTR
		RenderVertex(Vector3(1.0f, -1.0f, -1.0f), Vector3(33.0f / 255.0f, 140.0f / 255.0f, 14.0f / 255.0f))			//7 BBR
	};
	std::vector<uint32_t> indexBuffer = {
		2, 1, 0, 3, 2, 0,
		6, 2, 3, 7, 6, 3,
		5, 6, 7, 4, 5, 7,
		1, 5, 4, 0, 1, 4,
		6, 5, 1, 2, 6, 1,
		3, 0, 4, 7, 3, 4
	};
	RenderObject cube = RenderObject(vertexBuffer, indexBuffer);
	
	while (Application::Instance()->IsApplicationRunning())
	{
		Application::Instance()->Update();
		cube.Update();
		Graphics::Instance()->Draw();
	}
	return 0;
}