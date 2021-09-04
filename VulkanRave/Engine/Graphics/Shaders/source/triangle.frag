#version 450

layout(binding = 2) uniform UniformBuffer
{
	vec4 color;
} ubo;
layout(binding = 1) uniform sampler2D texSampler;

layout(location = 0) out vec4 outColor;
layout(location = 0) in vec2 tex;

void main() 
{
	outColor = ubo.color;
	outColor = texture(texSampler, tex);
}