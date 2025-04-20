#include "CollisionDetection.h"
#include <glm/gtc/matrix_transform.hpp>
#include <limits>
#include <cmath>

void CollisionDetection::resolveClothTableCollisions(std::vector<Particle>& particles, const Table& table, float deltaTime) {
    if (deltaTime <= 0.0f) return; // Avoid division by zero

    // Small offset to push particles slightly outside surfaces after collision
    const float epsilon = 0.003f; // Adjust as needed

    // Table top vertical range
    float top_thickness = table.table_height - table.top_center.y;
    float top_y_min = table.top_center.y; // Bottom of table top
    float top_y_max = table.table_height; // Top of table top

    for (auto& particle : particles) {
        if (particle.getStatic()) continue;

        // Get current and previous positions
        glm::vec3 position = particle.getPosition();
        glm::vec3 previousPosition = particle.getPreviousPosition();

        // Estimate velocity (used for friction)
        glm::vec3 velocity = (position - previousPosition) / deltaTime;

        // Compute XZ position relative to table center
        glm::vec2 xz_pos(position.x - table.top_center.x, position.z - table.top_center.z);
        float dist_xz = glm::length(xz_pos);
        glm::vec2 xz_pos_prev(previousPosition.x - table.top_center.x, previousPosition.z - table.top_center.z);
        float dist_xz_prev = glm::length(xz_pos_prev);

        // --- Check collision with table top ---
        if (dist_xz <= table.r_top && position.y <= table.table_height && position.y >= top_y_min) {
            // Check if it crossed downwards or started below/on surface
            if (previousPosition.y >= table.table_height || position.y < table.table_height) {
                // --- Collision Response ---
                glm::vec3 newPosition = position;
                newPosition.y = table.table_height + epsilon;

                // Collision normal (upward)
                glm::vec3 normal = glm::vec3(0.0f, 1.0f, 0.0f);

                // Adjust previous position for Verlet integration
                glm::vec3 correctionVector = normal * (newPosition.y - position.y);
                glm::vec3 projectedPrevPos = previousPosition + correctionVector;

                // Apply friction
                glm::vec3 effective_velocity = (newPosition - projectedPrevPos) / deltaTime;
                float velocityNormalComp = glm::dot(effective_velocity, normal);
                glm::vec3 velocityTangent = effective_velocity - velocityNormalComp * normal;

                float frictionCoefficient = 0.1f;
                if (glm::length(velocityTangent) > std::numeric_limits<float>::epsilon()) {
                    glm::vec3 frictionDirection = -glm::normalize(velocityTangent);
                    float speedTangent = glm::length(velocityTangent);
                    float frictionMagnitude = speedTangent * frictionCoefficient;

                    if (frictionMagnitude * deltaTime > speedTangent * deltaTime) {
                        projectedPrevPos = newPosition - velocityNormalComp * normal * deltaTime;
                    } else {
                        projectedPrevPos += glm::normalize(velocityTangent) * frictionMagnitude * deltaTime;
                    }
                }

                // Apply updates
                particle.setPosition(newPosition);
                particle.setPreviousPosition(projectedPrevPos);

                // Update position and velocity for subsequent checks
                position = newPosition;
                previousPosition = projectedPrevPos;
                velocity = (position - previousPosition) / deltaTime;
                xz_pos = glm::vec2(position.x - table.top_center.x, position.z - table.top_center.z);
                dist_xz = glm::length(xz_pos);
                xz_pos_prev = glm::vec2(previousPosition.x - table.top_center.x, previousPosition.z - table.top_center.z);
                dist_xz_prev = glm::length(xz_pos_prev);
            }
        }

        // --- Check collision with table edge (cylindrical boundary) using CCD ---
        // Ray from previousPosition to position
        glm::vec3 ray_start = previousPosition;
        glm::vec3 ray_end = position;
        glm::vec3 ray_dir = ray_end - ray_start;
        float ray_length = glm::length(ray_dir);

        if (ray_length > std::numeric_limits<float>::epsilon()) {
            ray_dir /= ray_length; // Normalize direction

            // Cylinder parameters (in XZ plane, centered at table.top_center)
            glm::vec2 cylinder_center(table.top_center.x, table.top_center.z);
            float radius = table.r_top;

            // Ray-cylinder intersection in XZ plane (2D circle intersection)
            glm::vec2 p0(ray_start.x, ray_start.z);
            glm::vec2 p1(ray_end.x, ray_end.z);
            glm::vec2 d = p1 - p0;
            glm::vec2 m = p0 - cylinder_center;

            // Quadratic equation coefficients: a*t^2 + b*t + c = 0
            float a = glm::dot(d, d);
            float b = 2.0f * glm::dot(m, d);
            float c = glm::dot(m, m) - radius * radius;

            float discriminant = b * b - 4.0f * a * c;

            if (discriminant >= 0.0f) {
                float sqrt_disc = std::sqrt(discriminant);
                float t0 = (-b - sqrt_disc) / (2.0f * a);
                float t1 = (-b + sqrt_disc) / (2.0f * a);

                // Find the earliest valid intersection time (0 <= t <= 1)
                float t = -1.0f;
                if (t0 >= 0.0f && t0 <= 1.0f) t = t0;
                else if (t1 >= 0.0f && t1 <= 1.0f) t = t1;

                if (t >= 0.0f && t <= 1.0f) {
                    // Check if intersection occurs within table top's vertical range
                    float y_intersect = ray_start.y + t * ray_dir.y;
                    if (y_intersect >= top_y_min && y_intersect <= top_y_max) {
                        // Intersection point
                        glm::vec3 intersect_point = ray_start + t * ray_dir * ray_length;

                        // Compute normal (outward in XZ plane)
                        glm::vec2 xz_intersect(intersect_point.x - table.top_center.x, intersect_point.z - table.top_center.z);
                        float dist_xz_intersect = glm::length(xz_intersect);
                        glm::vec3 normal;
                        if (dist_xz_intersect < std::numeric_limits<float>::epsilon()) {
                            normal = glm::vec3(1.0f, 0.0f, 0.0f); // Arbitrary direction at center
                        } else {
                            normal = glm::normalize(glm::vec3(xz_intersect.x, 0.0f, xz_intersect.y));
                        }

                        // Project particle to cylinder surface
                        glm::vec3 newPosition = intersect_point;
                        // Push slightly outside
                        newPosition += normal * epsilon;

                        // Adjust previous position for Verlet integration
                        // Reflect previous position across the intersection point
                        float remaining_time = (1.0f - t) * deltaTime;
                        glm::vec3 projectedPrevPos = newPosition - velocity * remaining_time;

                        // Apply friction
                        glm::vec3 effective_velocity = (newPosition - projectedPrevPos) / deltaTime;
                        float velocityNormalComp = glm::dot(effective_velocity, normal);
                        glm::vec3 velocityTangent = effective_velocity - velocityNormalComp * normal;

                        float frictionCoefficient = 0.2f;
                        if (glm::length(velocityTangent) > std::numeric_limits<float>::epsilon()) {
                            glm::vec3 frictionDirection = -glm::normalize(velocityTangent);
                            float speedTangent = glm::length(velocityTangent);
                            float frictionMagnitude = speedTangent * frictionCoefficient;

                            if (frictionMagnitude * deltaTime > speedTangent * deltaTime) {
                                projectedPrevPos = newPosition - velocityNormalComp * normal * deltaTime;
                            } else {
                                projectedPrevPos += glm::normalize(velocityTangent) * frictionMagnitude * deltaTime;
                            }
                        }

                        // Apply updates
                        particle.setPosition(newPosition);
                        particle.setPreviousPosition(projectedPrevPos);

                        // Update position and velocity for subsequent checks
                        position = newPosition;
                        previousPosition = projectedPrevPos;
                        velocity = (position - previousPosition) / deltaTime;
                        xz_pos = glm::vec2(position.x - table.top_center.x, position.z - table.top_center.z);
                        dist_xz = glm::length(xz_pos);
                        xz_pos_prev = glm::vec2(previousPosition.x - table.top_center.x, previousPosition.z - table.top_center.z);
                        dist_xz_prev = glm::length(xz_pos_prev);
                    }
                }
            }
        }

        // --- Check collision with table legs ---
        for (const auto& leg : table.legs) {
            glm::vec3 legCenter = leg.base_position + glm::vec3(0.0f, leg.height / 2.0f, 0.0f);
            glm::vec3 toParticle = position - legCenter;
            float yProjection = toParticle.y;

            // Check vertical overlap
            if (yProjection >= -leg.height / 2.0f && yProjection <= leg.height / 2.0f) {
                glm::vec2 xzToParticle(toParticle.x, toParticle.z);
                float dist_xz_leg = glm::length(xzToParticle);
                float penetrationDepth = leg.radius - dist_xz_leg;

                // Check horizontal overlap
                if (penetrationDepth > 0.0f) {
                    // Collision normal
                    glm::vec3 normal;
                    if (dist_xz_leg < std::numeric_limits<float>::epsilon()) {
                        normal = glm::vec3(1.0f, 0.0f, 0.0f);
                    } else {
                        normal = glm::normalize(glm::vec3(xzToParticle.x, 0.0f, xzToParticle.y));
                    }

                    // Project particle out of the leg
                    glm::vec3 correction = normal * (penetrationDepth + epsilon);
                    glm::vec3 newPosition = position + correction;

                    // Adjust previous position
                    glm::vec3 projectedPrevPos = previousPosition + correction;

                    // Apply friction
                    glm::vec3 effective_velocity = (newPosition - projectedPrevPos) / deltaTime;
                    float velocityNormalComp = glm::dot(effective_velocity, normal);
                    glm::vec3 velocityTangent = effective_velocity - velocityNormalComp * normal;

                    float frictionCoefficient = 0.3f;
                    if (glm::length(velocityTangent) > std::numeric_limits<float>::epsilon()) {
                        glm::vec3 frictionDirection = -glm::normalize(velocityTangent);
                        float speedTangent = glm::length(velocityTangent);
                        float frictionMagnitude = speedTangent * frictionCoefficient;

                        if (frictionMagnitude * deltaTime > speedTangent * deltaTime) {
                            projectedPrevPos = newPosition - velocityNormalComp * normal * deltaTime;
                        } else {
                            projectedPrevPos += glm::normalize(velocityTangent) * frictionMagnitude * deltaTime;
                        }
                    }

                    // Apply updates
                    particle.setPosition(newPosition);
                    particle.setPreviousPosition(projectedPrevPos);

                    // Break after resolving one leg collision
                    break;
                }
            }
        }
    } // End particle loop
}
