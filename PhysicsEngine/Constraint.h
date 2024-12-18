#pragma once
#include <memory>
#include <array>
#include "body.h"
#include <unordered_map>
class Constraint {
public:
    virtual ~Constraint() = default;

    // 应用约束
    virtual void Apply(const std::vector<oeBody*>& bodies, float timeStep) = 0;

    // 获取约束涉及的刚体
    const std::vector<oeBody*>& GetBodies() const {
        return bodies_;
    }

protected:
    std::vector<oeBody*> bodies_; // 存储约束涉及的刚体
};



class ContactConstraint : public Constraint {
public:
    ContactConstraint(oeBody* bodyA, oeBody* bodyB, const oeVec2& contactPoint, const oeVec2& normal, float bias)
        : bodyA_(bodyA), bodyB_(bodyB), contactPoint_(contactPoint), normal_(normal), bias(bias), accumulatedImpulse(0.0f) {
        rA_ = contactPoint_ - bodyA_->mass_center_;
        rB_ = contactPoint_ - bodyB_->mass_center_;
        bodies_.push_back(bodyA_);
        bodies_.push_back(bodyB_);
    }

    void Apply(const std::vector<oeBody*>& bodies, float timeStep) override {
        WarmStart(); // 应用累积冲量
       // SolveVelocityConstraints(bodies, timeStep); // 解决速度约束
        std::cout << 1;
    }

private:
    void WarmStart() {
        // 应用累积冲量
        oeVec2 warmStartImpulse = normal_ * accumulatedImpulse;
        if (bodyA_->inverse_mass_ > 0.0f) {
            bodyA_->velocity_ -= bodyA_->inverse_mass_ * warmStartImpulse;
            bodyA_->angular_velocity_ -= bodyA_->inverse_rotational_inertia_ * oeVec2::cross(rA_, warmStartImpulse);
        }
        if (bodyB_->inverse_mass_ > 0.0f) {
            bodyB_->velocity_ += bodyB_->inverse_mass_ * warmStartImpulse;
            bodyB_->angular_velocity_ += bodyB_->inverse_rotational_inertia_ * oeVec2::cross(rB_, warmStartImpulse);
        }
    }

    void SolveVelocityConstraints(const std::vector<oeBody*>& bodies, float timeStep) {
        // 计算相对速度
        oeVec2 relativeVelocity = bodyB_->velocity_ +
            oeVec2(-rB_.y, rB_.x) * bodyB_->angular_velocity_ -
            (bodyA_->velocity_ +
                oeVec2(-rA_.y, rA_.x) * bodyA_->angular_velocity_);

        // 计算速度沿法线方向的分量
        float velocityAlongNormal = oeVec2::dot(relativeVelocity, normal_);

        // 如果相对速度沿法线方向为正，说明物体正在分离，不需要施加约束
        if (velocityAlongNormal > 0.0f) return;

        // 计算雅可比矩阵
        J = { -normal_.x, -normal_.y, -oeVec2::cross(rA_, normal_),
              normal_.x, normal_.y, oeVec2::cross(rB_, normal_) };

        // 计算有效质量
        float effectiveMass = 0.0f;
        if (bodyA_->inverse_mass_ > 0.0f) {
            effectiveMass += J[0] * J[0] * bodyA_->inverse_mass_ +
                J[1] * J[1] * bodyA_->inverse_mass_ +
                J[2] * J[2] * bodyA_->inverse_rotational_inertia_;
        }
        if (bodyB_->inverse_mass_ > 0.0f) {
            effectiveMass += J[3] * J[3] * bodyB_->inverse_mass_ +
                J[4] * J[4] * bodyB_->inverse_mass_ +
                J[5] * J[5] * bodyB_->inverse_rotational_inertia_;
        }

        // 计算拉格朗日乘子
        lambda = (-velocityAlongNormal - bias) / effectiveMass;

        // 更新累积冲量
        accumulatedImpulse += lambda;

        // 应用冲量
        oeVec2 impulse = normal_ * lambda;
        if (bodyA_->inverse_mass_ > 0.0f) {
            bodyA_->velocity_ -= bodyA_->inverse_mass_ * impulse;
            bodyA_->angular_velocity_ -= bodyA_->inverse_rotational_inertia_ * oeVec2::cross(rA_, impulse);
        }
        if (bodyB_->inverse_mass_ > 0.0f) {
            bodyB_->velocity_ += bodyB_->inverse_mass_ * impulse;
            bodyB_->angular_velocity_ += bodyB_->inverse_rotational_inertia_ * oeVec2::cross(rB_, impulse);
        }
    }

private:
    oeBody* bodyA_;
    oeBody* bodyB_;
    oeVec2 contactPoint_;
    oeVec2 normal_;
    float bias;
    float lambda = 0.0f;
    std::array<float, 6> J{};
    oeVec2 rA_; // 碰撞点到物体A质心的向量
    oeVec2 rB_; // 碰撞点到物体B质心的向量
    float accumulatedImpulse; // 累积冲量
};


class ConstraintSolver {
public:
    void AddConstraint(std::shared_ptr<Constraint> constraint) {
        constraints_.push_back(constraint);
        for (auto& body : constraint->GetBodies()) {
            bodyToConstraints_[body].push_back(constraint);
        }
    }

    void RemoveConstraint(std::shared_ptr<Constraint> constraint) {
        constraints_.erase(std::remove(constraints_.begin(), constraints_.end(), constraint), constraints_.end());
        for (auto& body : constraint->GetBodies()) {
            auto& bodyConstraints = bodyToConstraints_[body];
            bodyConstraints.erase(std::remove(bodyConstraints.begin(), bodyConstraints.end(), constraint), bodyConstraints.end());
        }
    }

    void ClearConstraints() {
        constraints_.clear();
        bodyToConstraints_.clear();
    }

    void Solve(std::vector<oeBody*>& bodies, float timeStep) {
        for (int i = 0; i < iterations_; ++i) {
            for (auto& body : bodies) {
                for (auto& constraint : bodyToConstraints_[body]) {
                    constraint->Apply(bodies, timeStep);
                }
            }
        }
    }

    void SetIterations(int iterations) {
        iterations_ = iterations;
    }

private:
    std::vector<std::shared_ptr<Constraint>> constraints_;
    int iterations_ = 10; // 默认迭代次数
    std::unordered_map<oeBody*, std::vector<std::shared_ptr<Constraint>>> bodyToConstraints_; // 刚体到约束的映射
};