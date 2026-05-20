#pragma once

#include <Engine/Math/Vector.h>

namespace WEngine
{
	/*
	* Transform
	* 
	* Holds information for the position, the size and
	* the rotation in deg (0 - 360)
	* 
	*/

	struct Transform
	{
		Transform(): position(Vector3::Zero), size(Vector3::Zero) {}
		Transform(Vector3 pos, Vector3 rot, Vector3 size) : position(pos), rotation(rot), size(size) {}

		Vector3 position;
		Vector3 rotation;
		Vector3 size;

		Vector3 Forward();
		Vector3 Right();
		Vector3 Up();

		const static Transform Zero;
	};
}