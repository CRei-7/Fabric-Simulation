#pragma once
#include <glm/glm.hpp>
#include <glad/glad.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vector>

enum class ObjectType { Cube, Sphere };

class Object {
private:
    std::vector<float> vertices;
    std::vector<unsigned int> indices; // For sphere index buffer
    glm::vec3 center;
    float halfLength; // For object dimensions
    GLuint VAO, VBO, EBO;
    glm::vec3 color;
    

public:
    Object() : VAO(0), VBO(0), EBO(0), color(glm::vec3(1.0f, 0.0f, 1.0f)), objectType(ObjectType::Cube) {}
    ObjectType objectType; // To differentiate between object and sphere

    ~Object() {
        glDeleteVertexArrays(1, &VAO);
        glDeleteBuffers(1, &VBO);
        if (EBO) {
            glDeleteBuffers(1, &EBO);
        }
    }

    bool intersectsAABB(const glm::vec3& aabbMin, const glm::vec3& aabbMax) const {
        if (isCube()) {
            // Cube (AABB) vs AABB intersection check
            glm::vec3 cubeMin = getCenter() - glm::vec3(getHalfLength());
            glm::vec3 cubeMax = getCenter() + glm::vec3(getHalfLength());

            // Check overlap between two AABBs
            return (cubeMin.x <= aabbMax.x && cubeMax.x >= aabbMin.x) &&
                   (cubeMin.y <= aabbMax.y && cubeMax.y >= aabbMin.y) &&
                   (cubeMin.z <= aabbMax.z && cubeMax.z >= aabbMin.z);
        }
        else if (isSphere()) {
            // Sphere vs AABB intersection check
            glm::vec3 sphereCenter = getCenter();
            float sphereRadius = getHalfLength();

            // Find the closest point on the AABB to the sphere center
            glm::vec3 closestPoint;
            closestPoint.x = glm::clamp(sphereCenter.x, aabbMin.x, aabbMax.x);
            closestPoint.y = glm::clamp(sphereCenter.y, aabbMin.y, aabbMax.y);
            closestPoint.z = glm::clamp(sphereCenter.z, aabbMin.z, aabbMax.z);

            // Check if the closest point is within the sphere
            float distanceSq = glm::distance(sphereCenter, closestPoint) * glm::distance(sphereCenter, closestPoint);
            return distanceSq <= (sphereRadius * sphereRadius);
        }
        return false;
    }

