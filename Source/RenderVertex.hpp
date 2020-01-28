#pragma once
#include "Vector3.hpp"

struct RenderVertex
{
	Vector3 mPosition;
	Vector3 mColor;

	RenderVertex(Vector3 position, Vector3 color) : mPosition(std::move(position)), mColor(std::move(color)) {}
};