#include "precompiled.hpp"
#include "Matrix4.hpp"
#include "Vector3.hpp"
#include "MathConstants.hpp"
#include "Quaternion.hpp"

Quaternion::Quaternion(float inX, float inY, float inZ, float inW) : x(inX), y(inY), z(inZ), w(inW)
{

}

void Quaternion::RotateAngleAxis(float degrees, const Vector3 & axis)
{
	const float c = cos((degrees * piOver180) * 0.5f);
	const float s = sin((degrees * piOver180) * 0.5f);
	const Vector3 a = axis.GetNormal();
	x = a.x * s;
	y = a.y * s;
	z = a.z * s;
	w = c;
}

Matrix4 Quaternion::GetMatrix() const
{
	return Matrix4
	(
		1.0f - (2.0f * y * y) - (2.0f * z * z), (2.0f * x * y) - (2.0f * z * w), (2.0f * x * z) + (2.0f * y * w), 0.0f,

		(2.0f * x * y) + (2.0f * z * w), 1.0f - (2.0f * x * x) - (2.0f * z * z), (2.0f * y * z) - (2.0f * x * w), 0.0f,

		(2.0f * x * z) - (2.0f * y * w), (2.0f * y * z) + (2.0f * x * w), 1.0f - (2.0f * x * x) - (2.0f * y * y), 0.0f,

		0.0f, 0.0f, 0.0f, 1.0f
	);
}

float & Quaternion::operator[](uint32_t i)
{
	return quaternion[i];
}