#pragma once

#include <box3d/box3d.h>

#include "Engine/Core/World/Entity.h"
#include "Engine/Types/Physics/BodyHandle.h"
#include "Engine/Types/Physics/BodyType.h"
#include "Engine/Types/Physics/PhysicsBody.h"
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

		PhysicsBodyHandle CreateBody(PhysicsBodyType type, Entity *entity);
		void ChangeBodyPosition(PhysicsBodyHandle body, const Vector3& position);
		void ChangeBodyRotation(PhysicsBodyHandle body, const Vector3& rotation);
	private:
		void Setup();
		void Visualize();

		void UpdateAttachedEntity(PhysicsBody& body);

	private:
		b3WorldId m_worldID;
		wtl::vector<PhysicsBody> m_bodies;
	};
}



