#pragma once

class Vector4;
class Vector3
{
public:
	union //Change to variant
	{
		struct
		{
			float x;
			float y;
			float z;
		};
		float vector[3];
	};
	Vector3() : x(0.0f), y(0.0f), z(0.0f) {}
	Vector3(float inX, float inY, float inZ);
	Vector3(const Vector4& inVec);

	float& operator[](uint32_t i);
	Vector3 operator*(const float b) const;
	Vector3 operator+(const Vector3& b) const;
	Vector3 operator-(const Vector3& b) const;
	void operator=(const Vector4& b);

	float Magnitude() const;
	Vector3 Cross(const Vector3& b) const;
	float Dot(const Vector3& b) const;
	const Vector3 GetNormal();
	const Vector3 GetNormal() const;
};