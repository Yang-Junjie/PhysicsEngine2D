#include "world.h"




oeWorld::oeWorld(Renderer* renderer):renderer_(renderer)
{
}

oeWorld::~oeWorld()
{
}

void oeWorld::CreatCircle(CircleType type_data, Property prop_data)
{
	oeBody tmp(CIRCLE,type_data.radius, type_data.color, type_data.position, prop_data.mass_);
	++id_count;
	tmp.body_id_ = id_count;
	bodys_list_.push_back(tmp);
	//bodys_list_[id_count] = tmp;
}


void oeWorld::CreatBox(BoxType type_data, Property prop_data)
{
	oeBody tmp(BOX, type_data.width, type_data.color, type_data.position, prop_data.mass_);
	++id_count;
	tmp.body_id_ = id_count;
	bodys_list_.push_back(tmp);
	//bodys_list_[id_count] = tmp;
}


void oeWorld::CreatPolygon(PolygonType type_data,Property prop_data)
{
	oeBody tmp(POLTGON, type_data.vertices, type_data.vertces_count, type_data.color,prop_data.mass_);
	++id_count;
	tmp.body_id_ = id_count;
	bodys_list_.push_back(tmp);
	//bodys_list_[id_count] = tmp;
}

oeBody* oeWorld::FindBody(const int id)
{
	if (bodys_list_[id].body_id_ == -1) {
		return nullptr;
	}
	else {
		return &bodys_list_[id];
	}
}

void oeWorld::RenderBody()
{
	for (auto& body : bodys_list_) {
		if (body.shape_ == CIRCLE) {
			
			renderer_->drawCircle(body.mass_center_.x, body.mass_center_.y, body.radius_or_half_width_, body.color_,36);
		}
		else if (body.shape_ == POLTGON || body.shape_ == BOX) {
			renderer_->drawPolygon(body.vertices_, body.vertices_count_, body.color_);
		}
	}
}

void oeWorld::RenderAABB()
{
	
	for (auto& body : bodys_list_) {
		renderer_->drawAABB(body.aabb_,body.aabb_color_);
	}
}

void oeWorld::RenderNormal(  const oeBody& body, const oeVec2& vec)
{
	renderer_->drawVector(body.mass_center_, vec, body.aabb_color_);
}



void oeWorld::Interation(float time)
{
	//每次迭代前清除aabb相交的物体
	this->contact_body_.clear();
	BroadPhase();
	NarrowPhase();
}

 int oeWorld::GetBodyNum() const 
{
	return id_count+1;
}

std::vector<oeBody>* oeWorld::GetBodysList()
{
	return &bodys_list_;
}



void oeWorld::BroadPhase() {
	// 对需要对aabb上色的物体进行标记
	std::unordered_map<int, bool> colored;

	// 开始判断两个物体的aabb是否相交
	for (size_t i = 0; i < bodys_list_.size()-1; ++i) {
		auto& body_a = bodys_list_[i];
		body_a.GetAABB();

		for (size_t j = i + 1; j < bodys_list_.size(); ++j) {
			auto& body_b = bodys_list_[j];
			body_b.GetAABB();

			if (body_a.stationary_ && body_b.stationary_ ) {
				continue;
			}

			// 检测
			bool intersect = IntersectAABB(body_a.aabb_, body_b.aabb_);
			
			// 如果没有相交则继续下一个物体的检测
			if (!intersect) {
				
				
				continue;
			}
			else {
				colored[body_a.body_id_] = true;
				colored[body_b.body_id_] = true;
				std::pair<oeBody&, oeBody&> pair_body(body_a, body_b);
				contact_body_.push_back(pair_body);
				


			}
		}
	}
	
	//再对标记过的物体的aabb进行上色
	for (auto& body : bodys_list_) {
		if (colored[body.body_id_]) {
			
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
//分离物体
void oeWorld::SepareteBodies(oeBody& body_a, oeBody& body_b, oeVec2& separation_vector)
{
	if (body_a.stationary_) {
		body_b.Move(separation_vector / 2);
		
		RenderNormal(body_b, separation_vector * 5.0f / 2);
		///std::cout << separation_vector << std::endl;
	}
	else if (body_b.stationary_) {
		body_a.Move(-separation_vector / 2);
		
		RenderNormal(body_b, -separation_vector * 5.0f / 2);

	}
	else {
		body_a.Move(-separation_vector / 2);
		body_b.Move(separation_vector / 2);
		RenderNormal(body_a, -separation_vector*5.0f / 2);
		RenderNormal(body_b, separation_vector * 5.0f / 2);
	}
}



void oeWorld::NarrowPhase()
{
	//遍历AABB相交的物体
	for (auto& pair : this->contact_body_) {

		//对这两个物体使用碰撞检测，判断这两个物体是否发生碰撞

		IntersectData intersect_data = Collide(pair.first, pair.second);
		
		//如果发生碰撞，则计算分离向量，并分离这两个物体
		if (intersect_data.Collision) {
			
			oeVec2 separation_vector = intersect_data.normal * intersect_data.depth;

			SepareteBodies(pair.first, pair.second, separation_vector);

		}

	}
}


