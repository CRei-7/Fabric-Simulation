#include "Table.h"
#include "stb/stb_image.h"
#include <cmath>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/constants.hpp>
#include <iostream>

Table::Table(float r_top, float h_top, float r_leg, float h_leg, float table_height, int segments, glm::vec3 position, const std::string& texture_path)
    : r_top(r_top), table_height(table_height), index_count(0), textureID(0) {
    // Set top_center with position offset for x and z, y based on table_height
    top_center = glm::vec3(position.x, table_height - h_top, position.z);

    // Define legs with base at y=0, offset x and z by position
    float leg_offset = r_top - 0.1f;
    for (int i = 0; i < 4; ++i) {
        float angle = i * (glm::pi<float>() / 2.0f);
        glm::vec3 leg_pos = glm::vec3(position.x + leg_offset * cos(angle), top_center.y - h_leg, position.z + leg_offset * sin(angle));
        legs.push_back({leg_pos, r_leg, h_leg});
    }
    loadTexture(texture_path);
    setupGeometry();
}

Table::~Table() {
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO_positions);
    glDeleteBuffers(1, &VBO_normals);
    glDeleteBuffers(1, &VBO_texCoords);
    glDeleteBuffers(1, &EBO);
    if (textureID) glDeleteTextures(1, &textureID);
}

void Table::loadTexture(const std::string& path) {
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);

    // Set texture wrapping and filtering parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // Load image
    int width, height, nrChannels;
    stbi_set_flip_vertically_on_load(true);

    std::cout << "Loading texture from: " << path << std::endl;
    unsigned char* data = stbi_load(path.c_str(), &width, &height, &nrChannels, 0);

    if (data) {
        GLenum format;
        if (nrChannels == 1)
            format = GL_RED;
        else if (nrChannels == 3)
            format = GL_RGB;
        else if (nrChannels == 4)
            format = GL_RGBA;

        std::cout << "Texture loaded successfully:" << std::endl;
        std::cout << "Width: " << width << std::endl;
        std::cout << "Height: " << height << std::endl;
        std::cout << "Channels: " << nrChannels << std::endl;
        std::cout << "Format: " << format << std::endl;

        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    } else {
        std::cerr << "Failed to load texture: " << path << std::endl;
        std::cerr << "STB Error: " << stbi_failure_reason() << std::endl;
    }

    stbi_image_free(data);
    glBindTexture(GL_TEXTURE_2D, 0);

    // Check for OpenGL errors
    // GLenum err;
    // while ((err = glGetError()) != GL_NO_ERROR) {
    //     std::cerr << "OpenGL error: " << err << std::endl;
    // }
}

void Table::setupGeometry() {
    std::vector<glm::vec3> vertices;
    std::vector<glm::vec3> normals;
    std::vector<glm::vec2> texCoords;
    std::vector<GLuint> indices;

    // Generate table top (thin cylinder)
    generateCylinder(vertices, normals, texCoords, indices, top_center, r_top, table_height - top_center.y, 36);

    // Generate each leg
    for (const auto& leg : legs) {
        generateCylinder(vertices, normals, texCoords, indices, leg.base_position, leg.radius, leg.height, 36);
    }

    // Set up OpenGL buffers
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO_positions);
    glGenBuffers(1, &VBO_normals);
    glGenBuffers(1, &VBO_texCoords);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    // Vertex positions
    glBindBuffer(GL_ARRAY_BUFFER, VBO_positions);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), vertices.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);
    glEnableVertexAttribArray(0);

    // Normals
    glBindBuffer(GL_ARRAY_BUFFER, VBO_normals);
    glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(glm::vec3), normals.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);
    glEnableVertexAttribArray(1);

    // Texture coordinates
    glBindBuffer(GL_ARRAY_BUFFER, VBO_texCoords);
    glBufferData(GL_ARRAY_BUFFER, texCoords.size() * sizeof(glm::vec2), texCoords.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(glm::vec2), (void*)0);
    glEnableVertexAttribArray(2);

    // Indices
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), indices.data(), GL_STATIC_DRAW);
    index_count = indices.size();

    glBindVertexArray(0);
}

