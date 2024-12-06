#pragma once

#include <vector>
#include "Body.h"
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

struct ContactData {
    int contact_count = 0;
    oeVec2 contact1 = {0.0f,0.0f};
    oeVec2 contact2 = { 0.0f,0.0f };;
};


struct Manifold {
    oeBody* body_a = nullptr;
    oeBody* body_b = nullptr;
    ContactData contact_data;
    IntersectData intersect_data;
};

static float PointSegmentDistance(const oeVec2& p, const oeVec2& a, const oeVec2& b, oeVec2& cp) {
    oeVec2 ab = b - a;
    oeVec2 ap = p - a;
    float proj = oeVec2::dot(ap, ab);
    float abLenSq = oeVec2::LengthSquared(ab);
    float d = proj / abLenSq;

    if (d <= 0) {
        cp = a;
    }
    else if (d >= 1) {
        cp = b;
    }
    else {
        cp = a + ab * d;
    }

    return oeVec2::DistanceSquared(p, cp);
}

static ContactData FindPolygonsContactPoints(const oeVec2 verticesA[],const int verticesA_count, const oeVec2 verticesB[],const int verticesB_count) {
    ContactData result;
    result.contact_count = 0;
    float minDistSq = std::numeric_limits<float>::infinity();

    for (int i = 0; i < verticesA_count;i++) {
        for (size_t j = 0; j < verticesB_count; ++j) {
            oeVec2 va = verticesB[j];
            oeVec2 vb = verticesB[(j + 1) % verticesB_count];

            oeVec2 cp;
            float distSq = PointSegmentDistance(verticesA[i], va, vb, cp);

            if (std::abs(distSq - minDistSq) < 1e-6f) { // NearlyEqual
                if (!cp.NearlyEqual(result.contact1)) {
                    result.contact2 = cp;
                    result.contact_count = 2;
                }
            }
            else if (distSq < minDistSq) {
                minDistSq = distSq;
                result.contact_count = 1;
                result.contact1 = cp;
            }
        }
    }

    for (int i = 0; i < verticesB_count; i++) {
        for (size_t j = 0; j < verticesA_count; ++j) {
            oeVec2 va = verticesA[j];
            oeVec2 vb = verticesA[(j + 1) % verticesA_count];

            oeVec2 cp;
            float distSq = PointSegmentDistance(verticesB[i], va, vb, cp);

            if (std::abs(distSq - minDistSq) < 1e-6f) { // NearlyEqual
                if (!cp.NearlyEqual(result.contact1)) {
                    result.contact2 = cp;
                    result.contact_count = 2;
                }
            }
            else if (distSq < minDistSq) {
                minDistSq = distSq;
                result.contact_count = 1;
                result.contact1 = cp;
            }
        }
    }

    return result;
}

static oeVec2 FindCirclePolygonContactPoint(const oeVec2& circle_center, const oeVec2 polygon_vertices[],const int vertices_count) {
    float minDistSq = std::numeric_limits<float>::infinity();
    oeVec2 cp;

    for (size_t i = 0; i < vertices_count; ++i) {
        oeVec2 va = polygon_vertices[i];
        oeVec2 vb = polygon_vertices[(i + 1) % vertices_count];

        oeVec2 temp_cp;
        float distSq = PointSegmentDistance(circle_center, va, vb, temp_cp);
        if (distSq < minDistSq) {
            minDistSq = distSq;
            cp = temp_cp;
        }
    }

    return cp;
}

