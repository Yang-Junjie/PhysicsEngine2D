// AABBTree.h
#pragma once
#include <vector>
#include "AABBTreeNode.h"
#include "body.h"
class AABBTree {
public:
    // 插入一个新的物体到树中
    void Insert(const oeAABB& aabb, int id);

    // 从树中移除一个物体
    void Remove(int id);

    // 查询与给定AABB相交的所有物体ID
    void Query(const oeAABB& aabb, std::vector<int>& potential_contacts);

    // 重新平衡树（可选）
  //  void Rebalance();

private:
    AABBTreeNode* root_ = nullptr;  // 树的根节点

    // 辅助函数：递归插入节点
    AABBTreeNode* InsertRecursive(AABBTreeNode* node, const oeAABB& aabb, int id);

    // 辅助函数：递归移除节点
    AABBTreeNode* RemoveRecursive(AABBTreeNode* node, int id);

    // 辅助函数：递归查询
    void QueryRecursive(const AABBTreeNode* node, const oeAABB& aabb, std::vector<int>& potential_contacts);

    // 辅助函数：计算两个AABB的合并AABB
    oeAABB MergeAABB(const oeAABB& aabb1, const oeAABB& aabb2);
};