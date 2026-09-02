#if HAPTIC_BACKEND == HAPTIC_SDL

#include "Haptic.h"
#include "Editor/Types/EditorSystems.h"
#include "Engine/imgui/backends/imgui_impl_sdl3.h"
#include "Engine/Types/CoreSystems.h"
#include "Engine/Util/Log.h"
#include "Engine/Util/TimeAnalysis.h"
#include "Haptic/SDL/SDLContext.h"
#include "Haptic/SDL/SDLFetching.h"
#include "Haptic/SDL/SDLCore.h"

static SDLContext ctx;

namespace Haptic
{
    void Init(SDL_Window* window)
    {
        ctx.window = window;
        InitKeyboard(ctx);
        InitMouse(ctx);
        InitController(ctx);
    }

    void FetchInput()
    {
        WEngine::TimeSample sample("Haptic::FetchInput");
        AdvanceBelts(ctx);
        CheckControllerStatus(ctx);
        FetchAllInput(ctx);
        PollEvents();
        TranslateFetched(ctx);
        UpdateAllSenses(ctx);
    }

    void PollEvents()
    {
        WEngine::TimeSample sample("[Haptic]PollEvents");
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

    void EnableEditorMode()
    {
        ctx.isEditor = true;
    }

    void LoadInputMap(const wtl::vector<WEngine::InputSense> &mapContent, const std::string &mapName)
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

    void SelectInputMap(const std::string &mapName)
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

    void LoadOutputMap(const std::string &mapName)
    {

    }

    void SelectOutputMap(const std::string &mapName)
    {

    }

    void ApplyFullPatch(const wtl::vector<WEngine::InputPatch> &patches)
    {

    }

    void ApplySinglePatch(const WEngine::InputPatch &patch)
    {

    }

    wtl::vector<WEngine::InputPatch> GetPatchList()
    {

    }

    WEngine::Nullable<bool> GetActionJustPressed(const std::string &actionName)
    {
        if (ctx.selectedMap == nullptr)
            return {};
        if (!ctx.selectedMap->results.contains(actionName))
            return {};
        return ctx.selectedMap->results[actionName].justPressed;
    }

    WEngine::Nullable<bool> GetActionHeld(const std::string &actionName)
    {
        if (ctx.selectedMap == nullptr)
            return {};
        if (!ctx.selectedMap->results.contains(actionName))
            return {};
        return ctx.selectedMap->results[actionName].held;
    }

    WEngine::Nullable<bool> GetActionJustReleased(const std::string &actionName)
    {
        if (ctx.selectedMap == nullptr)
            return {};
        if (!ctx.selectedMap->results.contains(actionName))
            return {};
        return ctx.selectedMap->results[actionName].justReleased;
    }

    WEngine::Nullable<float32> GetFloat(const std::string &floatName)
    {
        if (ctx.selectedMap == nullptr)
            return {};
        if (!ctx.selectedMap->results.contains(floatName))
            return {};
        return ctx.selectedMap->results[floatName].trigger;
    }

    WEngine::Nullable<WEngine::Vector2> GetVector(const std::string &vectorName)
    {
        if (ctx.selectedMap == nullptr)
            return {};
        if (!ctx.selectedMap->results.contains(vectorName))
            return {};
        return ctx.selectedMap->results[vectorName].pos;
    }

    WEngine::Nullable<WEngine::InputVendor> GetVendor()
    {

    }

    bool GetDebugKeyJustPressed(uint8 keyNum)
    {
        if (keyNum > 12)
            return false;
        return ctx.rawKeys[0][(sizeT)WKey::DEBUG1 + keyNum - 1] && !ctx.rawKeys[1][(sizeT)WKey::DEBUG1 + keyNum - 1];
    }

    bool GetDebugKeyHeld(uint8 keyNum)
    {
        if (keyNum > 12)
            return false;
        return ctx.rawKeys[0][(sizeT)WKey::DEBUG1 + keyNum - 1];
    }

    bool GetDebugKeyJustReleased(uint8 keyNum)
    {
        if (keyNum > 12)
            return false;
        return !ctx.rawKeys[0][(sizeT)WKey::DEBUG1 + keyNum - 1] && ctx.rawKeys[1][(sizeT)WKey::DEBUG1 + keyNum - 1];
    }

    void Rumble(const std::string &outputName)
    {

    }

    void SetLED(const std::string &outputName)
    {

    }
}

#endif
