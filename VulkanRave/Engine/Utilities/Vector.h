#pragma once
#include "Engine/Utilities/GLM.h"
#include "Engine/Utilities/Types.h"

namespace rv
{
	template<size_t S, typename T>
	using Vector = glm::vec<S, T>;
	typedef Vector<2, float>	Vector2;
	typedef Vector<3, float>	Vector3;
	typedef Vector<4, float>	Vector4;
	typedef Vector<2, int>		Point;
	typedef Vector<2, uint>		Size;
}