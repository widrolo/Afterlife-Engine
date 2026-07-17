#include "Input.h"

#include "AssetRepo.h"
#include "Engine/Core/System/Haptic.h"
#include "Engine/Types/AssetMission.h"
#include "Engine/Types/CoreSystems.h"
#include "Engine/Util/Log.h"


using namespace WEngine;

void Input::LoadInputMap()
{
    YamlAssetMission mission;
    mission.name = "../Input/Test";
    CoreSystems::GetAssetRepo()->GetAsset(mission);

    if (!mission.root["input"])
    {
        WLog::SetConsoleError();
        WLog::ConsoleLog(std::format("Unable to parse map, no input defintion!"));
        return;
    }

    auto root = mission.root["input"];

    if (!root["mapName"])
    {
        WLog::SetConsoleError();
        WLog::ConsoleLog(std::format("Unable to parse map, no name!"));
        return;
    }

    std::string mapName = root["mapName"].as<std::string>();

    if (!root["inputs"])
    {
        WLog::SetConsoleError();
        WLog::ConsoleLog(std::format("Unable to map {}, no inputs defined!", mapName));
        return;
    }
    wtl::vector<InputSense> senses{};

    for (const auto& sense : root["inputs"])
        ParseSingleInput(sense.second, senses, mapName);

    Haptic::LoadInputMap(senses, mapName);
    Haptic::SelectInputMap(mapName);
}

bool Input::GetAction(const std::string &name, PressType press)
{
    Nullable<bool> pressed;

    switch (press)
    {
        case PressType::Press:
            pressed = Haptic::GetActionJustPressed(name);
            break;
        case PressType::Hold:
            pressed = Haptic::GetActionHeld(name);
            break;
        case PressType::Release:
            pressed = Haptic::GetActionJustReleased(name);
            break;
        default: ;
    }
    if (pressed.HasValue())
        return pressed.GetValue();
    return false;
}

float32 Input::GetFloat(const std::string &name)
{
    auto floatN = Haptic::GetFloat(name);
    if (floatN.HasValue())
        return floatN.GetValue();
    return 0.0f;
}

Vector2 Input::GetVector(const std::string &name)
{
    auto vectorN = Haptic::GetVector(name);
    if (vectorN.HasValue())
        return vectorN.GetValue();
    return Vector2::Zero;
}

void Input::ParseSingleInput(const YAML::Node& root, wtl::vector<InputSense>& senses, const std::string& mapName)
{
    InputSense sense;
    if (!root["name"])
    {
        WLog::SetConsoleError();
        WLog::ConsoleLog(std::format("Map {} has a nameless input!", mapName));
        return;
    }

    sense.senseName = root["name"].as<std::string>();

    if (!root["inputType"])
    {
        WLog::SetConsoleError();
        WLog::ConsoleLog(std::format("Input {} in map {} has no type!", sense.senseName, mapName));
        return;
    }

    std::string inputType = root["inputType"].as<std::string>();

    if (!root["controller"] && !root["keyboard"] && !root["mouse"])
    {
        WLog::SetConsoleError();
        WLog::ConsoleLog(std::format("Input {} in map {} has no purpose!", sense.senseName, mapName));
        return;
    }

    if (inputType != "Action" && inputType != "Float" && inputType != "Vector")
    {
        WLog::SetConsoleError();
        WLog::ConsoleLog(std::format("Input {} in map {} has no valid type!", sense.senseName, mapName));
        return;
    }

    if (inputType == "Action")
        ParseAction(root, sense, mapName);
    if (inputType == "Float")
        ParseFloat(root, sense, mapName);
    if (inputType == "Vector")
        ParseVector(root, sense, mapName);

    senses.push_back(sense);
}

