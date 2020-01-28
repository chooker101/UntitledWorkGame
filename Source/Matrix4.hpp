#pragma once

//Old matrix class
class Vector3;
class Vector4;
class Matrix4
{
public:
	union //Use variant
	{
		struct
		{
			struct //Col 1
			{
				float m11, m12, m13, m14;
			};
			struct //Col 2
			{
				float m21, m22, m23, m24;
			};
			struct //Col 3
			{
				float m31, m32, m33, m34;
			};
			struct //Col 4
			{
				float m41, m42, m43, m44;
			};
		};
		float matrix[4][4];
	};
	Matrix4();
	Matrix4(
		float _11, float _21, float _31, float _41,
		float _12, float _22, float _32, float _42,
		float _13, float _23, float _33, float _43,
		float _14, float _24, float _34, float _44);
	float Determinant();
	Matrix4 Adjugate();
	Matrix4 Inverse();
	Matrix4 Transpose();

	float* operator[](uint32_t i);
	const float* operator[](uint32_t i) const;
	Vector4 operator*(const Vector4& vec) const;
	Vector3 operator*(const Vector3& vec) const;
	Matrix4 operator*(float fl);
	Matrix4 operator*(const Matrix4& b);
};