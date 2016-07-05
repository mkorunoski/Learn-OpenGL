#version 420 core

in vec2 f_texCoords;

uniform sampler2D screenTexture;

out vec4 fragColor;

void InvertColors()
{
	fragColor = vec4(vec3(1.0f - texture(screenTexture, f_texCoords)), 1.0f); 
}

void Grayscale()
{
	vec4 sampled = texture(screenTexture, f_texCoords);
	float average = (sampled.x + sampled.y + sampled.z) / 3.0f;
	fragColor = vec4(average, average, average, 1.0f); 
}

void main()
{
	fragColor = texture(screenTexture, f_texCoords);
//	InvertColors();
}