#include "AnyComponent.h"

#include <Engine/Core/World/Entity.h>
#include <Engine/Types/SpawnArgs.h>

#include <Editor/Types/ComponentSettingDefinition.h>

#include "Editor/Core/Handlers/CompSettingsRepo.h"
#include "Editor/Types/EditorSystems.h"
#include "Engine/Core/Handlers/AssetRepo.h"
#include "Engine/Core/Handlers/RenderHandler.h"
#include "Engine/Core/System/Iris.h"
#include "Engine/Core/World/Sector.h"
#include "Engine/Math/Shapes/Shapes2D.h"
#include "Engine/Types/AssetMission.h"
#include "Engine/Types/CoreSystems.h"
#include "Engine/Util/Log.h"

namespace WEngine {
    struct RenderMissionSprite;
}

using namespace WEditor;

// we dont need to register this component, as the editor is hardcoded to spawn this
// component only.

AnyComponent::AnyComponent(WEngine::Entity *e)
{
    COMP_SETUP("AnyComponent")
}

void AnyComponent::SetupPhysicsData()
{
    physics.material = Iris::GetMaterial("Editor/PhysicsDebug").GetValue();

    WEngine::MeshAssetMission mission;
    mission.name = "../EditorData/Meshes/Physics/Cube";
    EditorSystems::GetAssetRepo()->GetAsset(mission);

    physics.modelCube = Iris::ALLOC_CreateModel(mission.model).GetValue();
}

void AnyComponent::Init(uint16 ID, uint8 dataSize, WEngine::ComponentArgs args)
{
    Init(ID, dataSize);

    ApplySpawnData(args);
}

void AnyComponent::Init(uint16 ID, uint8 dataSize)
{
    m_ID = ID;
    m_data.resize(dataSize);

    if (dataSize == 0)
        return;

    for (int i = 0; i < dataSize; i++)
    {
        auto type = EditorSystems::GetCompSettingsRepo()->GetSettingType(ID, i);
        switch (type)
        {
            case ComponentOptionType::Number:
                m_data[i] = 0;
                break;
            case ComponentOptionType::Float:
                m_data[i] = 0.0f;
                break;
            case ComponentOptionType::Bool:
                m_data[i] = false;
                break;
            case ComponentOptionType::String:
                m_data[i] = std::string("");
                break;
            case ComponentOptionType::Vector2:
                m_data[i] = WEngine::Vector2(0.0f, 0.0f);
                break;
            case ComponentOptionType::Vector3:
                m_data[i] = WEngine::Vector3(0.0f, 0.0f, 0.0f);
                break;
            case ComponentOptionType::Color:
                m_data[i] = WEngine::Color(255, 255, 255, 255);
                break;
            default:
                break;
        }
    }
}

void AnyComponent::Tick(float32 dt)
{
    TryUpdate();
}

void AnyComponent::SetData(uint8 location, OptionData data)
{
    m_data[location] = data;
}

void AnyComponent::ApplySpawnData(WEngine::ComponentArgs args)
{
    auto options = EditorSystems::GetCompSettingsRepo()->GetInternalOptions(m_ID);

    WEngine::Nullable<int64> resInt{};
    WEngine::Nullable<float32> resFloat{};
    WEngine::Nullable<bool> resBool{};
    WEngine::Nullable<std::string> resString{};
    WEngine::Nullable<WEngine::Vector2> resVector{};
    WEngine::Nullable<WEngine::Vector3> resVector3{};
    WEngine::Nullable<WEngine::Color> resColor{};


    for (int i = 0; i < options.size(); i++)
    {
        auto type = EditorSystems::GetCompSettingsRepo()->GetSettingType(m_ID, i);

        switch (type)
        {
            case ComponentOptionType::Number:
                resInt = args.GetIntFromParams(options[i].optionInternal);
                if (resInt.HasValue())
                    m_data[i] = (int32)resInt.GetValue();
                break;
            case ComponentOptionType::Float:
                resFloat = args.GetFloatFromParams(options[i].optionInternal);
                if (resFloat.HasValue())
                    m_data[i] = resFloat.GetValue();
                break;
            case ComponentOptionType::Bool:
                resBool = args.GetBoolFromParams(options[i].optionInternal);
                if (resBool.HasValue())
                    m_data[i] = resBool.GetValue();
                break;
            case ComponentOptionType::String:
                resString = args.GetStringFromParams(options[i].optionInternal);
                if (resString.HasValue())
                    m_data[i] = resString.GetValue();
                break;
            case ComponentOptionType::Vector2:
                resVector = args.GetVector2FromParams(options[i].optionInternal);
                if (resVector.HasValue())
                    m_data[i] = resVector.GetValue();
                break;
            case ComponentOptionType::Vector3:
                resVector3 = args.GetVector3FromParams(options[i].optionInternal);
                if (resVector3.HasValue())
                    m_data[i] = resVector3.GetValue();
                break;
            case ComponentOptionType::Color:
                resColor = args.GetColorFromParams(options[i].optionInternal);
                if (resColor.HasValue())
                    m_data[i] = resColor.GetValue();
                break;
            default:
                break;
        }

    }
}

