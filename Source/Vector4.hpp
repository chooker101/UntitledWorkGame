#pragma once

class Vector3;
class Vector4 //OOF
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
		float vector[4];
	};
	Vector4() {}
	Vector4(float inX, float inY, float inZ, float inW);
	Vector4(const Vector3& vec, float inW);
	float& operator[](uint32_t i);
};