#pragma once

#include <vector>
#include "Body.h"


//以下函数都使用const 引用减少copy内存开销

/*检测数据
Collision：是否碰撞（相交）
depth:碰撞深度
normal：分离向量
*/
struct IntersectData {
    bool Collision = false;
    float depth = std::numeric_limits<float>::max();
    oeVec2 normal;
};

//计算多边形在轴上的投影
static  oeVec2 ProjectVertices(const oeVec2& axis, const std::vector<oeVec2>& vertices) {
    float min = std::numeric_limits<float>::max();
    float max = std::numeric_limits<float>::lowest();

    for (const auto& vertex : vertices) {
        float projection = axis.dot(vertex);
        min = std::min(min, projection);
        max = std::max(max, projection);
    }

    return oeVec2(min, max);
}

//计算圆在某个轴上的投影
static oeVec2 ProjectCircle(const oeVec2& center, const float& radius, const oeVec2& axis) {
    oeVec2 interval;

    oeVec2 direction_vector = oeVec2::normalize(axis);
    oeVec2 direction_and_radius = direction_vector * radius;

    oeVec2 p1 = center + direction_and_radius;
    oeVec2 p2 = center - direction_and_radius;

    interval.x = oeVec2::dot(p1, axis);
    interval.y = oeVec2::dot(p2, axis);

    if (interval.x > interval.y) {
        float temp;
        temp = interval.x;
        interval.x = interval.y;
        interval.y = temp;
    }
    return interval;
}

//找到多边形距离点最近的顶点
static size_t FindVertexClosestPoint(const oeVec2& point, const std::vector<oeVec2>& vertices) {
    size_t result = 0;
    float min_distance = std::numeric_limits<float>::max();
    for (size_t i = 0; i < vertices.size(); ++i) {
        float distance = oeVec2::Distance(vertices[i], point);
        if (distance <= min_distance) {
            min_distance = distance;
            result = i;
        }
    }
    return result;
}




//如果点在AABB外将会给出点到AABB的距离的平方
//如果点在AABB上或者在AABB内将会给出0
static float DistSqdPointAABB(const oeVec2& p, const std::vector<oeVec2>& vertices_aabb) {
    float sqDist = 0.0f;
    for (int i = 0; i < 2; i++) {
        float v, min, max;
        if (i == 0)
        {
            v = p.x;
            min = vertices_aabb[3].x;
            max = vertices_aabb[1].x;
            /*当i为0时对应x
            v设置为点 p 的 x 坐标。
            min 设置为边界框左上角点的 x 坐标（vertices_aabb[3].x）。
            max 设置为边界框右下角点的 x 坐标（vertices_aabb[1].x）。*/
        }
        if (i == 1)
        {
            v = p.y;
            min = vertices_aabb[3].y;
            max = vertices_aabb[1].y;
            /*当i为1时对应y
            v设置为点 p 的 y 坐标。
            min 设置为边界框左上角点的 y 坐标（vertices_aabb[3].y）。
            max 设置为边界框右下角点的 y 坐标（vertices_aabb[1].y）。y*/
        }
        if (v >= min && v <= max) return 0.0f;//如果在AABB上内直接返回0；
        if (v < min) sqDist += (min - v) * (min - v);
        if (v > max) sqDist += (v - max) * (v - max);
    }
    return sqDist; // 计算平方根  
}


//计算点与直线的接触点
//其原理是计算点距离线段哪个端点近那么那个端点极有可能是接触点
static oeVec2 GetSegmentContactPoint(const oeVec2& point, const oeVec2& segment_start, const oeVec2& segment_end) {
    oeVec2 segment_vector = segment_end - segment_start;//计算线段向量
    oeVec2 point_to_segment_start_vector = point - segment_start;//计算从线段起点指向点的向量

    float projection_length = oeVec2::dot(point_to_segment_start_vector, segment_vector);
    float segment_length_squared = oeVec2::LengthSquared(segment_vector);
    float projection_parameter = projection_length / segment_length_squared;//计算投影参数 proj_param = (|AP||AB| * cos(θ)) / |AB|^2

    if (projection_parameter <= 0) {
        return segment_start;
    }
    if (projection_parameter >= 1) {
        return segment_end;
    }
    else {
        return segment_start + (segment_end - segment_start) * projection_parameter;
    }
}

