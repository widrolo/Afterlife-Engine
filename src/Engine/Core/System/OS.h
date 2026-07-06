#pragma once

// This file does not contain LLM generated documentation

#include <string>
#include <Engine/Types/CommonTypes.h>

#include "Engine/WTL/vector.h"

/**
 * This is the primary interface to talk to the operating system.
 * The different operating systems have their own implementation for these functions.
 * @note All types used are strictly os independent.
 */
class OS
{
public:
    /**
     * Retrieves the path of the current process.
     * @return The full path of the process.
     */
    static std::string GetProcessPath();

    /**
     * Sets the color of the text in the console.
     * @param color Text color.
     */
    static void SetConsoleColor(unsigned char color);

    /**
     * Spawns a new process,
     * @param executable The path to the executable.
     * @param arguments Arguments to be passed into the process.
     */
    static void CreateNewProcess(const std::string& executable, const wtl::vector<std::string>& arguments);

    /**
     * Finds all files within a given directory.
     * @param dir The directory to be scanned.
     * @return A collection of path names.
     * @note This is not recursive.
     * @note The files are in full path length.
     */
    static wtl::vector<std::string> GetAllFileNamesInDir(const std::string& dir);
};

#undef min