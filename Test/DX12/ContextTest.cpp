#include "CommandContext.hpp"

#define _CRTDBG_MAP_ALLOC
#ifdef _WIN32
#include <crtdbg.h>
#ifdef _DEBUG

#define new new (_NORMAL_BLOCK, __FILE__, __LINE__)
#endif
#endif

using namespace Microsoft::WRL;
using namespace Hitagi::Graphics::DX12;
int main(int argc, char const* argv[]) {
    ComPtr<ID3D12Device6> device;
    ComPtr<IDXGIFactory7> factory;
    unsigned              dxgiFactoryFlags = 0;

#if defined(_DEBUG)
    // Enable d3d12 debug layer.
    {
        Microsoft::WRL::ComPtr<ID3D12Debug3> debugController;
        if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController)))) {
            debugController->EnableDebugLayer();
            // Enable additional debug layers.
            dxgiFactoryFlags |= DXGI_CREATE_FACTORY_DEBUG;
        }
    }
#endif  // DEBUG
    ThrowIfFailed(CreateDXGIFactory2(dxgiFactoryFlags, IID_PPV_ARGS(&factory)));

    // Create device.
    {
        if (FAILED(D3D12CreateDevice(nullptr, D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&device)))) {
            Microsoft::WRL::ComPtr<IDXGIAdapter4> pWarpaAdapter;
            ThrowIfFailed(factory->EnumWarpAdapter(IID_PPV_ARGS(&pWarpaAdapter)));
            ThrowIfFailed(D3D12CreateDevice(pWarpaAdapter.Get(), D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&device)));
        }
    }

    CommandListManager mgr;
    mgr.Initialize(device.Get());
    for (size_t i = 0; i < 100000; i++) {
        CommandContext context(mgr, D3D12_COMMAND_LIST_TYPE_DIRECT);
    }

#ifdef _WIN32
    _CrtSetDbgFlag(_CrtSetDbgFlag(_CRTDBG_REPORT_FLAG) | _CRTDBG_LEAK_CHECK_DF);
    _CrtSetReportMode(_CRT_ERROR, _CRTDBG_MODE_DEBUG);
#endif
    return 0;
}
