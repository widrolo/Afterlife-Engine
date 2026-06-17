#pragma once
#include "Engine/Types/CommonTypes.h"
#include "Engine/Types/Rendering/MaterialDefinition.h"
#include "Engine/Types/Rendering/TextureInfo.h"

namespace WEngine
{

    enum class IrisAssetCommunicationType : uint8
    {
        GetMaterial,
        RetireMaterial
    };
    struct IrisAssetCommunication
    {
        // Input
        IrisAssetCommunicationType commType;
        MaterialDefinition matDef;

        // Output
        wtl::vector<uint64> texReferences;
    };

    enum class AssetIrisCommunicationType : uint8
    {
        StoreTexture,
        UnloadTexture,
    };
    struct AssetIrisCommunication
    {
        // Input
        AssetIrisCommunicationType commType;
        TextureInfo textureData;
        uint64 texReferenceIn;

        // Output
        uint64 texReferenceOut;
    };
}
