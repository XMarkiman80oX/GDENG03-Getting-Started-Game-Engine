#pragma once
#include "Matrix4x4.h"

__declspec(align(16))
struct constantBufferData
{
	Matrix4x4 m_world;
	Matrix4x4 m_view;
	Matrix4x4 m_proj;

	unsigned int m_time;
	unsigned int m_interpolation_enabled;

	float padding[2];
};