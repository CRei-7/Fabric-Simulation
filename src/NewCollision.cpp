// face  to face collision detection
#include "NewCollision.h"
#include <iostream>
#include "Particle.h"
#include "Object.h"
#include "BVH.h"

bool NewCollision::isColliding = false;
int NewCollision::bvhCollisionChecks = 0;
int NewCollision::collisionChecks = 0;
float NewCollision::offset = 0.01f;

bool NewCollision::checkTriangleObjectIntersection(
    const glm::vec3& v1, const glm::vec3& v2, const glm::vec3& v3,
    const Object& object, glm::vec3& intersectionPoint, glm::vec3& normal) {

    // Calculate triangle normal first - this is needed for both cube and sphere
    glm::vec3 edge1 = v2 - v1;
    glm::vec3 edge2 = v3 - v1;
    normal = -1.0f * glm::normalize(glm::cross(edge1, edge2));

    if (object.isCube()) {
        // Get cube bounds
        glm::vec3 center = object.getCenter();
        float halfLength = object.getHalfLength() + offset;
        glm::vec3 cubeMin = center - glm::vec3(halfLength);
        glm::vec3 cubeMax = center + glm::vec3(halfLength);

        // Calculate triangle bounds
        glm::vec3 triangleMin = glm::min(glm::min(v1, v2), v3);
        glm::vec3 triangleMax = glm::max(glm::max(v1, v2), v3);

        // Simple AABB overlap test first
        if (triangleMax.x < cubeMin.x || triangleMin.x > cubeMax.x ||
            triangleMax.y < cubeMin.y || triangleMin.y > cubeMax.y ||
            triangleMax.z < cubeMin.z || triangleMin.z > cubeMax.z) {
            return false;
        }

        // If any vertex is inside the cube, we have collision
        auto isInside = [&](const glm::vec3& p) {
            return p.x >= cubeMin.x && p.x <= cubeMax.x &&
                    p.y >= cubeMin.y && p.y <= cubeMax.y &&
                    p.z >= cubeMin.z && p.z <= cubeMax.z;
        };

        if (isInside(v1) || isInside(v2) || isInside(v3)) {
            // Use the center of the triangle as intersection point
            intersectionPoint = (v1 + v2 + v3) / 3.0f;
            // std::cout << "Collision detected with triangle vertices: "
            //                 << "v1(" << v1.x << ", " << v1.y << ", " << v1.z << "), "
            //                 << "v2(" << v2.x << ", " << v2.y << ", " << v2.z << "), "
            //                 << "v3(" << v3.x << ", " << v3.y << ", " << v3.z << ")\n";
            return true;
        }

        return false;
    }
    else if (object.isSphere()) {
        glm::vec3 center = object.getCenter();
        float radius = object.getHalfLength() + offset;

        // Check if any vertex is inside the sphere
        auto isInsideSphere = [&](const glm::vec3& p) {
            return glm::length(p - center) <= radius;
        };

        if (isInsideSphere(v1) || isInsideSphere(v2) || isInsideSphere(v3)) {
            intersectionPoint = (v1 + v2 + v3) / 3.0f;
            // std::cout << "Collision detected with triangle vertices: "
            //                 << "v1(" << v1.x << ", " << v1.y << ", " << v1.z << "), "
            //                 << "v2(" << v2.x << ", " << v2.y << ", " << v2.z << "), "
            //                 << "v3(" << v3.x << ", " << v3.y << ", " << v3.z << ")\n";
            return true;
        }

        return false;
    }
    return false;
}


// bool NewCollision::trianglesIntersectSAT(
//     const glm::vec3& v1, const glm::vec3& v2, const glm::vec3& v3,
//     const glm::vec3& u1, const glm::vec3& u2, const glm::vec3& u3)
// {
//     glm::vec3 axes[] = {
//         glm::normalize(glm::cross(v2 - v1, v3 - v1)),  // Normal of triangle 1
//         glm::normalize(glm::cross(u2 - u1, u3 - u1))   // Normal of triangle 2
//     };

