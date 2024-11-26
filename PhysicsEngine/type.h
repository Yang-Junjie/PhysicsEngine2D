#pragma once
#include "math.h"
struct CirlceType
{
	float radius = 0.1f;
	oeVec2 position = { 0.0f,0.0f };
	float color[4] = { 1.0f, 0.0f, 0.0f ,0.3f };
};


struct BoxType
{
	float width = 0.1f;
	oeVec2 position = { 0.0f,0.0f };
	float color[4] = { 1.0f, 0.0f, 0.0f ,0.3f };
};
