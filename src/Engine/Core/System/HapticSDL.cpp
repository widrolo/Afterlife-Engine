#if HAPTIC_BACKEND == HAPTIC_SDL

#include "Haptic.h"
#include "Editor/Types/EditorSystems.h"
#include "Engine/imgui/backends/imgui_impl_sdl3.h"
#include "Engine/Types/CoreSystems.h"
#include "Engine/Util/Log.h"
#include "Haptic/SDL/SDLContext.h"
#include "Haptic/SDL/SDLFetching.h"
#include "Haptic/SDL/SDLCore.h"

static SDLContext ctx;

void Haptic::Init(SDL_Window* window)
{
    ctx.window = window;
    InitKeyboard(ctx);
    InitMouse(ctx);
    InitController(ctx);
}

void Haptic::FetchInput()
{
    AdvanceBelts(ctx);
    CheckControllerStatus(ctx);
    FetchAllInput(ctx);
    PollEvents();
    TranslateFetched(ctx);
    UpdateAllSenses(ctx);
}

void Haptic::PollEvents()
{
    while (SDL_PollEvent(&ctx.event))
    {
        ImGui_ImplSDL3_ProcessEvent(&ctx.event);
        if (ctx.event.type == SDL_EVENT_WINDOW_CLOSE_REQUESTED)
        {
            if (!ctx.isEditor)
                WEngine::CoreSystems::ShutdownGame();
            else
                WEditor::EditorSystems::ShutdownEditor();
        }
    }
}

void Haptic::EnableEditorMode()
{
    ctx.isEditor = true;
}

void Haptic::LoadInputMap(const wtl::vector<WEngine::InputSense> &mapContent, const std::string &mapName)
{
    SDLMap map;
    map.name = mapName;

    for (const auto& sense : mapContent)
    {
        auto s = SenseToSDLSense(ctx, sense);
        map.senses.push_back(s);
    }

    ctx.maps.push_back(map);
}

void Haptic::SelectInputMap(const std::string &mapName)
{
    for (auto& map : ctx.maps)
    {
        if (map.name == mapName)
        {
            ctx.selectedMap = &map;
            return;
        }
    }
    WEngine::WLog::SetConsoleError();
    WEngine::WLog::ConsoleLog(std::format("Unable to find map {}", mapName));
}

void Haptic::LoadOutputMap(const std::string &mapName)
{

}

void Haptic::SelectOutputMap(const std::string &mapName)
{

}

void Haptic::ApplyFullPatch(const wtl::vector<WEngine::InputPatch> &patches)
{

}

void Haptic::ApplySinglePatch(const WEngine::InputPatch &patch)
{

}

wtl::vector<WEngine::InputPatch> Haptic::GetPatchList()
{

}

WEngine::Nullable<bool> Haptic::GetActionJustPressed(const std::string &actionName)
{
    if (ctx.selectedMap == nullptr)
        return {};
    if (!ctx.selectedMap->results.contains(actionName))
        return {};
    return ctx.selectedMap->results[actionName].justPressed;
}

WEngine::Nullable<bool> Haptic::GetActionHeld(const std::string &actionName)
{
    if (ctx.selectedMap == nullptr)
        return {};
    if (!ctx.selectedMap->results.contains(actionName))
        return {};
    return ctx.selectedMap->results[actionName].held;
}

WEngine::Nullable<bool> Haptic::GetActionJustReleased(const std::string &actionName)
{
    if (ctx.selectedMap == nullptr)
        return {};
    if (!ctx.selectedMap->results.contains(actionName))
        return {};
    return ctx.selectedMap->results[actionName].justReleased;
}

WEngine::Nullable<float32> Haptic::GetFloatDelta(const std::string &floatName)
{

}

WEngine::Nullable<float32> Haptic::GetFloatPos(const std::string &floatName)
{

}

WEngine::Nullable<WEngine::Vector2> Haptic::GetVectorPos(const std::string &vectorName)
{

}

WEngine::Nullable<WEngine::Vector2> Haptic::GetVectorDelta(const std::string &vectorName)
{

}

WEngine::Nullable<float32> Haptic::GetVectorLength(const std::string &vectorName)
{

}

WEngine::Nullable<WEngine::InputVendor> Haptic::GetVendor()
{

}

bool Haptic::GetDebugKeyJustPressed(uint8 keyNum)
{
    if (keyNum > 12)
        return false;
    return ctx.rawKeys[0][(sizeT)WKey::DEBUG1 + keyNum - 1] && !ctx.rawKeys[1][(sizeT)WKey::DEBUG1 + keyNum - 1];
}

bool Haptic::GetDebugKeyHeld(uint8 keyNum)
{
    if (keyNum > 12)
        return false;
    return ctx.rawKeys[0][(sizeT)WKey::DEBUG1 + keyNum - 1];
}

bool Haptic::GetDebugKeyJustReleased(uint8 keyNum)
{
    if (keyNum > 12)
        return false;
    return !ctx.rawKeys[0][(sizeT)WKey::DEBUG1 + keyNum - 1] && ctx.rawKeys[1][(sizeT)WKey::DEBUG1 + keyNum - 1];
}

void Haptic::Rumble(const std::string &outputName)
{

}

void Haptic::SetLED(const std::string &outputName)
{

}

#endif
