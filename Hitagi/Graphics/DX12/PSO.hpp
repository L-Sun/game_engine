#pragma once
#include "RootSignature.hpp"

namespace Hitagi::Graphics::backend::DX12 {
class PSO {
public:
    PSO()           = default;
    PSO(const PSO&) = delete;
    PSO& operator=(PSO&) = delete;
    PSO(PSO&&)           = default;
    PSO& operator=(PSO&&) = default;

    void                 SetRootSignature(const RootSignature& rootSignature) { m_RootSignature = &rootSignature; }
    ID3D12PipelineState* GetPSO() const { return m_PSO.Get(); }

protected:
    const RootSignature*                        m_RootSignature = nullptr;
    Microsoft::WRL::ComPtr<ID3D12PipelineState> m_PSO           = nullptr;
};

class GraphicsPSO : public PSO {
public:
    using PSO::PSO;

    void SetInputLayout(const std::vector<D3D12_INPUT_ELEMENT_DESC>& inputLayout);
    void SetVertexShader(CD3DX12_SHADER_BYTECODE code);
    void SetPixelShader(CD3DX12_SHADER_BYTECODE code);
    void SetBlendState(const D3D12_BLEND_DESC& desc);
    void SetRasterizerState(const D3D12_RASTERIZER_DESC& desc);
    void SetDepthStencilState(const D3D12_DEPTH_STENCIL_DESC& desc);
    void SetPrimitiveTopologyType(D3D12_PRIMITIVE_TOPOLOGY_TYPE type);
    void SetSampleMask(unsigned mask);
    void SetRenderTargetFormats(const std::vector<DXGI_FORMAT>& RTVFormats, DXGI_FORMAT DSVFormat, unsigned MsaaCount = 1, unsigned MsaaQuality = 0);

    void Finalize(ID3D12Device* device);

    GraphicsPSO Copy() const;

private:
    D3D12_GRAPHICS_PIPELINE_STATE_DESC    m_PSODesc{};
    std::vector<D3D12_INPUT_ELEMENT_DESC> m_InputLayouts;
};

class ComputePSO : public PSO {
public:
    ComputePSO();
    void SetComputeShader(CD3DX12_SHADER_BYTECODE code);
    void Finalize(ID3D12Device* device);

private:
    D3D12_COMPUTE_PIPELINE_STATE_DESC m_PSODesc{};
};

}  // namespace Hitagi::Graphics::backend::DX12