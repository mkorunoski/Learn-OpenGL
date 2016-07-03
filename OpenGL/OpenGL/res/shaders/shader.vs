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

out vec4 f_position;
out vec4 f_normal;
out vec2 f_texCoords;
out mat4 f_inverseTranspose;

void main()
{
	f_position	= model * vec4(position, 1.0f);
	f_normal 	= inverseTranspose * vec4(normal, 0.0f);
	f_texCoords = texCoords;
	f_inverseTranspose = inverseTranspose;

    gl_Position = projection * view * model * vec4(position, 1.0f);        
}