#pragma once
#include <yaml-cpp/yaml.h>

#include "Engine/Math/Vectors/Vec2f.h"
#include "Engine/Types/Input/InputSense.h"
#include "Engine/Types/Input/KeyCodes.h"
#include "Engine/WTL/vector.h"

// This file does not contain LLM generated documentation


class Input
{
public:
	static void LoadInputMap();

	static bool GetAction(const std::string &name, PressType press = PressType::Hold);
	static float32 GetFloat(const std::string &name);
	static WEngine::Vector2 GetVector(const std::string &name);

private:
	static void ParseSingleInput(const YAML::Node& root, wtl::vector<WEngine::InputSense>& senses, const std::string& mapName);
	static void ParseAction(const YAML::Node& root, WEngine::InputSense& sense, const std::string& mapName);
	static void ParseFloat(const YAML::Node& root, WEngine::InputSense& sense, const std::string& mapName);
	static void ParseVector(const YAML::Node& root, WEngine::InputSense& sense, const std::string& mapName);
};