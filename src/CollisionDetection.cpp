#include "CollisionDetection.h"
#include <glm/gtc/matrix_transform.hpp>

void CollisionDetection::resolveClothTableCollisions(std::vector<Particle>& particles, const Table& table, float deltaTime) {
    for (auto& particle : particles) {
        if (particle.getStatic()) continue;

        // Get current and previous positions, and compute velocity
        glm::vec3 position = particle.getPosition();
        glm::vec3 previousPosition = particle.getPreviousPosition();
        glm::vec3 velocity = (position - previousPosition) / deltaTime;

        // Check collision with table top
        glm::vec2 xz_pos(position.x - table.top_center.x, position.z - table.top_center.z);
        float dist_xz = glm::length(xz_pos);
        if (dist_xz <= table.r_top && position.y <= table.table_height && position.y > table.table_height - 0.01f) {
            // Particle is colliding with the tabletop
            glm::vec3 newPosition = position;
            newPosition.y = table.table_height; // Project to table surface

            // Collision normal (upward)
            glm::vec3 normal = glm::vec3(0.0f, 1.0f, 0.0f);

            // Split velocity into normal and tangential components
            float velocityNormal = glm::dot(velocity, normal);
            glm::vec3 velocityTangent = velocity - velocityNormal * normal;

            // Inelastic collision: remove normal velocity (no bounce)
            glm::vec3 newVelocity = velocityTangent;

            // Apply friction to tangential velocity
            float frictionCoefficient = 0.05f; // Tune this value (0.1 to 0.5)
            if (glm::length(velocityTangent) > 0.0f) {
                glm::vec3 frictionDirection = -glm::normalize(velocityTangent);
                newVelocity += frictionDirection * frictionCoefficient * glm::length(velocityTangent);
                if (glm::dot(newVelocity, velocityTangent) < 0) {
                    newVelocity = glm::vec3(0.0f); // Stop if friction reverses direction
                }
            }

            // Update previous position for Verlet integration
            glm::vec3 newPreviousPosition = newPosition - newVelocity * deltaTime;

            // Apply updates
            particle.setPosition(newPosition);
            particle.setPreviousPosition(newPreviousPosition);
        }

        // Check collision with table legs
        for (const auto& leg : table.legs) {
            glm::vec3 legCenter = leg.base_position + glm::vec3(0.0f, leg.height / 2.0f, 0.0f);
            glm::vec3 toParticle = position - legCenter;
            float yProjection = toParticle.y; // Relative to leg center
            if (yProjection >= -leg.height / 2.0f && yProjection <= leg.height / 2.0f) {
                glm::vec2 xzToParticle(toParticle.x, toParticle.z);
                float dist = glm::length(xzToParticle);
                if (dist < leg.radius && dist > 0.0f) {
                    // Project particle out of the leg
                    glm::vec3 normal = glm::normalize(glm::vec3(xzToParticle.x, 0.0f, xzToParticle.y));
                    glm::vec3 newPosition = legCenter + normal * leg.radius;
                    newPosition.y = position.y; // Preserve vertical position unless adjusted separately

                    // Split velocity
                    float velocityNormal = glm::dot(velocity, normal);
                    glm::vec3 velocityTangent = velocity - velocityNormal * normal;

                    // Inelastic collision
                    glm::vec3 newVelocity = velocityTangent;

                    // Apply friction
                    float frictionCoefficient = 0.3f;
                    if (glm::length(velocityTangent) > 0.0f) {
                        glm::vec3 frictionDirection = -glm::normalize(velocityTangent);
                        newVelocity += frictionDirection * frictionCoefficient * glm::length(velocityTangent);
                        if (glm::dot(newVelocity, velocityTangent) < 0) {
                            newVelocity = glm::vec3(0.0f);
                        }
                    }

                    // Update previous position
                    glm::vec3 newPreviousPosition = newPosition - newVelocity * deltaTime;

                    // Apply updates
                    particle.setPosition(newPosition);
                    particle.setPreviousPosition(newPreviousPosition);
                }
            }
        }
    }
}
