#include "PhysicsHandler.h"

#include <Engine/EngineDefines.h>

#include <box3d/box3d.h>

#include "AssetRepo.h"
#include "Engine/Types/CoreSystems.h"
#include "Engine/Util/Log.h"
#include "Engine/Util/TimeAnalysis.h"
#include "glm/trigonometric.hpp"

using namespace WEngine;

struct MoverPlaneContext
{
	b3CollisionPlane* planes;
	int32 capacity;
	int32 count;
};

bool CollectMoverPlanes(b3ShapeId shapeId, const b3PlaneResult* plane, int planeCount, void* context)
{
	(void)shapeId;
	auto* ctx = static_cast<MoverPlaneContext*>(context);

	for (int32 i = 0; i < planeCount && ctx->count < ctx->capacity; i++)
	{
		b3CollisionPlane& out = ctx->planes[ctx->count++];
		out.plane = plane[i].plane;
		out.pushLimit = max_float32;
		out.push = 0.0f;
		out.clipVelocity = true;
	}

	return true;
}

PhysicsHandler::PhysicsHandler()
{
	Setup();

}

void PhysicsHandler::Tick()
{
	TimeSample sample("PhysicsHandler::Tick");
	b3World_Step(m_worldID, PhysicsSettings::physicsTickRate * CoreSystems::GetTimeScale(), 8);
	for (auto& body : m_bodies)
		UpdateAttachedEntity(body);
}

PhysicsBodyHandle PhysicsHandler::CreateBody(PhysicsBodyType type, Transform *entity)
{
	TimeSample sample("PhysicsHandler::CreateBody");
	b3BodyDef bodyDef = b3DefaultBodyDef();
	if (type == PhysicsBodyType::Dynamic)
		bodyDef.type = b3_dynamicBody;
	bodyDef.position = reinterpret_cast<b3Pos&&>(entity->position);
	//bodyDef.rotation = reinterpret_cast<b3Quat&&>(entity->rotation);
	b3BodyId bodyId = b3CreateBody(m_worldID, &bodyDef);;

	PhysicsBody body{};
	body.bodyId = bodyId;
	body.transform = entity;
	body.type = type;
	m_bodies.push_back(body);

	return m_bodies.size();
}

CharacterBodyHandle PhysicsHandler::CreateCharacter(const b3Capsule& mover)
{
	CharacterBody character{};
	character.mover = mover;
	m_characterBodies.push_back(character);
	return m_characterBodies.size();
}

SectorPhysicsBodyHandle PhysicsHandler::CreateSectorBody(Transform &transform, b3MeshData *meshData)
{
	b3BodyDef bodyDef = b3DefaultBodyDef();
	bodyDef.position = reinterpret_cast<b3Pos&&>(transform.position);
	bodyDef.rotation = reinterpret_cast<b3Quat&&>(transform.rotation);

	b3ShapeDef shapeDef = b3DefaultShapeDef();
	shapeDef.density = 1.0f;
	shapeDef.baseMaterial.friction = 1.0f;

	b3Vec3 scale = reinterpret_cast<b3Vec3&&>(transform.size);

	b3BodyId groundId = b3CreateBody(m_worldID, &bodyDef);
	b3CreateMeshShape(groundId, &shapeDef, meshData, scale);

	m_secBodies.push_back(groundId);
	return m_secBodies.size();
}

void PhysicsHandler::ChangeBodyPosition(PhysicsBodyHandle body, const Vector3 &position)
{
	TimeSample sample("PhysicsHandler::ChangeBodyPosition");
	if (body == 0 || body > m_bodies.size())
		return;

	PhysicsBody& physicsBody = m_bodies[body - 1];

	b3Body_SetTransform(physicsBody.bodyId, Vector3::VecToB3D(position), b3Body_GetRotation(physicsBody.bodyId));
}

void PhysicsHandler::ChangeBodyRotation(PhysicsBodyHandle body, const Quaternion &rotation)
{
	TimeSample sample("PhysicsHandler::ChangeBodyRotation");
	if (body == 0 || body > m_bodies.size())
		return;

	PhysicsBody& physicsBody = m_bodies[body - 1];

	b3Quat rot = Quaternion::QuatToB3D(rotation);
	b3Pos pos = b3Body_GetPosition(physicsBody.bodyId);

	b3Body_SetTransform(physicsBody.bodyId, pos, rot);
}

