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

out vec3 eyeDirection;

void main()
{
	mat4 inverseProjection = inverse(projection);
    mat3 inverseModelview = transpose(mat3(view * model));
    vec3 unprojected = mat3(inverseProjection) * position;
    eyeDirection = inverseModelview * unprojected;

    gl_Position = vec4(position, 1.0f);        
}
