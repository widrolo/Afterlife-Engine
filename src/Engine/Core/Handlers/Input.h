#pragma once
#include <yaml-cpp/yaml.h>

#include "Engine/Types/Input/InputSense.h"
#include "Engine/WTL/vector.h"

// This file does not contain LLM generated documentation


namespace WEngine
{

	class Input
	{
	public:
		static void LoadInputMap();

	private:
		static void ParseSingleInput(const YAML::Node& root, wtl::vector<InputSense>& senses, const std::string& mapName);
		static void ParseAction(const YAML::Node& root, InputSense& sense, const std::string& mapName);
		static void ParseFloat(const YAML::Node& root, InputSense& sense, const std::string& mapName);
		static void ParseVector(const YAML::Node& root, InputSense& sense, const std::string& mapName);
	};
}
