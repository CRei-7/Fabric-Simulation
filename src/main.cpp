#include"Application.h"

// Uses Nvidia GPU if available
extern "C" {
    __declspec(dllexport) unsigned long NvOptimusEnablement = 0x00000001;
}

// Uses AMD GPU if available
extern "C" {
    __declspec(dllexport) int AmdPowerXpressRequestHighPerformance = 1;
}

int main(int, char**)
{
    Application app;
    if (!app.Init())
        return 1;
    app.MainLoop();
    app.Cleanup();
    return 0;
}
