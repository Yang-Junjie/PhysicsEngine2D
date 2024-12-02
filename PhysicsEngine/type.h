#pragma once
#include "math.h"
#include "body.h"
struct CircleType
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

struct PolygonType {
	int vertces_count = 3;
	oeVec2 vertices[the_maximum_vertices] = { {0.0f,0.1f },{0.0f,0.0f},{0.1f,0.0f} };
	float color[4] = { 1.0f, 0.0f, 0.0f ,0.3f };
};

struct Property {
	float mass_ = 1.0f;//g
};
