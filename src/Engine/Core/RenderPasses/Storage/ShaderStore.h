#pragma once
#include <string>

#include "Engine/Types/Rendering/Iris/Handles.h"
#include "Engine/Types/Rendering/Iris/Usage.h"

namespace WEngine::Rendering
{
    Iris::ShaderHandle GetShader(const std::string& name, Iris::ShaderStage stage);
}
