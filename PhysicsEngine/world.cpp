#include "world.h"



static void ResolveCollisionWithRotationAndFriction(Manifold& contact);

oeWorld::oeWorld(Renderer* renderer)
	:renderer_(renderer)
{
	
}

oeWorld::~oeWorld()
{
	ClearConstraints(); // 释放所有约束的内存
	ClearBodys();
}

void oeWorld::CreatCircle(CircleType type_data, Property prop_data)
{
	oeBody* tmp = new oeBody(CIRCLE,type_data.radius, type_data.color, type_data.position, prop_data.mass_, prop_data.stationary_, prop_data.restitution_, prop_data.inherent_static_friction_, prop_data.inherent_dynamic_friction_);
	++id_count;
	tmp->body_id_ = id_count;
	tmp->constant_force_ = prop_data.constant_force;
	bodys_list_.push_back(tmp);
	//bodys_list_[id_count] = tmp;
	//AddBody(*FindBody(id_count));
}

void oeWorld::CreatPolygon(PolygonType type_data,Property prop_data)
{
	oeBody* tmp = new oeBody(POLYGON, type_data.vertices, type_data.vertces_count, type_data.color,prop_data.mass_,prop_data.stationary_, prop_data.restitution_, prop_data.inherent_static_friction_, prop_data.inherent_dynamic_friction_);
	++id_count;
	tmp->body_id_ = id_count;
	tmp->constant_force_ = prop_data.constant_force;
	bodys_list_.push_back(tmp);
	
	//bodys_list_[id_count] = tmp;
	//AddBody(*FindBody(id_count));
}

void oeWorld::ClearBodys()
{
	for (auto& body : bodys_list_) {
		delete body;
	}
}

void oeWorld::AddConstraint(Constraint* constraint) {
	if (constraint == nullptr) {
		return; // 防止添加空指针
	}

	// 可选：检查约束是否已经存在
	for (auto* existing_constraint : globalConstraints) {
		if (existing_constraint == constraint) {
			return; // 约束已存在，不再重复添加
		}
	}

	// 将约束添加到全局约束列表中
	globalConstraints.push_back(constraint);
}

void oeWorld::ClearConstraints() {
	for (auto* constraint : globalConstraints) {
		delete constraint; // 释放每个约束的内存
	}
	globalConstraints.clear(); // 清空约束列表
}

oeBody* oeWorld::FindBody(const int id)
{
	if (bodys_list_[id]->body_id_ == -1) {
		return nullptr;
	}
	else {
		return bodys_list_[id];
	}
}

void oeWorld::RenderBody()
{
	for (auto& body : bodys_list_) {
		if (body->shape_ == CIRCLE) {
			
			renderer_->drawCircle(body->mass_center_.x, body->mass_center_.y, body->radius_, body->color_,36);
		}
		else if (body->shape_ == POLYGON) {
			renderer_->drawPolygon(body->vertices_, body->vertices_count_, body->color_);
		}
	}
}

void oeWorld::RenderAABB()
{
	
	for (auto& body : bodys_list_) {
		renderer_->drawAABB(body->aabb_,body->aabb_color_);
	}
}



void oeWorld::Interation(float time,int iterations)
{
	//每次迭代前清除aabb相交的物体
	this->contact_body_.clear();
	for (auto& body : bodys_list_) {
		if (!body->GetBodyState()) {
			body->Update(time, iterations);
			// 确保body.aabb_已经被正确设置
			body->GetAABB();

			
		}
	}
	
	BroadPhase();
	NarrowPhase();
	// 应用所有约束
	 // 应用所有约束
	for (int i = 0; i < iterations; ++i) {
		//std::cout << globalConstraints.size();
		for (auto* constraint : globalConstraints) {
			//std::cout << 1;
			constraint->Apply(bodys_list_, time);
		}
	}
	// 清除全局约束（避免累积）
//	ClearConstraints();
}


int oeWorld::GetBodyNum() const
{
	return id_count+1;
}

std::vector<oeBody*>* oeWorld::GetBodysList()
{
	return &bodys_list_;
}


//分离物体
void oeWorld::SepareteBodies(oeBody& body_a, oeBody& body_b, oeVec2& separation_vector)
{
	if (body_a.GetBodyState()) {
		body_b.Move(separation_vector / 2);
	}
	else if (body_b.GetBodyState()) {
		body_a.Move(-separation_vector / 2);
	}
	else {
		body_a.Move(-separation_vector / 2);
		body_b.Move(separation_vector / 2);
	}
}

