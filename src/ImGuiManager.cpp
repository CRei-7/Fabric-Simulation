#include "ImGuiManager.h"

ImGuiManager::ImGuiManager()
    : clear_color(ImVec4(0.45f, 0.55f, 0.60f, 1.00f)), show_demo_window(true), show_another_window(false)
{
}

ImGuiManager::~ImGuiManager()
{
}

void ImGuiManager::Init(GLFWwindow* window, const char* glsl_version)
{
    this->window = window;
    
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;  // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;   // Enable Gamepad Controls

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();

    // Setup Platform/Renderer bindings
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);
}

void ImGuiManager::BeginFrame()
{
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
}

void ImGuiManager::EndFrame()
{
    ImGui::Render();
}

void ImGuiManager::Render()
{
    // 1. Show demo window
    //if (show_demo_window)
    //    ImGui::ShowDemoWindow(&show_demo_window);

    // 2. Show a custom window
    static bool showSidebar = true;  // Sidebar visibility toggle

    ImGui::Begin("Sidebar Control", nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_AlwaysAutoResize);

    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.2f, 0.4f, 0.6f, 1.0f)); // Subtle blue button
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.3f, 0.5f, 0.7f, 1.0f));
    if (ImGui::Button(showSidebar ? "Hide Controls" : "Show Controls", ImVec2(120, 30)))
    {
        showSidebar = !showSidebar;
    }
    ImGui::PopStyleColor(2);
    ImGui::End();

    if (showSidebar) {

        // Set position and size for left sidebar
        ImGui::SetNextWindowPos(ImVec2(0, 20)); // Adjust position slightly for main menu bar
        ImGui::SetNextWindowSize(ImVec2(300, ImGui::GetIO().DisplaySize.y - 20)); // Adjust width as needed

        ImGui::Begin("Sidebar", &showSidebar, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_MenuBar);
        //ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(10, 10));
        

        if (ImGui::BeginMenuBar()) {
            if (ImGui::BeginMenu("Simulation"))
            {
                ImGui::MenuItem("Controls", nullptr, false, false);
                ImGui::EndMenu();
            }
            ImGui::EndMenuBar();
        }

        ImGui::PushTextWrapPos(ImGui::GetCursorPosX() + 290.0f);

        ImGui::BeginGroup();
        ImGui::TextColored(ImVec4(0.4f, 0.7f, 1.0f, 1.0f), "Simulation Settings");
        ImGui::Separator();

        if (StartSimulation) {
            ImGui::Checkbox("Start Simulation", StartSimulation);
        }
        if (useCustomDelta) {
            ImGui::Checkbox("Custom Delta Time", useCustomDelta);
        }
        if (*useCustomDelta == true && Delta) {
            ImGui::PushItemWidth(150);
            ImGui::SliderFloat("Custom DeltaTime", Delta, 0.001f, 0.1f, "%.3f");
            ImGui::PopItemWidth();
        }
        if (ClothReset) {
            if (ImGui::Button("Reset")) {
                *ClothReset = true;
            }
        }
        ImGui::EndGroup();

        ImGui::Spacing();
        ImGui::BeginGroup();
        ImGui::TextColored(ImVec4(0.4f, 0.7f, 1.0f, 1.0f), "Simulation Recording & Playback");
        ImGui::Separator();

        // Baking controls
        if (bakingSimulation && *bakingSimulation) {
            // Show progress while baking
            float progress = simulationTime ? (*simulationTime / *recordingDuration) : 0.0f;
            ImGui::ProgressBar(progress, ImVec2(-1, 0), "Recording...");
            ImGui::Text("Time: %.2f / %.2f seconds", *simulationTime, *recordingDuration);

            if (ImGui::Button("Stop Recording", ImVec2(120, 25))) {
                if (stopBakingSimulationCallback) {
                    stopBakingSimulationCallback();
                }
            }

            // Toggle for showing progress while baking
            if (showBakingProgress) {
                ImGui::Checkbox("Show Progress While Recording", showBakingProgress);
            }
        }
        else if (playingBakedSimulation && *playingBakedSimulation) {
            // Playback controls
            float progress = 0.0f;
            if (totalFrameCount && *totalFrameCount > 0) {
                progress = static_cast<float>(*currentBakedFrame) / static_cast<float>(*totalFrameCount);
            }

            ImGui::ProgressBar(progress, ImVec2(-1, 0), "Playing...");
            ImGui::Text("Frame: %zu / %zu", *currentBakedFrame, totalFrameCount ? *totalFrameCount : 0);

            ImGui::SliderFloat("Playback Speed", playbackSpeed, 0.1f, 3.0f, "%.1fx");

            if (ImGui::Button("Stop Playback", ImVec2(150, 30))) {
                if (stopPlayingBakedSimulationCallback) {
                    stopPlayingBakedSimulationCallback();
                }
            }
        }
        else {
            // Recording setup
            ImGui::PushItemWidth(150);
            ImGui::SliderFloat("Recording Time", recordingDuration, 1.0f, 50.0f, "%.1f s");
            ImGui::PopItemWidth();

            if (ImGui::Button("Start Recording", ImVec2(120, 25))) {
                if (startBakingSimulationCallback) {
                    startBakingSimulationCallback();
                }
            }

            ImGui::SameLine();

            // Only enable play button if there are frames to play
            bool hasFrames = totalFrameCount && *totalFrameCount > 0;
            if (ImGui::Button("Play Recording", ImVec2(120, 25)) && hasFrames) {
                if (playBakedSimulationCallback) {
                    playBakedSimulationCallback();
                }
            }

            if (!hasFrames && ImGui::IsItemHovered()) {
                ImGui::SetTooltip("No recorded simulation available");
            }

            // Save/Load buttons
            ImGui::Spacing();

            static char saveFilename[256] = "simulation.bin";
            ImGui::InputText("Filename", saveFilename, IM_ARRAYSIZE(saveFilename));

            ImGui::BeginGroup();
            if (ImGui::Button("Save Recording", ImVec2(120, 25)) && hasFrames) {
                if (saveBakedSimulationCallback) {
                    saveBakedSimulationCallback(std::string(saveFilename));
                }
            }

            ImGui::SameLine();

            if (ImGui::Button("Load Recording", ImVec2(120, 25))) {
                if (loadBakedSimulationCallback) {
                    loadBakedSimulationCallback(std::string(saveFilename));
                }
            }
            ImGui::EndGroup();
        }

        ImGui::EndGroup();

        ImGui::Spacing();
        ImGui::BeginGroup();
        ImGui::TextColored(ImVec4(0.4f, 0.7f, 1.0f, 1.0f), "Cloth Dynamics");
        ImGui::Separator();
        
        if (width && height) {
            ImGui::SliderFloat("Width", width, 0.1f, 3.0f);
            ImGui::SliderFloat("Height", height, 0.1f, 3.0f);
        }
        if (XDistance && YDistance) {
            ImGui::SliderFloat("disX", XDistance, 0.01f, 0.2f);
            ImGui::SliderFloat("disY", YDistance, 0.01f, 0.2f);
        }

        if (toggleWind) {
            ImGui::Checkbox("Wind Enabled", toggleWind);
        }
        if (toggleCloth) {
            // Track the previous state of the cloth orientation
            static bool prevToggleCloth = *toggleCloth;

            // Show the checkbox for cloth orientation
            if (ImGui::Checkbox("Cloth Orientation", toggleCloth)) {
                // If the state has changed, set clothNeedsReset to true
                if (*toggleCloth != prevToggleCloth) {
                    if (clothNeedsReset) {
                        *clothNeedsReset = true;
                    }
                    prevToggleCloth = *toggleCloth; // Update the previous state
                }
            }
        }

        // Add a slider for gravity
        if (gravity) {
            ImGui::SliderFloat("Gravity", gravity, -0.0f, -0.1f); // Adjust range as needed
        }

        ImGui::EndGroup();

        ImGui::Spacing();

        ImGui::BeginGroup();
        ImGui::TextColored(ImVec4(0.4f, 0.7f, 1.0f, 1.0f), "Spring Parameters");
        ImGui::Separator();

        if (k) {
            ImGui::InputFloat("Structural Spring Constant", k, 0.1f, 1.0f, "%.3f");  // Adjust format specifier as needed
        }

        if (ShearK) {
            ImGui::InputFloat("Shear Spring Constant", ShearK, 0.1f, 1.0f, "%.3f");  // Adjust format specifier as needed
        }

        if (BendK) {
            ImGui::InputFloat("Bending Spring Constant", BendK, 0.1f, 1.0f, "%.3f");  // Adjust format specifier as needed
        }

        ImGui::EndGroup();

        ImGui::Spacing();

        ImGui::BeginGroup();
        ImGui::TextColored(ImVec4(0.4f, 0.7f, 1.0f, 1.0f), "Objects");
        ImGui::Separator();
        if (SelectCube && SelectSphere && SelectTable) {
            ImGui::Checkbox("Show Cube", SelectCube);
            ImGui::SameLine();
            ImGui::Checkbox("Show Sphere", SelectSphere);
            ImGui::Checkbox("Show Table", SelectTable);

            if (*SelectCube) *SelectSphere = false;
            if (*SelectSphere) *SelectCube = false;
        }
        ImGui::EndGroup();

        ImGui::Spacing();

        ImGui::BeginGroup();
        ImGui::TextColored(ImVec4(0.4f, 0.7f, 1.0f, 1.0f), "Visualization");
        ImGui::Separator();

        if (ImGui::Button("Load Texture")) {
            showFileDialog = true;
        }
        if (!texturePath.empty()) {
            ImGui::Text("Texture: %s", texturePath.c_str());
        }

        if (showFileDialog) {
            ImGui::SetNextWindowSize(ImVec2(600, 400), ImGuiCond_FirstUseEver);
            
            IGFD::FileDialogConfig config;
            config.path = ".";
            
            ImGuiFileDialog::Instance()->OpenDialog("ChooseTextureFile", "Choose Texture", "Image files{.jpg,.jpeg,.png},.*", config);
            showFileDialog = false;
        }

        // Handles file selection
        if (ImGuiFileDialog::Instance()->Display("ChooseTextureFile")) {
            if (ImGuiFileDialog::Instance()->IsOk()) {
                texturePath = ImGuiFileDialog::Instance()->GetFilePathName();
            }
            ImGuiFileDialog::Instance()->Close();
        }

        if (showFur) {
            ImGui::Checkbox("Show Fur", showFur);
        }

        if (ShowParticle) {
            ImGui::Checkbox("Show Particles", ShowParticle);
        }
        if (ShowSpring) {
            ImGui::Checkbox("Show Springs", ShowSpring);
        }

        ImGui::EndGroup();

        ImGui::Spacing();

        ImGui::BeginGroup();
        ImGui::TextColored(ImVec4(0.4f, 0.7f, 1.0f, 1.0f), "Lighting");
        ImGui::Separator();

        if (LightPosition) {
            ImGui::DragFloat3("Light Position", glm::value_ptr(*LightPosition), 0.1f, -100.0f, 100.0f);
        }
        //ImGui::Checkbox("Demo Window", &show_demo_window);
        //ImGui::Checkbox("Another Window", &show_another_window);

        //ImGui::SliderFloat("float", &f, 0.0f, 1.0f); // Edit 1 float using a slider from 0.0f to 1.0f
        ImGui::ColorEdit3("Background Color", (float*)&clear_color); // Edit 3 floats representing a color

        if (LightColor) {  // Ensure the pointer is valid before using it
            ImGui::ColorEdit3("Light Color", (float*)LightColor);
        }

        ImGui::EndGroup();

        ImGui::Spacing();

        ImGui::BeginGroup();
        ImGui::TextColored(ImVec4(0.4f, 0.7f, 1.0f, 1.0f), "Material Properties");
        ImGui::Separator();

        static const char* materialTypes[] = {
            "Default",
            "Linen",
            "Cotton",
            "Silk",
            "Wool",
            "Polyester",
            "Denim",
            "Lycra"
        };

        struct MaterialPreset {
            float structuralK;
            float shearMultiplier;
            float bendMultiplier;
            const char* textureFile;
        };

        static const MaterialPreset materials[] = {
            //Default
            {100.0f, 0.35f, 0.15f, "./fabric_images/real_madrid.jpg"},
            // Linen
            {100.0f, 0.3f, 0.1f, "./fabric_images/linen.jpg"},
            // Cotton
            {90.0f,  0.3f, 0.1f, "./fabric_images/cotton.jpg"},
            // Silk
            {50.0f,  0.4f, 0.05f, "./fabric_images/silk.jpg"},
            // Wool
            {120.0f, 0.25f, 0.15f, "./fabric_images/wool.jpg"},
            // Polyester
            {80.0f,  0.5f, 0.2f, "./fabric_images/polyester.jpg"},
            // Denim
            {200.0f, 0.35f, 0.25f, "./fabric_images/denim.jpg"},
            // Lycra
            {30.0f,  0.7f, 0.02f, "./fabric_images/lycra.jpg"}
        };

        if (ImGui::BeginCombo("Material Type", materialTypes[currentMaterialIndex]))
        {
            for (int n = 0; n < IM_ARRAYSIZE(materialTypes); n++)
            {
                bool isSelected = (currentMaterialIndex == n);
                if (ImGui::Selectable(materialTypes[n], isSelected))
                {
                    currentMaterialIndex = n;

                    if (k && ShearK && BendK) {
                        *k = materials[n].structuralK;
                        *ShearK = *k * materials[n].shearMultiplier;
                        *BendK = *k * materials[n].bendMultiplier;
                        texturePath = materials[n].textureFile;
                    }

                    // If you have a clothNeedsReset flag, you might want to set it here
                    if (clothNeedsReset) {
                        *clothNeedsReset = true;
                    }

                }

                if (isSelected)
                    ImGui::SetItemDefaultFocus();
            }
            ImGui::EndCombo();
        }

        // Optional: Display additional material-specific parameters
        ImGui::Text("Selected Material: %s", materialTypes[currentMaterialIndex]);

        // You could add more material-specific controls here
        // For example, thickness, density, or custom spring constants

        ImGui::EndGroup();

        ImGui::Spacing();
        ImGui::Separator();

        //if (ImGui::Button("Button"))
        //    counter++;
        //ImGui::SameLine();
        //ImGui::Text("counter = %d", counter);

        ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
        
        //ImGui::PopStyleVar();
        ImGui::PopTextWrapPos();
        ImGui::End();
    }
}

