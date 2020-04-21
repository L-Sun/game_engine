#pragma once
#include <unordered_map>

#include "FileIOManager.hpp"

namespace Hitagi::Graphics {
enum struct ShaderType { VERTEX, PIXEL, GEOMETRY };

class VertexShader;
class PixelShader;

class ShaderManager : public IRuntimeModule {
public:
    int  Initialize() final;
    void Finalize() final;
    void Tick() final;

    void                LoadShader(std::filesystem::path shaderPath, ShaderType type, std::string name = "");
    const VertexShader& GetVertexShader(const std::string& name) const { return m_VertexShaders.at(name); }
    const PixelShader&  GetPixelShader(const std::string& name) const { return m_PixelShaders.at(name); }

private:
    std::unordered_map<std::string, VertexShader> m_VertexShaders;
    std::unordered_map<std::string, PixelShader>  m_PixelShaders;
};

class Shader {
    friend class ShaderManager;

public:
    Shader(Core::Buffer&& data) : m_ShaderData(std::move(data)) {}
    Shader(const Core::Buffer& data) : m_ShaderData(data) {}

    const Core::Buffer& GetShaderCode() const { return m_ShaderData; }

protected:
    Core::Buffer m_ShaderData;
};

class VertexShader : public Shader {
public:
    using Shader::Shader;
};
class PixelShader : public Shader {
public:
    using Shader::Shader;
};

}  // namespace Hitagi::Graphics