void oeWorld::BroadPhase() {
	// 对需要对aabb上色的物体进行标记
	std::unordered_map<int, bool> colored;

	//不能去重，否则当circle在polygon前方时会出bug
	// 开始判断两个物体的aabb是否相交
	for (size_t i = 0; i < bodys_list_.size(); ++i) {
		auto& body_a = bodys_list_[i];
		body_a->GetAABB();

		for (size_t j = 0; j < bodys_list_.size(); ++j) {
			auto& body_b = bodys_list_[j];
			body_b->GetAABB();


			if (body_a->GetBodyState() && body_b->GetBodyState() || body_a->body_id_ == body_b->body_id_) {
				continue;
			}

			// 检测
			bool intersect = Intersect::IntersectAABB(body_a->aabb_, body_b->aabb_);

			// 如果没有相交则继续下一个物体的检测
			if (!intersect) {


				continue;
			}
			else {
				colored[body_a->body_id_] = true;
				colored[body_b->body_id_] = true;
				std::pair<oeBody*, oeBody*> pair_body(body_a, body_b);
				contact_body_.push_back(pair_body);



			}
		}
	}
	//再对标记过的物体的aabb进行上色
	for (auto& body : bodys_list_) {
		if (colored[body->body_id_]) {

			body->aabb_color_[0] = 1.0f;
			body->aabb_color_[1] = 1.0f;
			body->aabb_color_[2] = 1.0f;
			body->aabb_color_[3] = 1.0f;

		}
		else {
			body->aabb_color_[0] = 1.0f;
			body->aabb_color_[1] = 0.0f;
			body->aabb_color_[2] = 0.0f;
			body->aabb_color_[3] = 1.0f;
		}
	}
}


void oeWorld::NarrowPhase()
{
	//遍历AABB相交的物体
	for (auto& pair : this->contact_body_) {

		//对这两个物体使用碰撞检测，判断这两个物体是否发生碰撞
		IntersectData intersect_data = Collide(*pair.first, *pair.second);
		
		//如果发生碰撞，则计算分离向量，并分离这两个物体
		if (intersect_data.Collision) {
			oeVec2 separation_vector = intersect_data.normal * intersect_data.depth;
			SepareteBodies(*pair.first, *pair.second, separation_vector);
			ContactData contact_data = FindContactPoints(*pair.first, *pair.second);
			Manifold manifold = { pair.first, pair.second,contact_data,intersect_data };
			ResolveCollisionWithRotationAndFriction(manifold);	
		}

	}
}

