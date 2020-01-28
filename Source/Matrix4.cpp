#include "precompiled.hpp"
#include "Vector3.hpp"
#include "Vector4.hpp"
#include "Matrix4.hpp"

Matrix4::Matrix4() :
	m11(1.0f),
	m12(0.0f),
	m13(0.0f),
	m14(0.0f),
	m21(0.0f),
	m22(1.0f),
	m23(0.0f),
	m24(0.0f),
	m31(0.0f),
	m32(0.0f),
	m33(1.0f),
	m34(0.0f),
	m41(0.0f),
	m42(0.0f),
	m43(0.0f),
	m44(1.0f)
{
}

Matrix4::Matrix4(float _11, float _21, float _31, float _41, float _12, float _22, float _32, float _42, float _13, float _23, float _33, float _43, float _14, float _24, float _34, float _44) :
	m11(_11),
	m12(_12),
	m13(_13),
	m14(_14),
	m21(_21),
	m22(_22),
	m23(_23),
	m24(_24),
	m31(_31),
	m32(_32),
	m33(_33),
	m34(_34),
	m41(_41),
	m42(_42),
	m43(_43),
	m44(_44)
{
}

//m11 m21 m31 m41
//m12 m22 m32 m42
//m13 m23 m33 m43
//m14 m24 m34 m44

float Matrix4::Determinant()
{
	return  (((m11 * (m22 * (m33 * m44 - (m34 * m43)) - m32 * (m23 * m44 - (m24 * m43)) + m42 * (m23 * m34 - (m24 * m33))))
		- (m21 * (m12 * (m33 * m44 - (m34 * m43)) - m32 * (m13 * m44 - (m14 * m43)) + m42 * (m13 * m34 - (m14 * m33)))))
		+ (m31 * (m12 * (m23 * m44 - (m24 * m43)) - m22 * (m13 * m44 - (m14 * m43)) + m42 * (m13 * m24 - (m14 * m23)))))
		- (m41 * (m12 * (m23 * m34 - (m24 * m33)) - m22 * (m13 * m34 - (m14 * m33)) + m32 * (m13 * m24 - (m14 * m23))));
}

Matrix4 Matrix4::Adjugate()
{
	return Matrix4
	(
		(m22 * ((m33 * m44) - (m34 * m43)) - m32 * ((m23 * m44) - (m24 * m43)) + m42 * ((m23 * m34) - (m24 * m33))),
		-(m21 * ((m33 * m44) - (m34 * m43)) - m31 * ((m23 * m44) - (m24 * m43)) + m41 * ((m23 * m34) - (m24 * m33))),
		(m21 * ((m32 * m44) - (m34 * m42)) - m31 * ((m22 * m44) - (m24 * m42)) + m41 * ((m22 * m34) - (m24 * m32))),
		-(m21 * ((m32 * m43) - (m33 * m42)) - m31 * ((m22 * m43) - (m23 * m42)) + m41 * ((m22 * m33) - (m23 * m32))),

		-(m12 * ((m33 * m44) - (m34 * m43)) - m13 * ((m32 * m44) - (m42 * m34)) + m14 * ((m32 * m43) - (m42 * m33))),
		(m11 * ((m33 * m44) - (m34 * m43)) - m31 * ((m13 * m44) - (m14 * m43)) + m41 * ((m13 * m34) - (m14 * m33))),
		-(m11 * ((m32 * m44) - (m34 * m42)) - m31 * ((m12 * m44) - (m14 * m42)) + m41 * ((m12 * m34) - (m14 * m32))),
		(m11 * ((m32 * m43) - (m33 * m42)) - m31 * ((m12 * m43) - (m13 * m42)) + m41 * ((m12 * m33) - (m13 * m32))),

		(m12 * ((m23 * m44) - (m24 * m43)) - m13 * ((m22 * m44) - (m24 * m42)) + m14 * ((m22 * m43) - (m23 * m42))),
		-(m11 * ((m23 * m44) - (m24 * m43)) - m13 * ((m21 * m44) - (m24 * m41)) + m14 * ((m21 * m43) - (m23 * m41))),
		(m11 * ((m22 * m44) - (m24 * m42)) - m21 * ((m12 * m44) - (m14 * m42)) + m41 * ((m12 * m24) - (m14 * m22))),
		-(m11 * ((m22 * m43) - (m23 * m42)) - m12 * ((m21 * m43) - (m23 * m41)) + m13 * ((m21 * m42) - (m22 * m41))),

		-(m12 * ((m23 * m34) - (m24 * m33)) - m13 * ((m22 * m34) - (m24 * m32)) + m14 * ((m22 * m33) - (m23 * m32))),
		(m11 * ((m23 * m34) - (m24 * m33)) - m21 * ((m13 * m34) - (m14 * m33)) + m31 * ((m13 * m24) - (m14 * m23))),
		-(m11 * ((m22 * m34) - (m24 * m32)) - m21 * ((m12 * m34) - (m14 * m32)) + m31 * ((m12 * m24) - (m14 * m22))),
		(m11 * ((m22 * m33) - (m23 * m32)) - m21 * ((m12 * m33) - (m13 * m32)) + m31 * ((m12 * m23) - (m13 * m22)))
	);
}

