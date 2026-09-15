#pragma once

#include <box3d/box3d.h>

#include "Engine/Math/Transform.h"
#include "Engine/Math/Other/Quaternion.h"
#include "Engine/Types/Physics/BodyHandle.h"
#include "Engine/Types/Physics/BodyType.h"
#include "Engine/Types/Physics/PhysicsBody.h"
#include "Engine/Types/Rendering/MeshInfo.h"
#include "Engine/WTL/vector.h"

namespace WEngine
{
	class Engine;
	class PhysicsWatchWidget;
	class PhysicsHandler
	{
		friend Engine;
		friend PhysicsWatchWidget;
	public:
		PhysicsHandler();

	public:
		void Tick();

		PhysicsBodyHandle CreateBody(PhysicsBodyType type, Transform *entity);
		SectorPhysicsBodyHandle CreateSectorBody(Transform& transform, b3MeshData* meshData);
		void ChangeBodyPosition(PhysicsBodyHandle body, const Vector3& position);
		void ChangeBodyRotation(PhysicsBodyHandle body, const Quaternion& rotation);
		void AttachBox(PhysicsBodyHandle body, const Vector3& size, const Vector3& offset);
		void AttachMesh(PhysicsBodyHandle body, const Vector3& size, const Vector3& offset, uint32 meshUID);
		void AttachMesh(PhysicsBodyHandle body, const MeshInfo& mesh);

		b3MeshData* CreateMesh(const byte* vertices, const byte* indices, sizeT vertCount, sizeT indCount);

	private:
		void Setup();
		void Visualize();

		void UpdateAttachedEntity(PhysicsBody& body);

	private:
		b3WorldId m_worldID;
		wtl::vector<PhysicsBody> m_bodies;
		wtl::vector<b3BodyId> m_secBodies;
	};
}



