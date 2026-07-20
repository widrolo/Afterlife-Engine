#pragma once

#include <memory>
#include <string>
#include <Testing/Types/Test.h>
#include "Engine/WTL/vector.h"

namespace WTest
{
    class TestCollection
    {
    public:
        TestCollection() = default;
        virtual ~TestCollection() = default;

    public:
        template<class T>
        void AddTest()
        {
            static_assert(std::is_base_of_v<Test, T>, "AddTest: T must inherit from Test");
            auto test = std::make_unique<T>();
            test->Setup();
            m_tests.push_back(std::move(test));
        }

        virtual void SetupCollection() = 0;
        virtual void RunAllTests();

    protected:
        wtl::vector<std::unique_ptr<Test>> m_tests;
        std::string m_name;
    };
}


