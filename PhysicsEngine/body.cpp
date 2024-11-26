#include "body.h"


Body::~Body() {

}

Body::Body():shape_(NONE)
{
	
}

Body::Body(Shape shape, float radius_or_width, float* color, oeVec2 mass_center):
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

Body::Body(Shape shape, oeVec2* vertices, int vertices_count, float* color):
	shape_(shape), vertices_count_(vertices_count)
{
	for (int i = 0; i < 4; i++)color_[i] = color[i];
	for (int i = 0; i < vertices_count; i++)vertices_[i] = vertices[i];
}

