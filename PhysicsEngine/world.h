#pragma once
#include "math.h"
#include "body.h"
#include <vector>
#include "type.h"
#include "Renderer.h"
#include "collision.h"

class oeWorld {
private:
	Renderer* renderer_;										//渲染器
	int id_count = -1;											//物体数下标从0，1，2，····，n
	std::vector<oeBody*> bodys_list_;							//物体列表
	std::vector<std::pair<oeBody*, oeBody*>> contact_body_;		//接触物体对
	
	
	void SepareteBodies(oeBody& body_a, oeBody& body_b, oeVec2& separation_vector); //分离接触物体
	void BroadPhase();																
	void NarrowPhase(float dt);
public:
	
	oeWorld(Renderer* renderer);
	~oeWorld();
	
	
	oeBody* CreatCircle(CircleType type_data, Property prop_data);
	oeBody* CreatPolygon(PolygonType type_data,Property prop_data);
	void ClearBodys();


	void RenderBody();
	void RenderAABB();

	int GetBodyNum() const;
	oeBody* FindBody(const int id);
	std::vector<oeBody*>* GetBodysList();

	//启动世界
	void Interation(float time,int iterations);

	
};