void ImGuiManager::RenderWireframeToggle()
{
    ImGui::PushFont(menu_font);
    ImGui::Begin("Mode Change");
    if (ImGui::Button(wireframeMode ? "Solid Mode" : "Wireframe Mode", ImVec2(150, 30)))
    {
        wireframeMode = !wireframeMode;
    }
    ImGui::PopFont();
    ImGui::End();
}

int ImGuiManager::GetFabricTypeUniform() {
    return currentMaterialIndex;
}

void ImGuiManager::Cleanup()
{
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}

void ImGuiManager::SetupMenuBar(GLFWwindow* window, bool* should_close)
{
    if (ImGui::BeginMainMenuBar())
    {
        if (ImGui::BeginMenu("File"))
        {
            if (ImGui::MenuItem("New", "Ctrl+N")) { /* Handle new */ }
            if (ImGui::MenuItem("Open...", "Ctrl+O")) { /* Handle open */ }
            ImGui::Separator();
            if (ImGui::MenuItem("Save", "Ctrl+S")) { /* Handle save */ }
            if (ImGui::MenuItem("Save As...")) { /* Handle save as */ }
            ImGui::Separator();
            if (ImGui::MenuItem("Exit Window", "Alt+F4")) { *should_close = true; }
            ImGui::EndMenu();
        }
        ImGui::EndMainMenuBar();
    }
}

