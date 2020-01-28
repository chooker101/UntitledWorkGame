#pragma once
#include "Vector3.hpp"

class Matrix4;
class Camera
{
private:
	float mFov; //InDegrees
	float mAspectRatio;
	float mNearPlane;
	float mFarPlane;
	Vector3 mLocalZ;
	Vector3 mLocalX;
	Vector3 mLocalY;
	Vector3 mPosition;
public:
	Camera();
	void MoveLocalZ(float dist);
	void MoveLocalX(float dist);
	void MoveLocalY(float dist);
	void RotateYaw(float degree);
	void RotatePitch(float degree);

	void SetPosition(const Vector3& pos);
	void SetLookAt(const Vector3& target);
	void SetFov(float fov) { mFov = fov; }

	const Vector3& GetPosition() const { return mPosition; }

	Matrix4 GetView() const;
	Matrix4 GetProjection() const;
};