//     // Check projections onto each axis
//     for (int i = 0; i < 2; i++) {
//         float min1, max1, min2, max2;
//         projectTriangleOntoAxis(axes[i], v1, v2, v3, min1, max1);
//         projectTriangleOntoAxis(axes[i], u1, u2, u3, min2, max2);
//         if (max1 < min2 || max2 < min1) return false; // No overlap, no collision
//     }

//     return true;
// }

// bool NewCollision::checkTriangleTriangleIntersectionForModel(
//     const glm::vec3& v1, const glm::vec3& v2, const glm::vec3& v3,  // Cloth triangle
//     const glm::vec3& u1, const glm::vec3& u2, const glm::vec3& u3,  // Imported model triangle
//     glm::vec3& intersectionPoint, glm::vec3& normal)
// {
//     normal = glm::normalize(glm::cross(v2 - v1, v3 - v1)); // Cloth triangle normal

//     // Check for triangle overlap using SAT (Separating Axis Theorem)
//     if (!trianglesIntersectSAT(v1, v2, v3, u1, u2, u3))
//         return false;

//     // If overlapping, estimate the intersection point as the midpoint of closest points
//     intersectionPoint = (v1 + v2 + v3 + u1 + u2 + u3) / 6.0f;
//     return true;
// }


// void NewCollision::traverseBVHForModel(BVHNode* nodeA, StaticBVHNode* nodeB,
//                                std::vector<GLuint>& clothTriangles,
//                                std::vector<GLuint>& modelTriangles) {
//     if (!nodeA || !nodeB) return;

//     // Check if AABB of cloth and model intersect
//     if (!nodeA->aabb.intersects(nodeB->aabb)) return;

//     // If both are leaves, collect their triangle indices
//     if (nodeA->isLeaf() && nodeB->isLeaf()) {
//         clothTriangles.insert(clothTriangles.end(),
//                               nodeA->triangleIndices.begin(), nodeA->triangleIndices.end());
//         modelTriangles.insert(modelTriangles.end(),
//                               nodeB->triangleIndices.begin(), nodeB->triangleIndices.end());
//         return;
//     }

//     // Recursively traverse both BVH trees
//     if (!nodeA->isLeaf()) {
//         traverseBVHForModel(nodeA->left, nodeB, clothTriangles, modelTriangles);
//         traverseBVHForModel(nodeA->right, nodeB, clothTriangles, modelTriangles);
//     }

//     if (!nodeB->isLeaf()) {
//         traverseBVHForModel(nodeA, nodeB->left, clothTriangles, modelTriangles);
//         traverseBVHForModel(nodeA, nodeB->right, clothTriangles, modelTriangles);
//     }
// }
void NewCollision::resolveCollision(
    std::vector<Particle>& particles,
    BVH* clothBVH,
    const std::vector<GLuint>& triangleIndices,
    const Object& object,
    float deltaTime,
    std::vector<GLuint>& collidingIndices, // Pass by reference
    float StaticFriction, float KineticFriction
    ) {

    // Add debug print
    // std::cout << "Starting collision resolution with " << triangleIndices.size() / 3
    //           << " triangles" << std::endl;

    // Validate input
    if (triangleIndices.size() % 3 != 0) {
        return;
    }

    if (!clothBVH || !clothBVH->root) return;

    clothBVH->refit(); // Update BVH with current positions

    std::vector<GLuint> potentialTriangles;
    traverseBVH(clothBVH->root, object, potentialTriangles);

    for (size_t i = 0; i < potentialTriangles.size(); i += 3) {
        bvhCollisionChecks++;
        if (i + 2 >= potentialTriangles.size()) break;
        GLuint i1 = potentialTriangles[i], i2 = potentialTriangles[i+1], i3 = potentialTriangles[i+2];
        if (i1 >= particles.size() || i2 >= particles.size() || i3 >= particles.size()) continue;

        Particle& p1 = particles[i1];
        Particle& p2 = particles[i2];
        Particle& p3 = particles[i3];
        glm::vec3 intersectionPoint, normal;

        // Perform detailed collision check for the triangle
        if (checkTriangleObjectIntersection(
            p1.getPosition(), p2.getPosition(), p3.getPosition(),
            object, intersectionPoint, normal)) {

            // Calculate penetration depths
            float penetrationDepth1 = glm::dot(normal, intersectionPoint - p1.getPosition());
            float penetrationDepth2 = glm::dot(normal, intersectionPoint - p2.getPosition());
            float penetrationDepth3 = glm::dot(normal, intersectionPoint - p3.getPosition());

            // Add colliding indices
            //collidingIndices.push_back(triangleIndices[i]);
            //collidingIndices.push_back(triangleIndices[i + 1]);
            //collidingIndices.push_back(triangleIndices[i + 2]);

            isColliding = true;

            // Resolve collision for each particle
            resolveParticleCollision(p1, normal, penetrationDepth1, deltaTime, StaticFriction, KineticFriction);
            resolveParticleCollision(p2, normal, penetrationDepth2, deltaTime, StaticFriction, KineticFriction);
            resolveParticleCollision(p3, normal, penetrationDepth3, deltaTime, StaticFriction, KineticFriction);
        }
    }
}

