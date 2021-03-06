#include "ThreadManager.hpp"

using namespace Hitagi;

int main() {
    g_ThreadManager->Initialize();

    auto x = g_ThreadManager->RunTask([] {
        return 3;
    });

    g_ThreadManager->Finalize();
    return 0;
}
