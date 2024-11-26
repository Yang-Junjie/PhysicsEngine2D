#pragma once
#include "math.h"
#include "body.h"
#include <vector>
#include "type.h"
#include "Renderer.h"

class oeWorld {
private:
	int id_count = -1;
	//std::vector<Body> bodys_list_;
	Body bodys_list_[100] = {};
public:
	
	oeWorld();
	~oeWorld();
	oeVec2 gravity = { 0.0f,-10.0f };
	void CreatCircle(CirlceType data);
	void CreatBox(BoxType data);
	Body& FindBody(const int id);
	void RenderBody(Renderer& renderer);


};