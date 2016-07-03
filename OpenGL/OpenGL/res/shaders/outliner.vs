#version 420 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec3 color;
layout (location = 3) in vec2 texCoords;

layout (std140, binding = 0) uniform TransformationBlock
{
	mat4 model;
	mat4 view;
	mat4 projection;
	mat4 inverseTranspose;
};

void main()
{
    gl_Position = projection * view * model * vec4(position, 1.0f);        
}