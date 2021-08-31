#pragma once
#include "Engine/Utilities/Vector.h"
#include "Engine/Utilities/Matrix.h"

namespace rv
{
	struct Camera2
	{
		Camera2() = default;
		Camera2(const Vector2& pos, float rotation = 0, float zoom = 1, const Vector2& ratio = { 1, 1 });

		void Concatenate();

		Vector2 position = { 0, 0 };
		float rotation = 0;
		float zoom = 1;
		Vector2 ratio = { 1, 1 };

		Matrix4 view = Matrix4( 1 );
	};

	struct Transform2
	{
		Transform2() = default;
		Transform2(const Vector2& pos, float rotation = 0, const Vector2& scale = { 1, 1 });
		Transform2(const Camera2& camera, const Vector2& pos = { 0, 0 }, float rotation = 0, const Vector2& scale = { 1, 1 });

		void Concatenate(const Camera2& camera);

		Vector2 position = { 0, 0 };
		float rotation = 0;
		Vector2 scale = { 1, 1 };

		Matrix4 model = Matrix4(1);
		Matrix4 modelview = Matrix4(1);
	};
}