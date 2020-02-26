#include <iostream>
#include <string>
#include "MemoryManager.hpp"
#include "AssetLoader.hpp"

using namespace My;

namespace My {
std::unique_ptr<MemoryManager> g_MemoryManager(new MemoryManager);
}  // namespace My

int main(int argc, char const* argv[]) {
    g_MemoryManager->Initialize();

    AssetLoader asset_loader;
    std::string shader_pgm = asset_loader.SyncOpenAndReadTextFileToString("Asset/Shaders/copy.vs");
    std::cout << shader_pgm << std::endl;
    g_MemoryManager->Finalize();

    return 0;
}
