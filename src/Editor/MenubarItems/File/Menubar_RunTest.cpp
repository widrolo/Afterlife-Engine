#include "Menubar_RunTest.h"

#include "Engine/Core/System/OS.h"

using namespace WEditor;

void Menubar_RunTest::Setup()
{
    m_entryName = "Run Test";
}

void Menubar_RunTest::OnClick()
{
    std::string exec = OS::GetProcessPath();
    wtl::vector<std::string> args {exec, "--game", "-testing"};
    OS::CreateNewProcess(exec, args);
}