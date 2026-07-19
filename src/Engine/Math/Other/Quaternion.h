#pragma once
#include "Engine/Math/Vectors/Vec4f.h"
#include "Engine/Math/Vectors/Vec3f.h"

namespace WEngine
{
    struct Quaternion
    {
        float32 x, y, z, w;

        Quaternion() : x(0.0f), y(0.0f), z(0.0f), w(1.0f) {}
        Quaternion(const Quaternion& other) : x(other.x), y(other.y), z(other.z), w(other.w) {}
        Quaternion(float32 x, float32 y, float32 z, float32 w) : x(x), y(y), z(z), w(w) {}
        Quaternion(const Vector4& other) : x(other.x), y(other.y), z(other.z), w(other.w) {}

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
    };
}