// void NewCollision::resolveParticleCollisionForModel(
//     Particle& particle,
//     const glm::vec3& normal)
// {
//     const float stiffness = 1000.0f;
//     const float damping = 0.2f;

//     glm::vec3 velocity = particle.getPosition() - particle.getPreviousPosition();
//     glm::vec3 velocityNormal = glm::dot(velocity, normal) * normal;
//     glm::vec3 velocityTangent = velocity - velocityNormal;

//     glm::vec3 force = -stiffness * normal - damping * velocityNormal;

//     particle.applyForce(force);

//     // Move particle to avoid penetration
//     particle.setPosition(particle.getPosition() + normal * 0.01f);
// }


void NewCollision::resolveCollisionWithOutBVH(
        std::vector<Particle>& particles,
        const std::vector<GLuint>& triangleIndices,
        const Object& object,
        float deltaTime,
        std::vector<GLuint>& collidingIndices, // Pass by reference
        float StaticFriction, float KineticFriction) {
        // Add debug print
        // std::cout << "Starting collision resolution with " << triangleIndices.size() / 3
        //           << " triangles" << std::endl;

        // Validate input
        if (triangleIndices.size() % 3 != 0) {
            return;
        }

        // Iterate through triangles
        for (size_t i = 0; i < triangleIndices.size(); i += 3) {
            // Bounds check
            collisionChecks++;

            if (i + 2 >= triangleIndices.size() ||
                triangleIndices[i] >= particles.size() ||
                triangleIndices[i + 1] >= particles.size() ||
                triangleIndices[i + 2] >= particles.size()) {
                continue;
            }

            // Get particles forming this triangle
            Particle& p1 = particles[triangleIndices[i]];
            Particle& p2 = particles[triangleIndices[i + 1]];
            Particle& p3 = particles[triangleIndices[i + 2]];

            glm::vec3 intersectionPoint, normal;

            if (checkTriangleObjectIntersection(
                p1.getPosition(), p2.getPosition(), p3.getPosition(),
                object, intersectionPoint, normal)) {

                // Calculate penetration depths
                float penetrationDepth1 = glm::dot(normal, intersectionPoint - p1.getPosition());
                float penetrationDepth2 = glm::dot(normal, intersectionPoint - p2.getPosition());
                float penetrationDepth3 = glm::dot(normal, intersectionPoint - p3.getPosition());

                // isColliding = true;
                // Push the indices of the colliding triangle
                //collidingIndices.push_back(triangleIndices[i]);
                //collidingIndices.push_back(triangleIndices[i + 1]);
                //collidingIndices.push_back(triangleIndices[i + 2]);

                isColliding = true;
                // Resolve collision for each particle
                resolveParticleCollision(p1, normal, penetrationDepth1, deltaTime, StaticFriction, KineticFriction);
                resolveParticleCollision(p2, normal, penetrationDepth2, deltaTime, StaticFriction, KineticFriction);
                resolveParticleCollision(p3, normal, penetrationDepth3, deltaTime, StaticFriction, KineticFriction);
            }
        }
    }


