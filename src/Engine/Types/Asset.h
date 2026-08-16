#pragma once

#include <string>

#include "CommonTypes.h"

namespace WEngine
{
    enum class AssetType
    {
        StaticMesh,
        Texture,
        Sound,
        Music
    };

    struct AssetRef
    {
        uint64 uid = 0;
        AssetType type;
        std::string name;
    };
}