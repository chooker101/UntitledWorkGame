#pragma once
#include "Quaternion.hpp"

class Matrix4;
class Vector3;
class Graphics;
struct RenderVertex;
class RenderObject
{
private:
	Vector3 mPosition = Vector3(0.0f, 0.0f, 0.0f);
	Vector3 mScale = Vector3(1.0f, 1.0f, 1.0f);
	Quaternion mXRotation;
	Quaternion mYRotation; //OOF

	float currentAngleX = 45.0f;
	float currentAngleY = 45.0f;

	std::vector<RenderVertex> mRawVertexBuffer;
	std::vector<uint32_t> mRawIndexBuffer;
	hal::Buffer mVertexBuffer;
	hal::Buffer mIndexBuffer;
	hal::DrawInfo mDrawInfo;
	hal::DrawBuffer mDrawBuffer;// Man this really needs const correctness
public:
	RenderObject(std::vector<RenderVertex> vertexBuffer, std::vector<uint32_t> indexBuffer);

	const hal::DrawInfo& GetDrawInfo() const { return mDrawInfo; }
	hal::DrawBuffer* GetDrawBuffer() { return &mDrawBuffer; }
	Matrix4 GetModelMatrix() const;

	void SetPosition(Vector3 position) { mPosition = std::move(position); }

	void Update();
};