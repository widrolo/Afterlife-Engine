#include "TestCollection.h"

#include "TestStatistics.h"
#include "Engine/Util/Log.h"

using namespace WTest;

void TestCollection::RunAllTests()
{
    WEngine::WLog::SetConsoleInfo();
    WEngine::WLog::LogTest(std::format("Running all tests in collection \"{}\"...", m_name));
    for (auto& test : m_tests)
    {
        try
        {
            bool res = test->Run();
            if (res)
            {
                WEngine::WLog::SetConsoleSuccess();
                WEngine::WLog::LogTest(std::format("Test \"{}\" passed!", test->GetName()));
                TestStatistics::AddPassed();
            }
            else
            {
                WEngine::WLog::SetConsoleError();
                WEngine::WLog::LogTest(std::format("Test \"{}\" failed!", test->GetName()));
                TestStatistics::AddFailed({m_name, test->GetName(), test->GetFailure()});
            }
        }
        catch (std::exception& e)
        {
            WEngine::WLog::SetConsoleError();
            WEngine::WLog::LogTest(std::format("Catastrophic failure: Test \"{}\" threw an exception! \n\t {}",
                test->GetName(), e.what()));
            TestStatistics::AddCrashed({m_name, test->GetName(), test->GetFailure()});
        }
    }
}
