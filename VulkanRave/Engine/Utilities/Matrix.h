#pragma once
#include "Engine/Utilities/GLM.h"

namespace rv
{
	template<size_t C, size_t R, typename T>
	using Matrix = glm::mat<C, R, T>;
	typedef Matrix<3, 3, float> Matrix3;
	typedef Matrix<4, 4, float> Matrix4;
}