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


    /*此函数将会判断2个物体的AABB是否相交*/
static bool IntersectAABB(oeAABB& a, oeAABB& b){
    // 检查 x 轴上的重叠
    if (a.max.x < b.min.x || a.min.x > b.max.x) return false;
    // 检查 y 轴上的重叠
    if (a.max.y < b.min.y || a.min.y > b.max.y) return false;

    // 如果以上条件都不满足，则两个 AABB 相交
    return true;
}


//找到多边形距离点最近的顶点
static size_t FindVertexClosestPoint(const oeVec2& point, const oeVec2 vertices[],const int vertices_count) {
    size_t result = 0;
    float min_distance = std::numeric_limits<float>::max();
    for (size_t i = 0; i < vertices_count; ++i) {
        float distance = oeVec2::Distance(vertices[i], point);
        if (distance <= min_distance) {
            min_distance = distance;
            result = i;
        }
    }
    return result;
}

//计算多边形在轴上的投影
static oeVec2 ProjectVertices(const oeVec2& axis, const oeVec2 vertices[], int vertices_count) {
    float min_proj = std::numeric_limits<float>::max();
    float max_proj = -std::numeric_limits<float>::max();

    for (int i = 0; i < vertices_count; ++i) {
        float projection = axis.dot(vertices[i]);
        min_proj = std::min(min_proj, projection);
        max_proj = std::max(max_proj, projection);
    }

    return oeVec2(min_proj, max_proj);
}



/*圆与圆碰撞检测*/
static IntersectData IntersectCircles(const oeVec2& center_a, const float& radius_a, const oeVec2& center_b, const float& radius_b) {
    IntersectData data;
    float both_distance = oeVec2::Distance(center_a, center_b);
    float sum_of_radii = radius_a + radius_b;

    if (both_distance >= sum_of_radii) {
        return data;
    }

    //计算单位方向向量
    oeVec2 unit_direction_vector_ab = center_b - center_a;
    unit_direction_vector_ab.normalize();

    //更新data
    data.Collision = true;
    data.normal = unit_direction_vector_ab;
    data.depth = sum_of_radii - both_distance;

    return data;
}


// 计算圆在某个轴上的投影
static oeVec2 ProjectCircle(const oeVec2& center, const float& radius, const oeVec2& axis) {
    float center_projection = axis.dot(center);
    return oeVec2(center_projection - radius, center_projection + radius);
}

