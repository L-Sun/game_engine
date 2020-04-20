#include <iostream>
#include <thread>
#include "BaseApplication.hpp"

using namespace Hitagi;

bool BaseApplication::m_Quit = false;

BaseApplication::BaseApplication(GfxConfiguration& cfg) : m_Config(cfg) {}

// Parse command line, read configuration, initialize all sub modules
int BaseApplication::Initialize() {
    int ret = 0;

    std::cout << m_Config;

    std::cout << "Initialize Memory Manager: ";
    if ((ret = g_MemoryManager->Initialize()) != 0) {
        std::cerr << "Failed. err = " << ret;
        return ret;
    }
    std::cout << "Success" << std::endl;

    std::cout << "Initialize Asset Loader: ";
    if ((ret = g_FileIOManager->Initialize()) != 0) {
        std::cerr << "Failed. err = " << ret;
        return ret;
    }
    std::cout << "Success" << std::endl;

    std::cout << "Initialize Input Manager: ";
    if ((ret = g_InputManager->Initialize()) != 0) {
        std::cerr << "Failed. err = " << ret;
        return ret;
    }
    std::cout << "Success" << std::endl;

    std::cout << "Initialize Scene Manager: ";
    if ((ret = g_SceneManager->Initialize()) != 0) {
        std::cerr << "Failed. err = " << ret;
        return ret;
    }
    std::cout << "Success" << std::endl;

    std::cout << "Initialize Graphics Manager: ";
    if ((ret = g_GraphicsManager->Initialize()) != 0) {
        std::cerr << "Failed. err = " << ret;
        return ret;
    }
    std::cout << "Success" << std::endl;

    std::cout << "Initialize Physics Manager: ";
    if ((ret = g_PhysicsManager->Initialize()) != 0) {
        std::cerr << "Failed. err = " << ret;
        return ret;
    }
    std::cout << "Success" << std::endl;

    std::cout << "Initialize GameLogic Manager: ";
    if ((ret = g_GameLogic->Initialize()) != 0) {
        std::cerr << "Failed. err = " << ret;
        return ret;
    }
    std::cout << "Success" << std::endl;

    std::cout << "Initialize Timer: ";
    if ((ret = m_Clock.Initialize()) != 0) {
        std::cerr << "Failed. err = " << ret;
        return ret;
    }
    m_Clock.Start();

#if defined(_DEBUG)
    std::cout << "Initialize Debug Manager: ";
    if ((ret = g_DebugManager->Initialize()) != 0) {
        std::cerr << "Failed. err =" << ret;
        return ret;
    }
    std::cout << "Success" << std::endl;

#endif

    return ret;
}

// Finalize all sub modules and clean up all runtime temporary files.
void BaseApplication::Finalize() {
#if defined(_DEBUG)
    g_DebugManager->Finalize();
#endif
    g_GameLogic->Finalize();
    g_GraphicsManager->Finalize();
    g_PhysicsManager->Finalize();
    g_SceneManager->Finalize();
    g_InputManager->Finalize();
    g_FileIOManager->Finalize();
    g_MemoryManager->Finalize();
}

// One cycle of the main loop
void BaseApplication::Tick() {
    m_Clock.Tick();
    g_MemoryManager->Tick();
    g_FileIOManager->Tick();
    g_InputManager->Tick();
    g_SceneManager->Tick();
    g_PhysicsManager->Tick();
    g_GraphicsManager->Tick();
    g_GameLogic->Tick();
#if defined(_DEBUG)
    g_DebugManager->Tick();
#endif
    m_FPS = 1.0 / m_Clock.deltaTime().count();
    if (m_FPSLimit != -1) {
        std::this_thread::sleep_for(std::chrono::seconds(1) / static_cast<double>(m_FPSLimit));
    }
}

void BaseApplication::SetCommandLineParameters(int argc, char** argv) {
    m_ArgSize = argc;
    m_Arg     = argv;
}

bool BaseApplication::IsQuit() { return m_Quit; }