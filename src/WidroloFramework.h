#pragma once

// This file should be included in every game logic file

#include <Engine/Core/System/Memory.h>

// Data Storage
#include <Engine/Types/AssetMission.h>
#include <Engine/Types/Audio.h>
#include <Engine/Types/Nullable.h>
#include <Engine/Types/CommonTypes.h>
#include <Engine/Types/CoreSystems.h>
#include <Engine/Types/Input/KeyCodes.h>
#include <Engine/Math/Math.h>
#include <Engine/Math/Transform.h>
#include <Engine/Types/Rendering/Color.h>
#include <Engine/Types/SpawnArgs.h>
#include <Engine/Types/Rendering/RenderMission.h>
#include <Engine/Types/Version.h>
#include <Engine/Core/Widget.h>

// Utils 
#include <Engine/Util/Log.h>
#include <Engine/Util/BitwiseMacros.h>

// etc.
#include <Engine/Core/World/Sector.h>
#include <Engine/UI/UIParser.h>

#include <Engine/Stores/Steam/SteamStore.h>