#include "MaterialDefinition.h"

#include "Engine/Core/System/Iris.h"
#include "Engine/Util/BitwiseMacros.h"
#include "Engine/Util/Log.h"

using namespace WEngine;

void MaterialDefinition::Parse(const YAML::Node &root)
{
    if (!root["material"])
    {
        WLog::SetConsoleError();
        WLog::ConsoleLog("Failed to load material definition!");
        return;
    }

    const YAML::Node material = root["material"];

    // we need to at least have the name to give a better error is something is missing later.
    if (!material["materialName"])
    {
        WLog::SetConsoleError();
        WLog::ConsoleLog("Material definition name not present!");
        return;
    }

    name = material["materialName"].as<std::string>();

    if (!material["shader"])
    {
        WLog::SetConsoleError();
        WLog::ConsoleLog(std::format("Material \"{}\" is missing one of the following fields:\n"
            "\t shader", name));
        return;
    }

    shaderName = material["shader"].as<std::string>();

    auto shaderN = Iris::GetShaderDef(shaderName);
    if (!shaderN.HasValue())
        return; // Iris already prints an error message

    const auto& fragInfo = shaderN.GetValue().fragInfo;

    bool disableStrictCheck = false;
    if (material["disableStrictCheck"])
        disableStrictCheck = material["disableStrictCheck"].as<bool>();

    // reading this unironically sounds like im praying on the materials' downfall.
    if (fragInfo.expectTextureCount != 0)
    {
        // since we have textures now, we have to ensure proper sanity checks.
        if (!material["textures"])
        {
            WLog::SetConsoleError();
            WLog::ConsoleLog(std::format("Material \"{}\" is missing the following field:\n"
                "\t textures.", name));
            return;
        }

        for (const auto& tex : material["textures"])
            texturesPackaging.push_back(tex.second.as<std::string>());

        if (texturesPackaging.size() != fragInfo.expectTextureCount)
        {
            WLog::SetConsoleError();
            WLog::ConsoleLog(std::format("Material sanity test tripped in \"{}\":\n"
                "\t unexpected texture count!", name));
            return;
        }

    }

    if (!fragInfo.expectedParams.empty())
    {
        if (!material["params"])
        {
            WLog::SetConsoleError();
            WLog::ConsoleLog(std::format("Material \"{}\" is missing one of the following fields:\n"
                "\t params", name));
            return;
        }

        ProcessParams(fragInfo, material);
    }

    valid = true;
}

bool MaterialDefinition::ProcessParams(const ShaderDefinition::FragmentInfo& fragInfo, const YAML::Node& matRoot)
{
    for (const auto& param : matRoot["params"])
    {
        bool found = false;
        ShaderSettingType expectedType;

        for (const auto& expected : fragInfo.expectedParams)
        {
            if (expected.first == param.first.as<std::string>())
            {
                found = true;
                expectedType = expected.second;
            }
        }
        if (!found)
            continue;
        if (!CheckParamType(expectedType, param.second))
            return false;

        ShaderSetting setting;
        setting.type = expectedType;
        setting.settingName = param.first.as<std::string>();
        switch (expectedType)
        {
            case ShaderSettingType::Float:
                setting.option = param.second.as<float32>();
                break;
            case ShaderSettingType::Int:
                setting.option = param.second.as<int32>();
                break;
            case ShaderSettingType::Vec2:
            {
                Vector2 vec;
                vec.x = param.second[0].as<float32>();
                vec.y = param.second[1].as<float32>();
                setting.option = vec;
                break;
            }
            case ShaderSettingType::Vec3:
            {
                Vector3 vec;
                vec.x = param.second[0].as<float32>();
                vec.y = param.second[1].as<float32>();
                vec.z = param.second[2].as<float32>();
                setting.option = vec;
                break;
            }
            case ShaderSettingType::Vec4:
            {
                Vector4 vec;
                vec.x = param.second[0].as<float32>();
                vec.y = param.second[1].as<float32>();
                vec.z = param.second[2].as<float32>();
                vec.w = param.second[3].as<float32>();
                setting.option = vec;
                break;
            }
            default: ;
        }
        params.push_back(setting);
    }
    return true;
}

bool MaterialDefinition::CheckParamType(ShaderSettingType expect, const YAML::Node& paramsRaw)
{
    switch (expect)
    {
        case ShaderSettingType::Float:
        case ShaderSettingType::Int:
            return paramsRaw.IsScalar();
        case ShaderSettingType::Vec2:
            return paramsRaw.IsSequence() && paramsRaw.size() == 2;
        case ShaderSettingType::Vec3:
            return paramsRaw.IsSequence() && paramsRaw.size() == 3;
        case ShaderSettingType::Vec4:
            return paramsRaw.IsSequence() && paramsRaw.size() == 4;

        default:
            return false;
    }
}

