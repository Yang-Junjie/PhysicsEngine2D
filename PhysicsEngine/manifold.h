#pragma once
#include "math.h"
#include "body.h"
struct IntersectData {
    /*
        相交检测数据
        Collision：是否碰撞（相交）
            depth: 碰撞深度
           normal：分离向量
    */
    bool Collision = false;
    float depth = std::numeric_limits<float>::max();
    oeVec2 normal;
    
};

struct ContactData {
    /*
       接触数据
       contact_count：接触点的个数一般小于等于2个
            contact1: 接触点一的坐标
            contact2：接触点二的坐标
   */
    int contact_count = 0;
    oeVec2 contact1 = { 0.0f,0.0f };
    oeVec2 contact2 = { 0.0f,0.0f };;
};


struct Manifold {
    /*
        接触流形数据
                body_a：接触物体A
                body_b：接触物体B
          contact_data：接触数据
        intersect_data：相交数据
    */
    oeBody* body_a = nullptr;
    oeBody* body_b = nullptr;
    ContactData contact_data;
    IntersectData intersect_data;
    oeVec2 r1, r2;
    float separation;
    float Pn;	// accumulated normal impulse
    float Pt;	// accumulated tangent impulse
    float Pnb;	// accumulated normal impulse for position bias
    float massNormal, massTangent;
    float bias;
};