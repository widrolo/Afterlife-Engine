#include "PhysicsTest.h"

PhysicsTest::PhysicsTest()
{
    WEngine::CoreSystems::GetPhysicsHandler();

    m_mesh = WEngine::CoreSystems::GetAssetRepo()->GetFirstAssetInDirOfType("/Testing/Cube", WEngine::AssetType::StaticMesh);
    m_texture = WEngine::CoreSystems::GetAssetRepo()->GetFirstAssetInDirOfType("/Testing/Cube", WEngine::AssetType::Texture);
    //m_collider = WEngine::CoreSystems::GetAssetRepo()->GetFirstAssetInDirOfType("/Testing/Monkey", WEngine::AssetType::PhysicsMesh);

    m_testBodies.reserve(500);
}

void PhysicsTest::Tick(float32 dt)
{
    static float32 timer;
    timer += dt;
    if (timer < 1.0f)
        return;
    timer = 0.0f;

    WEngine::WLog::ConsoleLog("Spawn");

    PhysicsTestBody phBody0;
    m_testBodies.push_back(phBody0);
    PhysicsTestBody& phBody = m_testBodies.back();

    phBody.transform = WEngine::Transform::Zero;
    phBody.transform.position.y = 10;

    phBody.body = WEngine::CoreSystems::GetPhysicsHandler()->CreateBody(WEngine::PhysicsBodyType::Dynamic, &phBody.transform);
    WEngine::CoreSystems::GetPhysicsHandler()->AttachBox(phBody.body, phBody.transform.size, WEngine::Vector3::Zero);
    WEngine::CoreSystems::GetPhysicsHandler()->AttachBox(phBody.body, phBody.transform.size, WEngine::Vector3::Zero);
}

void PhysicsTest::Draw()
{
    WEngine::RenderMission mission;
    mission.meshUID = m_mesh;
    mission.textureUID = m_texture;

    for (const auto& body : m_testBodies)
    {
        mission.transform = body.transform;
        WEngine::CoreSystems::GetRenderHandler()->AddToRenderQueue(mission);
    }
}
