#version 420 core

layout (location = 0) in vec2 position;
layout (location = 1) in vec2 texCoords;

out vec2 f_texCoords;

void main()
{
	f_texCoords = texCoords;   
	gl_Position = vec4(position.x, position.y, 0.0f, 1.0f);	     
}
