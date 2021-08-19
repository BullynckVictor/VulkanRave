#version 450

layout(location = 0) in vec2 position;
layout(location = 1) in vec4 color;
layout(location = 0) out vec4 fragColor;

layout(binding = 0) uniform UniformBuffer {
	mat4 matrix;
} ubo;

void main() 
{
	gl_Position = ubo.matrix * vec4(position, 0.0, 1.0);
	fragColor = color;
}