void NewCollision::resolveParticleCollision(
    Particle& particle,
    const glm::vec3& normal,
    float penetrationDepth,
    float deltaTime, float Fs, float Fk) {

    // std::cout << "Resolving particle collision" << std::endl;

    const float repulsionStrength = 500.0f;
    const float restitution = 0.5f;

    const float staticFrictionCoeff = Fs;
    const float kineticFrictionCoeff = Fk;

    // Calculate velocity
    glm::vec3 velocity = particle.getPosition() - particle.getPreviousPosition();

    // Apply repulsion force
    glm::vec3 repulsionForce = normal * (repulsionStrength * std::abs(penetrationDepth));
    particle.applyForce(repulsionForce);
    particle.update(deltaTime);

    // Apply restitution
    glm::vec3 velocityNormal = glm::dot(velocity, normal) * normal;
    //std::cout << "normal: { " << normal.x << ", " << normal.y << ", " << normal.z << "}\n";
    //std::cout << "velocityNormal: { " << velocity.x << ", " << velocity.y << ", " << velocity.z << "}\n";
    glm::vec3 velocityTangent = velocityNormal - velocity;
    glm::vec3 newVelocity = velocityTangent - velocityNormal * restitution;

    // Friction calculation
    float normalForce = glm::length(repulsionForce);
    glm::vec3 frictionDirection = glm::length(velocityTangent) > 0.0001f
        ? glm::normalize(velocityTangent)
        : glm::vec3(0.0f);

    // Choose between static and kinetic friction
    float frictionCoeff = glm::length(velocityTangent) < 0.0001f
        ? staticFrictionCoeff
        : kineticFrictionCoeff;

    // Calculate friction force
    glm::vec3 frictionForce = -frictionDirection * (normalForce * frictionCoeff);

    // Apply friction
    newVelocity += frictionForce / particle.getMass();

    // Update particle
    particle.setPreviousPosition(particle.getPosition() - newVelocity);

    // Correct position if there's penetration
    if (penetrationDepth < 0.0f) {
        particle.setPosition(particle.getPosition() - normal * penetrationDepth);
    }
}

// void NewCollision::resolveModelClothCollision(
//     std::vector<Particle>& clothParticles,
//     Model& model,
//     StaticBVH* modelBVH,
//     float collisionDistance,
//     float repulsionStrength) {

//     for (auto& particle : clothParticles) {
//         glm::vec3 particlePos = particle.getPosition();
//         Triangle nearestTri;
//         glm::vec3 closestPoint;
//         float minDist = FLT_MAX;

//         modelBVH->findNearestTriangle(particlePos, nearestTri, closestPoint, minDist);

//         if (minDist < collisionDistance) {
//             glm::vec3 normal = nearestTri.normal;
//             glm::vec3 dir = particlePos - closestPoint;
//             float penetration = collisionDistance - minDist;

//             // Apply repulsion force
//             particle.applyForce(normal * penetration * repulsionStrength);

//             // Position correction
//             particle.setPosition(closestPoint + normal * collisionDistance);
//         }
//     }
// }

// void NewCollision::resolveModelClothCollision(
//     std::vector<Particle>& clothParticles,
//     Model& model,
//     StaticBVH* modelBVH,
//     float collisionDistance,
//     float repulsionStrength) {

//     const float maxVelocity = 5.0f; // Prevent velocity explosions
//     const float damping = 0.95f;    // Energy loss on collision
//     const float correctionFactor = 0.5f; // Reduce over-correction

