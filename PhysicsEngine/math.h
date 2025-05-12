#pragma once
#include <cmath>   
#include <iostream>

const float OE_epsilon = 0.0000000000005f;

struct RotationTransform
{
    float cos;          
    float sin;         
    float positionX;    
    float positionY;   
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
        return os; // ����������Ա���ʽ����  
    }

    // �������
    oeVec2 operator+(const oeVec2& other) const
    {
        return { this->x + other.x, this->y + other.y };
    }

    // �������
    oeVec2 operator-(const oeVec2& other) const
    {
        return { this->x - other.x, this->y - other.y };
    }

    // ����ȡ����
    oeVec2 operator-() const
    {
        return { -this->x, -this->y };
    }

    // ����������
    oeVec2 operator*(float scalar) const
    {
        return { this->x * scalar, this->y * scalar };
    }

    // ����������
    friend oeVec2 operator*(float scalar, const oeVec2& vec)
    {
        return vec * scalar;
    }

    // ����������
    oeVec2 operator/(float scalar) const
    {
        return { this->x / scalar, this->y / scalar };
    }

    oeVec2& operator+=(const oeVec2& other) {
        this->x += other.x; // ����һ�������ֵ�ӵ���ǰ������
        this->y += other.y;
        return *this; // ���ص�ǰ���������
    }

    oeVec2& operator-=(const oeVec2& other) {
        this->x -= other.x; // ����һ�������ֵ�ӵ���ǰ������
        this->y -= other.y;
        return *this; // ���ص�ǰ���������
    }
    oeVec2& operator*=(const oeVec2& other) {
        this->x *= other.x; // ����һ�������ֵ�ӵ���ǰ������
        this->y *= other.y;
        return *this; // ���ص�ǰ���������
    }

    oeVec2& operator/=(const oeVec2& other) {
        this->x /= other.x; // ����һ�������ֵ�ӵ���ǰ������
        this->y /= other.y;
        return *this; // ���ص�ǰ���������
    }

    // ������ת
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
    // �ж����������Ƿ����
    bool operator==(const oeVec2& other) const
    {
        return this->x == other.x && this->y == other.y;
    }

    // �ж����������Ƿ����
    bool operator!=(const oeVec2& other) const
    {
        return !(*this == other);
    }

    // �ж����������Ƿ����
    bool equals(const oeVec2& other) const
    {
        return *this == other;
    }

    //����ȡģ
    float  len() const {
        return sqrt(this->x * this->x + this->y * this->y);
    }

    //����ȡģ
    static inline float len(oeVec2& v) {
        return sqrt(v.x * v.x + v.y * v.y);
    }

    //������һ��
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

    //������һ��
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

    //�����ĵ��
    float dot(const oeVec2& v) const {
        return this->x * v.x + this->y * v.y;
    }

    //�����ĵ��
    static inline float dot(const oeVec2& v1, const oeVec2& v2) {
        return v1.x * v2.x + v1.y * v2.y;
    }

    //�����Ĳ��
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

    //������ģ��ƽ��
    float LengthSquared() const {
        return this->x * this->x + this->y * this->y;
    }

    //������ģ��ƽ��
    static inline float LengthSquared(const oeVec2& v) {
        return v.x * v.x + v.y * v.y;
    }

    //���������ľ���
    static inline float Distance(const oeVec2 v1, const oeVec2 v2) {
        float dx = v1.x - v2.x;
        float dy = v1.y - v2.y;
        return sqrt(dx * dx + dy * dy);
    }

    //�������������ƽ��
    static inline float DistanceSquared(const oeVec2 v1, const oeVec2 v2) {
        return (v1.x - v2.x) * (v1.x - v2.x) + (v1.y - v2.y) * (v1.y - v2.y);
    }

    //�ǳ�С�ж�
    static inline bool NearlyEqualVec(const oeVec2 a, const oeVec2 b) {
        return oeVec2::DistanceSquared(a, b) < OE_epsilon * OE_epsilon;
    }

    inline bool NearlyEqual(const oeVec2& other, float epsilon = 1e-6f) const {
        return std::abs(x - other.x) <= epsilon && std::abs(y - other.y) <= epsilon;
    }

    //�ǳ�С�ж�
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