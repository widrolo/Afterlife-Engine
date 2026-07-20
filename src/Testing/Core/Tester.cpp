#ifdef TESTING


#include "Tester.h"

#include "Engine/Util/Log.h"
#include "Testing/Tests/Types/Version_Test.h"
#include "Testing/Types/TestStatistics.h"

using namespace WTest;

Tester::Tester()
{
    Setup();
    Run();

    WEngine::WLog::SetConsoleInfo();
    WEngine::WLog::LogTest("\nUnit tests have concluded, here are the statistics:");
    WEngine::WLog::SetConsoleSuccess();
    WEngine::WLog::LogTest(std::format("Passed: \t{}", TestStatistics::passed));
    WEngine::WLog::SetConsoleWarning();
    WEngine::WLog::LogTest(std::format("Failed: \t{}", TestStatistics::failed));
    if (TestStatistics::failed > 0)
    {
        for (const auto &point : TestStatistics::failPoint)
        {
            WEngine::WLog::SetConsoleWarning();
            WEngine::WLog::LogTest(std::format(" - Collection \"{}\" in \"{}\" at \"{}\"", point.collection,
                point.test, point.point));
        }
    }
    WEngine::WLog::SetConsoleError();
    WEngine::WLog::LogTest(std::format("Crashed:\t{}", TestStatistics::crashed));
    if (TestStatistics::crashed > 0)
    {
        for (const auto &point : TestStatistics::crashPoint)
        {
            WEngine::WLog::SetConsoleError();
            WEngine::WLog::LogTest(std::format(" - Collection \"{}\" in \"{}\"", point.collection, point.test));
        }
    }
}

void Tester::Setup()
{
    AddCollection<Version_TC>();
}

void Tester::Run()
{
    for (auto& test : m_collections)
        test->RunAllTests();
}

#endif
