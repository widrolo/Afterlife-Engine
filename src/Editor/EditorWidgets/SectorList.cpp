#include "SectorList.h"

#include "Editor/Types/EditorState.h"
#include <Engine/Core/World/Sector.h>

#include <Editor/Types/EditorSystems.h>
#include <Engine/EngineDefines.h>
#include <Engine/Core/Handlers/AssetRepo.h>

#include "Editor/Types/ComponentSettingDefinition.h"
#include "Engine/Types/Rendering/Color.h"
#include "Engine/Util/Log.h"
#include <Editor/Core/Handlers/CompSettingsRepo.h>

using namespace WEditor;

void SectorList::Setup()
{
    m_widgetName = "Loaded Sectors";
    m_windowFlags = ImGuiWindowFlags_NoResize;
}

void SectorList::RenderInternal()
{
    ImGui::SetWindowSize({ 250, 200 }, 0);
}
