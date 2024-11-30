#include "world.h"


oeWorld::oeWorld()
{
}

oeWorld::~oeWorld()
{
}

void oeWorld::CreatCircle(CirlceType data)
{
	oeBody tmp(CIRCLE,data.radius,data.color,data.position);
	++id_count;
	tmp.body_id_ = id_count;
	bodys_list_.push_back(tmp);
	//bodys_list_[id_count] = tmp;
}


void oeWorld::CreatBox(BoxType data)
{
	oeBody tmp(BOX, data.width, data.color, data.position);
	++id_count;
	tmp.body_id_ = id_count;
	bodys_list_.push_back(tmp);
	//bodys_list_[id_count] = tmp;
}

oeBody& oeWorld::FindBody(const int id)
{
	if (bodys_list_[id].body_id_ == 0) {
		return bodys_list_[id];
	}
	else {
		return bodys_list_[id];
	}
}

void oeWorld::RenderBody(Renderer& renderer)
{
	for (auto& body : bodys_list_) {
		if (body.shape_ == CIRCLE) {
			
			renderer.drawCircle(body.mass_center_.x, body.mass_center_.y, body.radius_or_width_, body.color_,36);
		}
		else if(body.shape_==BOX) {
		//	renderer.drawRectangle(body.mass_center_.x, body.mass_center_.y, body.radius_or_width_, body.radius_or_width_, body.color_);
			renderer.drawPolygon(body.vertices_, body.vertices_count_, body.color_);
		}
	}
}

void oeWorld::RenderAABB(Renderer& renderer)
{
	
	for (auto& body : bodys_list_) {
		renderer.drawAABB(body.aabb_,body.aabb_color_);
	}
}

void oeWorld::Interation(float time)
{
	BroadPhase();
}

int oeWorld::GetBodyNum()
{
	return id_count+1;
}

std::vector<oeBody>* oeWorld::GetBodysList()
{
	return &bodys_list_;
}



void oeWorld::BroadPhase() {

	// 开始判断两个物体的aabb是否相交
	for (size_t i = 0; i < bodys_list_.size(); ++i) {
		auto& body_a = bodys_list_[i];
		body_a.GetAABB();

		for (size_t j = i + 1; j < bodys_list_.size(); ++j) {
			auto& body_b = bodys_list_[j];
			body_b.GetAABB();

			if (body_a.stationary_ && body_b.stationary_) {
				continue;
			}

			// 检测
			bool intersect = IntersectAABB(body_a.aabb_, body_b.aabb_);
			
			// 如果没有相交则继续下一个物体的检测
			if (!intersect) {
				body_a.aabb_.draw_state = false;
				body_b.aabb_.draw_state = false;
				continue;
			}
			else {
				body_a.aabb_.draw_state = true;
				body_b.aabb_.draw_state = true;


			}
		}
	}
	
	//再对标记过的物体的aabb进行上色
	for (auto& body : bodys_list_) {
		if (body.aabb_.draw_state) {
			
			body.aabb_color_[0] = 1.0f;
			body.aabb_color_[1] = 1.0f;
			body.aabb_color_[2] = 1.0f;
			body.aabb_color_[3] = 1.0f;
			
		}
		else {
			body.aabb_color_[0] = 1.0f;
			body.aabb_color_[1] = 0.0f;
			body.aabb_color_[2] = 0.0f;
			body.aabb_color_[3] = 1.0f;
		}
	}
}
