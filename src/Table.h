#ifndef TABLE_H
#define TABLE_H

#include <glm/glm.hpp>
#include <vector>
#include <glad/glad.h>
#include <string>

struct Cylinder {
    glm::vec3 base_position; // Base center of the cylinder
    float radius;
    float height;
};

class Table {
public:
Table(float r_top, float h_top, float r_leg, float h_leg, float table_height, int segments, glm::vec3 position, const std::string& texture_path);
    ~Table();
    void setupGeometry();
    void render(GLuint shaderProgram, const glm::mat4& view, const glm::mat4& projection);

    // Collision parameters (public for CollisionDetection access)
    glm::vec3 top_center;
    float r_top;
    float table_height;
    std::vector<Cylinder> legs;

private:
    GLuint VAO, VBO_positions, VBO_normals, VBO_texCoords, EBO;
    GLuint textureID;
    unsigned int index_count;

    void generateCylinder(std::vector<glm::vec3>& vertices,
                          std::vector<glm::vec3>& normals,
                          std::vector<glm::vec2>& texCoords,
                          std::vector<GLuint>& indices,
                          const glm::vec3& base_position,
                          float radius,
                          float height,
                          int segments);

    void loadTexture(const std::string& path);
};

#endif // TABLE_H
