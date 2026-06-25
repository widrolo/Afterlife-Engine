@page scripting Getting Started

# Scripting

## Language
The only supported language is C++. The verison currently used is C++23 using GCC.

In the future, there will be Lua support, but that is far into the future. Its just C++ for now.

## Framework
The framework that Widrolo Engine provides is meant to make the development as easy as possible. 

### Widgets

When you want to make UI, whether for debugging or game UI, you inherit the WEngine::Widget class. Knowlege of ImGui is recommended for this one.

``` C++ 
// Game/GameWidgets/SampleWidget.h
class SampleWidget : public WEngine::Widget
{
public:
    using Widget::Widget;

public:
    void Setup() override;
protected:
    void RenderInternal() override;
};
```
``` C++ 
// Game/GameWidgets/SampleWidget.cpp

// Called once on init
void SampleWidget::Setup()
{
    m_widgetName = "Sample Widget";
    m_windowFlags |= ImGuiWindowFlags_NoResize;
}

// Called every frame. You dont need to use the ImGui begin and end functions
void SampleWidget::RenderInternal()
{
    SetSize({400, 350});

    ImGui::Text("Hello widget!");
}
```

### Game Systems
Sometimes, using components for game systems is just too jancky. If you truly need a singleton game system, that exists forever, then the Widrolo Engine is the right engine for you, as its natively supported. Lets just make a simple class.

``` C++ 
// Game/Core/SampleSystem.h

class SampleSystem
{
    // System constructors are forbidden from taking in arguments.
    SampleSystem();

    TickSystem();
}
``` 

Whatever you do inside is up to you. Lets now register it, 

``` C++ 
// Game/Types/GameSystems.h
#include <Engine/Types/CommonTypes.h>

class Game;
// use forward declarations.
class SampleSystem;
struct GameSystems
{
    friend Game;
private:
    _GLOBAL_ SampleSystem* sampleSystem;

public:
    static SampleSystem* GetSampleSystem() { return sampleSystem; }
};
``` 

Now that its registered, we can create it:

``` C++ 
// Game/Core/Game.cpp

void Game::InitGameSystems()
{
    StartSystemSingle<SampleSystem>(&GameSystems::sampleSystem, "Sample System");
}
```

Our system is now fully operational, but its not doing anything. If we want to run it at some specific point in the game loop, you might have seen that the game class presents us with numerous points where we can call our system. You can also call it in a component, via GameSystems::GetSampleSystem()->Whatever();