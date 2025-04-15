#pragma once
struct AABB {
    glm::vec3 min;
    glm::vec3 max;

    AABB() : min(glm::vec3(0.0f)), max(glm::vec3(0.0f)) {}
    AABB(const glm::vec3& min, const glm::vec3& max) : min(min), max(max) {}

    bool intersects(const AABB& other) const {
        return (min.x <= other.max.x && max.x >= other.min.x) &&
               (min.y <= other.max.y && max.y >= other.min.y) &&
               (min.z <= other.max.z && max.z >= other.min.z);
    }
    float getSize() const {
        glm::vec3 diff = max - min;
        return glm::length(diff);
    }
    glm::vec3 center() const {
        return (min + max) * 0.5f;
    }

    bool intersectsSphere(const glm::vec3& center, float radius) const {
        glm::vec3 closest = glm::clamp(center, min, max);
        float distance = glm::length(center - closest);
        return distance <= radius;
    }
    AABB createAABB(const glm::vec3& v1, const glm::vec3& v2, const glm::vec3& v3) {
            AABB aabb;
            aabb.min = glm::min(v1, glm::min(v2, v3));
            aabb.max = glm::max(v1, glm::max(v2, v3));
            return aabb;
        }
};