ImVec4& ImGuiManager::GetClearColor()
{
    return clear_color;
}

bool& ImGuiManager::ShowDemoWindow()
{
    return show_demo_window;
}

bool& ImGuiManager::ShowAnotherWindow()
{
    return show_another_window;
}

void ImGuiManager::SetSimulationBakingData(bool* bakingSim, bool* playingSim, float* recDuration, float* pbSpeed, bool* showProgress, float* simTime, size_t* currentFrame, size_t* totalFrames) {
    bakingSimulation = bakingSim;
    playingBakedSimulation = playingSim;
    recordingDuration = recDuration;
    playbackSpeed = pbSpeed;
    showBakingProgress = showProgress;
    simulationTime = simTime;
    currentBakedFrame = currentFrame;
    totalFrameCount = totalFrames;
}

void ImGuiManager::SetSimulationCallbacks(
    std::function<void()> startBaking,
    std::function<void()> stopBaking,
    std::function<void()> playBaked,
    std::function<void()> stopPlaying,
    std::function<void(const std::string&)> saveBaked,
    std::function<bool(const std::string&)> loadBaked
) {
    startBakingSimulationCallback = startBaking;
    stopBakingSimulationCallback = stopBaking;
    playBakedSimulationCallback = playBaked;
    stopPlayingBakedSimulationCallback = stopPlaying;
    saveBakedSimulationCallback = saveBaked;
    loadBakedSimulationCallback = loadBaked;
}