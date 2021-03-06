#pragma once
#include "../ICommandContext.hpp"

#include "CommandListManager.hpp"
#include "GpuResource.hpp"
#include "Allocator.hpp"
#include "DynamicDescriptorHeap.hpp"
#include "PSO.hpp"
#include "HitagiMath.hpp"

namespace Hitagi::Graphics::backend::DX12 {
class DX12DriverAPI;

class CommandContext {
public:
    CommandContext(DX12DriverAPI& driver, D3D12_COMMAND_LIST_TYPE type);
    CommandContext(const CommandContext&) = delete;
    CommandContext& operator=(const CommandContext&) = delete;
    CommandContext(CommandContext&&)                 = default;
    CommandContext& operator=(CommandContext&&) = delete;
    ~CommandContext();

    auto GetCommandList() noexcept { return m_CommandList; }

    void TransitionResource(GpuResource& resource, D3D12_RESOURCE_STATES newState, bool flushImmediate = false);
    void FlushResourceBarriers();

    void SetPSO(const PSO& pso) { m_CommandList->SetPipelineState(pso.GetPSO()); }

    void SetDescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE type, ID3D12DescriptorHeap* heap) {
        if (m_CurrentDescriptorHeaps[type] != heap) {
            m_CurrentDescriptorHeaps[type] = heap;
            BindDescriptorHeaps();
        }
    }

    void SetDynamicDescriptor(unsigned rootIndex, unsigned offset, const Descriptor& descriptor) {
        SetDynamicDescriptors(rootIndex, offset, {descriptor});
    }
    void SetDynamicDescriptors(unsigned rootIndex, unsigned offset, const std::vector<Descriptor>& descriptors) {
        m_DynamicViewDescriptorHeap.StageDescriptors(rootIndex, offset, descriptors);
    }
    void SetDynamicSampler(unsigned rootIndex, unsigned offset, const Descriptor& descriptor) {
        SetDynamicSamplers(rootIndex, offset, {descriptor});
    }
    void SetDynamicSamplers(unsigned rootIndex, unsigned offset, const std::vector<Descriptor>& descriptors) {
        m_DynamicSamplerDescriptorHeap.StageDescriptors(rootIndex, offset, descriptors);
    }
    uint64_t Finish(bool waitForComplete = false);
    void     Reset();

protected:
    void BindDescriptorHeaps();

    DX12DriverAPI&              m_Driver;
    ID3D12GraphicsCommandList5* m_CommandList      = nullptr;
    ID3D12CommandAllocator*     m_CommandAllocator = nullptr;

    std::array<D3D12_RESOURCE_BARRIER, 16> m_Barriers;
    unsigned                               m_NumBarriersToFlush = 0;

    LinearAllocator m_CpuLinearAllocator;
    LinearAllocator m_GpuLinearAllocator;

    std::array<ID3D12DescriptorHeap*, D3D12_DESCRIPTOR_HEAP_TYPE_NUM_TYPES> m_CurrentDescriptorHeaps{};

    DynamicDescriptorHeap m_DynamicViewDescriptorHeap;     // HEAP_TYPE_CBV_SRV_UAV
    DynamicDescriptorHeap m_DynamicSamplerDescriptorHeap;  // HEAP_TYPE_SAMPLER

    ID3D12RootSignature* m_RootSignature;

    D3D12_COMMAND_LIST_TYPE m_Type;
};

class GraphicsCommandContext : public CommandContext, public Graphics::IGraphicsCommandContext {
public:
    GraphicsCommandContext(DX12DriverAPI& driver)
        : CommandContext(driver, D3D12_COMMAND_LIST_TYPE_DIRECT) {}

    // Front end interface
    void SetViewPort(uint32_t x, uint32_t y, uint32_t width, uint32_t height) final;
    void SetRenderTarget(Graphics::RenderTarget& rt) final;
    void SetRenderTargetAndDepthBuffer(Graphics::RenderTarget& rt, Graphics::DepthBuffer& depthBuffer) final;
    void SetPipelineState(const Graphics::PipelineState& pipeline) final;
    void SetParameter(std::string_view name, const Graphics::ConstantBuffer& cb, size_t offset) final;
    void SetParameter(std::string_view name, const Graphics::TextureBuffer& texture) final;
    void SetParameter(std::string_view name, const Graphics::TextureSampler& sampler) final;

    void     Draw(const Graphics::MeshBuffer& mesh) final;
    void     Present(Graphics::RenderTarget& rt) final;
    uint64_t Finish(bool waitForComplete = false) final { return CommandContext::Finish(waitForComplete); }

    // Back end interface
    void SetRootSignature(const RootSignature& rootSignature) {
        if (m_RootSignature == rootSignature.GetRootSignature()) return;

        m_CommandList->SetGraphicsRootSignature(m_RootSignature = rootSignature.GetRootSignature());
        m_DynamicViewDescriptorHeap.ParseRootSignature(rootSignature);
        m_DynamicSamplerDescriptorHeap.ParseRootSignature(rootSignature);
    }

private:
    const Graphics::PipelineState* m_Pipeline;
};

class ComputeCommandContext : public CommandContext {
public:
    ComputeCommandContext(DX12DriverAPI& driver)
        : CommandContext(driver, D3D12_COMMAND_LIST_TYPE_COMPUTE) {}
};

class CopyCommandContext : public CommandContext {
public:
    CopyCommandContext(DX12DriverAPI& driver)
        : CommandContext(driver, D3D12_COMMAND_LIST_TYPE_COPY) {}

    void InitializeBuffer(GpuResource& dest, const uint8_t* data, size_t dataSize);
    void InitializeTexture(GpuResource& dest, const std::vector<D3D12_SUBRESOURCE_DATA>& subData);
};

}  // namespace Hitagi::Graphics::backend::DX12