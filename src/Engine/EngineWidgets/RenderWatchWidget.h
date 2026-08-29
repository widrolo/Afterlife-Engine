#pragma once
#include "Engine/Core/Widget.h"

struct MemListDebugInfo;

namespace WEngine
{
    class Sector;
    class RenderWatchWidget : public Widget
    {
    public:
        using Widget::Widget;

    public:
        void Setup() override;
    protected:
        void RenderInternal() override;

    private:
        void Header() const;
        void VramDisplay() const;
        void PrintVramUsage(const std::string& category, sizeT sizeInBytes) const;
        void VramDisplayBuffers() const;
        void VramDisplayTextures() const;
        void VramDisplayShaders() const;

        void RenderDisplay() const;
        void PrintRenderUsage(const std::string& category, sizeT count) const;
        void RenderDisplayDrawcall() const;
        void RenderDisplayBindings() const;

        void ShowPassSettings();
    };
}
