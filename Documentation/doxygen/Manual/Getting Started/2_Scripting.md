@page scripting Getting Started

# Scripting

## Language
The only supported language is C++. The verison currently used is C++23 using GCC.

In the future, there will be Lua support, but that is far into the future. Its just C++ for now.

## Framework
The framework that Widrolo Engine provides is meant to make the development as easy as possible. 

### Components
When you want to make game logic, you mostly rely on components. Making a component is quite simple, as you just inherit the WEngine::Component class. 

First, edit {Engine Root}/src/Game/GameComponentList.xmacro and append the line

``` C++
X(SampleClass)
```
This adds a new entry in the component registry. Now lets define the class:

``` C++ 
// Game/Components/SampleClass.h
#include <WidroloFramework.h>

class SampleClass : public WEngine::Component
{
public:
    SampleClass(WEngine::Entity* e);

public:
    // possible overridable classes, pick and choose which you need
    void Awake(WEngine::ComponentArgs ca) override;
    void LateAwake() override;
    void Start() override;
    void Tick(float32 dt) override;
    void PhysicsTick(float32 tr) override;
    void Draw() override;
    void DrawDebug() override;

    // Precomputed hash has of the word "SampleClass", please use the following:
    //
    //  {Engine Root}/Tools/Hasher/hasher.exe {Component Name}
    //
    // when computing this hash.
    COMP_HASH(0x8edac9639be9a1cb)
};
```

``` C++ 
// Game/Components/SampleClass.cpp
#include "SampleClass.h"

REGISTER_COMPONENT(SampleClass)

SampleClass::SampleClass(WEngine::Entity *e)
{
    COMP_SETUP("SampleClass")
}

```

We can now use the class, but it wont show up in WEGDE yet, we need to add an entry to {Engine Root}/GameData/Data/CompDef/BaseComponents.yaml so that WEDGE can see them. This is where it gets tricky, as we dont know what the ID of the component is. Game compnents start at ID 1024, and so we can use the line number od SampleClass at {Engine Root}/src/Game/GameComponentList.xmacro to determine ID with this function:

> 1024 + line - 1

Now in {Engine Root}/GameData/Data/CompDef/BaseComponents.yaml we can append a new component:

``` YAML
Sample Component:
    ID: 1024
    Name: "Sample Component"
    Description: "This component only exists for the tutorial."
```

Adding a new component of ID 1024 in WEDGE will show this component now.

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