void Input::ParseAction(const YAML::Node &root, InputSense& sense, const std::string &mapName)
{
    InputActionInternal action{};

    if (root["controller"])
    {
        auto con = root["controller"];
        if (!con["button"])
        {
            WLog::SetConsoleError();
            WLog::ConsoleLog(std::format("Input {} in map {}: controller defined but no button assigned!", sense.senseName, mapName));
            return;
        }
        action.buttonName = con["button"].as<std::string>();
    }
    if (root["keyboard"])
    {
        auto key = root["keyboard"];
        if (!key["key"])
        {
            WLog::SetConsoleError();
            WLog::ConsoleLog(std::format("Input {} in map {}: keyboard defined but no key assigned!", sense.senseName, mapName));
            return;
        }
        action.keyName = key["key"].as<std::string>();
    }
    if (root["mouse"])
    {
        auto mou = root["mouse"];
        if (!mou["button"])
        {
            WLog::SetConsoleError();
            WLog::ConsoleLog(std::format("Input {} in map {}: mouse defined but no button assigned!", sense.senseName, mapName));
            return;
        }
        action.mouseName = mou["button"].as<std::string>();
    }

    sense.inputInternal = action;
}

void Input::ParseFloat(const YAML::Node &root, InputSense& sense, const std::string &mapName)
{
    InputFloatInternal floatIn{};

    if (root["controller"])
    {
        auto con = root["controller"];
        if (!con["trigger"])
        {
            WLog::SetConsoleError();
            WLog::ConsoleLog(std::format("Input {} in map {}: controller defined but no button assigned!", sense.senseName, mapName));
            return;
        }
        floatIn.triggerName = con["trigger"].as<std::string>();
    }
    if (root["keyboard"])
    {
        auto keys = root["keyboard"];
        if (!keys["keys"])
        {
            WLog::SetConsoleError();
            WLog::ConsoleLog(std::format("Input {} in map {}: keyboard defined but no key assigned!", sense.senseName, mapName));
            return;
        }

        // amalgamation of both types of for loops.
        sizeT i = 0;
        for (const auto& key : keys["keys"])
        {
            if (i >= 2)
                break;
            floatIn.keyNames[i] = key.as<std::string>();
            i++;
        }
    }
    if (root["mouse"])
    {
        auto mou = root["mouse"];
        if (!mou["sensor"] || !mou["maxSpeed"])
        {
            WLog::SetConsoleError();
            WLog::ConsoleLog(std::format("Input {} in map {}: mouse defined but no button assigned!", sense.senseName, mapName));
            return;
        }
        floatIn.sensorName = mou["sensor"].as<std::string>();
        floatIn.mouseMaxSpeed = mou["maxSpeed"].as<float32>();
    }

    sense.inputInternal = floatIn;
}

void Input::ParseVector(const YAML::Node &root, InputSense& sense, const std::string &mapName)
{
    InputVectorInternal vec{};

    if (root["controller"])
    {
        auto con = root["controller"];
        if (!con["joy"])
        {
            WLog::SetConsoleError();
            WLog::ConsoleLog(std::format("Input {} in map {}: controller defined but no button assigned!", sense.senseName, mapName));
            return;
        }
        vec.joyName = con["joy"].as<std::string>();
    }
    if (root["keyboard"])
    {
        auto keys = root["keyboard"];
        if (!keys["keys"])
        {
            WLog::SetConsoleError();
            WLog::ConsoleLog(std::format("Input {} in map {}: keyboard defined but no key assigned!", sense.senseName, mapName));
            return;
        }

        // amalgamation of both types of for loops.
        sizeT i = 0;
        for (const auto& key : keys["keys"])
        {
            if (i >= 4)
                break;
            vec.keyNames[i] = key.as<std::string>();
            i++;
        }
    }
    if (root["mouse"])
    {
        auto mou = root["mouse"];
        if (!mou["sensor"])
        {
            WLog::SetConsoleError();
            WLog::ConsoleLog(std::format("Input {} in map {}: mouse defined but no button assigned!", sense.senseName, mapName));
            return;
        }
        vec.sensorName = mou["sensor"].as<std::string>();
    }

    sense.inputInternal = vec;
}


