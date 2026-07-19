#pragma once

#include <Engine/Math/Vector.h>
#include <Engine/Math/Other/Quaternion.h>

namespace WEngine
{
	struct Transform
	{
		Transform(): position(Vector3::Zero), rotation(Quaternion::Identity), size(Vector3::One) {}
		Transform(Vector3 pos, Vector3 rot, Vector3 size) : position(pos), rotation(Quaternion::EulerToQuaternion(rot)), size(size) {}
		Transform(Vector3 pos, Quaternion rot, Vector3 size) : position(pos), rotation(rot), size(size) {}

		Vector3 position;
		Quaternion rotation;
		Vector3 size;

		Vector3 GetEulerAngles() const {
			return Quaternion::QuaternionToEuler(rotation) * (180.0f / pi);
		}
		void SetEulerAngles(const Vector3& eulerDegrees) {
			rotation = Quaternion::EulerToQuaternion(eulerDegrees * (pi / 180.0f));
		}

		Vector3 Forward() const { return Quaternion::Rotate(rotation, Vector3(0, 0, -1)); }
		Vector3 Right()   const { return Quaternion::Rotate(rotation, Vector3(1, 0,  0)); }
		Vector3 Up()      const { return Quaternion::Rotate(rotation, Vector3(0, 1,  0)); }

		const static Transform Zero;
	};
}
