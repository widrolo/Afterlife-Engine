#include "PhysicsHandler.h"

#include <Engine/EngineDefines.h>

#include <box3d/box3d.h>

#include "Engine/Util/Log.h"
#include "glm/trigonometric.hpp"

using namespace WEngine;

PhysicsHandler::PhysicsHandler()
{
	Setup();
}

void PhysicsHandler::Tick()
{
	b3World_Step(m_worldID, PhysicsSettings::physicsTickRate, 1);
	for (auto& body : m_bodies)
		UpdateAttachedEntity(body);
}

PhysicsBodyHandle PhysicsHandler::CreateBody(PhysicsBodyType type, Entity *entity)
{
	b3BodyDef groundBodyDef = b3DefaultBodyDef();
	if (type == PhysicsBodyType::Dynamic)
		groundBodyDef.type = b3_dynamicBody;
	b3BodyId bodyId = b3CreateBody(m_worldID, &groundBodyDef);;

	b3BoxHull hull = b3MakeCubeHull(0.1f);;

	b3ShapeDef shapeDef = b3DefaultShapeDef();
	shapeDef.density = 1.0f;
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

void PhysicsHandler::ChangeBodyRotation(PhysicsBodyHandle body, const Vector3 &rotation)
{
	if (body == 0 || body > m_bodies.size())
		return;

	PhysicsBody& physicsBody = m_bodies[body - 1];

	b3Quat qx = b3MakeQuatFromAxisAngle({1.0f, 0.0f, 0.0f}, glm::radians(rotation.x));
	b3Quat qy = b3MakeQuatFromAxisAngle({0.0f, 1.0f, 0.0f}, glm::radians(rotation.y));
	b3Quat qz = b3MakeQuatFromAxisAngle({0.0f, 0.0f, 1.0f}, glm::radians(rotation.z));

	b3Quat rot = b3MulQuat(b3MulQuat(qz, qy), qx);
	b3Pos pos = b3Body_GetPosition(physicsBody.bodyId);

	b3Body_SetTransform(physicsBody.bodyId, pos, rot);
}

void PhysicsHandler::AttachBox(PhysicsBodyHandle body, const Vector3 &size, const Vector3 &offset)
{
	if (body == 0 || body > m_bodies.size())
		return;

	PhysicsBody& physicsBody = m_bodies[body - 1];

	b3BoxHull hull = b3MakeBoxHull(size.x, size.y, size.z);

	b3ShapeDef shapeDef = b3DefaultShapeDef();
	shapeDef.density = 1.0f;
	b3CreateHullShape(physicsBody.bodyId, &shapeDef, &hull.base);
}


void PhysicsHandler::Setup()
{
	auto def = b3DefaultWorldDef();
	def.gravity = {0.0f, -9.81f, 0.0f};
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
	b3Pos pos = b3Body_GetPosition(body.bodyId);
	tra.position = Vector3::B3DtoVec(pos);

	b3Quat rot = b3Body_GetRotation(body.bodyId);

	const float32 x = rot.v.x;
	const float32 y = rot.v.y;
	const float32 z = rot.v.z;
	const float32 w = rot.s;

	// roll (X)
	float sinr_cosp = 2.0f * (w * x + y * z);
	float cosr_cosp = 1.0f - 2.0f * (x * x + y * y);
	float angleX = atan2f(sinr_cosp, cosr_cosp);

	// pitch (Y)
	float sinp = 2.0f * (w * y - z * x);
	sinp = sinp > 1.0f ? 1.0f : (sinp < -1.0f ? -1.0f : sinp);
	float angleY = asinf(sinp);

	// yaw (Z)
	float siny_cosp = 2.0f * (w * z + x * y);
	float cosy_cosp = 1.0f - 2.0f * (y * y + z * z);
	float angleZ = atan2f(siny_cosp, cosy_cosp);

	tra.rotation = Vector3(glm::degrees(angleX), glm::degrees(angleY), glm::degrees(angleZ));

}
