#version 420 core
#extension GL_ARB_explicit_uniform_location : enable

// Input attributes base: 0
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 texCoords;
layout (location = 3) in vec3 tangent;

layout(std140) uniform ViewProjection
{
	mat4 view;
	mat4 projection;
};

// Transformation matrices base: 10
layout(location = 10) uniform mat4 model;
layout(location = 11) uniform mat4 inverseTranspose;

uniform sampler2D bump;

out VS_OUT
{
	vec4 position;	
	vec2 texCoords;
	mat3 TBN;
} vs_out;

void main()
{
	vs_out.position  = model * vec4(position, 1.0f);
	vs_out.texCoords = texCoords;	
	vec3 T = normalize((model * vec4(tangent, 0.0f)).xyz);
	vec3 N = normalize((model * vec4(normal, 0.0f)).xyz);
	vec3 B = normalize(cross(T, N));
	vs_out.TBN = mat3(T, B, N);

    gl_Position = projection * view * model * vec4(position, 1.0f);        
}