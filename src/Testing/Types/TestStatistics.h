#ifdef TESTING


#pragma once

#include <Engine/Types/CommonTypes.h>

namespace WTest
{
    struct FailurePoint
    {
        std::string collection;
        std::string test;
        uint64 point;
    };
    struct TestStatistics
    {
        friend class Tester;
    private:
        _GLOBAL_ uint64 passed;
        _GLOBAL_ uint64 failed;
        _GLOBAL_ uint64 crashed;

        _GLOBAL_ wtl::vector<FailurePoint> failPoint;
        _GLOBAL_ wtl::vector<FailurePoint> crashPoint;

    public:
        static void AddPassed()  { passed++; }
        static void AddFailed(const FailurePoint& collection)
        {
            failed++;
            failPoint.push_back(collection);
        }
        static void AddCrashed(const FailurePoint& collection)
        {
            crashed++;
            crashPoint.push_back(collection);
        }
    };
}

#endif