Matrix4 Matrix4::Inverse()
{
	const float determinant = Determinant();
	assert(determinant != 0);
	return Adjugate() * (1.0f / determinant);
}

Matrix4 Matrix4::Transpose()
{
	return Matrix4
	(
		m11, m12, m13, m14,
		m21, m22, m23, m24,
		m31, m32, m33, m34,
		m41, m42, m43, m44
	);
}

float* Matrix4::operator[](uint32_t i)
{
	assert(i < 4);
	return matrix[i];
}

const float* Matrix4::operator[](uint32_t i) const
{
	assert(i < 4);
	return matrix[i];
}

Vector4 Matrix4::operator*(const Vector4 & vec) const
{
	return Vector4
	(
		vec.x * matrix[0][0] + vec.y * matrix[0][1] + vec.z * matrix[0][2] + vec.w * matrix[0][3],
		vec.x * matrix[1][0] + vec.y * matrix[1][1] + vec.z * matrix[1][2] + vec.w * matrix[1][3],
		vec.x * matrix[2][0] + vec.y * matrix[2][1] + vec.z * matrix[2][2] + vec.w * matrix[2][3],
		vec.x * matrix[3][0] + vec.y * matrix[3][1] + vec.z * matrix[3][2] + vec.w * matrix[3][3]
	);
}

Vector3 Matrix4::operator*(const Vector3 & vec) const
{
	Vector4 tmp = Vector4(vec, 1.0f);
	tmp = this->operator*(tmp);
	return tmp;
}

Matrix4 Matrix4::operator*(float fl)
{
	return Matrix4
	(
		m11 * fl, m21 * fl, m31 * fl, m41 * fl,
		m12 * fl, m22 * fl, m32 * fl, m42 * fl,
		m13 * fl, m23 * fl, m33 * fl, m43 * fl,
		m14 * fl, m24 * fl, m34 * fl, m44 * fl
	);
}

Matrix4 Matrix4::operator*(const Matrix4& b)
{
	return Matrix4
	(
		(m11 * b.m11) + (m21 * b.m12) + (m31 * b.m13) + (m41 * b.m14), //m11
		(m11 * b.m21) + (m21 * b.m22) + (m31 * b.m23) + (m41 * b.m24), //m21
		(m11 * b.m31) + (m21 * b.m32) + (m31 * b.m33) + (m41 * b.m34), //m31
		(m11 * b.m41) + (m21 * b.m42) + (m31 * b.m43) + (m41 * b.m44), //m41

		(m12 * b.m11) + (m22 * b.m12) + (m32 * b.m13) + (m42 * b.m14), //m12
		(m12 * b.m21) + (m22 * b.m22) + (m32 * b.m23) + (m42 * b.m24), //m22
		(m12 * b.m31) + (m22 * b.m32) + (m32 * b.m33) + (m42 * b.m34), //m32
		(m12 * b.m41) + (m22 * b.m42) + (m32 * b.m43) + (m42 * b.m44), //m42

		(m13 * b.m11) + (m23 * b.m12) + (m33 * b.m13) + (m43 * b.m14), //m13
		(m13 * b.m21) + (m23 * b.m22) + (m33 * b.m23) + (m43 * b.m24), //m23
		(m13 * b.m31) + (m23 * b.m32) + (m33 * b.m33) + (m43 * b.m34), //m33
		(m13 * b.m41) + (m23 * b.m42) + (m33 * b.m43) + (m43 * b.m44), //m34

		(m14 * b.m11) + (m24 * b.m12) + (m34 * b.m13) + (m44 * b.m14), //m14
		(m14 * b.m21) + (m24 * b.m22) + (m34 * b.m23) + (m44 * b.m24), //m24
		(m14 * b.m31) + (m24 * b.m32) + (m34 * b.m33) + (m44 * b.m34), //m34
		(m14 * b.m41) + (m24 * b.m42) + (m34 * b.m43) + (m44 * b.m44)  //m44
	);
}
