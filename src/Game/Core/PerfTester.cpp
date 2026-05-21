#include "PerfTester.h"

#include "Engine/Types/SpawnArgs.h"


PerfTester::PerfTester(WEngine::Sector* testworld)
{
    WEngine::Transform t = WEngine::Transform();
    t.position = WEngine::Vector3::Zero;
    t.rotation = WEngine::Vector3::Zero;
    t.size = WEngine::Vector3::One;

    WEngine::ComponentArgs ca;

    ca.componentTypeId = 14;
    WEngine::SpawnArgs args;
    args.name = "e";
    args.transform = t;
    args.ca = {ca};

    const uint32 sideLen = 24;
    const float32 stride = 3.0f;
    for (uint32 i = 0; i < sideLen; ++i)
    {
        for (uint32 j = 0; j < sideLen; ++j)
        {
            for (uint32 k = 0; k < sideLen; ++k)
            {
                args.transform = t;
                testworld->AddEntityPost(args);
                t.position.x += stride;
            }
            t.position.y -= stride;
            t.position.x = 0.0f;
        }
        t.position.z += stride;
        t.position.y = 0.0f;
    }


}
