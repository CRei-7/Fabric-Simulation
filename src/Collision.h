#pragma once
#include <glm/glm.hpp>
#include "Particle.h"
#include "Object.h"

class Collision {
public:
    static void resolveCollision(Particle& particle, const Object& object, float deltaTime) {
        glm::vec3 position = particle.getPosition();
        glm::vec3 velocity = position - particle.getPreviousPosition(); // Calculate velocity

        if (object.isCube()) {
            // Check if the particle is inside the object
            if (object.checkCollision(position)) {
                // Find the closest point on the object to the particle's position
                glm::vec3 closestPoint = glm::clamp(position,
                    object.getCenter() - object.getHalfLength(),
                    object.getCenter() + object.getHalfLength());

                // Calculate penetration vector
                glm::vec3 penetrationVector = closestPoint - position;

                // Immediately correct the particle's position to the closest surface
                particle.setPosition(closestPoint);

                // Calculate the magnitude of the penetration depth
                float penetrationDepth = glm::length(penetrationVector);

                // for frictioanal force
                //calcualte normal force
                // glm::vec3 normal = glm::normalize(penetrationVector);
                // glm::vec3 normalForce = normal * glm::length(velocity);

                //tangential velocoty
                // glm::vec3 tangentialVelocity = velocity - glm::dot(velocity, normal) * normal;

                // Friction coefficients
                // float mu_s = 0.5f; // Static friction coefficient
                // float mu_d = 0.3f; // Dynamic friction coefficient

                // glm::vec3 frictionalForce;
                // if (glm::length(tangentialVelocity) < 0.01f) {
                //     // Static friction: Opposes motion completely
                //     frictionalForce = -tangentialVelocity * mu_s;
                // } else {
                //     // Dynamic friction: Opposes motion proportionally
                //     frictionalForce = -glm::normalize(tangentialVelocity) * mu_d * glm::length(normalForce);
                // }
                // Apply a repulsion force based on penetration depth
                if (penetrationDepth > 0.0f) {
                    float repulsionStrength = 500.0f; // Adjust strength as needed
                    glm::vec3 repulsionForce = glm::normalize(penetrationVector) * (repulsionStrength * penetrationDepth);

                    // Calculate a reactionary force based on the current velocity
                    float reactionStrength = 1.0f; // Adjust as needed for responsiveness
                    glm::vec3 reactionForce = -velocity * reactionStrength;

                    // Combine forces to ensure the particle is pushed away
                    glm::vec3 totalForce = repulsionForce + reactionForce;

                    // Apply the total force to push the particle away
                    particle.applyForce(totalForce);
                }
            }
            else {
                // If the particle is outside but close to the object, apply a repulsion force
                glm::vec3 closestPoint = glm::clamp(position,
                    object.getCenter() - object.getHalfLength(),
                    object.getCenter() + object.getHalfLength());

                float distanceToClosestPoint = glm::length(closestPoint - position);

                // Apply a repulsion force if the particle is within a certain distance
                if (distanceToClosestPoint < 0.005f) { // Threshold distance to detect proximity
                    float repulsionStrength = 0.5f; // Adjust strength as needed
                    glm::vec3 repulsionForce = glm::normalize(position - closestPoint) * (repulsionStrength * (0.1f - distanceToClosestPoint));
                    particle.applyForce(repulsionForce);

                    // Apply restitution to dampen the velocity upon proximity collision
                    float restitution = 0.001f; // Coefficient of restitution
                    glm::vec3 dampenedVelocity = velocity * (1.0f - restitution);
                    particle.setPreviousPosition(position - dampenedVelocity); // Update position based on dampened velocity
                }
            }
        }
        else if (object.isSphere()) {
            // Sphere collision logic
            glm::vec3 center = object.getCenter();
            float radius = object.getHalfLength(); // For spheres, halfLength represents the radius

            glm::vec3 direction = position - center;
            float distance = glm::sqrt(direction.x * direction.x + direction.y * direction.y + direction.z * direction.z);

            if (distance < radius) { // Particle is inside the sphere
                glm::vec3 normal = glm::normalize(direction);

                // Push the particle to the surface of the sphere
                glm::vec3 newPosition = center + normal * radius;
                particle.setPosition(newPosition);

                // Penetration depth
                float penetrationDepth = radius - distance;

                // Repulsion force to push the particle away from the sphere's center
                float repulsionStrength = 500.0f;
                glm::vec3 repulsionForce = normal * (repulsionStrength * penetrationDepth);

                // Reactionary force to oppose the current velocity
                float reactionStrength = 1.0f;
                glm::vec3 reactionForce = -velocity * reactionStrength;

                glm::vec3 totalForce = repulsionForce + reactionForce;
                particle.applyForce(totalForce);
            }
        }
    }

    static void resolveSelfCollision(Particle& particle, const std::vector<Particle>& particles) {
        glm::vec3 position = particle.getPosition();

        for (const auto& other : particles) {
            if (&particle != &other) { // Ensure we're not comparing the particle with itself
                glm::vec3 otherPosition = other.getPosition();
                float distance = glm::length(position - otherPosition);
                float combinedRadius = particle.getRadius() + other.getRadius();
                if (distance < combinedRadius) { // Collision detected
                    // Calculate the normal vector
                    glm::vec3 normal = glm::normalize(position - otherPosition);

                    // Resolve the collision by moving the particle away from the other particle
                    float penetrationDepth = combinedRadius - distance;
                    position += normal * (penetrationDepth * 0.5f); // Push half of the penetration depth away
                    particle.setPosition(position);
                }
            }
        }
    }

};