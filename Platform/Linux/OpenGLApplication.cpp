#include "glad/glad.h"
#include "OpenGL/OpenGLGraphicsManager.hpp"
#include "OpenGLApplication.hpp"

using namespace My;

namespace My {
extern GfxConfiguration          config;
std::unique_ptr<IApplication>    g_App(new OpenGLApplication(config));
std::unique_ptr<GraphicsManager> g_GraphicsManager(new OpenGLGraphicsManager);
std::unique_ptr<MemoryManager>   g_MemoryManager(new MemoryManager);
std::unique_ptr<AssetLoader>     g_AssetLoader(new AssetLoader);
std::unique_ptr<SceneManager>    g_SceneManager(new SceneManager);
std::unique_ptr<InputManager>    g_InputManager(new InputManager);
#if defined(_DEBUG)
std::unique_ptr<DebugManager> g_DebugManager(new DebugManager);
#endif
}  // namespace My

int OpenGLApplication::Initialize() {
    int result;
    result = GLFWApplication::Initialize();
    if (result != 0) exit(result);
    result = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
    if (!result) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    } else {
        result = 0;
    }
    glViewport(0, 0, m_Config.screenWidth, m_Config.screenHeight);
    return result;
}

void OpenGLApplication::Finalize() { GLFWApplication::Finalize(); }
void OpenGLApplication::Tick() {
    g_GraphicsManager->Clear();
    g_GraphicsManager->Draw();
    GLFWApplication::Tick();
}
