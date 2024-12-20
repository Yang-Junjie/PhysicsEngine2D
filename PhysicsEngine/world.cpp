#include "world.h"
#include <map>

using std::vector;
using std::map;
using std::pair;

static void ResolveCollisionWithRotationAndFriction(Manifold& contact);
static void PreStep(Manifold* manifold_, float inv_dt);
static void ApplyImpulse(Manifold*);

oeWorld::oeWorld(Renderer* renderer)
	:renderer_(renderer)
{
	
}

oeWorld::~oeWorld()
{
	
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



void oeWorld::Interation(float dt,int iterations)
{
	float inv_dt = dt > 0.0f ? 1.0f / dt : 0.0f;
	//每次迭代前清除aabb相交的物体
	this->contact_body_.clear();
	BroadPhase();

	for (auto& body : bodys_list_) {
		if (!body->GetBodyState()) {
			body->Update(dt, iterations);
		}
	}
	NarrowPhase(dt);
	
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


void oeWorld::NarrowPhase(float dt)
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





			//	|
			// 接触约束
			// PreStep(&manifold,dt);
			// ApplyImpulse(&manifold);
		}

	}
}

static void PreStep(Manifold* manifold_,float inv_dt)
{
	const float k_allowedPenetration = 0.01f;
	float k_biasFactor = 0.2f; //World::positionCorrection ? 0.2f : 0.0f;
	oeBody* body1 = manifold_->body_a;
	oeBody* body2 = manifold_->body_b;
	for (int i = 0; i < manifold_->contact_data.contact_count; ++i)
	{
		

		oeVec2 r1 = manifold_->contact_data.contact1 - body1->mass_center_;
		oeVec2 r2 = manifold_->contact_data.contact1 - body2->mass_center_;

		// Precompute normal mass, tangent mass, and bias.
		float rn1 = oeVec2::dot(r1, manifold_->intersect_data.normal);
		float rn2 = oeVec2::dot(r2, manifold_->intersect_data.normal);
		float kNormal = body1->inverse_mass_ + body2->inverse_mass_;
		kNormal += body1->inverse_rotational_inertia_ * (oeVec2::dot(r1, r1) - rn1 * rn1) + body2->inverse_rotational_inertia_ * (oeVec2::dot(r2, r2) - rn2 * rn2);
		manifold_->massNormal = 1.0f / kNormal;

		oeVec2 tangent = oeVec2::cross(manifold_->intersect_data.normal, 1.0f);
		float rt1 = oeVec2::dot(r1, tangent);
		float rt2 = oeVec2::dot(r2, tangent);
		float kTangent = body1->inverse_mass_ + body2->inverse_mass_;
		kTangent += body1->inverse_rotational_inertia_ * (oeVec2::dot(r1, r1) - rt1 * rt1) + body2->inverse_rotational_inertia_ * (oeVec2::dot(r2, r2) - rt2 * rt2);
		manifold_->massTangent = 1.0f / kTangent;

		manifold_->bias = -k_biasFactor * inv_dt * Min(0.0f, manifold_->separation + k_allowedPenetration);

		/*if (World::accumulateImpulses)
		{*/
		// Apply normal + friction impulse
		oeVec2 P = manifold_->Pn * manifold_->intersect_data.normal + manifold_->Pt * tangent;

		body1->velocity_ -= body1->inverse_mass_ * P;
		body1->angular_velocity_ -= body1->inverse_rotational_inertia_ * oeVec2::cross(r1, P);

		body2->velocity_ += body2->inverse_mass_ * P;
		body2->angular_velocity_ += body2->inverse_rotational_inertia_ * oeVec2::cross(r2, P);
		//}
	}
}

static void ApplyImpulse(Manifold* manifold_)
{
	oeBody* b1 = manifold_->body_a;
	oeBody* b2 = manifold_->body_b;
	
		
	float friction = sqrtf(b1->inherent_dynamic_friction_*b2->inherent_dynamic_friction_);

	for (int i = 0; i < manifold_->contact_data.contact_count; ++i)
	{

		manifold_->r1 = manifold_->contact_data.contact1 - b1->mass_center_;
		manifold_->r2 = manifold_->contact_data.contact2 - b2->mass_center_;

		// Relative velocity at contact
		oeVec2 dv = b2->velocity_ + oeVec2::cross(b2->angular_velocity_, manifold_->r2) - b1->velocity_ - oeVec2::cross(b1->angular_velocity_, manifold_->r1);

		// Compute normal impulse
		float vn = oeVec2::dot(dv, manifold_->intersect_data.normal);

		float dPn = manifold_->massNormal * (-vn + manifold_->bias);

		/*if (World::accumulateImpulses)
		{*/
		// Clamp the accumulated impulse
		float Pn0 = manifold_->Pn;
		manifold_->Pn = std::max(Pn0 + dPn, 0.0f);
		dPn = manifold_->Pn - Pn0;
		/*}
		else
		{
			dPn = Max(dPn, 0.0f);
		}*/

		// Apply contact impulse
		oeVec2 Pn = dPn * manifold_->intersect_data.normal;

		b1->velocity_ -= b1->inverse_mass_ * Pn;
		b1->angular_velocity_ -= b1->inverse_rotational_inertia_ * oeVec2::cross(manifold_->r1, Pn);

		b2->velocity_ += b2->inverse_mass_ * Pn;
		b2->angular_velocity_ += b2->inverse_rotational_inertia_ * oeVec2::cross(manifold_->r2, Pn);

		// Relative velocity at contact
		dv = b2->velocity_ + oeVec2::cross(b2->angular_velocity_, manifold_->r2) - b1->velocity_ - oeVec2::cross(b1->angular_velocity_, manifold_->r1);

		oeVec2 tangent = oeVec2::cross(manifold_->intersect_data.normal, 1.0f);
		float vt = oeVec2::dot(dv, tangent);
		float dPt = manifold_->massTangent * (-vt);

		/*if (World::accumulateImpulses)
		{*/
		// Compute friction impulse
		float maxPt = friction * manifold_->Pn;

		// Clamp friction
		float oldTangentImpulse = manifold_->Pt;
		manifold_->Pt = Clamp(oldTangentImpulse + dPt, -maxPt, maxPt);
		dPt = manifold_->Pt - oldTangentImpulse;
		/*}
		else
		{
			float maxPt = friction * dPn;
			dPt = Clamp(dPt, -maxPt, maxPt);
		}*/

		// Apply contact impulse
		oeVec2 Pt = dPt * tangent;

		b1->velocity_ -= b1->inverse_mass_ * Pt;
		b1->angular_velocity_ -= b1->inverse_rotational_inertia_ * oeVec2::cross(manifold_->r1, Pt);

		b2->velocity_ += b2->inverse_mass_ * Pt;
		b2->angular_velocity_ += b2->inverse_rotational_inertia_ * oeVec2::cross(manifold_->r2, Pt);
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


