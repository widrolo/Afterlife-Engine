#pragma once

#include <Engine/Types/CommonTypes.h>

namespace WEngine
{
    class Sector;
    class SectorEntry;
}
namespace WEditor
{
    struct EditorState
    {
        _GLOBAL_ bool EditorMode = false;

        _GLOBAL_ WEngine::Sector* SelectedSector = nullptr;
        _GLOBAL_ WEngine::SectorEntry* SelectedEntry = nullptr;
        _GLOBAL_ bool ViewportSelected = false;
    };
}