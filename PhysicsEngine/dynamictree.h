#pragma once
#include "body.h"
#include <vector>
#include <unordered_map>
#include <algorithm>
#include "collision.h"

class DynamicTree {
public:
    struct Node {
        oeAABB aabb;
        int parentId = -1;
        int leftId = -1;
        int rightId = -1;
        void* userData = nullptr;
        bool isLeaf = true;
    };

    DynamicTree() : nodes_(), freeList_() {}

    // 插入AABB并返回节点ID
    int Insert(const oeAABB& aabb, void* userData) {
        int nodeId = AllocateNode();
        nodes_[nodeId] = { aabb, -1, -1, -1, userData, true };
        UpdateNode(nodeId);
        return nodeId;
    }

    // 移除节点
    void Remove(int nodeId) {
        if (nodes_[nodeId].isLeaf) {
            FreeNode(nodeId);
        }
        else {
            // 如果是非叶子节点，递归删除子节点
            Remove(nodes_[nodeId].leftId);
            Remove(nodes_[nodeId].rightId);
            FreeNode(nodeId);
        }
    }

    // 更新节点的AABB
    void Update(int nodeId, const oeAABB& aabb) {
        if (!nodes_[nodeId].isLeaf) {
            // 非叶子节点不需要更新AABB
            return;
        }
        nodes_[nodeId].aabb = aabb;
        UpdateNode(nodeId);
    }

    // 查询与给定AABB相交的所有节点
    std::vector<int> Query(const oeAABB& aabb) {
        std::vector<int> result;
        QueryRec(0, aabb, result);
        return result;
    }
    void* GetUserData(int nodeId) {
        if (nodeId >= 0 && nodeId < static_cast<int>(nodes_.size())) {
            return nodes_[nodeId].userData;
        }
        return nullptr;
    }
private:
    std::vector<Node> nodes_;
    std::vector<int> freeList_;

    int AllocateNode() {
        if (!freeList_.empty()) {
            int id = freeList_.back();
            freeList_.pop_back();
            return id;
        }
        nodes_.push_back(Node());
        return static_cast<int>(nodes_.size()) - 1;
    }

    void FreeNode(int nodeId) {
        nodes_[nodeId] = Node();
        freeList_.push_back(nodeId);
    }

    void UpdateNode(int nodeId) {
        while (nodeId != -1) {
            Node& node = nodes_[nodeId];
            if (!node.isLeaf) {
                // 更新非叶子节点的AABB
                const Node& left = nodes_[node.leftId];
                const Node& right = nodes_[node.rightId];
                node.aabb.min.x = std::min(left.aabb.min.x, right.aabb.min.x);
                node.aabb.min.y = std::min(left.aabb.min.y, right.aabb.min.y);
                node.aabb.max.x = std::max(left.aabb.max.x, right.aabb.max.x);
                node.aabb.max.y = std::max(left.aabb.max.y, right.aabb.max.y);
            }
            nodeId = node.parentId;
        }
    }

    void QueryRec(int nodeId,const oeAABB& aabb, std::vector<int>& result) {
        if (nodeId == -1 || !Intersect::IntersectAABB(aabb, nodes_[nodeId].aabb)) {
            return;
        }
        if (nodes_[nodeId].isLeaf) {
            result.push_back(nodeId);
        }
        else {
            QueryRec(nodes_[nodeId].leftId, aabb, result);
            QueryRec(nodes_[nodeId].rightId, aabb, result);
        }
    }
   
};

