#include "GfxConfiguration.h"
#include "MyTest.hpp"
#include "MyPhysicsManager.hpp"

namespace My {
// clang-format off
GfxConfiguration config("MyTest", 8, 8, 8, 8, 24, 8, 0, 1024, 720);
std::unique_ptr<IPhysicsManager> g_hysicsManager(new MyPhysicsManager); 
std::unique_ptr<GameLogic>       g_GameLogic(new MyTest);
// clang-format on
}  // namespace My