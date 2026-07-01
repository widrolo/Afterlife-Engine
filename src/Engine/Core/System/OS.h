#pragma once

#include <string>
#include <Engine/Types/CommonTypes.h>

#include "Engine/WTL/vector.h"

// Temporary stuff from windows


// This used to be called the rosetta system, but apple owns it so yeah
class OS
{
public:
    static std::string GetProcessPath();

    static void SetConsoleColor(unsigned char color);
    static void CreateNewProcess(const std::string& executable, const wtl::vector<std::string>& arguments);
    static wtl::vector<std::string> GetAllFileNamesInDir(const std::string& dir);
};

#undef min