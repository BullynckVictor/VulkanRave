#version 450

layout(binding = 1) uniform UniformBuffer
{
	vec4 color;
} ubo;

layout(location = 0) out vec4 outColor;

void main() 
{
	outColor = ubo.color;
}