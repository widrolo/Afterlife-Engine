#pragma once

#include <Engine/Types/CommonTypes.h>

#include "Editor/Core/World/EditorSector.h"

namespace WEditor
{
    struct EditorState
    {
        _GLOBAL_ bool EditorMode = false;

        _GLOBAL_ EditorSector* SelectedSector = nullptr;
        _GLOBAL_ EditorSectorEntry* SelectedEntry = nullptr;
        _GLOBAL_ bool ViewportSelected = false;
    };
}
