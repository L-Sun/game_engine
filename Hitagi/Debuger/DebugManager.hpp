#pragma once
#include "IRuntimeModule.hpp"

namespace Hitagi {

class DebugManager : public IRuntimeModule {
public:
    int  Initialize() final;
    void Finalize() final;
    void Tick() final;
    void ToggleDebugInfo();
    void DrawDebugInfo();

protected:
    bool m_DrawDebugInfo = false;
};
extern std::unique_ptr<DebugManager> g_DebugManager;
}  // namespace Hitagi