void Table::render(GLuint shaderProgram, const glm::mat4& view, const glm::mat4& projection) {
    glUseProgram(shaderProgram);

    glm::mat4 model = glm::mat4(1.0f);
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "view"), 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

    // Bind texture
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glUniform1i(glGetUniformLocation(shaderProgram, "texture1"), 0);

    // Set useTexture uniform to true
    glUniform1i(glGetUniformLocation(shaderProgram, "useTexture"), 1);
    // Set lighting properties
    glUniform3fv(glGetUniformLocation(shaderProgram, "lightPos"), 1, glm::value_ptr(glm::vec3(0.0f, 1.0f, 0.0f)));
    glUniform3fv(glGetUniformLocation(shaderProgram, "viewPos"), 1, glm::value_ptr(glm::vec3(0.0f, 0.0f, 3.0f)));
    glUniform3fv(glGetUniformLocation(shaderProgram, "lightColor"), 1, glm::value_ptr(glm::vec3(1.0f, 1.0f, 1.0f))); // Add this line

    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, index_count, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);

    // Cleanup
    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
    glUseProgram(0);
}

void Table::generateCylinder(std::vector<glm::vec3>& vertices,
                             std::vector<glm::vec3>& normals,
                              std::vector<glm::vec2>& texCoords,
                             std::vector<GLuint>& indices,
                             const glm::vec3& base_position,
                             float radius,
                             float height,
                             int segments) {
    float y_bottom = base_position.y;
    float y_top = base_position.y + height;

    // Top face (normal: 0,1,0)
    unsigned int top_center_idx = vertices.size();
    vertices.push_back(base_position + glm::vec3(0.0f, height, 0.0f));
    normals.push_back(glm::vec3(0.0f, 1.0f, 0.0f));
    texCoords.push_back(glm::vec2(0.5f, 0.5f)); // Center of texture
    std::vector<unsigned int> top_edge;
    for (int i = 0; i < segments; ++i) {
        float theta = i * 2.0f * glm::pi<float>() / segments;
        float x = radius * cos(theta);
        float z = radius * sin(theta);
        vertices.push_back(base_position + glm::vec3(x, height, z));
        normals.push_back(glm::vec3(0.0f, 1.0f, 0.0f));
        float u = 0.5f + 0.5f * cos(theta);
        float v = 0.5f + 0.5f * sin(theta);
        texCoords.push_back(glm::vec2(u, v));
        top_edge.push_back(vertices.size() - 1);
    }

    // Bottom face (normal: 0,-1,0)
    unsigned int bottom_center_idx = vertices.size();
    vertices.push_back(base_position);
    normals.push_back(glm::vec3(0.0f, -1.0f, 0.0f));
    texCoords.push_back(glm::vec2(0.5f, 0.5f));
    std::vector<unsigned int> bottom_edge;
    for (int i = 0; i < segments; ++i) {
        float theta = i * 2.0f * glm::pi<float>() / segments;
        float x = radius * cos(theta);
        float z = radius * sin(theta);
        vertices.push_back(base_position + glm::vec3(x, 0.0f, z));
        normals.push_back(glm::vec3(0.0f, -1.0f, 0.0f));
        float u = 0.5f + 0.5f * cos(theta);
        float v = 0.5f + 0.5f * sin(theta);
        texCoords.push_back(glm::vec2(u, v));
        bottom_edge.push_back(vertices.size() - 1);
    }

    // Side vertices (separate for correct normals)
    std::vector<unsigned int> side_top, side_bottom;
    for (int i = 0; i < segments; ++i) {
        float theta = i * 2.0f * glm::pi<float>() / segments;
        float x = radius * cos(theta);
        float z = radius * sin(theta);
        glm::vec3 normal = glm::normalize(glm::vec3(x, 0.0f, z));

        vertices.push_back(base_position + glm::vec3(x, height, z));
        normals.push_back(normal);
        texCoords.push_back(glm::vec2(static_cast<float>(i) / segments, 1.0f));
        side_top.push_back(vertices.size() - 1);

        vertices.push_back(base_position + glm::vec3(x, 0.0f, z));
        normals.push_back(normal);
        texCoords.push_back(glm::vec2(static_cast<float>(i) / segments, 0.0f));
        side_bottom.push_back(vertices.size() - 1);
    }


    // Indices for top face
    for (int i = 0; i < segments; ++i) {
        indices.push_back(top_center_idx);
        indices.push_back(top_edge[i]);
        indices.push_back(top_edge[(i + 1) % segments]);
    }

    // Indices for bottom face
    for (int i = 0; i < segments; ++i) {
        indices.push_back(bottom_center_idx);
        indices.push_back(bottom_edge[(i + 1) % segments]);
        indices.push_back(bottom_edge[i]);
    }

    // Indices for side faces
    for (int i = 0; i < segments; ++i) {
        int next = (i + 1) % segments;
        // First triangle
        indices.push_back(side_top[i]);
        indices.push_back(side_bottom[i]);
        indices.push_back(side_bottom[next]);
        // Second triangle
        indices.push_back(side_top[i]);
        indices.push_back(side_bottom[next]);
        indices.push_back(side_top[next]);
    }
}
