#pragma once
#include <variant>
#include <vector>

#include "Color.h"
#include "Engine/Math/Matrix.h"
#include "Engine/Math/Vector.h"
#include "Engine/WTL/vector.h"
#include "GPU/Texture.h"

namespace WEngine
{
    using ShaderSettingData = std::variant<float32, int32, Vector2, Vector3, Vector4, Mat4x4>;
    enum class ShaderSettingType : uint8
    {
        Float,
        Int,
        Vec2,
        Vec3,
        Vec4,
        Matrix4,
    };
    struct ShaderSetting
    {
        ShaderSettingType type;
        ShaderSettingData option;
        std::string settingName;
    };

    using ShaderSettings = wtl::vector<ShaderSetting>;

}
