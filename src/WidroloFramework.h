#pragma once

// This file should be included in every game logic file


// Systems (types get included from these as well btw)
#include <Engine/Core/System/Memory.h>
#include <Engine/Core/System/Iris.h>
#include <Engine/Core/System/Echo.h>
#include <Engine/Core/System/Haptic.h>
#include <Engine/Core/System/OS.h>

// Deep Core handlers
#include <Engine/Core/Handlers/AssetRepo.h>
#include <Engine/Core/Handlers/SectorHandler.h>
#include <Engine/Core/Handlers/RenderHandler.h>
#include <Engine/Core/Handlers/Input.h>

// High Level Core handlers
#include <Engine/Core/Handlers/PhysicsHandler.h>
#include <Engine/Core/Handlers/RNGHandler.h>
#include <Engine/Core/Handlers/WidgetHandler.h>
#include <Engine/Core/Handlers/TimeHandler.h>

// Engine etc.
#include <Engine/Stores/Steam/SteamStore.h>
#include <Engine/Core/Widget.h>
#include <Engine/Core/World/Sector.h>
#include <Engine/Types/CoreSystems.h>

// Types - General
#include <Engine/Types/CommonTypes.h>
#include <Engine/Types/Nullable.h>
#include <Engine/Types/Version.h>
#include <Engine/Types/Refcounted.h>
#include <Engine/Types/ValueBelt.h>

// Types - WTL
#include <Engine/WTL/allocator.h>
#include <Engine/WTL/vector.h>

// Types - Input
#include <Engine/Types/Input/KeyCodes.h>
#include <Engine/Types/Input/InputSense.h>
#include <Engine/Types/Input/InputPatch.h>
#include <Engine/Types/Input/InputDevice.h>

// Types - Rendering
#include <Engine/Types/Rendering/Color.h>
#include <Engine/Types/Rendering/RenderMission.h>

// Types - Time
#include <Engine/Types/Time/Time.h>
#include <Engine/Types/Time/Day.h>
#include <Engine/Types/Time/Month.h>
#include <Engine/Types/Time/Date.h>

// Types - Etc
#include <Engine/Types/AssetMission.h>

// Math
#include <Engine/Math/Math.h>
#include <Engine/Math/Transform.h>
#include <Engine/Math/Matrix.h>
#include <Engine/Math/Vector.h>
#include <Engine/Math/Shapes.h>

// Utils
#include <Engine/Util/Log.h>
#include <Engine/Util/BitwiseMacros.h>
#include <Engine/Util/TimeAnalysis.h>
#include <Engine/Util/Timer.h>

