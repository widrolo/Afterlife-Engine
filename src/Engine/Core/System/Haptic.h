#pragma once

// This file does not contain LLM generated documentation

#include <string>
#include <Engine/Types/Nullable.h>

#include "Engine/Types/Input/InputPatch.h"
#include "Engine/Types/Input/InputSense.h"
#include "Engine/WTL/vector.h"

namespace  Haptic
{
    void Init(SDL_Window* window); // temporary SDL reference
    void FetchInput();
    void PollEvents();
    void EnableEditorMode();

    // ------------------------ INPUT/OUTPUT MAPS -------------------------
    void LoadInputMap(const wtl::vector<WEngine::InputSense>& mapContent, const std::string& mapName);
    void SelectInputMap(const std::string& mapName);
    void LoadOutputMap(const std::string& mapName);
    void SelectOutputMap(const std::string& mapName);

    // ----------------------------- PATCHING -----------------------------
    void ApplyFullPatch(const wtl::vector<WEngine::InputPatch>& patches);
    void ApplySinglePatch(const WEngine::InputPatch& patch);
    wtl::vector<WEngine::InputPatch> GetPatchList();

    // ------------------------------ INPUT -------------------------------
    WEngine::Nullable<bool> GetActionJustPressed(const std::string& actionName);
    WEngine::Nullable<bool> GetActionHeld(const std::string& actionName);
    WEngine::Nullable<bool> GetActionJustReleased(const std::string& actionName);

    WEngine::Nullable<float32> GetFloat(const std::string& floatName);

    WEngine::Nullable<WEngine::Vector2> GetVector(const std::string& vectorName);

    WEngine::Nullable<WEngine::InputVendor> GetVendor();

    bool GetDebugKeyJustPressed(uint8 keyNum);
    bool GetDebugKeyHeld(uint8 keyNum);
    bool GetDebugKeyJustReleased(uint8 keyNum);

    // ------------------------------ Output -------------------------------
    void Rumble(const std::string& outputName);
    void SetLED(const std::string& outputName);
};