//计算点与直线的最近点（可能的接触点）返回最近点(可能的接触点)
inline oeVec2 SegmentNearestEndpoint(const float& projection_parameter, const oeVec2& segment_start, const oeVec2& segment_end) {
    if (projection_parameter <= 0) {
        return segment_start;
    }
    if (projection_parameter >= 1) {
        return segment_end;
    }
    else {
        return segment_start + (segment_end - segment_start) * projection_parameter;
    }
}

//计算点距离线段最近的端点的距离的平方
static float DisSqdPointSegmentNearestEndpoint(const oeVec2& point, const oeVec2& segment_start, const oeVec2& segment_end) {
    oeVec2 segment_vector = segment_end - segment_start;//计算线段向量
    oeVec2 point_to_segment_start_vector = point - segment_start;//计算从线段起点指向点的向量

    float projection_length = oeVec2::dot(point_to_segment_start_vector, segment_vector);
    float segment_length_squared = oeVec2::LengthSquared(segment_vector);
    float projection_parameter = projection_length / segment_length_squared;//计算投影参数 proj_param = (|AP||AB| * cos(θ)) / |AB|^2

    oeVec2 closest_point = SegmentNearestEndpoint(projection_parameter, segment_start, segment_end);
    float distance_squared = oeVec2::DistanceSquared(point, closest_point);

    return distance_squared;
}


//此命名空间包含了寻找物体与物体的接触点的函数
//构建此命名空间是为了代码模块化，使代码结构更清晰
namespace FindContactPoint {

    /*找到圆与圆碰撞时的接触点*/
    static oeVec2 FindCirclesContactPoint(const oeVec2& center_a, const float& radius_a, const oeVec2& center_b) {
        oeVec2 v_ab = center_b - center_a;
        v_ab.normalize();
        oeVec2 contact_point = center_a + v_ab * radius_a;
        return contact_point;
    }


    /*找到圆与多边形碰撞时的接触点*/
    static oeVec2 FindCirclePolygonContactPoint(const oeVec2& circle_center, const std::vector<oeVec2>& polygon_vertices) {
        float min_distance_Sq = std::numeric_limits<float>::max();
        oeVec2 contact_point;

        for (int i = 0; i < static_cast<int>(polygon_vertices.size()); i++) {
            oeVec2 v_a = polygon_vertices[i];
            oeVec2 v_b = polygon_vertices[(i + 1) % polygon_vertices.size()];

            float distance_sq = DisSqdPointSegmentNearestEndpoint(circle_center, v_a, v_b);

            if (distance_sq < min_distance_Sq) {
                min_distance_Sq = distance_sq;
                contact_point = GetSegmentContactPoint(circle_center, v_a, v_b);
            }
        }
        return contact_point;
    }

