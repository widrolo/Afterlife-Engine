#include "TimeAnalysis.h"

using namespace WEngine;

TimeSample::TimeSample(const std::string& sectionName)
{
    m_sectionName = sectionName;
    m_parent = TimeAnalysisState::bottomSample;
    TimeAnalysisState::bottomSample = this;

    m_time.Reset();
}

TimeSample::TimeSample(const std::string &sectionName, bool isRoot)
{
    m_sectionName = sectionName;
    TimeAnalysisState::rootSample = this;
    TimeAnalysisState::bottomSample = this;

    m_time.Reset();
}

void TimeSample::SaveAndResetRoot()
{
    TimeRecord record;
    record.sectionName = m_sectionName;
    record.loggedTime = m_time.GetTime<TimeUnit::Microseconds>();
    record.children = std::move(m_children);

    TimeAnalysisState::lastSample = std::move(record);
    m_children.clear();
}

TimeSample::~TimeSample()
{
    m_loggedTime = m_time.GetTime<TimeUnit::Microseconds>();
    TimeAnalysisState::bottomSample = m_parent;

    // we need to do this here and not in the constructor since its not gonna update
    // a copy, and tracking relies on destruction of samples.
    if (m_parent != nullptr) // because root eventually also destructs...
    {
        TimeRecord record;
        record.sectionName = std::move(m_sectionName);
        record.loggedTime = m_loggedTime;
        record.children = std::move(m_children);

        m_parent->m_children.push_back(std::move(record));
    }
}
