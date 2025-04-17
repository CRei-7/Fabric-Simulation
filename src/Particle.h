#pragma once

#include <glm/glm.hpp>
#include <glad/glad.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

class Particle {
private:
    glm::vec3 position;
    glm::vec3 previousPosition;
    glm::vec3 acceleration;
    float mass;
    bool isStatic;
    glm::vec3 color;// for particle color
    float radius;
    bool isCollided = false;

    // OpenGL vertex buffer
    GLuint VAO = 0, VBO = 0;

public:
    Particle(glm::vec3 pos = glm::vec3(0.0f), bool pinned = false, float particleMass = 10.0f)
        : position(pos), previousPosition(pos), acceleration(0.0f), mass(particleMass), isStatic(pinned), radius(0.005f), color(glm::vec3(0.0f, 1.0f, 0.0f)) {}

    void applyForce(const glm::vec3& force) {
        if (!isStatic) {
            acceleration += force / mass;
        }
    }
    bool getStatic() {
        return isStatic;
    }
    void update(float deltaTime) {
        if (!isStatic) {
            glm::vec3 temp = position;
            position = 2.0f * position - previousPosition + acceleration * deltaTime * deltaTime; //Verlet Integration
            previousPosition = temp;
            acceleration = glm::vec3(0.0f); // Reset acceleration
        }
    }

    void setPreviousPosition(const glm::vec3& prevPos) {
        previousPosition = prevPos;
    }

    void setPosition(const glm::vec3& pos) {
        position = pos;
        previousPosition = pos;
    }

    glm::vec3 getPosition() const {
        return position;
    }

    glm::vec3 getPreviousPosition() const {
        return previousPosition;
    }

    float getRadius() const {
        return radius;
    }

    float getMass() const {
        return mass;
    }

    void render(GLuint shaderProgram, const glm::mat4& view, const glm::mat4& projection) {
        if (VAO == 0) {
            // Initialization of VAO and VBO for the particle (simple point representation)
            glGenVertexArrays(1, &VAO);
            glGenBuffers(1, &VBO);

            glBindVertexArray(VAO);
            glBindBuffer(GL_ARRAY_BUFFER, VBO);
            GLfloat particlePosition[] = { 0.0f, 0.0f, 0.0f }; // Local origin for particle
            glBufferData(GL_ARRAY_BUFFER, sizeof(particlePosition), particlePosition, GL_STATIC_DRAW);

            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
            glEnableVertexAttribArray(0);

            glBindBuffer(GL_ARRAY_BUFFER, 0);
            glBindVertexArray(0);
        }

        // Sets particle position and size
        glm::mat4 model = glm::translate(glm::mat4(1.0f), position);
        model = glm::scale(model, glm::vec3(radius)); // Scale to represent the size of the particle

        // Uses the shader program
        glUseProgram(shaderProgram);

        // Passing the model, view, and projection matrices to the shader
        GLuint modelLoc = glGetUniformLocation(shaderProgram, "model");
        GLuint viewLoc = glGetUniformLocation(shaderProgram, "view");
        GLuint projectionLoc = glGetUniformLocation(shaderProgram, "projection");
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));

        // Sets the particle color in the shader
        GLint colorLoc = glGetUniformLocation(shaderProgram, "Color");
        glUniform3fv(colorLoc, 1, &color[0]);

        // Rendering of the particle as a point
        glPointSize(5.5f); // Adjust point size based on radius
        glBindVertexArray(VAO);
        glDrawArrays(GL_POINTS, 0, 1);
        glBindVertexArray(0);

        // Cleanup
        glUseProgram(0);
    }

    void setColor(const glm::vec3& col) { color = col; }

    ~Particle() {
        if (VAO) glDeleteVertexArrays(1, &VAO);
        if (VBO) glDeleteBuffers(1, &VBO);
    }
};