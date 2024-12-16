/*
	by yangjunjie,website http://beisent.com
*/

#pragma once
#include <vector>
#include "math.h"
#include <iostream>
#include "forcegenerator.h"
#include "Constraint.h"
const int the_maximum_vertices = 8;

//形状
enum Shape {
	NONE,		//无形状一般用于初始化或是已经销毁的物体
	CIRCLE,		//圆形
	POLYGON		//多边形
};


//AABB
struct oeAABB
{
	bool draw_state = false;	//是否绘制AABB
	oeVec2 max;					//右上角顶点坐标
	oeVec2 min;					//左下角顶点坐标
};



//物体类
//功能：是本物理引擎最基本的元素， 所有的物体都是Body类，通常不能直接调用Body的构造函数创建物体，需要通过BodyManager的CreateBody函数来创建物体
//能创建多边形，圆形两种形状。创建的物体包含了基本的物理属性
class oeBody {

private:
	float density_ = 0.1f;						//密度

	float volume_ = 1.0f;						//体积
		
	float area_ = 1.0f;							//面积
public:
	float angle_ = 0.0f;						//物体旋转的角度

	oeVec2 force_ = oeVec2::Zero();				//变力
	
	oeVec2 constant_force_ = oeVec2::Zero();	//恒力

	float angular_velocity_ = 0.0f;				//物体的角速度

	oeVec2 velocity_ = { 0.0f,0.0f };			//物体的线速度	

	oeVec2 acceleration_ = { 0.0f,0.0f };		//物体的加速度

	int body_id_ = -1;							//物体的id，不存在表示为-1

	Shape shape_ = NONE;						//物体的形状

	float radius_ = 0.0f;						//物体的半径

					  /*r g b a*/
	float color_[4] = { 0,0,0,0 };				//物体的颜色分别对应，

	oeVec2 mass_center_ = { 0.0f,0.0f };		//物体的位置

	oeVec2 vertices_[the_maximum_vertices] = {};//polygon形状专属属性

	int vertices_count_ = 0;					//顶点数

	oeAABB aabb_;								//aabb包围盒的顶点

	float aabb_color_[4] = { 0,0,0,0 };			//aabb包围盒的颜色

	float mass_ = 0.0f;							//质量

	float inverse_mass_ = 0.0f;					//物体质量的倒数

	bool stationary_ = false;					//是否是静止物体

	float inherent_static_friction_ = 0.5f;		//固定静摩擦力

	float inherent_dynamic_friction_ = 0.3f;	//固定动摩擦力

	float rotational_inertia_ = 0.0f;			//转动惯量
	
	float inverse_rotational_inertia_ = 0.0f;	//转动惯量的倒数
	
	float restitution_ = 0.0f;					//物体的恢复系数

	//物体的析构函数
	~oeBody();
	//oeBody();

	//Circle构造函数
	oeBody(Shape shape, float radius_or_width, float* color,oeVec2 mass_center,float mass,bool state,float restitution,float inherent_static_friction_,float inherent_dynamic_friction_);

	//Polygon构造函数，重载函数
	oeBody(Shape shape, oeVec2* vertices,int vertices_count, float* color, float mass, bool state, float restitution, float inherent_static_friction_, float inherent_dynamic_friction_);
	
	//获取AABB包围盒
	void GetAABB();
	
	//将物体移动v
	void Move(const oeVec2 v);

	//将物体移动到v处
	void MoveTo(const oeVec2 v);
	
	//将物体旋转radian度单位是弧度
	void Rotation(const float radian);

	//设置物体的速度
	void SetVelocity(const oeVec2 v0);

	//设置物体的加速度
	void SetAcceleration(const oeVec2 a0);

	//设置物体的角速度
	void SetAngularVelocity(const float av0);

	//float  GetArea() const;
	//float  GetMass() const;
	//float  GetDensity() const;
	//float  GetVolume() const;
	//float  GetAngle() const;

	float  GetAngularVelocity() const;
	float  GetRestitution()const;
	bool   GetBodyState() const;
	oeVec2 GetVelocity() const;
	oeVec2 GetAcceleration() const;

	//获得多边形的质心坐标
	oeVec2 GetPolygonCentroid() const;

	void Update(float time,int iterations, std::vector<Constraint*>& globalConstraints);
};


