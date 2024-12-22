#pragma once
#include <cmath>   // For std::cos and std::sin
#include <iostream>

const float OE_epsilon = 0.0000000000005f;

struct RotationTransform
{
    float cos;          // Cosine of the rotation angle
    float sin;          // Sine of the rotation angle
    float positionX;    // Translation along the X-axis
    float positionY;    // Translation along the Y-axis
};

struct oeVec2
{
    float x;
    float y;
    oeVec2() :x(0.0f), y(0.0f) {};
    oeVec2(float x, float y) : x(x), y(y) {}

    static oeVec2 Zero() {
        return oeVec2{ 0.0f,0.0f };
    }

    friend std::ostream& operator<<(std::ostream& os, const oeVec2& p) {
        os << "(" << p.x << ", " << p.y << ")";
        return os; // 返回输出流以便链式调用  
    }

    // 向量相加
    oeVec2 operator+(const oeVec2& other) const
    {
        return { this->x + other.x, this->y + other.y };
    }

    // 向量相减
    oeVec2 operator-(const oeVec2& other) const
    {
        return { this->x - other.x, this->y - other.y };
    }

    // 向量取负数
    oeVec2 operator-() const
    {
        return { -this->x, -this->y };
    }

    // 向量的数乘
    oeVec2 operator*(float scalar) const
    {
        return { this->x * scalar, this->y * scalar };
    }

    // 向量的数乘
    friend oeVec2 operator*(float scalar, const oeVec2& vec)
    {
        return vec * scalar;
    }

    // 向量的数除
    oeVec2 operator/(float scalar) const
    {
        return { this->x / scalar, this->y / scalar };
    }

    oeVec2& operator+=(const oeVec2& other) {
        this->x += other.x; // 将另一个对象的值加到当前对象上
        this->y += other.y;
        return *this; // 返回当前对象的引用
    }

    oeVec2& operator-=(const oeVec2& other) {
        this->x -= other.x; // 将另一个对象的值加到当前对象上
        this->y -= other.y;
        return *this; // 返回当前对象的引用
    }
    oeVec2& operator*=(const oeVec2& other) {
        this->x *= other.x; // 将另一个对象的值加到当前对象上
        this->y *= other.y;
        return *this; // 返回当前对象的引用
    }

    oeVec2& operator/=(const oeVec2& other) {
        this->x /= other.x; // 将另一个对象的值加到当前对象上
        this->y /= other.y;
        return *this; // 返回当前对象的引用
    }

    // 向量旋转
    static inline void RotationTransform(float& x, float& y, const float radian)
    {
        float cos_angle = static_cast<float>(std::cos(radian));
        float sin_angle = static_cast<float>(std::sin(radian));
        float temp_x = cos_angle * x - sin_angle * y;
        y = sin_angle * x + cos_angle * y;
        x = temp_x;
    }

    static inline float AngleToRadian(float angle) {
        return static_cast<float>(angle * M_PI / 180.0f);
    }
    // 判断两个向量是否相等
    bool operator==(const oeVec2& other) const
    {
        return this->x == other.x && this->y == other.y;
    }

    // 判断两个向量是否不相等
    bool operator!=(const oeVec2& other) const
    {
        return !(*this == other);
    }

    // 判断两个向量是否相等
    bool equals(const oeVec2& other) const
    {
        return *this == other;
    }

    //向量取模
    float  len() const {
        return sqrt(this->x * this->x + this->y * this->y);
    }

    //向量取模
    static inline float len(oeVec2& v) {
        return sqrt(v.x * v.x + v.y * v.y);
    }

    //向量归一化
    void normalize() {
        float length = sqrt(this->x * this->x + this->y * this->y);
        if (length > OE_epsilon) {
            this->x /= length;
            this->y /= length;
        }
        else {
            this->x = 0.0f;
            this->y = 0.0f;
        }
    }

    // Normalize the vector
    oeVec2 normalize() const {
        float length = len();
        if (length > 0.0f) {
            return { x / length, y / length };
        }
        return { 0.0f, 0.0f };
    }

    //向量归一化
    static oeVec2 normalize(oeVec2 v) {
        float length = oeVec2::len(v);
        if (length > OE_epsilon) {
            v.x /= length;
            v.y /= length;
        }
        else {
            v.x = 0.0f;
            v.y = 0.0f;
        }
        return v;
    }

    //向量的点积
    float dot(const oeVec2& v) const {
        return this->x * v.x + this->y * v.y;
    }

    //向量的点积
    static inline float dot(const oeVec2& v1, const oeVec2& v2) {
        return v1.x * v2.x + v1.y * v2.y;
    }

    //向量的叉积
    float cross(const oeVec2& v) const {
        return this->x * v.y - this->y * v.x;
    }

    static inline oeVec2 cross(const oeVec2& a, float s)
    {
        return oeVec2(s * a.y, -s * a.x);
    }

    static inline oeVec2 cross(float s, const oeVec2& a)
    {
        return oeVec2(-s * a.y, s * a.x);
    }

    static inline float cross(const oeVec2& v1, const oeVec2& v2) {
        return v1.x * v2.y - v1.y * v2.x;
    }

    //向量的模的平方
    float LengthSquared() const {
        return this->x * this->x + this->y * this->y;
    }

    //向量的模的平方
    static inline float LengthSquared(const oeVec2& v) {
        return v.x * v.x + v.y * v.y;
    }

    //两个向量的距离
    static inline float Distance(const oeVec2 v1, const oeVec2 v2) {
        float dx = v1.x - v2.x;
        float dy = v1.y - v2.y;
        return sqrt(dx * dx + dy * dy);
    }

    //两个向量距离的平方
    static inline float DistanceSquared(const oeVec2 v1, const oeVec2 v2) {
        return (v1.x - v2.x) * (v1.x - v2.x) + (v1.y - v2.y) * (v1.y - v2.y);
    }

    //非常小判断
    static inline bool NearlyEqualVec(const oeVec2 a, const oeVec2 b) {
        return oeVec2::DistanceSquared(a, b) < OE_epsilon * OE_epsilon;
    }

    inline bool NearlyEqual(const oeVec2& other, float epsilon = 1e-6f) const {
        return std::abs(x - other.x) <= epsilon && std::abs(y - other.y) <= epsilon;
    }

    //非常小判断
    static inline bool NearlyEqual(const float a, const float b) {
        return std::abs(a - b) < OE_epsilon * OE_epsilon;
    }

    // Get the direction vector (normalized)
    oeVec2 GetDirection() const {
        float length = std::sqrt(x * x + y * y);
        if (length > 0.0f) {
            return { x / length, y / length };
        }
        return { 0.0f, 0.0f };
    }
    void clear() {
        x = 0.0f;
        y = 0.0f;
    }
    void set(float col1_x, float col1_y) {
        x = col1_x;
        y = col1_y;
    }
};