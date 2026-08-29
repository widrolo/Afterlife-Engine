#include "ShaderStore.h"

#include <unordered_map>

#include "Engine/Core/Handlers/AssetRepo.h"
#include "Engine/Core/System/Iris.h"
#include "Engine/Types/AssetMission.h"
#include "Engine/Types/CoreSystems.h"
#include "Engine/Types/Rendering/Iris/Descriptors.h"

using namespace WEngine::Rendering;
using namespace WEngine;

inline std::unordered_map<std::string, Iris::ShaderHandle> vertexShaders;
inline std::unordered_map<std::string, Iris::ShaderHandle> fragmentShaders;
inline std::unordered_map<std::string, Iris::ShaderHandle> computeShaders;
inline std::unordered_map<std::string, Iris::ShaderHandle> geometryShaders;
inline std::unordered_map<std::string, Iris::ShaderHandle> tessCtrlShaders;
inline std::unordered_map<std::string, Iris::ShaderHandle> tessEvalShaders;

std::string DebugName(const std::string& name, Iris::ShaderStage stage)
{
    std::string ret = name;
    switch (stage)
    {
        case Iris::ShaderStage::Vertex:
            ret += " Vertex Shader";
            break;
        case Iris::ShaderStage::Fragment:
            ret += " Fragment Shader";
            break;
        case Iris::ShaderStage::Compute:
            ret += " Compute Shader";
            break;
        case Iris::ShaderStage::Geometry:
            ret += " Geometry Shader";
            break;
        case Iris::ShaderStage::TessControl:
            ret += " TessControl Shader";
            break;
        case Iris::ShaderStage::TessEval:
            ret += " TessEval Shader";
            break;
    }
    return ret;
}

std::unordered_map<std::string, Iris::ShaderHandle>& GetStorage(Iris::ShaderStage stage)
{
    switch (stage)
    {
        case Iris::ShaderStage::Vertex:
            return vertexShaders;
        case Iris::ShaderStage::Fragment:
            return fragmentShaders;
        case Iris::ShaderStage::Compute:
            return computeShaders;
        case Iris::ShaderStage::Geometry:
            return geometryShaders;
        case Iris::ShaderStage::TessControl:
            return tessCtrlShaders;
        case Iris::ShaderStage::TessEval:
            return tessEvalShaders;
    }
}

Iris::ShaderHandle CreateShader(const std::string& name, Iris::ShaderStage stage)
{
    SpirVAssetMission mission;
    mission.name = name;
    mission.shaderType = stage;
    CoreSystems::GetAssetRepo()->GetAsset(mission);

    Iris::ShaderStageDesc desc{};
    desc.debugName = DebugName(name, stage);
    desc.stage = stage;
    desc.entryPoint = "main";
    desc.bytecode = mission.shaderCode;
    desc.bytecodeSize = mission.shaderSize;
    auto handle = Iris::CreateShader(desc);
    GetStorage(stage)[name] = handle;
    return handle;
}
namespace WEngine::Rendering
{
    Iris::ShaderHandle GetShader(const std::string& name, Iris::ShaderStage stage)
    {
        auto& storage = GetStorage(stage);
        auto it = storage.find(name);
        if (it != storage.end())
            return it->second;
        return CreateShader(name, stage);
    }
}