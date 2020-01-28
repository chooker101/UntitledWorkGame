#include "precompiled.hpp"
#include "Vector3.hpp"
#include "Vector4.hpp"

Vector4::Vector4(float inX, float inY, float inZ, float inW)
{
	vector[0] = inX;
	vector[1] = inY;
	vector[2] = inZ;
	vector[3] = inW;
}

Vector4::Vector4(const Vector3 & vec, float inW)
{
	vector[0] = vec.x;
	vector[1] = vec.y;
	vector[2] = vec.z;
	vector[3] = inW;
}

float & Vector4::operator[](uint32_t i)
{
	return vector[i];
}