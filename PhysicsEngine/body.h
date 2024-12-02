#pragma once
#include <vector>
#include "math.h"
#include <iostream>
const int the_maximum_vertices = 8;


enum Shape {
	NONE,
	CIRCLE,
	POLTGON,
	BOX
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
public:

	//以下是物体的特征值
	//物体的id
	int body_id_ = -1;

	//物体的形状
	Shape shape_ = NONE;

	float radius_or_half_width_ = 0.0f;

	//物体的颜色分别对应，rgba
	float color_[4] = { 0,0,0,0 };

	//position物体的位置
	oeVec2 mass_center_ = { 0.0f,0.0f };

	//polygon形状专属属性
	oeVec2 vertices_[the_maximum_vertices] = {};

	int vertices_count_ = 0;

	//aabb包围盒的顶点
	oeAABB aabb_;

	//aabb包围盒的颜色
	float aabb_color_[4] = { 0,0,0,0 };

	//是否是静止物体
	bool stationary_ = false;





	
	float density_ = 0.1f;

	float mass_ = 1.0f;

	float volume_ = 1.0f;

	float area_ = 1.0f;
	struct shuxing
	{
		

	////物理属性值
	////物体是否静止，false->非静止，true->静止
	//bool stationary_ = true;

	////物体的恢复系数
	//float restitution_ = 0.0f;

	////物体旋转的角度
	//float angle = 0.0f;

	////物体的角速度,对于二维情况下角速度的方向是垂直于屏幕的所以使用float
	//float angular_velocity_ = 0.0f;

	////物体的密度
	//float density_ = 0.0f;

	////物体的质量
	//float mass_ = 1.0f;

	////物体质量的倒数
	//float inverse_mass_ = 0.0f;

	

	////物体的线速度
	//oeVec2 velocity_ = { 0.0f,0.0f };

	////物体的加速度
	//oeVec2 acceleration_ = { 0.0f,0.0f };

	////固定静摩擦力
	//float inherent_static_friction_ = 0.5f;

	////固定动摩擦力
	//float inherent_dynamic_friction_ = 0.3f;

	////转动惯量
	//float rotational_inertia_ = 0.0f;

	////转动惯量的倒数
	//float inverse_rotational_inertia_ = 0.0f;
	};

	//物体的析构函数
	~oeBody();
	oeBody();

	//Circle Box构造函数
	//依次传入参数：shape,radius，color,mass>0,mass_center,body_id（可选，但不能使用重复id可通过body_list查看所有物体的id）
	oeBody(Shape shape, float radius_or_width, float* color,oeVec2 mass_center,float mass);

	//Polygon构造函数，重载函数
	//依次传入参数：shape,vertices，color,mass>0,mass_center,body_id（可选，但不能使用重复id可通过body_list查看所有物体的id）
	oeBody(Shape shape, oeVec2* vertices,int vertices_count, float* color, float mass);


	void Move(const oeVec2 v);
	void Rotation(const float angle);
	void GetAABB();
};


