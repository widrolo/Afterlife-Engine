#pragma once
#include <string>

#include "Engine/Types/CommonTypes.h"

namespace WTest
{
    class Test
    {
    public:
        Test() = default;
        virtual ~Test() = default;

    public:
        virtual void Setup() = 0;
        [[nodiscard]] virtual bool Run() = 0;
        [[nodiscard]] std::string GetName() const { return m_name; }
        [[nodiscard]] uint64 GetFailure() const { return m_failurePoint; }

    protected:
        std::string m_name{};
        // since a test can run multiple tests that may fail, this shows clearly where a test has failed
        // without a random log from a test. This allows the programmer to see where exactly a fail occurred.
        uint64 m_failurePoint{};
    };
}

#define failTest(point) do { m_failurePoint = (point); return false; } while(0)
