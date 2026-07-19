#pragma once

#include <Editor/Types/MenubarItem.h>

namespace WEditor
{
    class Menubar_RunTest : public MenubarItem
    {
    public:
        using MenubarItem::MenubarItem;

    protected:
        void Setup() override;
        void OnClick() override;

    };
}

