#pragma once

#include <Engine/Core/Widget.h>

#include "Editor/Core/World/EditorSectorEntry.h"

namespace WEditor
{
    class EntryData : public WEngine::Widget
    {
    public:
        using Widget::Widget;

    public:
        void Setup() override;
    protected:
        void RenderInternal() override;

    private:
        static constexpr sizeT NameBufferMaxSize = 256;
        static constexpr sizeT AssetBufferMaxSize = 1024;
        char* m_nameBuffer = nullptr;
        char* m_assetBuffer = nullptr;
        EditorSectorEntry* m_lastEntry = nullptr;
    };
}

