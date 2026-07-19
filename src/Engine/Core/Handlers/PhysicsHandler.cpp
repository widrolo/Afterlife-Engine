#include "PhysicsHandler.h"

#include <Engine/EngineDefines.h>

#include <box3d/box3d.h>

#include "Engine/Types/CoreSystems.h"
#include "Engine/Util/Log.h"
#include "glm/trigonometric.hpp"

using namespace WEngine;

PhysicsHandler::PhysicsHandler()
{
	Setup();
}

void PhysicsHandler::Tick()
{
	b3World_Step(m_worldID, PhysicsSettings::physicsTickRate * CoreSystems::GetTimeScale(), 8);
	for (auto& body : m_bodies)
		UpdateAttachedEntity(body);
}

PhysicsBodyHandle PhysicsHandler::CreateBody(PhysicsBodyType type, Entity *entity)
{
	b3BodyDef groundBodyDef = b3DefaultBodyDef();
	if (type == PhysicsBodyType::Dynamic)
		groundBodyDef.type = b3_dynamicBody;
	b3BodyId bodyId = b3CreateBody(m_worldID, &groundBodyDef);;

	b3BoxHull hull = b3MakeCubeHull(0.1f);

	b3ShapeDef shapeDef = b3DefaultShapeDef();
	shapeDef.density = 1.0f;
	shapeDef.baseMaterial.friction = 1.0f;
	b3CreateHullShape(bodyId, &shapeDef, &hull.base);

	PhysicsBody body{};
	body.bodyId = bodyId;
	body.entity = entity;
	body.type = type;
	m_bodies.push_back(body);

	return m_bodies.size();
}

void PhysicsHandler::ChangeBodyPosition(PhysicsBodyHandle body, const Vector3 &position)
{
	if (body == 0 || body > m_bodies.size())
		return;

	PhysicsBody& physicsBody = m_bodies[body - 1];

	b3Body_SetTransform(physicsBody.bodyId, Vector3::VecToB3D(position), b3Body_GetRotation(physicsBody.bodyId));
}

void PhysicsHandler::ChangeBodyRotation(PhysicsBodyHandle body, const Quaternion &rotation)
{
	if (body == 0 || body > m_bodies.size())
		return;

	PhysicsBody& physicsBody = m_bodies[body - 1];

	b3Quat rot = Quaternion::QuatToB3D(rotation);
	b3Pos pos = b3Body_GetPosition(physicsBody.bodyId);

	b3Body_SetTransform(physicsBody.bodyId, pos, rot);
}

void PhysicsHandler::AttachBox(PhysicsBodyHandle body, const Vector3 &size, const Vector3 &offset)
{
	if (body == 0 || body > m_bodies.size())
		return;

	PhysicsBody& physicsBody = m_bodies[body - 1];

	// b3SetLengthUnitsPerMeter doesnt seem to be working.
	const float32 factor = 0.5f;
	Vector3 newSize = size * factor;

	b3BoxHull hull = b3MakeBoxHull(newSize.x, newSize.y, newSize .z);

	b3ShapeDef shapeDef = b3DefaultShapeDef();
	shapeDef.density = 1.0f;
	shapeDef.baseMaterial.friction = 0.3f;
	b3CreateHullShape(physicsBody.bodyId, &shapeDef, &hull.base);
}

void PhysicsHandler::AttachMesh(PhysicsBodyHandle body, const MeshInfo &mesh)
{
	if (body == 0 || body > m_bodies.size())
		return;

	PhysicsBody& physicsBody = m_bodies[body - 1];



}


void PhysicsHandler::Setup()
{
	auto def = b3DefaultWorldDef();
	def.gravity = {0.0f, -20.0f, 0.0f};
	m_worldID = b3CreateWorld(&def);
}


void PhysicsHandler::Visualize()
{

}

void PhysicsHandler::UpdateAttachedEntity(PhysicsBody& body)
{
	if (body.type != PhysicsBodyType::Dynamic)
		return;

	auto& tra = body.entity->transform;

	b3Vec3 pos = b3Body_GetPosition(body.bodyId);
	b3Quat rot = b3Body_GetRotation(body.bodyId);

	tra.position = Vector3::B3DtoVec(pos);
	tra.rotation = Quaternion::B3DToQuat(rot);

}
