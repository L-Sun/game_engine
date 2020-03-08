#include "D3D12GraphicsManager.hpp"
#include "WindowsApplication.hpp"

namespace Hitagi {
extern GfxConfiguration          config;
std::unique_ptr<IApplication>    g_App(new WindowsApplication(config));
std::unique_ptr<GraphicsManager> g_GraphicsManager(new D3D12GraphicsManager);
std::unique_ptr<MemoryManager>   g_MemoryManager(new MemoryManager);
std::unique_ptr<AssetLoader>     g_AssetLoader(new AssetLoader);
std::unique_ptr<SceneManager>    g_SceneManager(new SceneManager);
std::unique_ptr<InputManager>    g_InputManager(new InputManager);
#if defined(_DEBUG)
std::unique_ptr<DebugManager> g_DebugManager(new DebugManager);
#endif
}  // namespace Hitagi