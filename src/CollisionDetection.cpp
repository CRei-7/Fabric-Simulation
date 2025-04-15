#include "CollisionDetection.h"
#include <glm/gtc/matrix_transform.hpp>

void CollisionDetection::resolveClothTableCollisions(std::vector<Particle>& particles, const Table& table) {
    for (auto& particle : particles) {
        glm::vec3 position = particle.getPosition();
        glm::vec3 previousPosition = particle.getPreviousPosition();

        // Table top collision
        glm::vec2 xz_pos(position.x - table.top_center.x, position.z - table.top_center.z);
        float dist_xz = glm::length(xz_pos);
        if (position.y < table.table_height && dist_xz < table.r_top) {
            position.y = table.table_height;
            previousPosition.y = position.y; // Stop vertical motion
            // Update particle position
            particle.setPosition(position);
            particle.setPreviousPosition(previousPosition);
        }

        // Table leg collision
        for (const auto& leg : table.legs) {
            float dx = position.x - leg.base_position.x;
            float dz = position.z - leg.base_position.z;
            float dist = sqrt(dx * dx + dz * dz);
            if (dist < leg.radius && position.y > leg.base_position.y &&
                position.y < leg.base_position.y + leg.height) {
                float factor = leg.radius / dist;
                position.x = leg.base_position.x + dx * factor;
                position.z = leg.base_position.z + dz * factor;
                previousPosition.x = position.x;
                previousPosition.z = position.z;
                // Update particle position
                particle.setPosition(position);
                particle.setPreviousPosition(previousPosition);
            }
        }
    }
}
