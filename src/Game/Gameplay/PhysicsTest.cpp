#include "PhysicsTest.h"

constexpr sizeT BodiesCount = 35;
constexpr float32 SpawnRate = 0.05f;

PhysicsTest::PhysicsTest()
{
    WEngine::CoreSystems::GetPhysicsHandler();

    m_mesh = WEngine::CoreSystems::GetAssetRepo()->GetFirstAssetInDirOfType("/Testing/Cubes/Cube", WEngine::AssetType::StaticMesh);
    m_texture = WEngine::CoreSystems::GetAssetRepo()->GetFirstAssetInDirOfType("/Testing/Cubes/Cube", WEngine::AssetType::Texture);
    // not for physics but rather for the rendering.
    m_collider = WEngine::CoreSystems::GetAssetRepo()->GetFirstAssetInDirOfType("/Testing/Cubes/Cube", WEngine::AssetType::PhysicsMesh);

    m_testBodies.reserve(BodiesCount);
}

void PhysicsTest::Tick(float32 dt)
{
    static float32 timer;
    timer += dt;
    if (timer < SpawnRate)
        return;
    timer = 0.0f;
    if (m_testBodies.size() >= BodiesCount)
        return;

    PhysicsTestBody phBody0;
    m_testBodies.push_back(phBody0);
    PhysicsTestBody& phBody = m_testBodies.back();

    auto rngPos = WEngine::CoreSystems::GetRNGHandler()->GetRandomVector2(5);
    phBody.transform = WEngine::Transform::Zero;
    phBody.transform.position.x = rngPos.x;
    phBody.transform.position.y = 10;
    phBody.transform.position.z = rngPos.y;

    phBody.body = WEngine::CoreSystems::GetPhysicsHandler()->CreateBody(WEngine::PhysicsBodyType::Dynamic, &phBody.transform);
    WEngine::CoreSystems::GetPhysicsHandler()->AttachBox(phBody.body, phBody.transform.size, WEngine::Vector3::Zero);
}

void PhysicsTest::Draw()
{
    WEngine::RenderMission mission;
    mission.meshUID = m_mesh;
    mission.textureUID = m_texture;
    mission.phyMeshUID = m_collider;
    mission.flags = WEngine::RenderMissionFlags::PhysicsDynamic;

    for (const auto& body : m_testBodies)
    {
        mission.transform = body.transform;
        mission.flags = WEngine::RenderMissionFlags::PhysicsDynamic;
        if (WEngine::CoreSystems::GetPhysicsHandler()->IsBodySleeping(body.body))
            mission.flags = WEngine::RenderMissionFlags::PhysicsSleeping;
        WEngine::CoreSystems::GetRenderHandler()->AddToRenderQueue(mission);
    }
}
