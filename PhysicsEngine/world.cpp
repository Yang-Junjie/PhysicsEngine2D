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

int oeWorld::GetBodyNum()
{
	return id_count+1;
}

std::vector<oeBody>* oeWorld::GetBodysList()
{
	return &bodys_list_;
}

