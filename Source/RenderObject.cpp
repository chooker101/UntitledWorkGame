#include "precompiled.hpp"
#include "Vector3.hpp"
#include "Matrix4.hpp"
#include "MathConstants.hpp"
#include "RenderVertex.hpp"
#include "Graphics.hpp"
#include "RenderObject.hpp"

RenderObject::RenderObject(std::vector<RenderVertex> vertexBuffer, std::vector<uint32_t> indexBuffer) :
	mRawVertexBuffer(std::move(vertexBuffer)),
	mRawIndexBuffer(std::move(indexBuffer)),
	mVertexBuffer(static_cast<uint32_t>(mRawVertexBuffer.size() * sizeof(RenderVertex)), reinterpret_cast<uint8_t*>(mRawVertexBuffer.data()), hal::BufferType::VertexBuffer),
	mIndexBuffer(static_cast<uint32_t>(mRawIndexBuffer.size() * sizeof(uint32_t)), reinterpret_cast<uint8_t*>(mRawIndexBuffer.data()), hal::BufferType::IndexBuffer),
	mDrawInfo(Graphics::Instance()->GetPipeline()),
	mDrawBuffer(Graphics::Instance()->GetCommandPool(), { &mDrawInfo })
{
	mDrawInfo.AddBuffer(&mVertexBuffer); //Make a way to pass to constructor
	mDrawInfo.AddBuffer(&mIndexBuffer);

	Graphics::Instance()->AddRenderObject(this);
	Graphics::Instance()->RebuildRenderInfo();
}

Matrix4 RenderObject::GetModelMatrix() const
{
	return (mXRotation.GetMatrix() * mYRotation.GetMatrix()) *
		Matrix4
		(
			mScale.x, 0.0f, 0.0f, mPosition.x,
			0.0f, mScale.y, 0.0f, mPosition.z, //I believe this is because Z is "up"
			0.0f, 0.0f, mScale.z, mPosition.y,
			0.0f, 0.0f, 0.0f, 1.0f
		);
}

void RenderObject::Update()
{
	currentAngleX += 1.0f;
	currentAngleX = fmod(currentAngleX, 360.0f);
	currentAngleY += 1.0f;
	currentAngleY = fmod(currentAngleY, 360.0f);
	mXRotation.RotateAngleAxis(currentAngleX, up);
	mYRotation.RotateAngleAxis(currentAngleY, right); 
	mDrawInfo.UpdateBuffer(hal::BufferType::VertexBuffer, 0, reinterpret_cast<uint8_t*>(mRawVertexBuffer.data()));
	mDrawInfo.UpdateBuffer(hal::BufferType::IndexBuffer, 0, reinterpret_cast<uint8_t*>(mRawIndexBuffer.data()));
}
