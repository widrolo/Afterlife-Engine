#pragma once

#include <Game/Gameplay/Freecam.h>

class Game
{
public:
    Game();
    ~Game();

public:
    void PreGameLoop();
    void GameLoopBegin(float64 frameDelta);
    void GameLoopTickEarly();
    void GameLoopTick();
    void GameLoopTickLate();
    void GameLoopPhysicsEarly();
    void GameLoopPhysics();
    void GameLoopPhysicsLate();
    void GameLoopAudioEarly();
    void GameLoopAudioLate();
    void GameLoopWidgetEarly();
    void GameLoopDrawEarly();
    void GameLoopDraw();
    void GameLoopDrawLate();
    void GameLoopFinish();


private:
    void InitGameSystems();

private:
    float64 m_dt;
    Freecam m_freecam;
};