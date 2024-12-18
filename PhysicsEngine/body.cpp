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
oeVec2 oeBody::GetPolygonCentroid() const {
	if (vertices_count_ < 3) return oeVec2(0, 0); // 至少需要3个顶点来形成一个多边形
	float total_area = area_;
	if (total_area == 0.0f) return oeVec2(0, 0); // 面积为0时无法计算质心

	float cx = 0.0f, cy = 0.0f;

	for (int i = 0; i < vertices_count_; ++i) {
		int j = (i + 1) % vertices_count_;
		float factor = (vertices_[i].x * vertices_[j].y - vertices_[j].x * vertices_[i].y);
		cx += (vertices_[i].x + vertices_[j].x) * factor;
		cy += (vertices_[i].y + vertices_[j].y) * factor;
	}

	cx /= (6.0f * total_area);
	cy /= (6.0f * total_area);

	return oeVec2(cx, cy);
}

//求三角形的转动惯量
static float inertiaTriangle(const oeVec2& a, const oeVec2& b, const oeVec2& c, float mass) {
	float areaTriangle = GetTriangleArea(a, b, c);
	float aLength = oeVec2::Distance(a, b);
	float bLength = oeVec2::Distance(b, c);
	float cLength = oeVec2::Distance(c, a);
	float Ic = static_cast<float>(mass / 6 * (std::pow(aLength, 2) + std::pow(bLength, 2) + std::pow(cLength, 2)));
	return Ic;
}

//求不规则多边形的转动惯量
static float inertiaPolygon(const oeVec2 points[],int vertices_count, float mass) {
	float totalInertia = 0.0;
	for (int i = 0; i < vertices_count - 2; ++i) {
		oeVec2 a = points[0];
		oeVec2 b = points[i + 1];
		oeVec2 c = points[i + 2];
		totalInertia += inertiaTriangle(a, b, c, mass);
	}
	return totalInertia;
}

void oeBody::Update(float time, int iterations)
{
	
	if (stationary_)return;
	time /= iterations;
	//acceleration_ = force_/ mass_;
	velocity_ += (acceleration_ + oeVec2{0,-10})* time;
	oeVec2 displacement =  velocity_ * time;
	float angle = angular_velocity_ * time;
	
	acceleration_ = oeVec2::Zero();
	Rotation(angle);
	Move(displacement);
	GetAABB();

	// 更新约束
	//static ConstraintSolver solver;
	//solver.SetIterations(iterations);

	// 解决所有约束
	//solver.Solve();
}

oeBody::oeBody()
{
}

oeBody::~oeBody() {

}

oeBody::oeBody(Shape shape, float radius, float* color, oeVec2 mass_center, float mass, bool body_state, float restitution, float inherent_static_friction, float inherent_dynamic_friction):
	shape_(shape), radius_(radius),mass_center_(mass_center), stationary_(body_state),restitution_(restitution),
	inherent_static_friction_(inherent_static_friction),inherent_dynamic_friction_(inherent_dynamic_friction)
{	
	mass_ = 0.0f;
	rotational_inertia_ = 0.0f;

	if (!body_state) {
		mass_ = mass;
		rotational_inertia_ = (1.0f / 12.0f) * mass * radius * radius;	
	}

		/*if (body_state) {
			density_ = mass / volume_;
		}*/
	
	inverse_mass_ = mass_ >0 ? 1.0f / mass_ : 0.0f;
	
	
	inverse_rotational_inertia_ = rotational_inertia_>0 ? 1.0f / rotational_inertia_:0.0f;
	
	area_ = static_cast<float>(2.0f * M_PI * radius);
	volume_ = static_cast<float>(M_PI * radius * radius);
	density_ = mass / volume_;
	
	for (int i = 0; i < 4; i++)color_[i] = color[i];
	GetAABB();  // 初始化AABB
}

