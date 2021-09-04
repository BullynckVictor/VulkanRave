#version 450

layout(location = 0) in vec2 position;
layout(location = 1) in vec2 inTex;
layout(location = 0) out vec2 tex;

layout(binding = 0) uniform UBO { mat4 matrix; } ubo;

void main()
{
	gl_Position = ubo.matrix * vec4(position, 0.0, 1.0);
	tex = inTex;
}