static oeVec2 FindCirclesContactPoint(const oeVec2& centerA, float radiusA, const oeVec2& centerB) {
    oeVec2 v_AB = oeVec2::normalize(centerB - centerA);
    return centerA + v_AB * radiusA;
}

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
static int FindVertexClosestPoint(const oeVec2& point, const oeVec2 vertices[],const int vertices_count) {
    int result = 0;
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

// 计算圆在某个轴上的投影
static oeVec2 ProjectCircle(const oeVec2& center, const float& radius, const oeVec2& axis) {
    float center_projection = axis.dot(center);
    return oeVec2(center_projection - radius, center_projection + radius);
}

// 判断两个区间是否重叠
static bool overlaps(const oeVec2& interval_a, const oeVec2& interval_b) {
    return !(interval_a.y < interval_b.x || interval_b.y < interval_a.x);
}


namespace Intersect {
   
    namespace SAT {
       
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
        

        static IntersectData IntersectCircleToPolygon(
            const oeVec2& circle_center, float circle_radius,
            const oeVec2& polygon_mass_center, const oeVec2 polygon_vertices[], int vertices_count,
            bool reverse_normal = false
        ) {
            IntersectData data;
            data.normal = { 0.0f, 0.0f };
            data.depth = std::numeric_limits<float>::max();

            // 遍历多边形的每条边
            for (size_t i = 0; i < vertices_count; ++i) {
                oeVec2 va = polygon_vertices[i];
                oeVec2 vb = polygon_vertices[(i + 1) % vertices_count];

                // 计算边的法向量
                oeVec2 edge = vb - va;
                oeVec2 axis(-edge.y, edge.x);
                axis.normalize();
                
               
                // 计算多边形和圆在该轴上的投影
                auto project_vertices = ProjectVertices(axis, polygon_vertices, vertices_count);
                auto project_circle = ProjectCircle(circle_center, circle_radius, axis);

                // 判断投影是否重叠，如果不重叠则返回False
                if (!overlaps(project_vertices, project_circle)) {
                    data.Collision = false;
                    return data;
                }

                // 计算投影的重叠深度
                float axis_depth = std::min(project_circle.y - project_vertices.x, project_vertices.y - project_circle.x);

                // 如果深度小于当前最小深度，则更新法向量和深度
                if (axis_depth < data.depth) {
                    data.depth = axis_depth;
                    data.normal = axis;
                }
               
            }

          
            // 计算圆心到接触点的方向向量
            oeVec2 direction = polygon_mass_center - circle_center;
            
           
            // 确保法向量指向从圆形到多边形的方向
            if (direction.dot(data.normal) < 0) {
                data.normal = -data.normal;
            }
          
            //// 如果需要反转法向量，则进行反转
            if (reverse_normal) {
                data.normal = -data.normal;
            }
          
            // 返回True和法向量和深度
            data.Collision = true;
            return data;
        }


        static IntersectData IntersectPolygonToCircle(
            const oeVec2& polygon_mass_center, const oeVec2 polygon_vertices[], int vertices_count,
            const oeVec2& circle_center, float circle_radius) 
        {
            return IntersectCircleToPolygon(circle_center, circle_radius, polygon_mass_center, polygon_vertices, vertices_count,true);
        }


        /*多边形与多边形碰撞检测*/
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
    }
    namespace GJK {

        struct Simplex {
            std::vector<oeVec2> points;

            void Add(const oeVec2& point) {
                points.push_back(point);
            }

            void RemoveLast() {
                if (!points.empty()) {
                    points.pop_back();
                }
            }

            bool GetClosestOutPoint(oeVec2& out, const oeVec2& direction) {
                if (points.size() == 1) {
                    out = points[0];
                    return true;
                }
                else if (points.size() == 2) {
                    oeVec2 a = points[0];
                    oeVec2 b = points[1];
                    oeVec2 ab = b - a;
                    float dotABDir = ab.dot(direction);

                    if (dotABDir > 0.0f) {
                        out = b;
                        return true;
                    }
                    else if (dotABDir < 0.0f) {
                        out = a;
                        return true;
                    }
                    else {
                        out = a;
                        return false;
                    }
                }
                else if (points.size() == 3) {
                    oeVec2 a = points[0];
                    oeVec2 b = points[1];
                    oeVec2 c = points[2];

                    oeVec2 ab = b - a;
                    oeVec2 ac = c - a;
                    oeVec2 bc = c - b;

                    oeVec2 ao = -a;
                    oeVec2 bo = -b;
                    oeVec2 co = -c;

                    if (ab.cross(ao) * ac.cross(ao) >= 0.0f &&
                        ab.cross(bc) * ab.cross(bo) >= 0.0f &&
                        ac.cross(bc) * ac.cross(co) >= 0.0f) {
                        return false;
                    }

                    float distAB = ab.cross(ao);
                    float distAC = ac.cross(ao);
                    float distBC = bc.cross(bo);

                    if (distAB <= 0.0f) {
                        out = a;
                        return true;
                    }
                    if (distAC >= 0.0f) {
                        out = c;
                        return true;
                    }
                    if (distBC <= 0.0f) {
                        out = b;
                        return true;
                    }

                    return false;
                }
                return false;
            }

            bool ContainsOrigin() {
                if (points.size() == 3) {
                    oeVec2 a = points[0];
                    oeVec2 b = points[1];
                    oeVec2 c = points[2];

                    oeVec2 ao = -a;
                    oeVec2 bo = -b;
                    oeVec2 co = -c;

                    if (oeVec2::cross(b - a, ao) * oeVec2::cross(c - a, ao) >= 0.0f &&
                        oeVec2::cross(c - b, bo) * oeVec2::cross(a - b, bo) >= 0.0f &&
                        oeVec2::cross(a - c, co) * oeVec2::cross(b - c, co) >= 0.0f) {
                        return true;
                    }
                }
                return false;
            }
        };

        static oeVec2 Support(const oeVec2* verticesA, int verticesA_count,
            const oeVec2* verticesB, int verticesB_count,
            const oeVec2& direction) {
            oeVec2 farthestA = verticesA[0];
            float maxDotA = farthestA.dot(direction);
            for (int i = 1; i < verticesA_count; ++i) {
                oeVec2 v = verticesA[i];
                float dot = v.dot(direction);
                if (dot > maxDotA) {
                    maxDotA = dot;
                    farthestA = v;
                }
            }

            oeVec2 farthestB = verticesB[0];
            float minDotB = farthestB.dot(-direction);
            for (int i = 1; i < verticesB_count; ++i) {
                oeVec2 v = verticesB[i];
                float dot = v.dot(-direction);
                if (dot < minDotB) {
                    minDotB = dot;
                    farthestB = v;
                }
            }

            return farthestA - farthestB;
        }

        static bool EPASolve(Simplex& simplex, const oeVec2* verticesA, int verticesA_count,
            const oeVec2* verticesB, int verticesB_count,
            oeVec2& normal, float& depth) {
            while (simplex.points.size() < 3) {
                oeVec2 closestPoint;
                if (!simplex.GetClosestOutPoint(closestPoint, -closestPoint)) {
                    normal = -closestPoint;
                    normal.normalize();
                    depth = 0.0f;
                    return true;
                }
                oeVec2 support = Support(verticesA, verticesA_count, verticesB, verticesB_count, -closestPoint);
                simplex.Add(support);
            }

            while (true) {
                size_t bestIndex = 0;
                float minDistance = std::numeric_limits<float>::max();
                for (size_t i = 0; i < simplex.points.size(); ++i) {
                    oeVec2 p1 = simplex.points[i];
                    oeVec2 p2 = simplex.points[(i + 1) % simplex.points.size()];
                    oeVec2 edge = p2 - p1;
                    oeVec2 faceNormal(-edge.y, edge.x);
                    faceNormal.normalize();

                    float distance = -faceNormal.dot(p1); // d = -N·P

                    if (distance < minDistance) {
                        minDistance = distance;
                        bestIndex = i;
                        normal = faceNormal;
                    }
                }

                if (minDistance >= 0.0f) {
                    depth = minDistance;
                    return true;
                }

                oeVec2 support = Support(verticesA, verticesA_count, verticesB, verticesB_count, normal);
                float distanceToSupport = normal.dot(support);

                if (distanceToSupport <= minDistance) {
                    depth = -minDistance;
                    return true;
                }

                simplex.Add(support);
            }
        }

        static IntersectData GJKIntersectPolygons(
            const oeVec2* verticesA, int verticesA_count,
            const oeVec2* verticesB, int verticesB_count
        ) {
            IntersectData data = { false, 0.0f, oeVec2(0, 0) };

            // 初始方向为从 A 的质心指向 B 的质心
            oeVec2 direction = verticesB[0] - verticesA[0];
            if (direction.LengthSquared() == 0.0f) {
                direction = oeVec2(1, 0); // 避免除以零的情况
            }
            else {
                direction.normalize();
            }

            Simplex simplex;
            oeVec2 supportPoint;

            while (true) {
                supportPoint = Support(verticesA, verticesA_count, verticesB, verticesB_count, -direction);

                if (supportPoint.dot(direction) <= 0.0f) {
                    return data;
                }

                simplex.Add(supportPoint);

                if (simplex.ContainsOrigin()) {
                    // 使用 EPA 算法获取碰撞法线和深度
                    if (EPASolve(simplex, verticesA, verticesA_count, verticesB, verticesB_count, data.normal, data.depth)) {
                        data.Collision = true;
                        return data;
                    }
                }

                oeVec2 closestPoint;
                if (!simplex.GetClosestOutPoint(closestPoint, -supportPoint)) {
                    data.Collision = true;
                    data.normal = -closestPoint;
                    data.normal.normalize();
                    data.depth = 0.0f;
                    return data;
                }
                direction = -closestPoint;
                if (direction.LengthSquared() == 0.0f) {
                    return data;
                }
                direction.normalize();
            }
        }
    }
    
}


static ContactData FindContactPoints(const oeBody& body_a, const oeBody& body_b) {
    ContactData result;

    Shape shape_type_a = body_a.shape_;
    Shape shape_type_b = body_b.shape_;
    

    if (shape_type_a == POLYGON) {
        if (shape_type_b == POLYGON) {
            result = FindPolygonsContactPoints(body_a.vertices_,body_a.vertices_count_,body_b.vertices_,body_b.vertices_count_);
        }
        else if (shape_type_b == CIRCLE) {
            result.contact1 = FindCirclePolygonContactPoint(body_b.mass_center_, body_a.vertices_,body_a.vertices_count_);
            result.contact_count = 1;
        }
    }
    else if (shape_type_a == CIRCLE) {
        if (shape_type_b == POLYGON) {
            result.contact1 = FindCirclePolygonContactPoint(body_a.mass_center_, body_b.vertices_,body_b.vertices_count_);
            result.contact_count = 1;
        }
        else if (shape_type_b == CIRCLE) {
            result.contact1 = FindCirclesContactPoint(body_a.mass_center_, body_a.radius_,body_b.mass_center_);
            result.contact_count = 1;
        }
    }

    return result;
}
static IntersectData  Collide(oeBody& body_a, oeBody& body_b) {
    IntersectData intersect_data;
    Shape shape_type_a = body_a.shape_;
    Shape shape_type_b = body_b.shape_;
    
  
    if ( shape_type_a == POLYGON) {
        if ( shape_type_b == POLYGON) {
            intersect_data = Intersect::SAT::IntersectPolygons(body_a.mass_center_, body_a.vertices_,body_a.vertices_count_,
                                               body_b.mass_center_, body_b.vertices_,body_b.vertices_count_);
            return intersect_data;
        }
        else if (shape_type_b == CIRCLE) {
            intersect_data = Intersect::SAT::IntersectPolygonToCircle( body_a.mass_center_, body_a.vertices_,body_a.vertices_count_, body_b.mass_center_, body_b.radius_);
            return intersect_data;
        }
    }
    else if (shape_type_a == CIRCLE) {
        if (shape_type_b == CIRCLE) {
            intersect_data = Intersect::SAT::IntersectCircles(body_a.mass_center_, body_a.radius_, body_b.mass_center_, body_b.radius_);
            return intersect_data;
        }
        else if (shape_type_b == POLYGON) {
            intersect_data = Intersect::SAT::IntersectCircleToPolygon(body_a.mass_center_, body_a.radius_, body_b.mass_center_, body_b.vertices_, body_b.vertices_count_);
            return intersect_data;
        }
    }

    return intersect_data;
}

