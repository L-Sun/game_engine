#include "GpuBuffer.hpp"
#include "CommandContext.hpp"

namespace Hitagi::Graphics::DX12 {

GpuBuffer::GpuBuffer(ID3D12Device6* device, size_t numElement, size_t elementSize, const void* initialData,
                     CommandContext* context)
    : m_Device(device),
      m_ElementCount(numElement),
      m_ElementSize(elementSize),
      m_BufferSize(numElement * elementSize),
      m_ResourceFlags(D3D12_RESOURCE_FLAG_NONE) {
    m_UsageState = D3D12_RESOURCE_STATE_COMMON;

    auto desc      = CD3DX12_RESOURCE_DESC::Buffer(m_BufferSize);
    auto heapProps = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);

    ThrowIfFailed(m_Device->CreateCommittedResource(&heapProps, D3D12_HEAP_FLAG_NONE, &desc, m_UsageState, nullptr,
                                                    IID_PPV_ARGS(&m_Resource)));
    UpdateGpuVirtualAddress();

    if (initialData) {
        assert(context != nullptr);
        assert(context->m_Device == m_Device);
        context->InitializeBuffer(*this, initialData, m_BufferSize);
    }
}

TextureBuffer::TextureBuffer(CommandContext& context, D3D12_CPU_DESCRIPTOR_HANDLE handle, DXGI_SAMPLE_DESC sampleDesc,
                             const Resource::Image& image) {
    m_UsageState = D3D12_RESOURCE_STATE_COPY_DEST;

    D3D12_RESOURCE_DESC desc = {};
    desc.DepthOrArraySize    = 1;
    desc.Dimension           = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
    desc.Flags               = D3D12_RESOURCE_FLAG_NONE;
    desc.Height              = image.GetHeight();
    desc.MipLevels           = 1;
    desc.SampleDesc          = sampleDesc;
    desc.Width               = image.GetWidth();

    if (image.GetBitcount() == 32)
        desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    else if (image.GetBitcount() == 8)
        desc.Format = DXGI_FORMAT_R8_UNORM;

    auto heapProps = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
    ThrowIfFailed(context.m_Device->CreateCommittedResource(&heapProps, D3D12_HEAP_FLAG_NONE, &desc, m_UsageState,
                                                            nullptr,
                                                            IID_PPV_ARGS(m_Resource.ReleaseAndGetAddressOf())));

    m_Resource->SetName(L"Texture");
    D3D12_SUBRESOURCE_DATA texResource;
    texResource.pData    = image.getData();
    texResource.RowPitch = image.GetPitch();

    context.InitializeTexture(*this, 1, &texResource);
    m_CpuDescriptorHandle = handle;
    context.m_Device->CreateShaderResourceView(m_Resource.Get(), nullptr, m_CpuDescriptorHandle);
}

}  // namespace Hitagi::Graphics::DX12