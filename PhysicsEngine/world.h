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
	oeVec2 gravity_ = { 0.0f,-10.0f };
	Renderer* renderer_;
	int id_count = -1;
	std::vector<oeBody> bodys_list_;
	std::vector<std::pair<oeBody&, oeBody&>> contact_body_;

	void SepareteBodies(oeBody& body_a, oeBody& body_b, oeVec2& separation_vector);
	void BroadPhase();
	void NarrowPhase();
	
public:
	
	oeWorld(Renderer* renderer);
	~oeWorld();
	
	
	void CreatCircle(CircleType type_data, Property prop_data);
	void CreatBox(BoxType type_data, Property prop_data);
	void CreatPolygon(PolygonType type_data,Property prop_data);
	void RenderBody();
	void RenderAABB();
	void RenderNormal(const oeBody& body,const oeVec2& vec);

	int GetBodyNum() const;
	oeBody* FindBody(const int id);
	std::vector<oeBody>* GetBodysList();


	void Interation(float time);
};