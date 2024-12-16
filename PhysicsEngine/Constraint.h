#include <vector>
#include <memory>
#include <array>

// 基类 Constraint
class Constraint {
public:
    virtual ~Constraint() = default;
    virtual void Apply(oeBody& bodyA, oeBody& bodyB, float timeStep) = 0;
};

// 具体的 ContactConstraint 类
class ContactConstraint : public Constraint {
public:
    // 构造函数和其他成员函数省略...

    void Apply(oeBody& bodyA, oeBody& bodyB, float timeStep) override {
        // 计算相对位置向量
        oeVec2 rA = contactPoint_ - bodyA.mass_center_;
        oeVec2 rB = contactPoint_ - bodyB.mass_center_;

        // 计算相对速度
        oeVec2 relativeVelocity = bodyB.velocity_ +
            oeVec2(-rB.y, rB.x) * bodyB.angular_velocity_ -
            (bodyA.velocity_ +
                oeVec2(-rA.y, rA.x) * bodyA.angular_velocity_);

        // 计算速度约束
        float velocityAlongNormal = oeVec2::dot(relativeVelocity, normal_);

        // 如果相对速度沿法线方向为正，说明物体正在分离，不需要施加约束
        if (velocityAlongNormal > 0.0f) return;

        // 计算雅可比矩阵
        J = { -normal_.x, -normal_.y, -oeVec2::cross(rA, normal_),
              normal_.x, normal_.y, oeVec2::cross(rB, normal_) };

        // 计算有效质量
        float effectiveMass = 0.0f;
        if (bodyA.inverse_mass_ > 0.0f) {
            effectiveMass += J[0] * J[0] * bodyA.inverse_mass_ +
                J[1] * J[1] * bodyA.inverse_mass_ +
                J[2] * J[2] * bodyA.inverse_rotational_inertia_;
        }
        if (bodyB.inverse_mass_ > 0.0f) {
            effectiveMass += J[3] * J[3] * bodyB.inverse_mass_ +
                J[4] * J[4] * bodyB.inverse_mass_ +
                J[5] * J[5] * bodyB.inverse_rotational_inertia_;
        }

        // 计算拉格朗日乘子
        lambda = (-velocityAlongNormal - bias) / effectiveMass;

        // 应用冲量
        oeVec2 impulse = normal_ * lambda;
        if (bodyA.inverse_mass_ > 0.0f) {
            bodyA.velocity_ -= bodyA.inverse_mass_ * impulse;
            bodyA.angular_velocity_ -= bodyA.inverse_rotational_inertia_ * oeVec2::cross(rA, impulse);
        }
        if (bodyB.inverse_mass_ > 0.0f) {
            bodyB.velocity_ += bodyB.inverse_mass_ * impulse;
            bodyB.angular_velocity_ += bodyB.inverse_rotational_inertia_ * oeVec2::cross(rB, impulse);
        }
    }

private:
    oeBody* bodyA_;
    oeBody* bodyB_;
    oeVec2 contactPoint_;
    oeVec2 normal_;
    float bias;
    float lambda;
    std::array<float, 6> J;
};

// 约束求解器类
class ConstraintSolver {
public:
    void AddConstraint(std::shared_ptr<Constraint> constraint) {
        constraints_.push_back(constraint);
    }

    void Solve(std::vector<oeBody*>& bodies, float timeStep) {
        for (int i = 0; i < iterations_; ++i) {
            for (auto& constraint : constraints_) {
                // 直接调用约束的 Apply 方法，无需类型转换
                constraint->Apply(*bodies[0], *bodies[1], timeStep); // 这里假设每个约束只涉及两个刚体
            }
        }
    }

    void SetIterations(int iterations) {
        iterations_ = iterations;
    }

private:
    std::vector<std::shared_ptr<Constraint>> constraints_;
    int iterations_ = 10; // 默认迭代次数
};