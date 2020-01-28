#include "precompiled.hpp"
#include "Matrix4.hpp"
#include "Vector3.hpp"
#include "Vector4.hpp"
#include "Quaternion.hpp"
#include "Application.hpp"
#include "MathConstants.hpp"
#include "Camera.hpp"

Camera::Camera()
	: mFov(50.0f),
	mNearPlane(0.1f),
	mFarPlane(1000.f)
{
	mAspectRatio = (float)Application::Instance()->GetWidth() / (float)Application::Instance()->GetHeight();
	mPosition = Vector3(0.0f, 0.0f, 5.0f);
	mLocalZ = (mPosition - Vector3(0.0f, 0.0f, 0.0f)).GetNormal();
	mLocalX = up.Cross(mLocalZ);
	mLocalY = mLocalZ.Cross(mLocalX);
}

void Camera::MoveLocalZ(float dist)
{
	mPosition = mPosition + (mLocalZ * dist);
}

void Camera::MoveLocalX(float dist)
{
	mPosition = mPosition + (mLocalX * dist);
}

void Camera::MoveLocalY(float dist)
{
	mPosition = mPosition + (mLocalY * dist);
}

void Camera::RotateYaw(float degree)
{
	if (degree != 0.0f)
	{
		Quaternion rotate = {};
		rotate.RotateAngleAxis(degree, up);
		Vector4 vec = Vector4(mLocalZ, 1.0f);
		mLocalZ = rotate.GetMatrix() * vec;
		mLocalZ = mLocalZ.GetNormal();
		mLocalX = up.Cross(mLocalZ).GetNormal();
		mLocalY = mLocalZ.Cross(mLocalX).GetNormal();
	}
}

void Camera::RotatePitch(float degree)
{
	if (degree != 0.0f)
	{
		Quaternion rotate = {};
		rotate.RotateAngleAxis(-degree, mLocalX);
		Vector4 vec = Vector4(mLocalZ, 1.0f);
		Vector3 newLook = {};
		newLook = rotate.GetMatrix() * vec;
		newLook = newLook.GetNormal();
		float dot = newLook.Dot(up);

		if (abs(dot) < 0.999f)
		{
			mLocalZ = newLook;
			mLocalX = up.Cross(mLocalZ).GetNormal();
			mLocalY = mLocalZ.Cross(mLocalX).GetNormal();
		}
	}
}

void Camera::SetPosition(const Vector3 & pos)
{
	mPosition = pos;
}

void Camera::SetLookAt(const Vector3 & target)
{
	mLocalZ = (mPosition - target).GetNormal();
	mLocalX = up.Cross(mLocalZ);
	mLocalY = mLocalZ.Cross(mLocalX);
}

Matrix4 Camera::GetView() const
{
	const float dx = -mLocalX.Dot(mPosition);
	const float dy = -mLocalY.Dot(mPosition);
	const float dz = -mLocalZ.Dot(mPosition);
	return Matrix4
	(
		mLocalX.x, mLocalX.y, mLocalX.z, dx,
		mLocalY.x, mLocalY.y, mLocalY.z, dy,
		mLocalZ.x, mLocalZ.y, mLocalZ.z, dz,
		0.0f, 0.0f, 0.0f, 1.0f
	);
}

Matrix4 Camera::GetProjection() const
{
	const float halffovtan = tan((mFov * piOver180) * 0.5f);
	const float h = 1 / halffovtan;
	const float w = 1 / (halffovtan * mAspectRatio);
	const float fp = mFarPlane;
	const float np = mNearPlane;

	return Matrix4
	(
		w, 0.0f, 0.0f, 0.0f,
		0.0f, h, 0.0f, 0.0f,
		0.0f, 0.0f, fp / (np - fp), -(fp * np) / (fp - np),
		0.0f, 0.0f, -1.0f, 0.0f
	);
}