//     std::cout << "Model BVH root AABB: ["
//                   << modelBVH->root->aabb.min.x << "," << modelBVH->root->aabb.min.y << "," << modelBVH->root->aabb.min.z
//                   << "] - ["
//                   << modelBVH->root->aabb.max.x << "," << modelBVH->root->aabb.max.y << "," << modelBVH->root->aabb.max.z
//                   << "]\n";

//         std::cout << "Cloth particle positions sample: ["
//                   << clothParticles[0].getPosition().x << ","
//                   << clothParticles[0].getPosition().y << ","
//                   << clothParticles[0].getPosition().z << "]\n";

//     for (auto& particle : clothParticles) {
//         glm::vec3 particlePos = particle.getPosition();
//         Triangle nearestTri;
//         glm::vec3 closestPoint;
//         float minDist = FLT_MAX;

//         modelBVH->findNearestTriangle(particlePos, nearestTri, closestPoint, minDist);

//         if (minDist < collisionDistance) {
//             glm::vec3 normal = nearestTri.normal;
//             glm::vec3 dirToParticle = particlePos - closestPoint;
//             float penetration = collisionDistance - minDist;

//             // Compute velocity before modifying position
//             glm::vec3 velocity = (particlePos - particle.getPreviousPosition()) * damping;

//             // Gradual position correction
//             glm::vec3 correction = normal * (penetration * correctionFactor);
//             particle.setPosition(particlePos + correction);

//             // Conservative force application
//             glm::vec3 force = normal * (penetration * repulsionStrength);
//             particle.applyForce(force);

//             // Velocity control (limit extreme changes)
//             if (glm::length(velocity) > maxVelocity) {
//                 velocity = glm::normalize(velocity) * maxVelocity;
//             }

//             // Update previous position correctly
//             particle.setPreviousPosition(particle.getPosition() - velocity);
//         }
//     }
// }

// void NewCollision::resolveModelClothCollision(
//     std::vector<Particle>& particles,
//     BVH* clothBVH,
//     StaticBVH* modelBVH, // BVH for the imported model
//     float deltaTime,
//     std::vector<GLuint>& collidingIndices)
// {
//     if (!clothBVH || !clothBVH->root || !modelBVH || !modelBVH->root)
//         return;

//     clothBVH->refit();  // Update BVH for cloth
//     // modelBVH->refit();  // Update BVH for the imported model

//     // Get potential colliding triangles from both models
//     std::vector<GLuint> clothTriangles, modelTriangles;
//     traverseBVHForModel(clothBVH->root, modelBVH->root, clothTriangles, modelTriangles);

//     for (size_t i = 0; i < clothTriangles.size(); i += 3) {
//         for (size_t j = 0; j < modelTriangles.size(); j += 3) {

//             // Get triangle indices
//             GLuint i1 = clothTriangles[i], i2 = clothTriangles[i + 1], i3 = clothTriangles[i + 2];
//             GLuint u1 = modelTriangles[j], u2 = modelTriangles[j + 1], u3 = modelTriangles[j + 2];

//             if (i1 >= particles.size() || i2 >= particles.size() || i3 >= particles.size())
//                 continue;

//             Particle& p1 = particles[i1];
//             Particle& p2 = particles[i2];
//             Particle& p3 = particles[i3];

//             glm::vec3 intersectionPoint, normal;

//             // Perform triangle-triangle intersection test
//             if (checkTriangleTriangleIntersectionForModel(
//                     p1.getPosition(), p2.getPosition(), p3.getPosition(),
//                     modelBVH->vertices[u1], modelBVH->vertices[u2], modelBVH->vertices[u3],
//                     intersectionPoint, normal))
//             {
//                 collidingIndices.push_back(i1);
//                 collidingIndices.push_back(i2);
//                 collidingIndices.push_back(i3);

//                 // Resolve collision by applying correction to cloth particles
//                 resolveParticleCollisionForModel(p1, normal);
//                 resolveParticleCollisionForModel(p2, normal);
//                 resolveParticleCollisionForModel(p3, normal);
//             }
//         }
//     }
// }