OptionData AnyComponent::GetData(uint8 location)
{
    return m_data[location];
}

WEngine::Nullable<OptionData> AnyComponent::FindDataByName(const std::string &name)
{
    auto options = EditorSystems::GetCompSettingsRepo()->GetInternalOptions(m_ID);
    for (int i = 0; i < options.size(); i++)
    {
        if (options[i].optionInternal == name)
            return WEngine::Nullable<OptionData>(m_data[i]);
    }
    return WEngine::Nullable<OptionData>();
}

void AnyComponent::Draw()
{
    TryDrawGameGraphics();
    TryDrawDebugGraphics();

    DrawOnSelected();
}

void AnyComponent::TryUpdate()
{
    switch (m_ID)
    {
        case 5:
            UpdateMeshComp();
        default: break;
    }
}

void AnyComponent::TryDrawGameGraphics()
{
    switch (m_ID)
    {
        case 5:
            TryDrawMeshComp();
        default: break;
    }
}

void AnyComponent::TryDrawDebugGraphics()
{
    switch (m_ID)
    {
        case 8:
            TryDrawBoxCollision();
        default: break;
    }
}

void AnyComponent::DrawOnSelected()
{

}

void AnyComponent::TryDrawMeshComp()
{
    WEngine::RenderMission mission{};
    mission.model = model;
    mission.material = material;
    mission.isStationary = false;
    mission.transform = entity->transform;
    mission.key = 0;

    EditorSystems::GetRenderHandler()->AddToRenderQueue(mission);
}

void AnyComponent::UpdateMeshComp()
{
    auto mesh = FindDataByName("meshName");
    auto mat = FindDataByName("materialName");

    std::string missingMatName = "Editor/PhysicsDebug";
    std::string modelName, matName;
    bool hasMat = false;

    if (!mesh.HasValue())
        return;

    modelName = std::get<std::string>(mesh.GetValue());
    if (mat.HasValue())
    {
        matName = std::get<std::string>(mat.GetValue());
        hasMat = true;
    }

    if (hasMat)
    {
        if (m_nameCache[1] != matName)
        {
            m_nameCache[1] = matName;
            auto matN = Iris::GetMaterial(matName);

            if (matN.HasValue())
            {
                material = matN.GetValue();
                return;
            }

            matN = Iris::ALLOC_CompileMaterial(matName);

            if (!matN.HasValue())
            {
                material = 0;
                return;
            }
            material = matN.GetValue();
        }
    }
    else
    {
        auto matN = Iris::GetMaterial(missingMatName);
        if (matN.HasValue())
            material = matN.GetValue();
        else
            material = 0;
    }

    if (material == 0)
        material = Iris::GetMaterial(missingMatName).GetValue();

    if (m_nameCache[0] != modelName)
    {
        m_nameCache[0] = modelName;
        auto modelN = Iris::GetModel(modelName);
        WEngine::WLog::ConsoleLog(std::format("Updated Model to {}", modelName));

        if (modelN.HasValue())
        {
            model = modelN.GetValue();
            return;
        }

        WEngine::MeshAssetMission mission{};

        mission.name = modelName;
        EditorSystems::GetAssetRepo()->GetAsset(mission);

        if (!mission.model.valid)
        {
            model = 0;
            return;
        }

        modelN = Iris::ALLOC_CreateModel(mission.model);

        if (!modelN.HasValue())
        {
            model = 0;
            return;
        }
        model = modelN.GetValue();
    }
}

void AnyComponent::TryDrawBoxCollision()
{
    auto sizeN = FindDataByName("size");
    auto offsetN = FindDataByName("offset");

    if (!sizeN.HasValue())
        return;
    WEngine::Vector3 size = std::get<WEngine::Vector3>(sizeN.GetValue());

    WEngine::Vector3 offset = WEngine::Vector3::Zero;
    if (offsetN.HasValue())
        offset = std::get<WEngine::Vector3>(offsetN.GetValue());

    auto transform = entity->transform;
    transform.position = transform.position + offset;
    transform.size = size;

    WEngine::RenderMission mission{};
    mission.model = physics.modelCube;
    mission.material = physics.material;
    mission.isStationary = false;
    mission.transform = transform;
    mission.key = 0;

    EditorSystems::GetRenderHandler()->AddToRenderQueue(mission);

}

