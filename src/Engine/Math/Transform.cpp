#include "Transform.h"

#include <cmath>
#include <numbers>

using namespace WEngine;

const Transform Transform::Zero = { Vector3::Zero, Quaternion::Identity, Vector3::One };
