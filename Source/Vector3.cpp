#include "precompiled.hpp"
#include "Vector4.hpp"
#include "Vector3.hpp"

Vector3::Vector3(float inX, float inY, float inZ) : x(inX), y(inY), z(inZ)
{
}

Vector3::Vector3(const Vector4 & inVec) : x(inVec.x), y(inVec.y), z(inVec.z)
{
}

float& Vector3::operator[](uint32_t i)
{
	assert(i < 3); //Todo: add debug log and skip on release build
	return vector[i];
}

Vector3 Vector3::operator*(const float b) const
{
	return Vector3(x * b, y * b, z * b);
}

Vector3 Vector3::operator+(const Vector3 & b) const
{
	return Vector3(x + b.x, y + b.y, z + b.z);
}

Vector3 Vector3::operator-(const Vector3 & b) const
{
	return Vector3(x - b.x, y - b.y, z - b.z);
}

void Vector3::operator=(const Vector4 & b)
{
	x = b.x;
	y = b.y;
	z = b.z;
}

float Vector3::Magnitude() const
{
	assert(x + y + z != 0.0f);
	return sqrt((x * x) + (y * y) + (z * z));
}

Vector3 Vector3::Cross(const Vector3 & b) const
{
	return Vector3
	(
		(y * b.z) - (z * b.y),
		(z * b.x) - (x * b.z),
		(x * b.y) - (y * b.x)
	);
}

float Vector3::Dot(const Vector3 & b) const
{
	return (x * b.x) + (y * b.y) + (z * b.z);
}

const Vector3 Vector3::GetNormal()
{
	const float dec = 1.0f / Magnitude();
	return this->operator*(dec);
}

const Vector3 Vector3::GetNormal() const
{
	const float dec = 1.0f / Magnitude();
	return this->operator*(dec);
}