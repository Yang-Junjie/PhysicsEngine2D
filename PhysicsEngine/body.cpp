#include "body.h"
// 计算三角形面积（逆时针顶点）
static float GetTriangleArea(const oeVec2& v0, const oeVec2& v1, const oeVec2& v2) {
	return 0.5f * (v0.x * (v1.y - v2.y) + v1.x * (v2.y - v0.y) + v2.x * (v0.y - v1.y));
}

// 计算多边形面积（逆时针顶点）
static float GetPolygonArea(const oeVec2 vertices[], const int vertices_count) {
	if (vertices_count < 3) return 0.0f; // 至少需要3个顶点来形成一个多边形

	float area = 0.0f;
	for (int i = 0; i < vertices_count; ++i) {
		int j = (i + 1) % vertices_count;
		area += vertices[i].x * vertices[j].y - vertices[j].x * vertices[i].y;
	}
	return 0.5f * area; // 由于顶点是逆时针的，这里不需要取绝对值
}
// 计算多边形质心（逆时针顶点）
oeVec2 GetPolygonCentroid(const oeVec2 vertices[], const int vertices_count) {
	if (vertices_count < 3) return oeVec2(0, 0); // 至少需要3个顶点来形成一个多边形

	float total_area = GetPolygonArea(vertices, vertices_count);
	if (total_area == 0.0f) return oeVec2(0, 0); // 面积为0时无法计算质心

	float cx = 0.0f, cy = 0.0f;

	for (int i = 0; i < vertices_count; ++i) {
		int j = (i + 1) % vertices_count;
		float factor = (vertices[i].x * vertices[j].y - vertices[j].x * vertices[i].y);
		cx += (vertices[i].x + vertices[j].x) * factor;
		cy += (vertices[i].y + vertices[j].y) * factor;
	}

	cx /= (6.0f * total_area);
	cy /= (6.0f * total_area);

	return oeVec2(cx, cy);
}

oeVec2 GetMassCenter(const oeVec2 vertices[], const int vertices_count)
{


	int num_vertices = vertices_count;
	if (num_vertices < 3) {
		// 至少需要3个顶点来形成一个三角形  
		return oeVec2(0, 0);
	}

	std::vector<std::vector<int>> triangular_index(num_vertices - 2, std::vector<int>(3, 0));

	// 生成三角形索引（这里假设是简单的凸多边形，按顺序连接顶点形成三角形）  
	for (int i = 0; i < num_vertices - 2; ++i) {
		triangular_index[i][0] = 0;
		triangular_index[i][1] = i + 1;
		triangular_index[i][2] = i + 2;
	}

	float total_area = 0.0f;
	float cx = 0.0f, cy = 0.0f;

	for (const auto& indices : triangular_index) {
		oeVec2 v0 = vertices[indices[0]];
		oeVec2 v1 = vertices[indices[1]];
		oeVec2 v2 = vertices[indices[2]];

		float A = 0.5f * (v0.x * (v1.y - v2.y) + v1.x * (v2.y - v0.y) + v2.x * (v0.y - v1.y)); // 三角形面积  
		total_area += A;

		cx += (v0.x + v1.x + v2.x) * A; // 质心x坐标累加  
		cy += (v0.y + v1.y + v2.y) * A; // 质心y坐标累加  
	}

	cx /= (3.0f * total_area);
	cy /= (3.0f * total_area);

	return oeVec2(cx, cy);
}

oeBody::~oeBody() {

}

oeBody::oeBody():shape_(NONE)
{
	
}

oeBody::oeBody(Shape shape, float radius_or_half_width, float* color, oeVec2 mass_center,float mass):
	shape_(shape), radius_or_half_width_(radius_or_half_width),mass_center_(mass_center),mass_(mass)
{
	if (shape == BOX) {
		vertices_count_ = 4;
		vertices_[0] = oeVec2(mass_center_.x - radius_or_half_width_, mass_center_.y + radius_or_half_width_);
		vertices_[1] = oeVec2(mass_center_.x - radius_or_half_width_, mass_center_.y - radius_or_half_width_);
		vertices_[2] = oeVec2(mass_center_.x + radius_or_half_width_, mass_center_.y - radius_or_half_width_);
		vertices_[3] = oeVec2(mass_center_.x + radius_or_half_width_, mass_center_.y + radius_or_half_width_);
		area_ = radius_or_half_width * radius_or_half_width;
		volume_ = area_ *1.0f;
		density_ = mass_ / volume_;
	}
	for (int i = 0; i < 4; i++)color_[i] = color[i];
	if (shape == CIRCLE) {
		area_ = 2 * M_PI * radius_or_half_width;
		volume_ = M_PI * radius_or_half_width * radius_or_half_width;
		density_ = mass_ / volume_;
	}
	
}

oeBody::oeBody(Shape shape, oeVec2* vertices, int vertices_count, float* color, float mass):
	shape_(shape), vertices_count_(vertices_count), mass_(mass)
{
	for (int i = 0; i < 4; i++)color_[i] = color[i];
	for (int i = 0; i < vertices_count; i++)vertices_[i] = vertices[i];
	if (vertices_count == 3) {
		area_ = GetTriangleArea(vertices[0], vertices[1], vertices[2]);
	}
	else {
		area_ = GetPolygonArea(vertices, vertices_count);
	}
	volume_ = area_*1.0f;
	density_ = mass_ / volume_;
	mass_center_ = GetPolygonCentroid(vertices, vertices_count);
}

void oeBody::Move(const oeVec2 v)
{

	if (shape_ == CIRCLE) {
		mass_center_.y += v.y;
		mass_center_.x += v.x;

	}
	else if (shape_ == BOX || shape_ ==POLTGON) {
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
		const float center_y = mass_center_.y;
		const float radius = radius_or_half_width_;

		aabb_.min = { center_x - radius , center_y - radius };
		aabb_.max = { center_x + radius , center_y + radius};
	}
	else if (shape_ == BOX || shape_ == POLTGON) {
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
		aabb_.min = { min_x , min_y };
		aabb_.max = { max_x , max_y };
	}
}

