#include "Quaternion.h"

#include <Engine/Math/Math.h>

#include "Engine/Math/Vectors/VecMath.h"
#include "Engine/Util/Log.h"
#include <glm/glm.hpp>
using namespace WEngine;

const Quaternion Quaternion::Zero = { 0.0f, 0.0f, 0.0f, 0.0f };
const Quaternion Quaternion::One = { 1.0f, 1.0f, 1.0f, 1.0f };
const Quaternion Quaternion::Identity = { 0.0f, 0.0f, 0.0f, 1.0f };


Quaternion Quaternion::operator*(const Quaternion &other) const
{
    return {
        w * other.x + x * other.w + y * other.z - z * other.y,
        w * other.y - x * other.z + y * other.w + z * other.x,
        w * other.z + x * other.y - y * other.x + z * other.w,
        w * other.w - x * other.x - y * other.y - z * other.z
    };
}

void Quaternion::ConjugateThis()
{
    x = -x;
    y = -y;
    z = -z;
}

void Quaternion::InverseThis()
{
    ConjugateThis();
    float32 magSqr = MagnitudeSqr();
    x /= magSqr;
    y /= magSqr;
    z /= magSqr;
    w /= magSqr;
}

Quaternion Quaternion::Conjugate(const Quaternion& q)
{
    Quaternion result = q;
    result.ConjugateThis();
    return result;
}

Quaternion Quaternion::Inverse(const Quaternion& q)
{
    Quaternion result = q;
    result.InverseThis();
    return result;
}

Vector3 Quaternion::Rotate(const Quaternion& q, const Vector3& v)
{
    Vector3 qv(q.x, q.y, q.z);
    Vector3 t = VecMath::Cross(qv, v);
    t = t + t;
    Vector3 u = VecMath::Cross(qv, t);
    t = t * q.w;
    return v + u + t;
}

Quaternion Quaternion::Slerp(const Quaternion &a, const Quaternion &b, float32 t)
{
    float dot = Dot(a, b);

    Quaternion bAdj = b;
    if (dot < 0.0f)
    {
        bAdj.x = -b.x;
        bAdj.y = -b.y;
        bAdj.z = -b.z;
        bAdj.w = -b.w;
        dot = -dot;
    }

    const float32 DOT_THRESHOLD = 0.9995f;
    if (dot > DOT_THRESHOLD)
    {
        Quaternion result(
            a.x + t * (bAdj.x - a.x),
            a.y + t * (bAdj.y - a.y),
            a.z + t * (bAdj.z - a.z),
            a.w + t * (bAdj.w - a.w)
        );
        result.Normalize();
        return result;
    }

    float32 theta_0 = Math::Acos(dot);
    float32 theta = theta_0 * t;
    float32 sin_theta = Math::Sin(theta);
    float32 sin_theta_0 = Math::Sin(theta_0);

    float32 s0 = Math::Cos(theta) - dot * sin_theta / sin_theta_0;
    float32 s1 = sin_theta / sin_theta_0;

    return {
        s0 * a.x + s1 * bAdj.x,
        s0 * a.y + s1 * bAdj.y,
        s0 * a.z + s1 * bAdj.z,
        s0 * a.w + s1 * bAdj.w
    };
}

Quaternion Quaternion::EulerToQuaternion(const Vector3& euler)
{

    float32 cr = Math::Cos(-euler.x * 0.5f);
    float32 sr = Math::Sin(-euler.x * 0.5f);
    float32 cp = Math::Cos(-euler.y * 0.5f);
    float32 sp = Math::Sin(-euler.y * 0.5f);
    float32 cy = Math::Cos(-euler.z * 0.5f);
    float32 sy = Math::Sin(-euler.z * 0.5f);

    Quaternion q;
    q.w = cr * cp * cy + sr * sp * sy;
    q.x = sr * cp * cy - cr * sp * sy;
    q.y = cr * sp * cy + sr * cp * sy;
    q.z = cr * cp * sy - sr * sp * cy;

    return q;
}

Vector3 Quaternion::QuaternionToEuler(const Quaternion &q)
{
    Vector3 euler;

    float32 sinp = 2.0f * (q.w * q.y - q.z * q.x);
    sinp = Math::Clamp(sinp, -1.0f, 1.0f);

    if (Math::Abs(sinp) > 0.9999f)
    {
        euler.y = -Math::Asin(sinp);
        euler.x = -2.0f * Math::Atan2(q.x, q.w);
        euler.z = 0.0f;
        return euler;
    }

    // x axis (roll)
    float32 sinr_cosp = 2 * (q.w * q.x + q.y * q.z);
    float32 cosr_cosp = 1 - 2 * (q.x * q.x + q.y * q.y);
    euler.x = -Math::Atan2(sinr_cosp, cosr_cosp);

    // y axis (pitch)
    euler.y = -Math::Asin(sinp);

    // z axis (yaw)
    float32 siny_cosp = 2 * (q.w * q.z + q.x * q.y);
    float32 cosy_cosp = 1 - 2 * (q.y * q.y + q.z * q.z);
    euler.z = -Math::Atan2(siny_cosp, cosy_cosp);

    return euler;
}

float32 Quaternion::Dot(const Quaternion &a, const Quaternion &b)
{
    return a.x * b.x + a.y * b.y + a.z * b.z + a.w * b.w;
}

void Quaternion::Normalize()
{
    float32 len = Magnitude();
    if (len == 0.0f)
    {
        WLog::SetConsoleWarning();
        WLog::ConsoleLog("Tried to normalize a degenerate quaternion!");
        return;
    }
    x = x / len;
    y = y / len;
    z = z / len;
    w = w / len;
}

float32 Quaternion::Magnitude() const
{
    return Math::Sqrt(x * x + y * y + z * z + w * w);
}

float32 Quaternion::MagnitudeSqr() const
{
    return x * x + y * y + z * z + w * w;
}

Quaternion Quaternion::B3DToQuat(const b3Quat &quat)
{
    return Quaternion{quat.v.x, quat.v.y, quat.v.z, quat.s};
}
b3Quat Quaternion::QuatToB3D(const Quaternion &quat)
{
    return b3Quat{quat.x, quat.y, quat.z, quat.w};
}
