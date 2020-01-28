#pragma once

class Vector3;
class Matrix4;
class Quaternion
{
public:
	union
	{
		struct
		{
			float x;
			float y;
			float z;
			float w;
		};
		float quaternion[4];
	};
	Quaternion() = default;
	Quaternion(float inX, float inY, float inZ, float inW);
	void RotateAngleAxis(float degrees, const Vector3& axis);
	Matrix4 GetMatrix() const;
	float& operator[](uint32_t i);
};