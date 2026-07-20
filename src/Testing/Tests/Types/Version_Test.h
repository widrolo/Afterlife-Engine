#pragma once
#ifdef TESTING

#include <Testing/Types/Test.h>
#include <Testing/Types/TestCollection.h>

namespace WTest
{
    class Version_TC : public TestCollection
    {
    public:
        void SetupCollection() override;
    };

    class Version_Comparison_Test : public Test
    {
    public:
        void Setup() override;
        bool Run() override;
    };
    class Version_String_Test : public Test
    {
    public:
        void Setup() override;
        bool Run() override;
    };
}

#endif