    //找到多边形与多边形的接触点
    static std::vector<oeVec2> FindPolygonsContactPoints(const std::vector<oeVec2>& vertices_a, const std::vector<oeVec2>& vertices_b) {
        //初始化接触点
        std::vector<oeVec2> contact_points;
        oeVec2 contact1;//第一个接触点
        oeVec2 contact2;//第二个接触点，如果没有第二个接触点，则保持为原点 
        size_t contact_count = 0;

        //初始化最小距离的平方为无穷大，用于后续比较
        float min_distancesq = std::numeric_limits<float>::max();

        //遍历第一个多边形的每个顶点，并检查它与第二个多边形每条边的距离  
        for (oeVec2 point : vertices_a) {

            for (size_t j = 0; j < vertices_b.size(); j++) {
                oeVec2 v_a = vertices_b[j];//第二个多边形的当前边的起点
                oeVec2 v_b = vertices_b[(j + 1) % vertices_b.size()];//第二个多边形的当前边的终点，使用模运算处理闭合多边形  
                float distancesq = DisSqdPointSegmentNearestEndpoint(point, v_a, v_b);
                oeVec2 cloesest_point = GetSegmentContactPoint(point, v_a, v_b);

                //如果当前距离的平方几乎等于最小距离的平方，并且新接触点不等于第一个接触点  
                if (oeVec2::NearlyEqual(distancesq, min_distancesq)) {
                    if (!oeVec2::NearlyEqualFv(cloesest_point, contact1)) {
                        contact2 = cloesest_point;
                        contact_count = 2;
                    }
                }
                else if (distancesq < min_distancesq) {
                    min_distancesq = distancesq;
                    contact_count = 1;
                    contact1 = cloesest_point;
                }
            }
        }
        // 对第二个多边形重复上述过程，以处理可能由于多边形复杂形状而遗漏的接触点
        for (oeVec2 point : vertices_b) {

            for (size_t j = 0; j < vertices_a.size(); j++) {
                oeVec2 v_a = vertices_a[j];//第二个多边形的当前边的起点
                oeVec2 v_b = vertices_a[(j + 1) % vertices_a.size()];//第二个多边形的当前边的终点，使用模运算处理闭合多边形  
                float distancesq = DisSqdPointSegmentNearestEndpoint(point, v_a, v_b);
                oeVec2 cloesest_point = GetSegmentContactPoint(point, v_a, v_b);


                //如果当前距离的平方几乎等于最小距离的平方，并且新接触点不等于第一个接触点
                if (oeVec2::NearlyEqual(distancesq, min_distancesq)) {
                    if (!oeVec2::NearlyEqualFv(cloesest_point, contact1)) {
                        contact2 = cloesest_point;
                        contact_count = 2;
                    }
                }
                else if (distancesq < min_distancesq) {
                    min_distancesq = distancesq;
                    contact_count = 1;
                    contact1 = cloesest_point;

                }
            }
        }

        //判断接触点个数
        if (contact_count == 1) {
            contact_points.push_back(contact1);
        }
        else {
            contact_points.push_back(contact1);
            contact_points.push_back(contact2);
        }


        return contact_points;
    }


};


    /*此函数将会判断2个物体的AABB是否相交*/
static bool IntersectAABB(oeAABB& a, oeAABB& b){
    // 检查 x 轴上的重叠
    if (a.max.x < b.min.x || a.min.x > b.max.x) return false;
    // 检查 y 轴上的重叠
    if (a.max.y < b.min.y || a.min.y > b.max.y) return false;

    // 如果以上条件都不满足，则两个 AABB 相交
    return true;
}


 

//
////找到两个碰撞物体的接触点
//static std::vector<oeVec2> FindContactPoints(oeBody& body_a, oeBody& body_b) {
//    std::vector<oeVec2> contact_points;
//    Shape shape_type_a = body_a.shape_;
//
//    Shape shape_type_b = body_b.shape_;
//    if (shape_type_a == POLTGON) {
//        if (shape_type_b == POLTGON) {
//            contact_points = FindContactPoint::FindPolygonsContactPoints(body_a.vertices_, body_b.vertices_);
//            return contact_points;
//        }
//        else if (shape_type_b == CIRCLE) {
//            contact_points.push_back(FindContactPoint::FindCirclePolygonContactPoint(body_b.mass_center_, body_a.vertices_));
//            return contact_points;
//        }
//    }
//    else if (shape_type_a == CIRCLE) {
//        if (shape_type_b == CIRCLE) {
//            contact_points.push_back(FindContactPoint::FindCirclesContactPoint(body_a.mass_center_, body_a.radius_, body_b.mass_center_));
//            return contact_points;
//        }
//        else if (shape_type_b == POLTGON) {
//            contact_points.push_back(FindContactPoint::FindCirclePolygonContactPoint(body_a.mass_center_, body_b.vertices_));
//            return contact_points;
//        }
//
//    }
//    return contact_points;
//}


//判断两个物体是否碰撞，返回物体的碰撞bool值，碰撞深度,碰撞向量
static IntersectData  Collide(oeBody& body_a, oeBody& body_b) {
    IntersectData intersect_data;
    Shape shape_type_a = body_a.shape_;
    Shape shape_type_b = body_b.shape_;

    if (shape_type_a == POLTGON) {
        if (shape_type_b == POLTGON) {
            
        }
        else if (shape_type_b == CIRCLE) {
            
        }
    }
    else if (shape_type_a == CIRCLE) {
        if (shape_type_b == CIRCLE) {
            
        }
        else if (shape_type_b == POLTGON) {
            
        }
    }

    return intersect_data;
}