    void SetupCube(float length, const glm::vec3& cubeCenter) {
        objectType = ObjectType::Cube;
        center = cubeCenter;
        halfLength = length / 2.0f;

        vertices = {
            // Front face (counter-clockwise winding order)
            center.x - halfLength, center.y - halfLength, center.z + halfLength,  0.0f,  0.0f,  1.0f,
            center.x - halfLength, center.y + halfLength, center.z + halfLength,  0.0f,  0.0f,  1.0f,
            center.x + halfLength, center.y + halfLength, center.z + halfLength,  0.0f,  0.0f,  1.0f,
            center.x + halfLength, center.y + halfLength, center.z + halfLength,  0.0f,  0.0f,  1.0f,
            center.x + halfLength, center.y - halfLength, center.z + halfLength,  0.0f,  0.0f,  1.0f,
            center.x - halfLength, center.y - halfLength, center.z + halfLength,  0.0f,  0.0f,  1.0f,

            // Back face (counter-clockwise when viewed from behind, CW when viewed from front)
            center.x - halfLength, center.y - halfLength, center.z - halfLength,  0.0f,  0.0f, -1.0f,
            center.x + halfLength, center.y - halfLength, center.z - halfLength,  0.0f,  0.0f, -1.0f,
            center.x + halfLength, center.y + halfLength, center.z - halfLength,  0.0f,  0.0f, -1.0f,
            center.x + halfLength, center.y + halfLength, center.z - halfLength,  0.0f,  0.0f, -1.0f,
            center.x - halfLength, center.y + halfLength, center.z - halfLength,  0.0f,  0.0f, -1.0f,
            center.x - halfLength, center.y - halfLength, center.z - halfLength,  0.0f,  0.0f, -1.0f,

            // Left face (counter-clockwise when viewed from outside)
            center.x - halfLength, center.y - halfLength, center.z - halfLength, -1.0f,  0.0f,  0.0f,
            center.x - halfLength, center.y + halfLength, center.z - halfLength, -1.0f,  0.0f,  0.0f,
            center.x - halfLength, center.y + halfLength, center.z + halfLength, -1.0f,  0.0f,  0.0f,
            center.x - halfLength, center.y + halfLength, center.z + halfLength, -1.0f,  0.0f,  0.0f,
            center.x - halfLength, center.y - halfLength, center.z + halfLength, -1.0f,  0.0f,  0.0f,
            center.x - halfLength, center.y - halfLength, center.z - halfLength, -1.0f,  0.0f,  0.0f,

            // Right face (counter-clockwise when viewed from outside)
            center.x + halfLength, center.y - halfLength, center.z - halfLength,  1.0f,  0.0f,  0.0f,
            center.x + halfLength, center.y - halfLength, center.z + halfLength,  1.0f,  0.0f,  0.0f,
            center.x + halfLength, center.y + halfLength, center.z + halfLength,  1.0f,  0.0f,  0.0f,
            center.x + halfLength, center.y + halfLength, center.z + halfLength,  1.0f,  0.0f,  0.0f,
            center.x + halfLength, center.y + halfLength, center.z - halfLength,  1.0f,  0.0f,  0.0f,
            center.x + halfLength, center.y - halfLength, center.z - halfLength,  1.0f,  0.0f,  0.0f,

            // Bottom face (Clockwise winding)
            center.x - halfLength, center.y - halfLength, center.z - halfLength,  0.0f, -1.0f,  0.0f,
            center.x - halfLength, center.y - halfLength, center.z + halfLength,  0.0f, -1.0f,  0.0f,
            center.x + halfLength, center.y - halfLength, center.z + halfLength,  0.0f, -1.0f,  0.0f,
            center.x + halfLength, center.y - halfLength, center.z + halfLength,  0.0f, -1.0f,  0.0f,
            center.x + halfLength, center.y - halfLength, center.z - halfLength,  0.0f, -1.0f,  0.0f,
            center.x - halfLength, center.y - halfLength, center.z - halfLength,  0.0f, -1.0f,  0.0f,

            // Top face (Clockwise winding)
            center.x - halfLength, center.y + halfLength, center.z + halfLength,  0.0f,  1.0f,  0.0f,
            center.x - halfLength, center.y + halfLength, center.z - halfLength,  0.0f,  1.0f,  0.0f,
            center.x + halfLength, center.y + halfLength, center.z - halfLength,  0.0f,  1.0f,  0.0f,
            center.x + halfLength, center.y + halfLength, center.z - halfLength,  0.0f,  1.0f,  0.0f,
            center.x + halfLength, center.y + halfLength, center.z + halfLength,  0.0f,  1.0f,  0.0f,
            center.x - halfLength, center.y + halfLength, center.z + halfLength,  0.0f,  1.0f,  0.0f
        };

        // Generate VAO, VBO
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);

        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

