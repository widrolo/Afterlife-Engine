#pragma once
#include <string>

#include "Timer.h"
#include "Engine/Types/CommonTypes.h"
#include "Engine/WTL/vector.h"

namespace WEngine
{
    struct TimeRecord
    {
        std::string sectionName;
        float64 loggedTime = 0;
        wtl::vector<TimeRecord> children;
    };

    // root is a special one, it should never get destroyed.
    class TimeSample
    {
    public:
        TimeSample() = default;
        TimeSample(const std::string& sectionName);
        TimeSample(const std::string& sectionName, bool isRoot);

        // assumes we are root.
        void SaveAndResetRoot();

        ~TimeSample();
    private:
        wtl::vector<TimeRecord> m_children;
        TimeSample* m_parent{};
        std::string m_sectionName;
        StopWatch m_time;
        float64 m_loggedTime;
    };
    struct TimeAnalysisState
    {
        _GLOBAL_ TimeRecord lastSample;
        _GLOBAL_ TimeSample* rootSample;
        // since we dont have MT, we always have at least one bottom most recording sample.
        // New samples will use this as their parent. On death they will assign this to their parent.
        _GLOBAL_ TimeSample* bottomSample;
    };
}
