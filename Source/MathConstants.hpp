#pragma once

class Vector3;
constexpr float pi = 3.14159265358979f;
constexpr float tau = 6.28318530717958f;

constexpr float piOver180 = pi / 180.0f;
static const Vector3 up = Vector3(0.0f, 1.0f, 0.0f);
static const Vector3 right = Vector3(1.0f, 0.0f, 0.0f);
static const Vector3 back = Vector3(0.0f, 0.0f, 1.0f);