#ifndef APPLICATION_H
#define APPLICATION_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <glm/glm.hpp>
#include <random>
#include <cmath>
#include "ImGuiManager.h"
#include "Particle.h"
#include "Spring.h"
#include "Object.h"
#include "Shader.h"
#include "Model.h"
#include "BVH.h"
#include "Table.h"

// #include "stb_image.h"

class Application
{
public:
    Application();
    ~Application();
    bool Init();
    void MainLoop();
    void Cleanup();

    Shader* shader;
    Shader* tableShader;
    Shader* importedModelShader;
    BVH* clothBVH;
    Model* ourModel;
    Mesh* modelMesh;
    // Mesh* table;

    void TextureSetup();

private:
    Table* table;
    GLFWwindow* window;
    ImGuiManager imgui_manager;
    const char* glsl_version;
    // GLuint vertexArray;
    GLuint vertexBuffer;
    GLuint VBO;//Vertex Buffer Object which stores a large number of vertices in the GPU's memory
    GLuint VAO;//Vertex Array Object whcih stores all the vertex attribute settings in a single object
    GLuint EBO;//Element Buffer Object for referencing values in VBO using indices
    GLuint normalVBO;
    GLuint texCoordVBO;
    GLuint texture;

    GLuint furTexCoordVBO;
    GLuint furNormalVBO;
    GLuint furLengthVBO;

    GLuint furVAO;
    GLuint furVBO;
    GLuint furEBO;

    glm::vec3 cameraPos;//camera position
    glm::vec3 cameraFront;//for specifying the direction in which camera is pointing
    glm::vec3 cameraUp;//positive y-axis for camera

    //for mouse movement
    bool firstMouse;//to remove sudden jump
    float yaw; //horizontal movement
    float pitch; //vertical movement
    float lastX; //last x position of mouse
    float lastY; //last y position of mouse
    float fov; //for scrolling

    float deltaTime;//time between current and last frame
    float lastFrame;

    void SetupOpenGL();
    void processInput(GLFWwindow* window);
    std::string readShaderSource(const char* filePath);

    bool cursorVisible;//This is used to make sure when cursor is visible, yaw or pitch doesn't occur
    bool cKeyPressed;//Press 'C' for toggling of visibility of cursor
    void mouse_callback(GLFWwindow* window, double xpos, double ypos);
    void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

    bool toggle_wind;
    bool tKeyPressed;//Press 'T' for wind
    bool fKeyPressed;
    void setupCloth();

    bool toggleClothOrientation;//true for hanging cloth and false for falling cloth
    bool oKeyPressed;//Press 'O' for change in orientation
    bool clothNeedsReset;//for reseting cloth for new orientation

    //cloth parameters
    float windTimer;
    float windChangeInterval;
    glm::vec3 windDirection;
    float windScale;
    float windOffsetSpeed;
    float gravity;

    std::vector<Particle> particles;
    std::vector<Spring> springs;
    std::vector<glm::vec3> vertices;
    std::vector<GLuint> indices;
    std::vector<glm::vec3> normals;
    std::vector<glm::vec3> furVertices;
    std::vector<GLuint> furIndices;
    std::vector<GLuint> collidingIndices;

    void setupClothMesh(const std::vector<Particle>& particles, int column, int row);
    void renderClothMesh(GLuint shaderProgram, const std::vector<Particle>& particles, const glm::mat4& view, const glm::mat4& projection);

    void generateFurStrands(const std::vector<Particle>& particles, int column, int row);
    //void generateFurStrands(const std::vector<Particle>& particles, const std::vector<GLuint>& indices, int furLayers, int furDensity);
    std::vector<glm::vec2> furTexCoords;
    std::vector<float> furLengths;

    glm::vec3 lightPos;  //light position
    std::vector<glm::vec2> texCoords;
    void calculateNormals();

    bool ShowFur;
    bool StartSimulation;
    bool ShowParticle;
    bool ShowSpring;

    glm::vec3 lightColor;

    float k; // Structural Spring constant
    float shearK; // Shear spring constant
    float bendK;

    float StaticFrictionCoefficient, KineticFrictionCoefficient;

    std::string filename;

    std::unique_ptr<Object> currentObject;
    bool SelectCube = false;
    bool SelectSphere = false;
    bool SelectTable = false;
};

#endif // APPLICATION_H