static void ResolveCollisionWithRotationAndFriction(Manifold& contact) {
	oeBody* bodyA = contact.body_a;
	oeBody* bodyB = contact.body_b;
	
	oeVec2 normal = contact.intersect_data.normal;
	oeVec2 contact1 = contact.contact_data.contact1;
	oeVec2 contact2 = contact.contact_data.contact2;

	const int contactCount = contact.contact_data.contact_count;

	float e = std::min(bodyA->restitution_, bodyB->restitution_); // 碰撞恢复系数
	float sf = (bodyA->inherent_static_friction_ + bodyB->inherent_static_friction_) * 0.5f; // 静摩擦系数
	float df = (bodyA->inherent_dynamic_friction_ + bodyB->inherent_dynamic_friction_) * 0.5f; // 动摩擦系数

	std::vector<oeVec2> contactList(2, oeVec2(0, 0)); 

	std::vector<oeVec2> frictionImpulseList(contactCount, oeVec2(0, 0)); // 摩擦冲量列表
	std::vector<float> jList(contactCount, 0.0f); // 冲量大小列表
	std::vector<oeVec2> impulseList(contactCount, oeVec2(0, 0)); // 冲量向量列表
	std::vector<oeVec2> raList(contactCount, oeVec2(0, 0)); // 碰撞点到物体A质心的向量列表
	std::vector<oeVec2> rbList(contactCount, oeVec2(0, 0)); // 碰撞点到物体B质心的向量列表

	contactList[0] = contact1;
	contactList[1] = contact2;

	// 计算冲量
	for (int i = 0; i < contactCount; ++i) {
		oeVec2 ra = contactList[i] - bodyA->mass_center_; // 碰撞点到物体A质心的向量
		oeVec2 rb = contactList[i] - bodyB->mass_center_; // 碰撞点到物体B质心的向量

		raList[i] = ra;
		rbList[i] = rb;

		oeVec2 raPerp(-ra.y, ra.x); // ra向量的法向量
		oeVec2 rbPerp(-rb.y, rb.x); // rb向量的法向量

		oeVec2 angularLinearVelocityA = raPerp * bodyA->GetAngularVelocity(); // 物体A的角速度对raPerp的贡献
		oeVec2 angularLinearVelocityB = rbPerp * bodyB->GetAngularVelocity(); // 物体B的角速度对rbPerp的贡献

		oeVec2 relativeVelocity = (bodyB->GetVelocity() + angularLinearVelocityB) - (bodyA->GetVelocity() + angularLinearVelocityA); // 相对速度

		float contactVelocityMag = oeVec2::dot(relativeVelocity, normal); // 相对速度在法线方向上的分量

		if (contactVelocityMag > 0) {
			continue;
		}

		float raPerpDotN = oeVec2::dot(raPerp, normal); // raPerp向量与法线向量的点积
		float rbPerpDotN = oeVec2::dot(rbPerp, normal); // rbPerp向量与法线向量的点积

		float denom = bodyA->inverse_mass_ + bodyB->inverse_mass_ + (raPerpDotN * raPerpDotN) * bodyA->inverse_rotational_inertia_ + (rbPerpDotN * rbPerpDotN) * bodyB->inverse_rotational_inertia_; // 冲量计算的分母

		float j = -(1 + e) * contactVelocityMag / denom; // 冲量大小

		jList[i] = j;
		impulseList[i] = j * normal; // 冲量向量
	}

	// 应用冲量
	for (int i = 0; i < contactCount; ++i) {
		oeVec2 impulse = impulseList[i];
		oeVec2 ra = raList[i];
		oeVec2 rb = rbList[i];
		bodyA->velocity_ += -impulse * bodyA->inverse_mass_;
		bodyA->angular_velocity_ += -oeVec2::cross(ra, impulse) * bodyA->inverse_rotational_inertia_;
		bodyB->velocity_ += impulse * bodyB->inverse_mass_;
		bodyB->angular_velocity_ += oeVec2::cross(rb, impulse) * bodyB->inverse_rotational_inertia_;
		
	}

	// 计算摩擦冲量
	for (int i = 0; i < contactCount; ++i) {
		oeVec2 ra = contactList[i] - bodyA->mass_center_; // 碰撞点到物体A质心的向量
		oeVec2 rb = contactList[i] - bodyB->mass_center_; // 碰撞点到物体B质心的向量

		oeVec2 raPerp(-ra.y, ra.x); // ra向量的法向量
		oeVec2 rbPerp(-rb.y, rb.x); // rb向量的法向量

		oeVec2 angularLinearVelocityA = raPerp * bodyA->angular_velocity_; // 物体A的角速度对raPerp的贡献
		oeVec2 angularLinearVelocityB = rbPerp * bodyB->angular_velocity_; // 物体B的角速度对rbPerp的贡献

		oeVec2 relativeVelocity = (bodyB->velocity_ + angularLinearVelocityB) - (bodyA->velocity_ + angularLinearVelocityA); // 相对速度

		oeVec2 tangent = relativeVelocity - oeVec2::dot(relativeVelocity, normal) * normal; // 切向方向
		if (tangent.LengthSquared() < 1e-6f) { // 检查是否接近零向量
			continue;
		}
		else {
			tangent.normalize(); // 归一化切向向量
		}

		float raPerpDotT = oeVec2::dot(raPerp, tangent); // raPerp向量与切向向量的点积
		float rbPerpDotT = oeVec2::dot(rbPerp, tangent); // rbPerp向量与切向向量的点积

		float denom = bodyA->inverse_mass_ + bodyB->inverse_mass_ + (raPerpDotT * raPerpDotT) * bodyA->inverse_rotational_inertia_ + (rbPerpDotT * rbPerpDotT) * bodyB->inverse_rotational_inertia_; // 冲量计算的分母

		float jt = -oeVec2::dot(relativeVelocity, tangent) / denom; // 切向冲量大小
		float j = jList[i]; // 法向冲量大小

		if (std::abs(jt) <= j * sf) { // 根据静摩擦系数判断是否满足静摩擦条件
			frictionImpulseList[i] = jt * tangent; // 静摩擦冲量
		}
		else {
			frictionImpulseList[i] = -j * tangent * df; // 动摩擦冲量
		}
	}

	// 应用摩擦冲量
	for (int i = 0; i < contactCount; ++i) {
		oeVec2 frictionImpulse = frictionImpulseList[i];
		oeVec2 ra = raList[i];
		oeVec2 rb = rbList[i];
		bodyA->velocity_ += -frictionImpulse * bodyA->inverse_mass_;
		bodyA->angular_velocity_ += -oeVec2::cross(ra, frictionImpulse) * bodyA->inverse_rotational_inertia_;
		bodyB->velocity_ += frictionImpulse * bodyB->inverse_mass_;
		bodyB->angular_velocity_ += oeVec2::cross(rb, frictionImpulse) * bodyB->inverse_rotational_inertia_;
	}
}


