#pragma once
#include "Engine/Core/World/Sector.h"


class PerfTester
{
public:
    PerfTester(WEngine::Sector* testworld);

private:
    void LotsOfCubesMonkey(WEngine::Sector* testworld);
    void Dragon(WEngine::Sector* testworld);
};
