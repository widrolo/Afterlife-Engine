#pragma once

#include <Engine/Core/Widget.h>

namespace WEditor
{
    class EntryList : public WEngine::Widget
    {
    public:
        using Widget::Widget;

    public:
        void Setup() override;
    protected:
        void RenderInternal() override;

    private:
        static constexpr sizeT NameBufferMaxSize = 256;
        char* m_nameBuffer = nullptr;
    };
}

