#pragma once
#include "math.h"
#include "body.h"
#include <vector>
#include "type.h"
#include "Renderer.h"
#include "collision.h"
#include <unordered_map>

class oeWorld {
private:
	

	int id_count = -1;
	std::vector<oeBody> bodys_list_;
	oeVec2 gravity = { 0.0f,-10.0f };
	void BroadPhase();
	std::vector<std::pair<oeBody&, oeBody&>> contact_body_;
public:
	
	oeWorld();
	~oeWorld();
	
	
	void CreatCircle(CirlceType data);
	void CreatBox(BoxType data);
	oeBody& FindBody(const int id);

	void RenderBody(Renderer& renderer);
	void RenderAABB(Renderer& renderer);
	void Interation(float time);
	int GetBodyNum();
	std::vector<oeBody>* GetBodysList();

};