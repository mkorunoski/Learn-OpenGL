#version 420 core

#define near 0.1f
#define far 1000.0f

in vec2 f_texCoords;

uniform sampler2D screenTexture;

out vec4 fragColor;

float LinearizeDepth(float depth) 
{
    float z = depth * 2.0 - 1.0;
    return (2.0 * near * far) / (far + near - z * (far - near));	
}

void main()
{
	float depth = texture(screenTexture, f_texCoords).r;
	fragColor = vec4(vec3(depth), 1.0f);
}