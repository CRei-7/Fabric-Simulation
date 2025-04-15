#pragma once

#include <iostream>
#include <glm/glm.hpp>
#include "Particle.h"
#include "Object.h"
#include "BVH.h"
#include <glad/glad.h>
#include "Model.h"

class NewCollision {
public:
    static bool isColliding;
    static int bvhCollisionChecks;
    static int collisionChecks;
    static bool checkTriangleTriangleIntersectionForModel(
        const glm::vec3& v1, const glm::vec3& v2, const glm::vec3& v3,  // Cloth triangle
        const glm::vec3& u1, const glm::vec3& u2, const glm::vec3& u3,  // Imported model triangle
        glm::vec3& intersectionPoint, glm::vec3& normal);

    static bool checkTriangleObjectIntersection(
        const glm::vec3& v1, const glm::vec3& v2, const glm::vec3& v3,
        const Object& object, glm::vec3& intersectionPoint, glm::vec3& normal);

    static bool trianglesIntersectSAT(
        const glm::vec3& v1, const glm::vec3& v2, const glm::vec3& v3,
        const glm::vec3& u1, const glm::vec3& u2, const glm::vec3& u3);

    static void projectTriangleOntoAxis(
        const glm::vec3& axis,
        const glm::vec3& p1, const glm::vec3& p2, const glm::vec3& p3,
        float& minProj, float& maxProj);

    static void resolveCollision(
        std::vector<Particle>& particles,
        BVH* clothBVH,
        const std::vector<GLuint>& triangleIndices,
        const Object& object,
        float deltaTime,
        std::vector<GLuint>& collidingIndices // Pass by reference);
    );
    static void resolveCollisionWithOutBVH(
        std::vector<Particle>& particles,
        const std::vector<GLuint>& triangleIndices,
        const Object& object,
        float deltaTime,
        std::vector<GLuint>& collidingIndices // Pass by reference);
    );
    // static void traverseBVHForModel(BVHNode* nodeA, StaticBVHNode* nodeB,
    //                                std::vector<GLuint>& clothTriangles,
    //                                std::vector<GLuint>& modelTriangles);
      static void traverseBVH(
          BVHNode* node,
          const Object& object,
          std::vector<GLuint>& potentialTriangles);
    // static void initializeBVH(const std::vector<Particle>& particles, const std::vector<GLuint>& triangleIndices);
    // static void updateBVH(const std::vector<Particle>& particles);

    // static void resolveCollisionForTriangle(
    //     Particle& p1, Particle& p2, Particle& p3,
    //     const glm::vec3& normal, const glm::vec3& intersectionPoint);
    static void traverseBothBVHs(BVHNode* nodeA,
        BVHNode* nodeB,
        std::vector<std::pair<GLuint, GLuint>> & potentialPairs );

    // static void resolveModelClothCollision(
    //     std::vector<Particle>& clothParticles,
    //     BVH* clothBVH,
    //     StaticBVH* modelBVH,
    //     float deltaTime,
    //     std::vector<GLuint>& collidingIndices);

    static void resolveParticleCollisionForModel(
        Particle& particle,
        const glm::vec3& normal);

private:
    static void resolveParticleCollision(
        Particle& particle,
        const glm::vec3& normal,
        float penetrationDepth);

    // static void traverseBVHForCollisions(
    //        BVHNode* node,
    //        const std::vector<Particle>& particles,
    //        const Object& object,
    //        std::vector<GLuint>& collidingIndices
    //    );
};
