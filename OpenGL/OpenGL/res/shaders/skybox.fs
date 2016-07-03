#version 420 core

in vec3 eyeDirection;

uniform samplerCube skyboxTex;

out vec4 fragColor;

void main()
{
	fragColor = texture(skyboxTex, eyeDirection);	
}