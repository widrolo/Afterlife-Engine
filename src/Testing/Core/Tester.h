#pragma once
#include <memory>

#include "Engine/WTL/vector.h"
#include "Testing/Types/TestCollection.h"

namespace WTest
{
    class Tester
    {
    public:
        Tester();
        ~Tester() = default;

    private:
        template<class T>
        void AddCollection()
        {
            static_assert(std::is_base_of_v<TestCollection, T>, "AddCollection: T must inherit from TestCollection");
            auto test = std::make_unique<T>();
            test->SetupCollection();
            m_collections.push_back(std::move(test));
        }

        void Setup();
        void Run();

    private:
        wtl::vector<std::unique_ptr<TestCollection>> m_collections;
    };
}

