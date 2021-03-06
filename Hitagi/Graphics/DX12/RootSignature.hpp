#pragma once
#include "D3Dpch.hpp"

namespace Hitagi::Graphics::backend::DX12 {
struct RootParameter : public CD3DX12_ROOT_PARAMETER1 {
public:
    using CD3DX12_ROOT_PARAMETER1::CD3DX12_ROOT_PARAMETER1;
    ~RootParameter() {
        if (ParameterType == D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE) delete[] DescriptorTable.pDescriptorRanges;
    }

    void InitAsDescriptorRange(D3D12_DESCRIPTOR_RANGE_TYPE type, UINT shaderRegister, UINT count, UINT space = 0,
                               D3D12_SHADER_VISIBILITY visibility = D3D12_SHADER_VISIBILITY_ALL) {
        InitAsDescriptorTable(1, visibility);
        SetTableRange(0, type, shaderRegister, count, space);
    }

    void InitAsDescriptorTable(UINT rangeCount, D3D12_SHADER_VISIBILITY visibility = D3D12_SHADER_VISIBILITY_ALL) {
        auto* range = new D3D12_DESCRIPTOR_RANGE1[rangeCount];
        CD3DX12_ROOT_PARAMETER1::InitAsDescriptorTable(rangeCount, range, visibility);
    }

    void SetTableRange(UINT rangeIndex, D3D12_DESCRIPTOR_RANGE_TYPE type, UINT shaderRegister, UINT count,
                       UINT space = 0, D3D12_DESCRIPTOR_RANGE_FLAGS flags = D3D12_DESCRIPTOR_RANGE_FLAG_NONE) {
        assert(rangeIndex < DescriptorTable.NumDescriptorRanges);
        D3D12_DESCRIPTOR_RANGE1& range =
            *const_cast<D3D12_DESCRIPTOR_RANGE1*>(DescriptorTable.pDescriptorRanges + rangeIndex);
        range = CD3DX12_DESCRIPTOR_RANGE1(type, count, shaderRegister, space, flags);
    }
};

class RootSignature {
public:
    // the info of parameter in descriptor table. (rootIndex, offset)
    using ParameterTable = std::unordered_map<std::string, std::pair<size_t, size_t>>;

    RootSignature(uint32_t numRootParams = 0, uint32_t numStaticSamplers = 0);
    ~RootSignature()                    = default;
    RootSignature(const RootSignature&) = delete;
    RootSignature& operator=(const RootSignature&) = delete;
    RootSignature(RootSignature&&)                 = default;
    RootSignature& operator=(RootSignature&&) = default;

    void Reset(uint32_t numRootParams, uint32_t numStaticSamplers);

    void Destroy();

    void Finalize(
        ID3D12Device*              device,
        D3D12_ROOT_SIGNATURE_FLAGS flags   = D3D12_ROOT_SIGNATURE_FLAG_NONE,
        D3D_ROOT_SIGNATURE_VERSION version = D3D_ROOT_SIGNATURE_VERSION_1_1);

    void InitStaticSampler(UINT shaderRegister, const D3D12_SAMPLER_DESC& nonStaticSamplerDesc,
                           D3D12_SHADER_VISIBILITY visibility);

    ID3D12RootSignature*              GetRootSignature() const { return m_RootSignature.Get(); }
    const D3D12_ROOT_SIGNATURE_DESC1& GetRootSignatureDesc() const { return m_RootSignatureDesc; }
    uint32_t                          GetDescriptorTableBitMask(D3D12_DESCRIPTOR_HEAP_TYPE type) const {
        return type == D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER ? m_SamplerTableBitMask : m_DescriptorTableBitMask;
    }
    uint32_t GetNumDescriptorsInTable(uint32_t rootIndex) const {
        assert(rootIndex < 32);
        return m_NumDescriptorsPerTable[rootIndex];
    };

    unsigned GetNumRootConstants() const { return m_NumRootConstants; }
    unsigned GetNumRootDecriptors() const { return m_NumRootDescriptors; }
    unsigned GetNumDescriptorTables() const { return m_NumDescriptorTables; }

    RootParameter&       operator[](size_t index) { return m_ParamArray[index]; }
    const RootParameter& operator[](size_t index) const { return m_ParamArray[index]; }

    void UpdateParameterInfo(const std::string& name, size_t rootIndex, size_t offset) {
        m_NameTable[name] = {rootIndex, offset};
    }
    auto& GetParameterTable(std::string_view name) const noexcept {
        return m_NameTable.at(std::string(name));
    }

private:
    bool m_Finalized = false;

    D3D12_ROOT_SIGNATURE_DESC1                  m_RootSignatureDesc{};
    Microsoft::WRL::ComPtr<ID3D12RootSignature> m_RootSignature;

    std::array<uint32_t, 32> m_NumDescriptorsPerTable;
    unsigned                 m_NumParameters;
    unsigned                 m_NumSamplers;
    unsigned                 m_NumInitializedStaticSamplers;

    std::vector<RootParameter>             m_ParamArray;
    std::vector<D3D12_STATIC_SAMPLER_DESC> m_SamplerArray;

    ParameterTable m_NameTable;

    unsigned m_NumDescriptorTables = 0;
    unsigned m_NumRootConstants    = 0;
    unsigned m_NumRootDescriptors  = 0;

    uint32_t m_SamplerTableBitMask    = 0;
    uint32_t m_DescriptorTableBitMask = 0;
};

}  // namespace Hitagi::Graphics::backend::DX12