// 判断两个区间是否重叠
static bool overlaps(const oeVec2& interval_a, const oeVec2& interval_b) {
    return !(interval_a.y < interval_b.x || interval_b.y < interval_a.x);
}
static IntersectData IntersectCirclePolygon(
    const oeVec2& circle_center, const float& circle_radius,
    const oeVec2 polygon_mass_center, const oeVec2 polygon_vertices[], const int vertices_count
) {
    IntersectData data;
    data.depth = std::numeric_limits<float>::max();
    data.Collision = true;  // 默认假设发生碰撞

    // 计算圆形在任意轴上的投影
    auto ProjectCircle = [&](const oeVec2& axis) -> oeVec2 {
        float projection = oeVec2::dot(circle_center, axis);
        return oeVec2(projection - circle_radius, projection + circle_radius);
    };

    // 判断两个投影区间是否重叠
    auto Overlaps = [](const oeVec2& proj_a, const oeVec2& proj_b) -> bool {
        return !(proj_a.y < proj_b.x || proj_b.y < proj_a.x);
    };

    // 遍历多边形的每条边，检查边的法向量作为分离轴
    for (int i = 0; i < vertices_count && data.Collision; ++i) {
        oeVec2 v_a = polygon_vertices[i];
        oeVec2 v_b = polygon_vertices[(i + 1) % vertices_count];

        // 计算边的法向量（逆时针顺序，法向量指向外部）
        oeVec2 edge_vector = v_b - v_a;
        oeVec2 axis = oeVec2(-edge_vector.y, edge_vector.x); // 左手法则
        axis.normalize();

        // 调试输出
        std::cout << "Edge from " << v_a << " to " << v_b << "\n";
        std::cout << "Normal: " << axis << "\n";

        // 计算多边形和圆在该轴上的投影
        oeVec2 project_polygon = ProjectVertices(axis, polygon_vertices, vertices_count);
        oeVec2 project_circle = ProjectCircle(axis);

        // 判断投影是否重叠，如果不重叠说明没发生碰撞
        if (!Overlaps(project_polygon, project_circle)) {
            data.Collision = false;
            return data;  // 没有重叠，返回
        }

        // 如果重叠，计算投影的重叠深度
        float axis_depth = std::min(project_circle.y - project_polygon.x, project_polygon.y - project_circle.x);

        // 如果深度小于当前最小深度，则更新法向量和深度
        if (axis_depth < data.depth) {
            data.depth = axis_depth;
            data.normal = axis;
        }
    }

    // 如果所有轴上都有重叠，设置碰撞结果
    if (data.Collision) {
        oeVec2 direction_vector = circle_center - polygon_mass_center;

        // 如果法向量需要反转，确保最终结果正确
        if (oeVec2::dot(direction_vector, data.normal) < 0) {
            data.normal = -data.normal;
        }
    }

    return data;
}
static IntersectData IntersectPolygons(
    const oeVec2& mass_center_a, const oeVec2* vertices_a, int verticesA_count,
    const oeVec2& mass_center_b, const oeVec2* vertices_b, int verticesB_count
) {
    // 检查输入的多边形是否有足够的顶点
    if (verticesA_count < 3 || verticesB_count < 3) {
        IntersectData data = { false, 0.0f, oeVec2(0, 0) };
        return data;
    }

    IntersectData data = { true, std::numeric_limits<float>::max(), oeVec2(0, 0) };

    auto CheckAxis = [&](const oeVec2& axis) -> bool {
        // 计算多边形在该轴上的投影
        oeVec2 proj_a = ProjectVertices(axis, vertices_a, verticesA_count);
        oeVec2 proj_b = ProjectVertices(axis, vertices_b, verticesB_count);

        // 判断投影是否重叠
        if (proj_a.y < proj_b.x || proj_b.y < proj_a.x) {
            data.Collision = false;
            return false;
        }

        // 计算投影的重叠深度
        float axis_depth = std::min(proj_b.y - proj_a.x, proj_a.y - proj_b.x);

        // 如果深度小于当前最小深度，则更新法向量和深度
        if (axis_depth < data.depth) {
            data.depth = axis_depth;
            data.normal = axis;
        }
        return true;
        };

    // 遍历多边形A的每条边
    for (int i = 0; i < verticesA_count && data.Collision; ++i) {
        oeVec2 v_a = vertices_a[i];
        oeVec2 v_b = vertices_a[(i + 1) % verticesA_count];

        // 计算边的法向量（逆时针顺序，法向量指向外部）
        oeVec2 edge_vector = v_b - v_a;
        oeVec2 axis = oeVec2(-edge_vector.y, edge_vector.x); // 左手法则
        axis.normalize();

        // 检查是否重叠
        if (!CheckAxis(axis)) {
            return data; // 没有重叠，返回
        }
    }

    // 遍历多边形B的每条边
    for (int i = 0; i < verticesB_count && data.Collision; ++i) {
        oeVec2 v_a = vertices_b[i];
        oeVec2 v_b = vertices_b[(i + 1) % verticesB_count];

        // 计算边的法向量（逆时针顺序，法向量指向外部）
        oeVec2 edge_vector = v_b - v_a;
        oeVec2 axis = oeVec2(-edge_vector.y, edge_vector.x); // 左手法则
        axis.normalize();

        // 检查是否重叠
        if (!CheckAxis(axis)) {
            return data; // 没有重叠，返回
        }
    }

    // 如果所有轴上都有重叠，设置碰撞结果
    if (data.Collision) {
        oeVec2 direction_vector = mass_center_b - mass_center_a;

        // 如果法向量需要反转，确保最终结果正确
        if (oeVec2::dot(direction_vector, data.normal) < 0) {
            data.normal = -data.normal;
        }
    }

    return data;
}

static IntersectData  Collide(oeBody& body_a, oeBody& body_b) {
    IntersectData intersect_data;
    Shape shape_type_a = body_a.shape_;
    Shape shape_type_b = body_b.shape_;

    if (shape_type_a == BOX || shape_type_a == POLTGON) {
        if (shape_type_b == BOX || shape_type_b == POLTGON) {
            intersect_data = IntersectPolygons(body_a.mass_center_, body_a.vertices_,body_a.vertices_count_,
                                               body_b.mass_center_, body_b.vertices_,body_b.vertices_count_);
            
            return intersect_data;
        }
        else if (shape_type_b == CIRCLE) {
            intersect_data = IntersectCirclePolygon(body_b.mass_center_, body_b.radius_or_half_width_, body_a.mass_center_, body_a.vertices_,body_a.vertices_count_);
            return intersect_data;
        }
    }
    else if (shape_type_a == CIRCLE) {
        if (shape_type_b == CIRCLE) {
            intersect_data =IntersectCircles(body_a.mass_center_, body_a.radius_or_half_width_, body_b.mass_center_, body_b.radius_or_half_width_);
            return intersect_data;
        }
        else if (shape_type_b == POLTGON|| shape_type_b == BOX) {
            intersect_data = IntersectCirclePolygon(body_a.mass_center_, body_a.radius_or_half_width_, body_b.mass_center_, body_b.vertices_, body_b.vertices_count_);
            return intersect_data;
        }
    }

    return intersect_data;
}

