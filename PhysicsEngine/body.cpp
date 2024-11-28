#include "body.h"


oeBody::~oeBody() {

}

oeBody::oeBody():shape_(NONE)
{
	
}

oeBody::oeBody(Shape shape, float radius_or_width, float* color, oeVec2 mass_center):
	shape_(shape),radius_or_width_(radius_or_width),mass_center_(mass_center)
{
	if (shape == BOX) {
		vertices_count_ = 4;
		vertices_[0] = oeVec2(mass_center_.x - radius_or_width_, mass_center_.y + radius_or_width_);
		vertices_[1] = oeVec2(mass_center_.x - radius_or_width_, mass_center_.y - radius_or_width_);
		vertices_[2] = oeVec2(mass_center_.x + radius_or_width_, mass_center_.y - radius_or_width_);
		vertices_[3] = oeVec2(mass_center_.x + radius_or_width_, mass_center_.y + radius_or_width_);
	}
	for (int i = 0; i < 4; i++)color_[i] = color[i];
}

oeBody::oeBody(Shape shape, oeVec2* vertices, int vertices_count, float* color):
	shape_(shape), vertices_count_(vertices_count)
{
	for (int i = 0; i < 4; i++)color_[i] = color[i];
	for (int i = 0; i < vertices_count; i++)vertices_[i] = vertices[i];
}

void oeBody::Move(const oeVec2 v)
{

	if (shape_ == CIRCLE) {
		mass_center_.y += v.y;
		mass_center_.x += v.x;

	}
	else if (shape_ == BOX) {
		for (int i = 0; i < vertices_count_; ++i) {
			vertices_[i].x += v.x;
			vertices_[i].y += v.y;
		}
		mass_center_ = (vertices_[0] + vertices_[2]) / 2.0f;
	}
}


void oeBody::Rotation(const float radian)
{
	if (shape_ == CIRCLE) {
	}
	else if (shape_ == BOX) {
		// 平移所有顶点到原点  
		oeVec2 origin = mass_center_;
		for (auto& vertex : vertices_) {
			vertex -= origin;
		}

		// 旋转所有顶点  
		for (auto& vertex : vertices_) {
			oeVec2::Transform(vertex.x, vertex.y, radian);
		}

		// 平移回原来的位置  
		for (auto& vertex : vertices_) {
			vertex += origin;
		}
		mass_center_ = (vertices_[0] + vertices_[2]) / 2.0f;
	}
}

void oeBody::GetAABB()
{
	if (shape_ == CIRCLE) {
		const float center_x = mass_center_.x;
		const float center_y =mass_center_.y;
		const float radius =radius_or_width_;

		aabb_.upperBound = { center_x - radius , center_y + radius };
		aabb_.lowerBound = { center_x + radius , center_y - radius };
	}
	else if (shape_ == BOX) {
		float max_x = -std::numeric_limits<float>::max();
		float max_y = -std::numeric_limits<float>::max();
		float min_x = std::numeric_limits<float>::max();
		float min_y = std::numeric_limits<float>::max();
		for (int i = 0; i <vertices_count_; ++i) {
			if (vertices_[i].x > max_x) {
				max_x = vertices_[i].x;
			}
			if (vertices_[i].x < min_x) {
				min_x = vertices_[i].x;
			}
			if (vertices_[i].y > max_y) {
				max_y = vertices_[i].y;
			}
			if (vertices_[i].y < min_y) {
				min_y = vertices_[i].y;
			}
		}
		aabb_.upperBound = { min_x , max_y };
		aabb_.lowerBound = { max_x , min_y };
	}
}

