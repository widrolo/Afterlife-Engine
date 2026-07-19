#pragma once
#include "Engine/Math/Matrices/CommonMatracies.h"
#include "Engine/Math/Vectors/Vec4f.h"
#include "Engine/Math/Vectors/Vec3f.h"
#include <glm/glm.hpp>


namespace WEngine
{
    struct Quaternion
    {
        float32 x, y, z, w;

        Quaternion() : x(0.0f), y(0.0f), z(0.0f), w(1.0f) {}
        Quaternion(const Quaternion& other) : x(other.x), y(other.y), z(other.z), w(other.w) {}
        Quaternion(float32 x, float32 y, float32 z, float32 w) : x(x), y(y), z(z), w(w) {}
        Quaternion(const Vector4& other) : x(other.x), y(other.y), z(other.z), w(other.w) {}
        Quaternion(const Vector3& other)
        {
            *this = EulerToQuaternion(other);
        }

        const static Quaternion Zero;
        const static Quaternion One;
        const static Quaternion Identity;

        Quaternion operator*(const Quaternion& other) const;

        void ConjugateThis();
        void InverseThis();

        [[nodiscard]] static Quaternion Conjugate(const Quaternion& q);
        [[nodiscard]] static Quaternion Inverse(const Quaternion& q);
        [[nodiscard]] static Vector3 Rotate(const Quaternion& q, const Vector3& v);
        [[nodiscard]] static Quaternion Slerp(const Quaternion& a, const Quaternion& b, float32 t);

        [[nodiscard]] static Quaternion EulerToQuaternion(const Vector3& euler);
        [[nodiscard]] static Vector3 QuaternionToEuler(const Quaternion& q);

        [[nodiscard]] static float32 Dot(const Quaternion& a, const Quaternion& b);

        void Normalize();
        [[nodiscard]] float32 Magnitude() const;
        [[nodiscard]] float32 MagnitudeSqr() const;

        [[nodiscard]] static Quaternion B3DToQuat(const b3Quat& quat);
        [[nodiscard]] static b3Quat QuatToB3D(const Quaternion& quat);
    };
}

template<>
struct std::formatter<WEngine::Quaternion>
{
    template<typename ParseContext>
    constexpr auto parse(ParseContext& ctx)
    {
        return ctx.begin();
    }

    template<typename FormatContext>
    auto format(const WEngine::Quaternion& v, FormatContext& ctx) const
    {
        return format_to(ctx.out(), "[{} | {} | {} | {}]", v.x, v.y, v.z, v.w);
    }
};