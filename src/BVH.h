#pragma once
#include <glm/glm.hpp>
#include <vector>
#include "Particle.h"
#include "AABB.h"


class BVHNode {
public:
    AABB aabb;
    std::vector<GLuint> triangleIndices;
    BVHNode* left = nullptr;
    BVHNode* right = nullptr;

    ~BVHNode();
    void refit(const std::vector<Particle>& particles);
    bool isLeaf() const;
    void query(const AABB& queryAABB, std::vector<size_t>& results) const;
};

class BVH {
public:
    BVHNode* root;
    const std::vector<Particle>& particles;

    BVH(const std::vector<Particle>& particles, const std::vector<GLuint>& triangleIndices);
    ~BVH();
    void refit();
    void query(const AABB& queryAABB, std::vector<size_t>& results) const;
    static bool checkAABBOverlap(const AABB& a, const AABB& b);

private:
    BVHNode* build(const std::vector<GLuint>& triangleIndices);
    AABB computeCentroidAABB(const std::vector<GLuint>& triangleIndices);
};