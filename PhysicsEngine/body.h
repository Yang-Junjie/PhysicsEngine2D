#pragma once
#include <vector>
#include "math.h"
#include <iostream>
#include "forcegenerator.h"
const int the_maximum_vertices = 8;


enum Shape {
	NONE,
	CIRCLE,
	POLYGON
	
};

struct oeAABB
{
	bool draw_state = false;
	oeVec2 max;
	oeVec2 min;
};



//物体类
//功能：是本物理引擎最基本的元素， 所有的物体都是Body类，通常不能直接调用Body的构造函数创建物体，需要通过BodyManager的CreateBody函数来创建物体
//能创建多边形，圆形两种形状。创建的物体包含了基本的物理属性
class oeBody {

private:
	
	//密度
	float density_ = 0.1f;

	//体积
	float volume_ = 1.0f;
	
	//面积
	float area_ = 1.0f;

	
public:
	//物体旋转的角度
	float angle_ = 0.0f;

	oeVec2 force_;
	
	oeVec2 constant_force_;

	//物体的角速度,对于二维情况下角速度的方向是垂直于屏幕的所以使用float
	float angular_velocity_ = 0.0f;

	//物体的线速度
	oeVec2 velocity_ = { 0.0f,0.0f };

	//物体的加速度
	oeVec2 acceleration_ = { 0.0f,0.0f };

	//物体的id
	int body_id_ = -1;

	//物体的形状
	Shape shape_ = NONE;

	float radius_ = 0.0f;

	//物体的颜色分别对应，rgba
	float color_[4] = { 0,0,0,0 };

	//position物体的位置
	oeVec2 mass_center_ = { 0.0f,0.0f };

	//polygon形状专属属性
	oeVec2 vertices_[the_maximum_vertices] = {};

	//顶点数
	int vertices_count_ = 0;

	//aabb包围盒的顶点
	oeAABB aabb_;

	//aabb包围盒的颜色
	float aabb_color_[4] = { 0,0,0,0 };

	//质量
	float mass_ = 0.0f;

	//物体质量的倒数
	float inverse_mass_ = 0.0f;

	//是否是静止物体
	bool stationary_ = false;

	//固定静摩擦力
	float inherent_static_friction_ = 0.5f;

	//固定动摩擦力
	float inherent_dynamic_friction_ = 0.3f;

	float rotational_inertia_ = 0.0f;
	//转动惯量的倒数
	float inverse_rotational_inertia_ = 0.0f;
	// 物体的恢复系数
	float restitution_ = 0.0f;

	//物体的析构函数
	~oeBody();
	//oeBody();

	//Circle构造函数
	oeBody(Shape shape, float radius_or_width, float* color,oeVec2 mass_center,float mass,bool state,float restitution,float inherent_static_friction_,float inherent_dynamic_friction_);

	//Polygon构造函数，重载函数
	oeBody(Shape shape, oeVec2* vertices,int vertices_count, float* color, float mass, bool state, float restitution, float inherent_static_friction_, float inherent_dynamic_friction_);


	void Move(const oeVec2 v);
	void MoveTo(const oeVec2 v);
	void Rotation(const float angle);

	void SetVelocity(const oeVec2 v0);
	void SetAcceleration(const oeVec2 a0);
	void SetAngle(const float angle);
	void SetAngularVelocity(const float av0);

	float  GetArea() const;
	float  GetMass() const;
	float  GetDensity() const;
	float  GetVolume() const;
	float  GetAngle() const;
	float  GetAngularVelocity() const;
	float  GetRestitution()const;
	bool   GetBodyState() const;
	oeVec2 GetVelocity() const;
	oeVec2 GetAcceleration() const;

	
	



	void GetAABB();
	oeVec2 GetPolygonCentroid() const;


	void Update(float time,int iterations);
};


