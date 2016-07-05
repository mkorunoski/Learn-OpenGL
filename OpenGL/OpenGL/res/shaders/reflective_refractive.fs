#version 420 core
#extension GL_ARB_explicit_uniform_location : enable

in vec4 f_position;
in vec4 f_normal;

// Fragment shader uniforms base: 20
layout(location = 20) uniform vec3 			eyePosition;

// Texture samplers base: 30
layout(location = 30) uniform samplerCube 	skyboxTex;

out vec4 fragColor;

void main()
{
	vec4 incident = normalize(f_position - vec4(eyePosition, 1.0f));
	vec4 refl = reflect(incident, normalize(f_normal));
	float ratio = 1.00f / 1.52f;    
    vec4 refr = refract(incident, normalize(f_normal), ratio);
    float a = 0.4f;
	fragColor = a * texture(skyboxTex, refl.xyz) + (1.0f - a) * texture(skyboxTex, refr.xyz);
}