        // Position attribute
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);

        // Normal attribute
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(1);

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
    }


    void SetupSphere(float radius, const glm::vec3& sphereCenter, int sectorCount = 36, int stackCount = 18) {
        objectType = ObjectType::Sphere;
        center = sphereCenter;
        halfLength = radius; // In case of a sphere, halfLength is the radius
        vertices.clear();
        indices.clear();

        float x, y, z, xy;                              // Vertex position
        float nx, ny, nz, lengthInv = 1.0f / radius;    // Normalized vertex normal
        float sectorStep = 2 * glm::pi<float>() / sectorCount;
        float stackStep = glm::pi<float>() / stackCount;
        float sectorAngle, stackAngle;

        // Compute vertices for sphere (position and normal)
        for (int i = 0; i <= stackCount; ++i) {
            stackAngle = glm::pi<float>() / 2 - i * stackStep; // From pi/2 to -pi/2
            xy = radius * cosf(stackAngle);                    // r * cos(u)
            z = radius * sinf(stackAngle);                     // r * sin(u)

            for (int j = 0; j <= sectorCount; ++j) {
                sectorAngle = j * sectorStep;                  // From 0 to 2pi

                // Vertex position (x, y, z)
                x = xy * cosf(sectorAngle);                    // r * cos(u) * cos(v)
                y = xy * sinf(sectorAngle);                    // r * cos(u) * sin(v)
                vertices.push_back(x + center.x);
                vertices.push_back(y + center.y);
                vertices.push_back(z + center.z);

                // Normalized normal (nx, ny, nz)
                nx = x * lengthInv;
                ny = y * lengthInv;
                nz = z * lengthInv;
                vertices.push_back(nx);
                vertices.push_back(ny);
                vertices.push_back(nz);
            }
        }

        // Compute indices for sphere (triangles)
        for (int i = 0; i < stackCount; ++i) {
            int k1 = i * (sectorCount + 1); // Beginning of current stack
            int k2 = k1 + sectorCount + 1;  // Beginning of next stack

            for (int j = 0; j < sectorCount; ++j, ++k1, ++k2) {
                // Fixed code (CCW winding):
                if (i != 0) {
                    indices.push_back(k1);
                    indices.push_back(k1 + 1);  // Changed order
                    indices.push_back(k2);
                }

                if (i != (stackCount - 1)) {
                    indices.push_back(k1 + 1);
                    indices.push_back(k2 + 1);  // Changed order
                    indices.push_back(k2);
                }
            }
        }

        // Generate VAO, VBO, EBO
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glGenBuffers(1, &EBO);

        glBindVertexArray(VAO);

        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

        // Position attribute
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);

        // Normal attribute
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(1);

        glBindVertexArray(0);
    }


    void render(GLuint shaderProgram, const glm::mat4& view, const glm::mat4& projection, const glm::vec3& lightPos, const glm::vec3& viewPos, const glm::vec3& color) {
        glm::mat4 model = glm::mat4(1.0f);

        glUseProgram(shaderProgram);
        glUniform1i(glGetUniformLocation(shaderProgram, "useTexture"), 0);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, 0);

        // Set uniform matrices
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "view"), 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

        // Set lighting uniforms
        glUniform3fv(glGetUniformLocation(shaderProgram, "lightPos"), 1, glm::value_ptr(lightPos));
        glUniform3fv(glGetUniformLocation(shaderProgram, "viewPos"), 1, glm::value_ptr(viewPos));
        glUniform3fv(glGetUniformLocation(shaderProgram, "Color"), 1, glm::value_ptr(color));

        glBindVertexArray(VAO);
        if (objectType == ObjectType::Cube) {
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }
        else {
            glDrawElements(GL_TRIANGLES, (GLsizei)indices.size(), GL_UNSIGNED_INT, 0);
        }
        glBindVertexArray(0);

        glUseProgram(0);
    }

    bool checkCollision(const glm::vec3& point) const {
        if (objectType == ObjectType::Cube) {
            return (point.x >= center.x - halfLength && point.x <= center.x + halfLength &&
                point.y >= center.y - halfLength && point.y <= center.y + halfLength &&
                point.z >= center.z - halfLength && point.z <= center.z + halfLength);
        }
        else {
            // For a sphere, we check if the point is within the radius
            float distanceSquared = glm::dot(point - center, point - center);
            return distanceSquared <= (halfLength * halfLength); // For spheres, halfLength is the radius
        }
    }

    bool isCube() const {
        if (objectType == ObjectType::Cube)
            return true;
        return false;
    }

    bool isSphere() const {
        if (objectType == ObjectType::Sphere)
            return true;
        return false;
    }


    glm::vec3 getCenter() const {
        return center;
    }

    float getHalfLength() const {
        return halfLength; // For object, it's half the side length, for sphere, it's the radius
    }

    void setColor(const glm::vec3& newColor) {
        color = newColor;
    }
};
