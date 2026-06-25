@page components Scripting

# Components

Whenever you want to make game logic, you mostly rely on components. Making a component is quite simple, as you just inherit the WEngine::Component class. 

Components are pretty much the same thing as GameObjects in Unity: They are small programmable systems that can hold data and call to Engine functions. They are attached and managed by Entities, which are themselves managed by Sectors

## Creating a class

First, edit {Engine Root}/src/Game/GameComponentList.xmacro and append the line

``` C++
X(SampleComponent)
```
This adds a new entry in the component registry. Now lets define the class:

``` C++ 
// Game/Components/SampleComponent.h
#include <WidroloFramework.h>

class SampleComponent : public WEngine::Component
{
public:
    SampleComponent(WEngine::Entity* e);

public:
    // possible overridable classes, pick and choose which you need
    void Awake(WEngine::ComponentArgs ca) override;
    void LateAwake() override;
    void Start() override;
    void Tick(float32 dt) override;
    void PhysicsTick(float32 tr) override;
    void Draw() override;
    void DrawDebug() override;

    // Precomputed hash has of the word "SampleComponent", please use the following:
    //
    //  {Engine Root}/Tools/Hasher/hasher.exe {Component Name}
    //
    // when computing this hash.
    COMP_HASH(0x8edac9639be9a1cb)
};
```

For more info about the overridden functions, please consult the game loop page.

``` C++ 
// Game/Components/SampleComponent.cpp
#include "SampleComponent.h"

REGISTER_COMPONENT(SampleComponent)

SampleComponent::SampleComponent(WEngine::Entity *e)
{
    COMP_SETUP("SampleComponent")
}
```

Forgetting one of these component macros will usually result in a compilation error, or a crash during runtime. Having a wrong hash is no problem, you can just type in whatever, its just used during GetComponent so just make sure to avoid collisions (or ideally, use the hasher).

## WEDGE integration

We can now use the class, but it wont show up in WEGDE yet, we need to add an entry to {Engine Root}/GameData/Data/CompDef/BaseComponents.yaml so that WEDGE can see them. This is where it gets tricky, as we dont know what the ID of the component is. Game compnents start at ID 1024, and so we can use the line number of SampleComponent at {Engine Root}/src/Game/GameComponentList.xmacro to determine ID with this function:

> 1024 + line - 1

note that it is very fragile. This formula assumes that you only define components in order, with no missing lines or comments. Removing a component will move all subsequent components down one ID if removed in the xmacro file. I recommend removing the class, but keeping the xmacro definition, maybe append "_deprecated" to the name to make it clear.

Now in {Engine Root}/GameData/Data/CompDef/BaseComponents.yaml we can append a new component:

``` YAML
Sample Component:
    ID: 1024
    Name: "Sample Component"
    Description: "This component only exists for the tutorial."
```

Adding a new component of ID 1024 in WEDGE will show this component now.
Note that the the way that WEDGE is completely unaware of your components, and only does whatever you define in BaseComponents.yaml; please be aware that there are no sanity checks within WEDGE that check for validity.

## Per instance arguments

You most likely also want to define the per component arguments, so that the sectors can read them and pass them onto the component. 

While engines like Unity allow you to simply define a serializable field, Widrolo Engine gives you way more control at the cost of development time.

Lets add an integer argument to our component:

``` YAML
Sample Component:
    ID: 1024
    Name: "Sample Component"
    Description: "This component only exists for the tutorial."
    Settings:
        NumTest:
            Name: "Int Input"
            Description: "A basic input for integers."
            Type: "Number"
            Internal: intInput
```

Upon saving the file and reloading WEDGE, the component settings should look like this:

\image html 1_wedge.png

There are many different types. In the definition there is an "Editor Tester" entry at ID 512. The game will crash if you attach it to anything, its meant to be as a copy&template. It shows every available type.

To test if the component is defined correctly, add the component to a new entity in an easy to access sector. If the game doesn't crash, then your component is properly registered. If it crashes, the engine will tell you exactly whats wrong.

As said before, explicitness is key here. Therefore you must define in you component what to do with the argument. The Awake function is specifically made for this task. Lets add an Awake function to our component:

``` C++ 
// Game/Components/SampleComponent.cpp

void SampleComponent::Awake(WEngine::ComponentArgs ca)
{
    auto numN = ca.GetIntFromParams("intInput");

    if (numN.HasValue())
        WEngine::WLog::ConsoleLog(std::format("Our test number: {}", numN.GetValue()));
}
```

All arguments are sampled from Get{Type}FromParams method in the ca variable thats passed in. You must Specify the type to get, and then the internal name we defined earlier. They all return a Nullable type, so you must first check whether the value exists or not. You may then do additional processing in the awake function.

For example, the Mesh Renderer loads the assets in the Awake function. Such level of control is not really guaranteed in other engines.

## Future

Note that later down the line, WEDGE might include tooling to semi automate this process at a more convenient level.
