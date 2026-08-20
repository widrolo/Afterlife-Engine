#pragma once
#include <string>

#include "Engine/Math/Transform.h"
#include "Engine/Types/CommonTypes.h"

namespace WEngine
{
    // In short, this is basically an object within the world.
    // this should in the best case fit into a cache line
    class SectorEntry
    {
    public:
        SectorEntry(uint64 meshUID, uint64 textureUID, uint64 colMeshUID)
            : m_mesh(meshUID), m_texture(textureUID), m_colMesh(colMeshUID) {}
        SectorEntry(const std::string& assetName);

        [[nodiscard]] bool HasVisuals() const { return m_mesh != 0 && m_texture != 0; }
        [[nodiscard]] bool HasCollision() const { return m_colMesh != 0; }

        [[nodiscard]] uint32 GetMesh() const { return m_mesh; }
        [[nodiscard]] uint32 GetTexture() const { return m_texture; }
        [[nodiscard]] uint32 GetCollisionMesh() const { return m_colMesh; }

        [[nodiscard]] const Transform& GetTransform() const { return m_transform; }

    private:
        uint32 m_mesh = 0;
        uint32 m_texture = 0;
        uint32 m_colMesh = 0;

        Transform m_transform;
    };
}
