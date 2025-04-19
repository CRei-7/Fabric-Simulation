#ifndef IMGUIMANAGER_H
#define IMGUIMANAGER_H

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "ImGuiFileDialog.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>


class ImGuiManager
{
public:
    ImGuiManager();
    ~ImGuiManager();

    bool wireframeMode = false;
    void Init(GLFWwindow* window, const char* glsl_version);
    void BeginFrame();
    void EndFrame();
    void Render();
    void Cleanup();
    void RenderWireframeToggle();

    void SetupMenuBar(GLFWwindow* window, bool* should_close);

    ImVec4& GetClearColor();
    bool& ShowDemoWindow();
    bool& ShowAnotherWindow();

    void SetToggleWind(bool* ptr) { toggleWind = ptr; }
    void SetToggleCloth(bool* orientation, bool* reset) { toggleCloth = orientation; clothNeedsReset = reset; }

    void SetGravity(float* ptr) { gravity = ptr; }

    void SetFur(bool* ptr) { showFur = ptr; }
    void SetSimulation(bool* ptr) { StartSimulation = ptr; }

    void ParticleShow(bool* ptr) { ShowParticle = ptr; }
    void SpringShow(bool* ptr) { ShowSpring = ptr; }

    void SetK(float* ptr) { k = ptr; }
    void SetShearK(float* ptr) { ShearK = ptr; }
    void SetBendK(float* ptr) { BendK = ptr; }

    void SetLightPosition(glm::vec3* ptr) { LightPosition = ptr; }
    void SetLightColor(glm::vec3* ptr) { LightColor = ptr; }

    void SetTexturePath(std::string* ptr) { texturePath = *ptr; }
    std::string GetTexturePath() { return texturePath; }

    void SetSphere(bool* ptr) { SelectSphere = ptr; }
    void SetCube(bool* ptr) { SelectCube = ptr; }
    void SetTable(bool* ptr) { SelectTable = ptr; }

    void DeltaTimeFlag(bool* ptr) { useCustomDelta = ptr; }
    void SetDeltaTime(float* ptr) { Delta = ptr; }

    int GetFabricTypeUniform();

    void SetSimulationBakingData(
        bool* bakingSim,
        bool* playingSim,
        float* recDuration,
        float* pbSpeed,
        bool* showProgress,
        float* simTime,
        size_t* currentFrame,
        size_t* totalFrames
    );

    void SetSimulationCallbacks(
        std::function<void()> startBaking,
        std::function<void()> stopBaking,
        std::function<void()> playBaked,
        std::function<void()> stopPlaying,
        std::function<void(const std::string&)> saveBaked,
        std::function<bool(const std::string&)> loadBaked
    );

    void ResetCloth(bool* ptr) { ClothReset = ptr; }

    void SetDimensions(float* ptr1, float* ptr2) { width = ptr1; height = ptr2; }
    void SetSeparation(float* ptr1, float* ptr2) { XDistance = ptr1; YDistance = ptr2; }

private:
    ImVec4 clear_color;
    bool show_demo_window;
    bool show_another_window;
    ImFont* menu_font;

    GLFWwindow* window; // Store the GLFW window pointer
    bool* toggleWind;   // Pointer to Application's toggle_wind
    bool* toggleCloth;  // Pointer to Application's toggleClothOrientation
    bool* clothNeedsReset;

    float* gravity;

    bool* showFur;
    bool* StartSimulation;
    bool* ShowSpring;
    bool* ShowParticle;

    glm::vec3* LightColor;
    glm::vec3* LightPosition;

    float* k;
    float* ShearK;
    float* BendK;

    std::string texturePath;
    bool showFileDialog = false;

    bool* SelectSphere;
    bool* SelectCube;
    bool* SelectTable;

    int currentMaterialIndex = 0;
    //int* fabricType;

    bool* bakingSimulation = nullptr;
    bool* playingBakedSimulation = nullptr;
    float* recordingDuration = nullptr;
    float* playbackSpeed = nullptr;
    bool* showBakingProgress = nullptr;
    float* simulationTime = nullptr;
    size_t* currentBakedFrame = nullptr;
    size_t* totalFrameCount = nullptr;
    std::function<void()> startBakingSimulationCallback;
    std::function<void()> stopBakingSimulationCallback;
    std::function<void()> playBakedSimulationCallback;
    std::function<void()> stopPlayingBakedSimulationCallback;
    std::function<void(const std::string&)> saveBakedSimulationCallback;
    std::function<bool(const std::string&)> loadBakedSimulationCallback;

    bool* useCustomDelta = nullptr;
    float* Delta;

    bool* ClothReset;

    float* width;
    float* height;
    float* XDistance;
    float* YDistance;
};

#endif // IMGUIMANAGER_H