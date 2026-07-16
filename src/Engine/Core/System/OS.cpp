#include "Engine/Core/System/OS.h"

#include <cstring>
#include <iostream>
#include <ctime>

#ifdef WE_Windows
#include <Windows.h>
#include <intrin.h>
#endif

#ifdef WE_Linux
#include <unistd.h>
#include <cpuid.h>
#include <sys/time.h>
#include <cerrno>
#endif

#include <Engine/Types/CommonTypes.h>
#include <Engine/Util/Log.h>
#include <filesystem>


#ifdef WE_Windows
std::string OS::GetProcessPath()
{
    char buffer[1024];
	GetModuleFileNameA(NULL, buffer, MAX_PATH);
	std::string path = buffer;
    return path;
}

void OS::SetConsoleColor(unsigned char color)
{
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

    SetConsoleTextAttribute(hConsole, color);
}

void OS::CreateNewProcess(const std::string &executable, const wtl::vector<std::string> &arguments)
{
    std::string commandLine = "\"" + executable + "\"";

    for (const auto& arg : arguments)
    {
        commandLine += " \"" + arg + "\"";
    }

    STARTUPINFOA si{};
    PROCESS_INFORMATION pi{};

    si.cb = sizeof(si);

    std::vector<char> cmd(commandLine.begin(), commandLine.end());
    cmd.push_back('\0');

    BOOL success = ::CreateProcessA(
        executable.c_str(),
        cmd.data(),
        nullptr,
        nullptr,
        FALSE,
        0,
        nullptr,
        nullptr,
        &si,
        &pi);

    if (!success)
    {
        DWORD error = GetLastError();

        WEngine::WLog::SetConsoleError();
        WEngine::WLog::ConsoleLog(
            std::format("Couldn't create process because CreateProcess failed; Code: {}",
                        error));
        return;
    }

    CloseHandle(pi.hThread);
    CloseHandle(pi.hProcess);
}

wtl::vector<std::string> OS::GetAllFileNamesInDir(const std::string &dir)
{
    const std::filesystem::path path = dir;

    wtl::vector<std::string> files;
    for (const auto& entry : std::filesystem::directory_iterator(path))
    {
        if (std::filesystem::is_regular_file(entry.path()))
            files.push_back(entry.path().string());
    }
    return files;
}

#endif

#ifdef WE_Linux

std::string OS::GetProcessPath()
{
    char buffer[1024];

    int64 len = readlink("/proc/self/exe", buffer, sizeof(buffer) - 1);

    if (len != -1)
        buffer[len] = 0; 
    else
        perror("could not find get the process path");

    std::string path = buffer;

    return path;
}

void OS::SetConsoleColor(unsigned char color)
{
    const char* ansiColors[] = {
        "\033[30m", // 0 - Black
        "\033[34m", // 1 - Blue
        "\033[32m", // 2 - Green
        "\033[36m", // 3 - Aqua/Cyan
        "\033[31m", // 4 - Red
        "\033[35m", // 5 - Purple
        "\033[33m", // 6 - Yellow
        "\033[37m", // 7 - White (light gray)
        "\033[90m", // 8 - Bright Black (dark gray)
        "\033[94m", // 9 - Bright Blue
        "\033[92m", // 10 - Bright Green
        "\033[96m", // 11 - Bright Aqua/Cyan
        "\033[91m", // 12 - Bright Red
        "\033[95m", // 13 - Bright Purple
        "\033[93m", // 14 - Bright Yellow
        "\033[97m"  // 15 - Bright White
    };

    if (color < 16)
        std::cout << ansiColors[color];
}

void OS::CreateNewProcess(const std::string& executable, const wtl::vector<std::string>& arguments)
{
    if (fork() == 0)
    {
        char* argv[arguments.size() + 2];
        for (sizeT i = 0; i < arguments.size(); i++)
            argv[i] = (char*)arguments[i].c_str();
        argv[arguments.size() + 2] = nullptr;
        int32 ret = execve(executable.c_str(), argv, environ);
        if (ret == -1)
        {
            WEngine::WLog::SetConsoleError();
            WEngine::WLog::ConsoleLog(std::format("Couldn't create Process because execve failed; Code: {}, '{}'", errno, strerror(errno)));
            exit(-1);
        }
    }
}

wtl::vector<std::string> OS::GetAllFileNamesInDir(const std::string &dir)
{
    const std::filesystem::path path = dir;

    wtl::vector<std::string> files;
    for (const auto& entry : std::filesystem::directory_iterator(path))
    {
        if (std::filesystem::is_regular_file(entry.path()))
            files.push_back(entry.path().string());
    }
    return files;
}

#endif
