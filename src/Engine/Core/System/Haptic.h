#pragma once

// This file does not contain LLM generated documentation

#include <string>
#include <Engine/Types/Nullable.h>

#include "Engine/Types/Input/InputPatch.h"
#include "Engine/Types/Input/InputSense.h"
#include "Engine/WTL/vector.h"

class Haptic
{
public:
    static void Init(SDL_Window* window); // temporary SDL reference
    static void FetchInput();
    static void PollEvents();
    static void EnableEditorMode();

    // ------------------------ INPUT/OUTPUT MAPS -------------------------
    static void LoadInputMap(const wtl::vector<WEngine::InputSense>& mapContent, const std::string& mapName);
    static void SelectInputMap(const std::string& mapName);
    static void LoadOutputMap(const std::string& mapName);
    static void SelectOutputMap(const std::string& mapName);

    // ----------------------------- PATCHING -----------------------------
    static void ApplyFullPatch(const wtl::vector<WEngine::InputPatch>& patches);
    static void ApplySinglePatch(const WEngine::InputPatch& patch);
    static wtl::vector<WEngine::InputPatch> GetPatchList();

    // ------------------------------ INPUT -------------------------------
    static WEngine::Nullable<bool> GetActionJustPressed(const std::string& actionName);
    static WEngine::Nullable<bool> GetActionHeld(const std::string& actionName);
    static WEngine::Nullable<bool> GetActionJustReleased(const std::string& actionName);

    static WEngine::Nullable<float32> GetFloat(const std::string& floatName);

    static WEngine::Nullable<WEngine::Vector2> GetVector(const std::string& vectorName);

    static WEngine::Nullable<WEngine::InputVendor> GetVendor();

    static bool GetDebugKeyJustPressed(uint8 keyNum);
    static bool GetDebugKeyHeld(uint8 keyNum);
    static bool GetDebugKeyJustReleased(uint8 keyNum);

    // ------------------------------ Output -------------------------------
    static void Rumble(const std::string& outputName);
    static void SetLED(const std::string& outputName);
};
