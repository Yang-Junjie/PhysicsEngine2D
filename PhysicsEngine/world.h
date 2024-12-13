#pragma once
#include "math.h"
#include "body.h"
#include <vector>
#include "type.h"
#include "Renderer.h"
#include "collision.h"
#include <unordered_map>
#include <unordered_map>
#include <unordered_set>
#include <list>



class oeWorld {
private:
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
	void CreatPolygon(PolygonType type_data,Property prop_data);
	void RenderBody();
	void RenderAABB();
	void RenderNormal(const oeBody& body,const oeVec2& vec);

	int GetBodyNum() const;
	oeBody* FindBody(const int id);
	std::vector<oeBody>* GetBodysList();
    
	

	void Interation(float time,int iterations);
};


class gird {

public:
	float cell;
};