void PhysicsHandler::AttachBox(PhysicsBodyHandle body, const Vector3 &size, const Vector3 &offset)
{
	TimeSample sample("PhysicsHandler::AttachBox");
	if (body == 0 || body > m_bodies.size())
		return;

	PhysicsBody& physicsBody = m_bodies[body - 1];

	// b3SetLengthUnitsPerMeter doesnt seem to be working.
	const float32 factor = 0.5f;
	Vector3 newSize = size * factor;

	b3Vec3 off = Vector3::VecToB3D(offset);
	//off.z = -off.z;
	b3BoxHull hull = b3MakeOffsetBoxHull(newSize.x, newSize.y, newSize .z, off);

	b3ShapeDef shapeDef = b3DefaultShapeDef();
	shapeDef.density = 1.0f;
	shapeDef.baseMaterial.friction = 0.3f;
	b3CreateHullShape(physicsBody.bodyId, &shapeDef, &hull.base);
}

void PhysicsHandler::AttachMesh(PhysicsBodyHandle body, const Vector3 &size, const Vector3 &offset, uint32 meshUID)
{
	TimeSample sample("PhysicsHandler::AttachBox");
	if (body == 0 || body > m_bodies.size())
		return;

	PhysicsBody& physicsBody = m_bodies[body - 1];

	auto mesh = CoreSystems::GetAssetRepo()->GetMeshData(meshUID);

	const float32 factor = 0.5f;
	Vector3 newSize = size * factor;

	b3ShapeDef shapeDef = b3DefaultShapeDef();
	shapeDef.density = 1.0f;
	shapeDef.baseMaterial.friction = 0.3f;
	b3CreateMeshShape(physicsBody.bodyId, &shapeDef, mesh, Vector3::VecToB3D(size));
}

void PhysicsHandler::AttachMesh(PhysicsBodyHandle body, const MeshInfo &mesh)
{
	TimeSample sample("PhysicsHandler::AttachMesh");
	if (body == 0 || body > m_bodies.size())
		return;

	PhysicsBody& physicsBody = m_bodies[body - 1];



}

void PhysicsHandler::MoveCharacter(CharacterBodyHandle character, const Vector3 &translation)
{
	if (character == 0 || character > m_characterBodies.size())
		return;

	b3Capsule& mover = m_characterBodies[character - 1].mover;

	b3Pos origin = b3Pos_zero;
	b3Vec3 transl = Vector3::VecToB3D(translation);

	b3QueryFilter filter = b3DefaultQueryFilter();

	float32 fraction = b3World_CastMover(m_worldID, origin, &mover, transl, filter, nullptr, nullptr);
	b3Vec3 safeDelta = b3MulSV(fraction, transl);

	mover.center1 = b3Add(mover.center1, safeDelta);
	mover.center2 = b3Add(mover.center2, safeDelta);

	constexpr sizeT MaxPlanes = 16;
	b3CollisionPlane collisionPlanes[MaxPlanes];
	MoverPlaneContext planeContext{ collisionPlanes, (int32)MaxPlanes, 0 };

	b3World_CollideMover(m_worldID, origin, &mover, filter, CollectMoverPlanes, &planeContext);
	b3PlaneSolverResult result = b3SolvePlanes(b3Vec3_zero, collisionPlanes, planeContext.count);
	mover.center1 = b3Add(mover.center1, result.delta);
	mover.center2 = b3Add(mover.center2, result.delta);
}

Vector3 PhysicsHandler::GetCharacterPosition(CharacterBodyHandle character)
{
	if (character == 0 || character > m_characterBodies.size())
		return {};

	b3Capsule& mover = m_characterBodies[character - 1].mover;
	return *(Vector3*)&mover.center1;
}

b3MeshData* PhysicsHandler::CreateMesh(const byte* vertices, const byte* indices, sizeT vertCount, sizeT indCount)
{
	b3MeshDef meshDef{};
	meshDef.vertices = (b3Vec3*)vertices;
	meshDef.indices = (int32*)indices;
	meshDef.vertexCount = (int32)vertCount;
	meshDef.triangleCount = (int32)indCount / 3; // watch this break
	meshDef.weldVertices = true;
	meshDef.weldTolerance = 0.01f;
	meshDef.identifyEdges = true;

	return b3CreateMesh(&meshDef, NULL, 0);
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
	TimeSample sample("PhysicsHandler::UpdateAttachedEntity");
	if (body.type != PhysicsBodyType::Dynamic)
		return;

	auto* tra = body.transform;

	b3Vec3 pos = b3Body_GetPosition(body.bodyId);
	b3Quat rot = b3Body_GetRotation(body.bodyId);

	tra->position = Vector3::B3DtoVec(pos);
	tra->rotation = Quaternion::B3DToQuat(rot);

	return;
}
