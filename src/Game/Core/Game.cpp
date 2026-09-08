#include "Game.h"

#include <string>
#include <Engine/Core/System/Memory.h>
#include <Engine/Util/Log.h>

#include "Engine/Util/TimeAnalysis.h"
#include "Game/Types/GameState.h"


Game::Game()
{

}

Game::~Game()
{

}

void Game::PreGameLoop()
{
    InitGameSystems();
}

void Game::GameLoopBegin(float64 frameDelta)
{
    WEngine::TimeSample sample("Game::GameLoopBegin");
    m_dt = frameDelta;
}

void Game::GameLoopTickEarly()
{
    WEngine::TimeSample sample("Game::GameLoopTickEarly");
}

void Game::GameLoopTick()
{
    WEngine::TimeSample sample("Game::GameLoopTick");

    if (Input::GetAction("toggleFreecam", PressType::Press))
    {
        FreecamState::active = !FreecamState::active;
        m_player.UpdateFreecam(m_freecam);
    }

    if (FreecamState::active)
    {
        m_freecam.Tick((float32)m_dt);
        m_freecam.UploadCamera();
    }
    else
    {
        m_player.Tick((float32)m_dt);
        m_player.UploadCamera();
    }
}

void Game::GameLoopTickLate()
{
    WEngine::TimeSample sample("Game::GameLoopTickLate");
}

void Game::GameLoopPhysicsEarly()
{
    WEngine::TimeSample sample("Game::GameLoopPhysicsEarly");

}

void Game::GameLoopPhysics()
{
    WEngine::TimeSample sample("Game::GameLoopPhysics");

}

void Game::GameLoopPhysicsLate()
{
    WEngine::TimeSample sample("Game::GameLoopPhysicsLate");

}

void Game::GameLoopAudioEarly()
{
    WEngine::TimeSample sample("Game::GameLoopAudioEarly");

}

void Game::GameLoopAudioLate()
{
    WEngine::TimeSample sample("Game::GameLoopAudioLate");

}

void Game::GameLoopWidgetEarly()
{
    WEngine::TimeSample sample("Game::GameLoopWidgetEarly");

}

void Game::GameLoopDrawEarly()
{
    WEngine::TimeSample sample("Game::GameLoopDrawEarly");

}

void Game::GameLoopDraw()
{
    WEngine::TimeSample sample("Game::GameLoopDraw");
    if (FreecamState::active)
        m_freecam.RenderNotif();
}

void Game::GameLoopDrawLate()
{
    WEngine::TimeSample sample("Game::GameLoopDrawLate");

}

void Game::GameLoopFinish()
{

}

template<class T>
void StartSystemSingle(T** container, std::string name)
{
    *container = (T*)WAllocator::Construct<T>();
    if (*container == nullptr)
    {
        WEngine::WLog::SetConsoleError();
        WEngine::WLog::ConsoleLog(std::format("{} creation failed", name));
        std::exit(-1);
    }
}

void Game::InitGameSystems()
{

}
