#include "Game.h"

#include <string>
#include <Engine/Core/System/Memory.h>
#include <Engine/Util/Log.h>


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
    m_dt = frameDelta;
}

void Game::GameLoopTickEarly()
{

}

void Game::GameLoopTick()
{
    m_freecam.Tick((float32)m_dt);
    m_freecam.UploadCamera();
}

void Game::GameLoopTickLate()
{

}

void Game::GameLoopPhysicsEarly()
{

}

void Game::GameLoopPhysics()
{

}

void Game::GameLoopPhysicsLate()
{

}

void Game::GameLoopAudioEarly()
{

}

void Game::GameLoopAudioLate()
{

}

void Game::GameLoopWidgetEarly()
{

}

void Game::GameLoopDrawEarly()
{

}

void Game::GameLoopDraw()
{

}

void Game::GameLoopDrawLate()
{

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
