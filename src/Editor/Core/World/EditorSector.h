#pragma once
#include "EditorSectorEntry.h"
#include "Engine/WTL/vector.h"

namespace WEditor
{
    class EditorSector
    {
    public:
        std::string name;
        wtl::vector<EditorSectorEntry> entries;
        bool isUnsaved = false;
    };
}