oeBody::oeBody(Shape shape, oeVec2* vertices, int vertices_count, float* color, float mass, bool body_state, float restitution, float inherent_static_friction, float inherent_dynamic_friction):
	shape_(shape), vertices_count_(vertices_count),stationary_(body_state),restitution_(restitution), 
	inherent_static_friction_(inherent_static_friction), inherent_dynamic_friction_(inherent_dynamic_friction)
{
	mass_ = 0.0f;
	rotational_inertia_ = 0.0f;
	if (!body_state) {
		mass_ = mass;
		float width = (vertices[3].x - vertices[0].x);
		float height = (vertices[0].y - vertices[1].y);
		rotational_inertia_ = inertiaPolygon(vertices,vertices_count,mass);//正方形的转动惯量，明天实现多边形的转动惯量
	}
	inverse_mass_ = mass_ > 0 ? 1.0f / mass_ : 0.0f;
	inverse_rotational_inertia_ = rotational_inertia_ > 0 ? 1.0f / rotational_inertia_ : 0.0f;

	for (int i = 0; i < 4; i++)color_[i] = color[i];
	for (int i = 0; i < vertices_count; i++)vertices_[i] = vertices[i];
	if (vertices_count == 3) {
		area_ = GetTriangleArea(vertices[0], vertices[1], vertices[2]);
	}
	else {
		area_ = GetPolygonArea(vertices, vertices_count);
	}
	volume_ = area_*1.0f;
	density_ = mass / volume_;
	mass_center_ = GetPolygonCentroid();
	GetAABB();  // 初始化AABB
}
void oeBody::Move(const oeVec2 v) {
	if (shape_ == CIRCLE) {
		mass_center_.y += v.y;
		mass_center_.x += v.x;
		GetAABB(); // 更新AABB包围盒
	}
	else if (shape_ == POLYGON) {
		for (int i = 0; i < vertices_count_; ++i) {
			vertices_[i].x += v.x;
			vertices_[i].y += v.y;
		}
		mass_center_ = GetPolygonCentroid();
		GetAABB(); // 更新AABB包围盒
	}
}

void oeBody::MoveTo(const oeVec2 v) {
	if (this->shape_ == CIRCLE) {
		this->mass_center_ = v;
		GetAABB(); // 更新AABB包围盒
	}
	else if (this->shape_ == POLYGON) {
		int vertices_num = vertices_count_;
		oeVec2 origin = this->mass_center_;
		for (int i = 0; i < vertices_count_; ++i) {
			vertices_[i].x -= origin.x;
			vertices_[i].y -= origin.y;
		}
		for (int i = 0; i < vertices_count_; ++i) {
			vertices_[i].x += v.x;
			vertices_[i].y += v.y;
		}
		this->mass_center_ = GetPolygonCentroid();
		GetAABB(); // 更新AABB包围盒
	}
}

void oeBody::Rotation(const float radian) {
	if (shape_ == CIRCLE) {
		// 圆形不需要旋转
	}
	else if (shape_ == POLYGON) {
		oeVec2 origin = mass_center_;
		for (auto& vertex : vertices_) {
			vertex -= origin;
			oeVec2::RotationTransform(vertex.x, vertex.y, radian);
			vertex += origin;
		}
		mass_center_ = (vertices_[0] + vertices_[2]) / 2.0f;
		GetAABB(); // 更新AABB包围盒
	}
}
void oeBody::SetVelocity(const oeVec2 v0) { 
	velocity_ = v0; 
}

void oeBody::SetAcceleration(const oeVec2 a0)
{
	acceleration_ = a0;
}

void oeBody::SetAngularVelocity(const float av0)
{
	angular_velocity_ = av0;
}
//
//float oeBody::GetArea() const
//{
//	return area_;
//}
//
//float oeBody::GetMass()const
//{
//	return mass_;
//}
//
//float oeBody::GetDensity()const
//{
//	return density_;
//}
//
//float oeBody::GetVolume()const
//{
//	return volume_;
//}

oeVec2 oeBody::GetVelocity()const
{
	return velocity_;
}

oeVec2 oeBody::GetAcceleration()const
{
	return acceleration_;
}

//float oeBody::GetAngle()const
//{
//	return angle_;
//}

float oeBody::GetAngularVelocity()const
{
	return angular_velocity_;
}

float oeBody::GetRestitution() const
{
	return restitution_;
}

bool oeBody::GetBodyState()const
{
	return stationary_;
}


void oeBody::GetAABB()
{
	if (shape_ == CIRCLE) {
		const float center_x = mass_center_.x;
		const float center_y = mass_center_.y;
		const float radius = radius_;

		aabb_.min = { center_x - radius , center_y - radius };
		aabb_.max = { center_x + radius , center_y + radius};
	}
	else if ( shape_ == POLYGON) {
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

