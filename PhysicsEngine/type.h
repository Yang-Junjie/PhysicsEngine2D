#pragma once
#include "math.h"
#include "body.h"
struct CircleType
{
	float radius = 0.1f;
	oeVec2 position = { 0.0f,0.0f };
	float color[4] = { 1.0f, 0.0f, 0.0f ,0.3f };
};

struct PolygonType {
	int vertices_count = 4;
	oeVec2 vertices[the_maximum_vertices] = {	{-0.1f, 0.1f},	// --------
												{-0.1f,-0.1f},	// |	  |
												{ 0.1f,-0.1f},	// |	  |
												{ 0.1f, 0.1f} };// --------
	float color[4] = { 1.0f, 0.0f, 0.0f ,0.3f };
};


struct Property {
	//是否是静止物体
	bool stationary_ = false;

	//物体的质量
	float mass_ = 1.0f;//g

	//物体的恢复系数
	float restitution_ = 0.0f;

	//固定静摩擦力
	float inherent_static_friction_ = 0.5f;

	//固定动摩擦力
	float inherent_dynamic_friction_ = 0.3f;

	oeVec2 constant_force = { 0.0f,0.0f };
};








