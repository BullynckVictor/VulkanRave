#include "Engine/General/Transform.h"
#include "Engine/Libraries/glm/gtc/matrix_transform.hpp"
#include <algorithm>

rv::Camera2::Camera2(const Vector2& pos, float rotation, float zoom, const Vector2& ratio)
	:
	position(pos),
	rotation(rotation),
	zoom(zoom),
	ratio(ratio)
{
	Concatenate();
}

void rv::Camera2::Concatenate()
{
	Matrix4 identity = Matrix4(1);
	ratio /= std::min(ratio.x, ratio.y);
	view = glm::ortho(-ratio.x / zoom, ratio.x / zoom, ratio.y / zoom, -ratio.y / zoom) * glm::inverse(
		glm::translate(identity, Vector3(position, 0)) *
		glm::rotate(identity, rotation, Vector3(0, 0, 1))
	);
}

rv::Transform2::Transform2(const Vector2& pos, float rotation, const Vector2& scale)
	:
	position(pos),
	rotation(rotation),
	scale(scale)
{
}

rv::Transform2::Transform2(const Camera2& camera, const Vector2& pos, float rotation, const Vector2& scale)
	:
	position(pos),
	rotation(rotation),
	scale(scale)
{
	Concatenate(camera);
}

void rv::Transform2::Concatenate(const Camera2& camera)
{
	Matrix4 identity = Matrix4(1);
	model =
		glm::translate(identity, Vector3(position.x, position.y, 0)) *
		glm::rotate(identity, rotation, Vector3(0, 0, 1)) *
		glm::scale(identity, Vector3(scale.x, scale.y, 1.0f))
	;
	modelview = model * camera.view;
}
